#include "stdafx.h"
#include "Attack.h"
#include "AttackManager.h"
#include "AttackData.h"

#include "App/App.h"
#include "World/World.h"

#include "Effect/Effect.h"
#include "Effect/EffectManager.h"

#include "Collide/CollideObject.h"
#include "Collide/CollideCaster.h"
#include "Collide/Shape/CollideShape.h"

#include "Player/Player.h"
#include "Item/ItemManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	攻撃（基底）
//
////////////////////////////////////////////////////////////////////////////////
Attack::Attack( AttackManager* pManager )
{
	_ASSERT( pManager );

	m_pManager		= pManager;
	m_pData			= NULL;
	m_bDelete		= false;
	m_FrameCount	= 0;

	m_pCaster		= NULL;
	m_pTimeKey		= NULL;
}
Attack::~Attack()
{
	EffectEndAppear( m_pEffect );

	SafeDelete( m_pTimeKey );
	SafeDelete( m_pCaster );
}

//******************************************************************************
//	初期化
//******************************************************************************
void Attack::Init( const AttackData* pData, const Matrix3& Mat )
{
	_ASSERT( pData );

	//	パラメータ
	m_pData			= pData;
	m_FrameCount	= 0;
	m_Matrix		= Mat;

	//	派生先
	_OnInit();

	//	エフェクトを作る
	m_pEffect = _AppearEffect( m_pData->GetParam().EffectName, Mat );

	//	当たり判定を作る
	CollideShape* pCastShape = _CreateCollideShape();
	if( pCastShape )
	{
		m_pCaster = new CollideCaster;
		MEM_CHECK( m_pCaster );

		m_pCaster->Init( pCastShape );
		m_pCaster->SetCalcType( COLLIDE_CALC_POS_NORMAL );
		m_pCaster->SetOwner( COLLIDE_OWNER_ATTACK, this );
		m_pCaster->SetFilter( COLLIDE_FILTER_ATTACK );
		m_pCaster->SetListener( this );
	}

	//	タイムキー
	m_pTimeKey = new AttackTimeKey( this );
	MEM_CHECK( m_pTimeKey );
}

//******************************************************************************
//	更新
//******************************************************************************
void Attack::Update( void )
{
	if( IsDelete() ) return;

	//	派生先
	_OnUpdate();

	//	当たり判定実行
	if( m_pCaster )
	{
		m_pCaster->UpdateMatrix( &m_Matrix );

		COLLIDE_CAST_RESULT Result;
		if( m_pCaster->Cast( Result ) )
		{
			for( COLLIDE_RESULT_LIST::iterator it = Result.ResultList.begin(); it != Result.ResultList.end(); ++it )
			{
				if( !_OnCastHit( *it ) )
				{
					//	判定中断
					break;
				}
			}
		}
	}

	//	タイムキー
	int TimeKeyLoopFrame = m_pData->GetParam().TimeKeyLoopFrame;
	int TimeKeyFrame = TimeKeyLoopFrame <= 0? m_FrameCount : m_FrameCount % TimeKeyLoopFrame;
	m_pTimeKey->Update( TimeKeyFrame );

	//	フレームカウント
	m_FrameCount++;

	//	タイムキークリア
	if( TimeKeyLoopFrame > 0 && m_FrameCount % TimeKeyLoopFrame == 0 )
	{
		m_ApplyKeyList.clear();
		m_pTimeKey->Reset();
	}

	//	終了時間チェック
	if( GetEndFrame() &&
		m_FrameCount >= GetEndFrame() )
	{
		_OnEndFrame();
		Delete();
	}
	
	//	画面外チェック
	float OutRad = _GetOutRadius();
	if( OutRad != 0.0f )
	{
		Rect FieldRect = m_pManager->GetWorld()->GetFieldRect();
		FieldRect.Widen( OutRad );
		if( !FieldRect.IsInside( m_Matrix.GetPos() ) )
		{
			_OnOutField();
			Delete();
		}
	}
}

//******************************************************************************
//	消去
//******************************************************************************
void Attack::Erase( void )
{
	//	アイテム出す
	m_pManager->GetWorld()->GetItemManager()->AppearItem( m_pData->GetParam().ItemNum, GetMatrix().GetPos(), m_pData->GetParam().ItemSpeed );

	//	削除
	Delete();
}

//******************************************************************************
//	マトリクス設定
//******************************************************************************
void Attack::SetMatrix( const Matrix3& Mat )
{
	m_Matrix = Mat;

	//	エフェクト
	if( m_pEffect ) m_pEffect->SetMatrix( Mat );
}

//******************************************************************************
//	終了フレームを取得
//******************************************************************************
int Attack::GetEndFrame( void ) const
{
	return m_pData->GetParam().EndFrame;
}

//******************************************************************************
//	タイムキーの数取得
//******************************************************************************
uint Attack::GetTimeKeyNum( ATTACK_TIME_KEY_TYPE Type ) const
{
	return m_pData->GetTimeKeyNum( Type );
}

//******************************************************************************
//	タイムキーパラメータ取得
//******************************************************************************
const ATTACK_TIME_KEY* Attack::GetTimeKey( ATTACK_TIME_KEY_TYPE Type, int Index ) const
{
	return m_pData->GetTimeKey( Type, Index );
}

//******************************************************************************
//	タイムキーパラメータの適用（単独）
//******************************************************************************
void Attack::ApplyTimeKey( ATTACK_TIME_KEY_TYPE Type, const ATTACK_TIME_KEY_PARAM& Key )
{
	KeyList::iterator it = m_ApplyKeyList.find( &Key );
	if( it != m_ApplyKeyList.end() ) return;

	switch( Type )
	{
	case ATTACK_TIME_KEY_EFFECT:
		_AppearEffect( Label(Key.Appear.Name), Matrix3::RotZ(Key.Appear.Rot) * GetMatrix() );
		break;

	case ATTACK_TIME_KEY_ATTACK:
		_AppearAttack( Label(Key.Appear.Name), Matrix3::RotZ(Key.Appear.Rot) * GetMatrix() );
		break;
		
	case ATTACK_TIME_KEY_COLOR:
		_SetEffectColor( Color(Key.Col) );
		break;
		
	case ATTACK_TIME_KEY_HIT:
		_SetHitEnable( Key.bFlag );
		break;

	case ATTACK_TIME_KEY_RADIUS:
		_SetHitRadius( Key.Val[0] );
		break;

	case ATTACK_TIME_KEY_LENGTH:
		_SetHitLength( Key.Val[0] );
		break;

	default:
		return;
	}
	
	m_ApplyKeyList.insert( &Key );
}

//******************************************************************************
//	タイムキーパラメータの適用（補間）
//******************************************************************************
void Attack::ApplyTimeKey( ATTACK_TIME_KEY_TYPE Type, const ATTACK_TIME_KEY_PARAM& Key, const ATTACK_TIME_KEY_PARAM& Next, float f )
{
	switch( Type )
	{
	case ATTACK_TIME_KEY_COLOR:
		_SetEffectColor( Color::Lerp( Key.Col, Next.Col, f ) );
		break;
		
	case ATTACK_TIME_KEY_RADIUS:
		_SetHitRadius( Lerp( Key.Val[0], Next.Val[0], f ) );
		break;

	case ATTACK_TIME_KEY_LENGTH:
		_SetHitLength( Lerp( Key.Val[0], Next.Val[0], f ) );
		break;
	}
}

//------------------------------------------------------------------------------
//	当たり判定の半径を変更
//------------------------------------------------------------------------------
void Attack::_SetHitRadius( float Rad )
{
	if( m_pCaster )
	{
		CollideShape* pShape = m_pCaster->GetShape();
		if( pShape ) pShape->SetRadius( Rad );
	}
}

//------------------------------------------------------------------------------
//	当たり判定の長さを変更
//------------------------------------------------------------------------------
void Attack::_SetHitLength( float Len )
{
	if( m_pCaster )
	{
		CollideShape* pShape = m_pCaster->GetShape();
		if( pShape ) pShape->SetLength( Len );
	}
}

//------------------------------------------------------------------------------
//	エフェクトの発生
//------------------------------------------------------------------------------
Effect* Attack::_AppearEffect( Label Name )
{
	return _AppearEffect( Name, GetMatrix() );
}

//------------------------------------------------------------------------------
//	エフェクトの発生
//------------------------------------------------------------------------------
Effect* Attack::_AppearEffect( Label Name, const Matrix3& Mat )
{
	if( Name.IsEmpty() ) return NULL;

	const EffectFile* pEffectFile = m_pData->GetEffectFile( Name );
	if( !pEffectFile ) return NULL;
	
	return GetApp()->GetEffectMng()->CreateEffect( pEffectFile, Mat );
}

//------------------------------------------------------------------------------
//	攻撃の発生
//------------------------------------------------------------------------------
void Attack::_AppearAttack( Label Name )
{
	_AppearAttack( Name, GetMatrix() );
}

//------------------------------------------------------------------------------
//	攻撃の発生
//------------------------------------------------------------------------------
void Attack::_AppearAttack( Label Name, const Matrix3& Mat )
{
	if( Name.IsEmpty() ) return;

	const AttackData* pData = m_pData->GetAttackData( Name );
	if( pData )
	{
		m_pManager->AddAttack( pData, Mat );
	}
}

//------------------------------------------------------------------------------
//	エフェクトの色変更
//------------------------------------------------------------------------------
void Attack::_SetEffectColor( Color Col )
{
	if( m_pEffect )
	{
		m_pEffect->SetColor( Col );
	}
}

//------------------------------------------------------------------------------
//	当たり判定の有無を設定
//------------------------------------------------------------------------------
void Attack::_SetHitEnable( bool bEnable )
{
	if( m_pCaster ) m_pCaster->SetHitEnable( bEnable );
}

//------------------------------------------------------------------------------
//	ヒット時の反動方向を得る
//------------------------------------------------------------------------------
Point Attack::_GetHitVel( Point Normal ) const
{
	switch( m_pData->GetParam().HitVelType )
	{
	case ATTACK_VEL_NORMAL:		return -Normal;
	case ATTACK_VEL_DIR:		return m_Matrix.GetAxisY();
	}
	return Point();
}

//------------------------------------------------------------------------------
//	当たり判定をするか
//------------------------------------------------------------------------------
bool Attack::IsHit( const COLLIDE_OWNER& Owner ) const
{
	switch( Owner.Type )
	{
	case COLLIDE_OWNER_PLAYER:
		{
			if( m_pData->GetParam().bNotPlayerHit ) return false;
			
			//	無敵状態
			Player* pPlayer = (Player*)Owner.pData;
			if( pPlayer->IsInvincible() ) return false;
		}
		return true;
	case COLLIDE_OWNER_BLADE:
	case COLLIDE_OWNER_SPEAR:	return !m_pData->GetParam().bNotAttackHit;
	case COLLIDE_OWNER_GROUND:	return !m_pData->GetParam().bNotGroundHit;
	}
	return true;
}