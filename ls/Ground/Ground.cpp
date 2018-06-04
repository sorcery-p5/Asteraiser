#include "stdafx.h"
#include "Ground.h"
#include "GroundManager.h"
#include "GroundData.h"

#include "App/App.h"
#include "World/World.h"

#include "SpriteFrame/SpriteFrame.h"
#include "Collide/CollideObject.h"
#include "Collide/Shape/CollideShapeFrame.h"

#include "Effect/EffectManager.h"
#include "Effect/Effect.h"


////////////////////////////////////////////////////////////////////////////////
//
//	地形
//
////////////////////////////////////////////////////////////////////////////////
Ground::Ground( GroundManager* pManager )
{
	m_pManager	= pManager;

	m_pData			= NULL;
	m_pSprite		= NULL;
	m_pCollide		= NULL;

	m_bDelete		= false;
	m_State			= STATE_MAIN;
	m_FrameCount	= 0;
	m_bEnd			= false;
}

Ground::~Ground()
{
	SafeDelete( m_pSprite );
	SafeDelete( m_pCollide );
}

//******************************************************************************
//	初期化
//******************************************************************************
void Ground::Init( const GroundData* pData, const Matrix3& Mat, Label Name, bool bNoStart )
{
	_ASSERT( pData );

	m_pData			= pData;
	m_Matrix		= Mat;
	m_Pos			= Mat.GetPos();
	m_Rot			= Mat.GetRotZ();
	m_Name			= Name;
	m_FrameCount	= 0;

	//	スプライト
	const SpriteFrameFile* pSpriteFile = m_pData->GetSpriteFrameFile( m_pData->GetParam().SpriteName );
	if( pSpriteFile )
	{
		m_pSprite = new SpriteFrame;
		MEM_CHECK( m_pSprite );

		m_pSprite->Init( pSpriteFile, Mat );
	}

	//	アニメ
	if( !m_pData->GetParam().StartAnimeName.IsEmpty() && !bNoStart )
	{
		SafePtr( m_pSprite )->ChangeAnime( m_pData->GetParam().StartAnimeName );
		m_State = STATE_START;
	}
	else
	{
		SafePtr( m_pSprite )->ChangeAnime( m_pData->GetParam().AnimeName );
		m_State = STATE_MAIN;
	}

	//	当たり判定
	if( m_pData->GetParam().bHitEnable )
	{
		m_pCollide		= new CollideObject;
		MEM_CHECK( m_pCollide );

		m_pCollide->Init( new CollideShapeFrame( m_pSprite ), COLLIDE_LAYER_GROUND );
		m_pCollide->SetOwner( COLLIDE_OWNER_GROUND, this );
		m_pCollide->SetSolveEnable( true );
		m_pCollide->SetListener( this );
	}
}

//******************************************************************************
//	更新
//******************************************************************************
void Ground::Update( void )
{
	//	位置更新
	m_Pos	+= m_pData->GetParam().Speed;
	m_Rot	+= m_pData->GetParam().RotSpeed;
	m_Matrix = Matrix3::TransRotZ( m_Pos, m_Rot );
	
	//	スプライト更新
	SafePtr(m_pSprite)->Update( &m_Matrix );

	//	あたり位置更新
	SafePtr(m_pCollide)->UpdateMatrix( &m_Matrix );

	//	ステートの更新
	_UpdateState();

	//	時間
	m_FrameCount++;
	if( m_pData->GetParam().Time > 0 &&
		m_FrameCount >= m_pData->GetParam().Time )
	{
		Delete();
	}
}

//******************************************************************************
//	描画
//******************************************************************************
void Ground::Draw( void )
{
	SafePtr(m_pSprite)->Draw( GetWorld()->GetDrawOffset() );
}

//******************************************************************************
//	終了させる
//******************************************************************************
void Ground::End( void )
{
	if( !m_pData->GetParam().EndAnimeName.IsEmpty() )
	{
		//	終了アニメ
		PushAnime( m_pData->GetParam().EndAnimeName );
		m_bEnd = true;
	}
	else
	{
		//	削除
		Delete();
	}
}

//******************************************************************************
//	アニメを予約追加する
//******************************************************************************
void Ground::PushAnime( Label Name )
{
	m_PushAnimeList.push_back( Name );
}

//******************************************************************************
//	ヒットエフェクトを生成
//******************************************************************************
void Ground::CreateHitEffect( const Matrix3& Mat ) const
{
	const EffectFile* pEffectFile = m_pData->GetEffectFile( m_pData->GetParam().HitEffectName );
	if( !pEffectFile ) return;
	
	GetApp()->GetEffectMng()->CreateEffect( pEffectFile, Mat );
}

//******************************************************************************
//	当たった攻撃を消去させるか
//******************************************************************************
bool Ground::IsAttackErase( void ) const
{
	return m_pData->GetParam().bAttackErase;
}

//******************************************************************************
//	当たった攻撃側のヒットエフェクトを発生させるか
//******************************************************************************
bool Ground::IsAttackHitEffect( void ) const
{
	return m_pData->GetParam().bAttackHitEffect;
}

//******************************************************************************
//	ワールド取得
//******************************************************************************
World* Ground::GetWorld( void ) const
{
	return m_pManager->GetWorld();
}

//******************************************************************************
//	当たりにヒットするかどうか調べる
//******************************************************************************
bool Ground::IsHit( const COLLIDE_OWNER& Owner ) const
{
	switch( Owner.Type )
	{
	case COLLIDE_OWNER_PLAYER:
		{
			return !m_pData->GetParam().bNotPlayerHit;
		}
		break;

	case COLLIDE_OWNER_ENEMY:
		{
			return !m_pData->GetParam().bNotEnemyHit;
		}
		break;

	case COLLIDE_OWNER_BLADE:
	case COLLIDE_OWNER_SPEAR:
	case COLLIDE_OWNER_ATTACK:
		{
			return !m_pData->GetParam().bNotAttackHit;
		}
		break;
	}

	return true;
}

//------------------------------------------------------------------------------
//	ステートの更新
//------------------------------------------------------------------------------
void Ground::_UpdateState( void )
{
	switch( m_State )
	{
	//	開始
	case STATE_START:
		if( m_pSprite )
		{
			//	アニメ終了で通常状態に
			if( m_pSprite->IsAnimeLast() )
			{
				SafePtr( m_pSprite )->ChangeAnime( m_pData->GetParam().AnimeName );
				m_State = STATE_MAIN;
			}
		}
		break;

	//	通常
	case STATE_MAIN:
		if( m_pSprite )
		{
			if( !m_PushAnimeList.empty() )
			{
				//	アニメが終了したら、予約アニメを流す
				if( m_pSprite->IsAnimeLast() )
				{
					SafePtr( m_pSprite )->ChangeAnime( m_PushAnimeList.front() );

					m_PushAnimeList.pop_front();

					if( m_bEnd )
					{
						m_State = STATE_END;
					}
				}
			}
		}
		break;

	//	終了
	case STATE_END:
		if( m_pSprite )
		{
			//	アニメ終了で終了
			if( m_pSprite->IsAnimeLast() )
			{
				SafePtr( m_pSprite )->ChangeAnime( m_pData->GetParam().EndAnimeName );
				Delete();
			}
		}
		break;
	}
}