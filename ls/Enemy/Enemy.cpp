#include "stdafx.h"
#include "Enemy.h"
#include "EnemyData.h"
#include "EnemyManager.h"
#include "EnemyIndicate.h"

#include "State/EnemyState.h"

#include "App/App.h"
#include "World/World.h"
#include "Config/Config.h"

#include "SpriteFrame/SpriteFrame.h"
#include "Effect/EffectManager.h"
#include "Effect/Effect.h"
#include "Effect/EffectHolder.h"
#include "Collide/CollideSystem.h"
#include "Collide/Shape/CollideShape.h"
#include "Collide/Shape/CollideShapePoint.h"

#include "Attack/Attack.h"
#include "Player/Player.h"
#include "Item/ItemManager.h"
#include "Score/ScoreManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	敵
//
////////////////////////////////////////////////////////////////////////////////
Enemy::Enemy( EnemyManager* pManager )
{
	_ASSERT( pManager );
	m_pManager	= pManager;

	m_pData				= NULL;
	m_pSprite			= NULL;
	m_pState			= NULL;
	m_pCollide			= NULL;
	m_pCaster			= NULL;
	m_pEffectHolder		= NULL;

	m_bDelete			= false;
	m_Life				= 0;
	m_Defense			= 1.0f;
	m_KnockbackCount	= 0;
	m_InvincibleCount	= 0;
	m_HideCount			= 0;
	m_bInvincible		= false;
	m_bHide				= false;
	m_bNoDamage			= false;
	m_bOutField			= false;
	m_bNoCount			= false;
	m_bRepulse			= false;

	m_bDamage			= false;
	m_bRepulseDamage	= false;
	m_bRulerHit			= false;
	m_bPause			= false;
	m_ComboNum			= 0;

	m_pAttachMatrix		= NULL;
}
Enemy::~Enemy()
{
	SafeDelete( m_pAttachMatrix );
	SafeDelete( m_pEffectHolder );
	SafeDelete( m_pState );
	SafeDelete( m_pCollide );
	SafeDelete( m_pCaster );
	SafeDelete( m_pSprite );
}

//******************************************************************************
//	初期化
//******************************************************************************
void Enemy::Init( const EnemyData* pData, const Matrix3& Mat, Label Name )
{
	_ASSERT( pData );

	m_pData		= pData;
	m_Name		= Name;
	m_Matrix	= Mat;
	m_Pos		= Mat.GetPos();
	m_Rot		= Mat.GetRotZ();
	m_Life		= m_pData->GetParam().Life;
	m_Defense	= m_pData->GetParam().Defense;
	m_bOutField	= m_pData->GetParam().bOutField;
	m_bNoCount	= m_pData->GetParam().bNoCount;
	m_bRepulse	= m_pData->GetParam().bRepulse;

	//	スプライト作成
	m_pSprite = new SpriteFrame;
	MEM_CHECK( m_pSprite );
	m_pSprite->Init( m_pData->GetSpriteFrameFile( m_pData->GetParam().SpriteName ), Mat );

	//	ステート作成
	m_pState = new EnemyState( this );
	MEM_CHECK( m_pState );
	m_pState->Init();

	//	当たり判定生成
	m_pCollide		= new CollideObject;
	m_pCaster		= new CollideCaster;
	MEM_CHECK( m_pCollide && m_pCaster );

	CollideShape* pShape = NULL;
	if( m_pData->GetParam().bCollideFrame )	pShape = new CollideShapeFrame( m_pSprite );
	else									pShape = new CollideShapePoint( m_pData->GetParam().CollideRadius );

	m_pCollide->Init( pShape, COLLIDE_LAYER_ENEMY );
	m_pCollide->SetOwner( COLLIDE_OWNER_ENEMY, this );
	m_pCollide->SetHitEnable( m_pData->GetParam().bHitEnable );
	m_pCollide->SetSolveEnable( true );
	m_pCollide->SetListener( this );

	m_pCaster->Init( new CollideShapePoint( m_pData->GetParam().CollideRadius ) );
	m_pCaster->SetOwner( COLLIDE_OWNER_ENEMY, this );
	m_pCaster->SetFilter( COLLIDE_FILTER_ENEMY );
	m_pCaster->SetCalcType( COLLIDE_CALC_ALL );
	m_pCaster->SetHitEnable( m_pData->GetParam().bHitEnable );

	//	エフェクト管理
	m_pEffectHolder = new EffectHolder;
	MEM_CHECK( m_pEffectHolder );
}

//******************************************************************************
//	更新
//******************************************************************************
void Enemy::Update( void )
{
	if( IsDelete() ) return;

	//	追従の更新
	_UpdateAttachMatrix();

	if( !m_bRulerHit )
	{
		//	ステート更新
		m_pState->Update();

		//	位置
		m_Pos += m_Vel + m_MoveVel;

		//	画面内に納める
		if( !IsOutField() )
		{
			Rect FieldRect = GetWorld()->GetFieldRect();
			m_Pos = FieldRect.Inside( m_Pos );
		}

		//	速度の調整
		m_Vel *= m_pData->GetParam().Decel;
		m_MoveVel.Zero();

		//	マトリクスを更新
		m_Matrix = Matrix3::TransRotZ( m_Pos, m_Rot );
			
		//	通知をクリア
		m_vNotify.clear();
	}
	else
	{
		m_bRulerHit = false;
	}

	//	当たり判定
	_CastCollide();

	//	ヒットエフェクトの更新
	m_pEffectHolder->SetPause( m_bRulerHit );
	m_pEffectHolder->Update( NULL );

	//	スプライト更新
	m_pSprite->Pause( m_bPause || m_bRulerHit );
	m_pSprite->Update( &m_Matrix );

	//	あたり位置更新
	m_pCollide->UpdateMatrix( &m_Matrix );

	//	攻撃の更新
	_UpdateLinkAttack();

	//	ダメージフラグをクリア
	m_bDamage			= false;
	m_bRepulseDamage	= false;

	DecreaseZero( m_InvincibleCount, 1 );
	DecreaseZero( m_HideCount, 1 );

	//	死亡チェック
	if( m_Life <= 0 )
	{
		//	ルーラー中は死なない
		if( !m_bRulerHit )
		{
			//	当たり判定消失
			if( m_pCollide ) m_pCollide->SetHitEnable( false );

			//	ステートに通知
			m_pState->ChangeHandlerState( ENEMY_HANDLER_STATE_DEAD );
		}

		//	色の設定
		SafePtr( m_pSprite )->SetColor( m_pData->GetParam().DeadColor );
	}

	if( !m_bRulerHit )
	{
		//	カウントダウン
		DecreaseZero( m_KnockbackCount, 1 );

		//	コンボカウントをリセット
		if( !IsKnockback() )
		{
			m_ComboNum = 0;
		}
	}
}

//******************************************************************************
//	描画
//******************************************************************************
void Enemy::Draw( void )
{
	m_pSprite->Draw( GetWorld()->GetDrawOffset() );
}

//******************************************************************************
//	ダメージを与える
//******************************************************************************
float Enemy::Damage( float Damage, Point Vel, int Knockback, bool bRepulse, bool bAntiHide )
{
	//	無敵
	if( IsInvincible() )
	{
		return -1.0f;
	}

	//	ダメージ無効
	if( IsNoDamage() )
	{
		return -1.0f;
	}

	//	隠れ属性
	if( !bAntiHide && IsHide() )
	{
		return -1.0f;
	}

	//	インジケート表示
	if( !m_pData->GetParam().bNoIndicate )
	{
		m_pManager->GetIndicate()->AddEnemy( this );
	}

	float Exhaust = Min( (float)m_Life, Damage );

	//	超ダメージ
	if( GetConfig()->GetDebugInfo().bPlayerHyperAttack )
	{
		Damage *= 100.0f;
	}

	//	防御力による軽減
	Damage *= m_Defense;

	//	ダメージ受ける
	DecreaseZero( m_Life, (int)Damage );

	//	死亡
	if( m_Life <= 0 && !m_bRulerHit )
	{
		m_Vel += Vel * m_pData->GetParam().ReduceVel;
	
		m_InvincibleCount	= 0;
		m_HideCount			= 0;
		m_KnockbackCount	= 0;
		m_ComboNum			= 0;
	
		//	当たり判定消失
		if( m_pCollide ) m_pCollide->SetHitEnable( false );

		//	ステートに通知
		m_pState->ChangeHandlerState( ENEMY_HANDLER_STATE_DEAD );
	}
	//	反発
	else if( IsRepulse() && bRepulse )
	{
		m_Vel.Zero();

		m_InvincibleCount	= 0;
		m_HideCount			= 0;
		m_KnockbackCount	= 0;
		m_ComboNum			= 0;
		m_bRepulseDamage	= true;

		//	ステートに通知
		if( !m_pData->GetParam().bNotRepulseNotify )
		{
			m_pState->ChangeHandlerState( ENEMY_HANDLER_STATE_REPULSE );
		}
	}
	//	通常ダメージ
	else
	{
		m_Vel += Vel * m_pData->GetParam().ReduceVel;
		
		int	Invincible		= m_pData->GetParam().DamageInvincible;

		//	ルーラー中は無敵時間軽減
		if( m_bRulerHit ) Invincible *= 0.4f;

		m_InvincibleCount	= Max( m_InvincibleCount, Invincible );
		m_KnockbackCount	= Max( m_KnockbackCount, (int)(Knockback * m_pData->GetParam().ReduceKnockback) );

		m_ComboNum++;
		
		//	ステートに通知
		m_pState->ChangeHandlerState( ENEMY_HANDLER_STATE_DAMAGE );

		//	攻撃に通知
		_NotifyLinkAttack( ATTACK_NOTIFY_DAMAGE );
	}

	//	消耗レート掛ける
	Exhaust *= m_pData->GetParam().ExhaustRate;

	return Exhaust;
}

//******************************************************************************
//	攻撃を登録
//******************************************************************************
void Enemy::RegisterAttack( Attack* pAttack, Label Name, pstr pBone, bool bAttach )
{
	if( !pAttack || !pBone ) return;

	//	敵に登録
	pAttack->SetOwner( this );

	LINK_ATTACK LinkAttack;
	LinkAttack.pAttack	= pAttack;
	LinkAttack.Name		= Name;
	LinkAttack.Bone		= pBone;
	LinkAttack.bAttach	= bAttach;

	m_listLinkAttack.push_back( LinkAttack );
}

//******************************************************************************
//	登録された攻撃を削除
//******************************************************************************
void Enemy::DeleteLinkAttack( Label Name )
{
	for( LinkAttackList::iterator it = m_listLinkAttack.begin(); it != m_listLinkAttack.end(); ++it )
	{
		if( it->pAttack &&
			(Name.IsEmpty() || it->Name == Name ) )
		{
			it->pAttack->Delete();
		}
	}
}

//******************************************************************************
//	攻撃からの通知
//******************************************************************************
void Enemy::OnAttackNotify( Label Notify )
{
	//	シールド破損
	if( Notify == ENEMY_ATTACK_NOTIFY_BREAK )
	{
		m_HideCount	= m_pData->GetParam().DamageInvincible;

		m_pState->ChangeHandlerState( ENEMY_HANDLER_STATE_BREAK );
		
		//	インジケート
		if( !m_pData->GetParam().bNoIndicate )
		{
			m_pManager->GetIndicate()->AddEnemy( this );
		}
	}
	//	ブレード折れた
	else if( Notify == ENEMY_ATTACK_NOTIFY_CRASH )
	{
		m_HideCount			= m_pData->GetParam().DamageInvincible;
		m_KnockbackCount	= Max( m_KnockbackCount, (int)(m_pData->GetParam().CrashKnockback * m_pData->GetParam().ReduceKnockback) );
		
		//	後退
		m_Vel += -m_Matrix.GetAxisY() * m_pData->GetParam().CrashVel;

		m_pState->ChangeHandlerState( ENEMY_HANDLER_STATE_CRASH );

		//	インジケート
		if( !m_pData->GetParam().bNoIndicate )
		{
			m_pManager->GetIndicate()->AddEnemy( this );
		}
	}
	//	反発
	else if( Notify == ENEMY_ATTACK_NOTIFY_REPULSE )
	{
		m_pState->ChangeHandlerState( ENEMY_HANDLER_STATE_REPULSE );

		m_bRepulseDamage = true;
		
		//	インジケート
		if( !m_pData->GetParam().bNoIndicate )
		{
			m_pManager->GetIndicate()->AddEnemy( this );
		}
	}

	m_vNotify.push_back( Notify );
}

//******************************************************************************
//	通知
//******************************************************************************
void Enemy::OnNotify( Label Notify )
{
	m_vNotify.push_back( Notify );

	//	ハンドラー通知
	m_pState->ChangeHandlerState( Notify );
}

//******************************************************************************
//	追従対象を登録
//******************************************************************************
void Enemy::SetAttach( Enemy* pTarget, Label Bone )
{
	if( !pTarget ) return;

	SafeDelete( m_pAttachMatrix );

	m_pAttachMatrix = new ATTACH_MATRIX;
	m_pAttachMatrix->pTarget	= pTarget;
	m_pAttachMatrix->Bone		= Bone;
	m_pAttachMatrix->PrevMat	= pTarget->GetBoneMatrix( Bone );
}

//******************************************************************************
//	子に相当する敵を生成する
//******************************************************************************
void Enemy::AddChildEnemy( Label Name, Label BoneName, const Matrix3& Mat, bool bAttach, Label AppearName )
{
	//	敵を生成
	const EnemyData*	pEnemyData	= m_pData->GetEnemyData( Name );
	Matrix3				BaseMat		= Mat * GetBoneMatrix( BoneName );
	
	Enemy* pChildEnemy = m_pManager->AddEnemy( pEnemyData, BaseMat, AppearName );

	//	親子登録
	m_vChild.push_back( pChildEnemy );
	pChildEnemy->SetParent( this );

	//	追従設定
	if( bAttach )
	{
		pChildEnemy->SetAttach( this, BoneName );
	}
}

//******************************************************************************
//	親へ通知
//******************************************************************************
void Enemy::NotifyParent( Label Name )
{
	SafePtr( m_pParent )->OnNotify( Name );
}

//******************************************************************************
//	子へ通知
//******************************************************************************
void Enemy::NotifyChild( Label AppearName, Label Name )
{
	for( EnemyVec::iterator it = m_vChild.begin(); it != m_vChild.end(); ++it )
	{
		if( !(*it) ) continue;

		if( AppearName.IsEmpty() ||
			(*it)->GetName() == AppearName )
		{
			(*it)->OnNotify( Name );
		}
	}
}

//******************************************************************************
//	敵が死亡済みかどうか
//******************************************************************************
bool Enemy::IsChildDefeat( Label Name ) const
{
	for( EnemyVec::const_iterator it = m_vChild.begin(); it != m_vChild.end(); ++it )
	{
		if( !(*it) ) continue;

		if( (*it)->GetName() == Name &&
			((*it)->IsDead() || (*it)->IsNoCount() ) )
		{
			return true;
		}
	}

	return false;
}

//******************************************************************************
//	すべての敵が死亡済みかどうか
//******************************************************************************
bool Enemy::IsChildDefeatAll( void ) const
{
	for( EnemyVec::const_iterator it = m_vChild.begin(); it != m_vChild.end(); ++it )
	{
		if( !(*it) ) continue;

		if( !(*it)->IsNoCount() && !(*it)->IsDead() )
		{
			return false;
		}
	}
	return true;
}

//******************************************************************************
//	すべての敵が居ないかどうか
//******************************************************************************
bool Enemy::IsChildEmpty( void ) const
{
	for( EnemyVec::const_iterator it = m_vChild.begin(); it != m_vChild.end(); ++it )
	{
		if( !(*it)->IsNoCount() )
		{
			return false;
		}
	}
	return true;
}

//******************************************************************************
//	通知があるかどうか
//******************************************************************************
bool Enemy::IsNotify( Label Notify ) const
{
	NotifyVec::const_iterator it = StdFind( m_vNotify, Notify );
	return it != m_vNotify.end();
}

//******************************************************************************
//	エフェクト作成
//******************************************************************************
Effect* Enemy::CreateEffect( Label Name )
{
	if( Name.IsEmpty() ) return NULL;

	const EffectFile* pEffectFile = m_pData->GetEffectFile( Name );
	if( !pEffectFile ) return NULL;
	
	return GetApp()->GetEffectMng()->CreateEffect( pEffectFile, m_Matrix );
}

//******************************************************************************
//	ヒットエフェクト追加
//******************************************************************************
void Enemy::AddHitEffect( Effect* pEffect )
{
	m_pEffectHolder->AddEffect( pEffect, false );
}

//******************************************************************************
//	サウンドを再生
//******************************************************************************
Sound* Enemy::PlaySound( Label Name, float Vol )
{
	const SoundFile* pFile = m_pData->GetSoundFile( Name );
	if( !pFile ) return NULL;

	Sound* pSound = GetTone()->PlaySound( pFile, SOUND_CHANNEL_SE, false, Vol );
	if( !pSound ) return NULL;

	return pSound;
}

//******************************************************************************
//	破壊
//******************************************************************************
void Enemy::Destroy( void )
{
	//	アイテム発生
	GetWorld()->GetItemManager()->AppearItem( m_pData->GetParam().ItemNum, m_Matrix.GetPos(), m_pData->GetParam().ItemSpeed );
	
	//	スコア通知
	GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_ENEMY_DESTROY, GetScoreRate() );

	//	削除
	Delete();	
}

//******************************************************************************
//	逃走
//******************************************************************************
void Enemy::Escape( void )
{
	//	スコア通知
	if( !IsNoCount() )
	{
		GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_ENEMY_ESCAPE );
	}
	
	//	削除
	Delete();
}

//******************************************************************************
//	耐久力の増減
//******************************************************************************
void Enemy::AddLife( int Value )
{
	m_Life = Clamp( m_Life + Value, 0, GetMaxLife() );

	//	インジケート表示
	if( !m_pData->GetParam().bNoIndicate )
	{
		m_pManager->GetIndicate()->AddEnemy( this );
	}
}

//******************************************************************************
//	当たり判定の有効設定
//******************************************************************************
void Enemy::SetHitEnable( bool bEnable )
{
	if( m_pCollide ) m_pCollide->SetHitEnable( bEnable );
	if( m_pCaster ) m_pCaster->SetHitEnable( bEnable );
}

//******************************************************************************
//	地形との当たり判定の有効設定
//******************************************************************************
void Enemy::SetGroundHitEnable( bool bEnable )
{	
	if( m_pCaster ) m_pCaster->SetHitEnable( bEnable );
}

//******************************************************************************
//	ボーンのマトリクスを取得
//******************************************************************************
const Matrix3& Enemy::GetBoneMatrix( Label BoneName ) const
{
	if( m_pSprite )
	{
		const Matrix3* pMat = m_pSprite->GetBoneMatrix( BoneName );
		if( pMat )
		{
			return *pMat;
		}
	}
	return GetMatrix();
}

//******************************************************************************
//	最大ライフ取得
//******************************************************************************
int Enemy::GetMaxLife( void ) const
{
	return m_pData->GetParam().Life;
}

//******************************************************************************
//	表示用の名前を取得
//******************************************************************************
pstr Enemy::GetPreviewName( void ) const
{
	return m_pData->GetParam().PreviewName.c_str();
}

//******************************************************************************
//	当たり判定が有効か
//******************************************************************************
bool Enemy::IsHitEnable( void ) const
{
	if( m_pCollide && m_pCollide->IsHitEnable() ) return true;
	if( m_pCaster && m_pCaster->IsHitEnable() ) return true;

	return false;
}

//******************************************************************************
//	無敵かどうか
//******************************************************************************
bool Enemy::IsInvincible( void ) const
{
	if( m_bInvincible || m_InvincibleCount > 0 ) return true;

	return false;
}

//******************************************************************************
//	隠れ属性かどうか
//******************************************************************************
bool Enemy::IsHide( void ) const
{
	if( m_bHide || m_HideCount > 0 ) return true;

	for( LinkAttackList::const_iterator it = m_listLinkAttack.begin(); it != m_listLinkAttack.end(); ++it )
	{
		//	所持する攻撃から隠れ属性を探す
		if( it->pAttack )
		{
			if( it->pAttack->IsHide() ) return true;
		}
	}

	return false;
}

//******************************************************************************
//	反発するかどうか
//******************************************************************************
bool Enemy::IsRepulse( void ) const
{
	return m_bRepulse && !m_bRulerHit;
}

//******************************************************************************
//	ボスかどうか
//******************************************************************************
bool Enemy::IsBoss( void ) const
{
	return m_pData->GetParam().bBoss;
}

//******************************************************************************
//	体当たりによる攻撃力を得る
//******************************************************************************
float Enemy::GetBodyDamage( void ) const
{
	return m_pData->GetParam().BodyDamage;
}

//******************************************************************************
//	体当たりによる反動を得る
//******************************************************************************
float Enemy::GetBodyDamageVel( void ) const
{
	return m_pData->GetParam().BodyDamageVel;
}

//******************************************************************************
//	スコア倍率を得る
//******************************************************************************
float Enemy::GetScoreRate( void ) const
{
	return m_pData->GetParam().ScoreRate;
}

//******************************************************************************
//	移動目標位置を取得
//******************************************************************************
const Point* Enemy::GetMoveTarget( void ) const
{
	if( !m_pState ) return NULL;

	return m_pState->GetMoveTarget();
}

//******************************************************************************
//	旋回目標位置を取得
//******************************************************************************
const Point* Enemy::GetRotTarget( void ) const
{
	if( !m_pState ) return NULL;

	return m_pState->GetRotTarget();
}

//******************************************************************************
//	ワールド取得
//******************************************************************************
World* Enemy::GetWorld( void ) const
{
	return m_pManager->GetWorld();
}

//******************************************************************************
//	乱数発生の取得
//******************************************************************************
Rand& Enemy::GetRand( void ) const
{
	return m_pManager->GetRand();
}

//******************************************************************************
//	当たりにヒットするかどうか調べる
//******************************************************************************
bool Enemy::IsHit( const COLLIDE_OWNER& Owner ) const
{
	switch( Owner.Type )
	{
	case COLLIDE_OWNER_PLAYER:
		{
			Player* pPlayer = (Player*)(Owner.pData);
			if( pPlayer->IsInvincible() )			return false;
			if( IsInvincible() )					return false;
			if( IsHide() )							return false;
			if( !m_pData->GetParam().bPlayerHit )	return false;
		}
		break;
	}

	return true;
}

//------------------------------------------------------------------------------
//	当たり判定のキャストを行う
//------------------------------------------------------------------------------
void Enemy::_CastCollide( void )
{
	m_pCaster->UpdateMatrix( &m_Matrix );

	COLLIDE_CAST_RESULT Result;
	if( m_pCaster->Cast( Result ) )
	{
		for( COLLIDE_RESULT_LIST::iterator it = Result.ResultList.begin(); it != Result.ResultList.end(); ++it )
		{
			switch( it->pOwner->Type )
			{
			case COLLIDE_OWNER_RULER:
				{
					if( !IsNoDamage() )
					{
						m_bRulerHit = true;
					}
				}
				break;
			}
		}

		//	地形当たりの矯正
		if( m_pData->GetParam().bGroundHit )
		{
			m_Matrix.SetPos( Result.SolvePos );
		}
	}
}

//------------------------------------------------------------------------------
//	位置追従の更新
//------------------------------------------------------------------------------
void Enemy::_UpdateAttachMatrix( void )
{
	if( !m_pAttachMatrix ) return;
	
	//	対象が消滅
	if( !m_pAttachMatrix->pTarget )
	{
		SafeDelete( m_pAttachMatrix );
		return;
	}

	//	差分を取る
	const Matrix3& TargetMat = m_pAttachMatrix->pTarget->GetBoneMatrix( m_pAttachMatrix->Bone );
	Point RelPos = TargetMat.GetPos() - m_pAttachMatrix->PrevMat.GetPos();
	float RelRot = TargetMat.GetRotZ() - m_pAttachMatrix->PrevMat.GetRotZ();

	//	位置を更新
	m_Pos	+= RelPos;
	m_Rot	+= RelRot;
	
	//	行列更新
	m_Matrix = Matrix3::TransRotZ( m_Pos, m_Rot );
	m_pAttachMatrix->PrevMat = TargetMat;
}

//------------------------------------------------------------------------------
//	攻撃の更新
//------------------------------------------------------------------------------
void Enemy::_UpdateLinkAttack( void )
{
	for( LinkAttackList::iterator it = m_listLinkAttack.begin(); it != m_listLinkAttack.end(); )
	{
		if( it->pAttack )
		{
			if( it->bAttach )
			{
				//	位置取得
				it->pAttack->SetMatrix( GetBoneMatrix( it->Bone ) );
			}

			++it;
		}
		else
		{
			//	削除
			it = m_listLinkAttack.erase( it );
		}
	}
}

//------------------------------------------------------------------------------
//	攻撃に通知
//------------------------------------------------------------------------------
void Enemy::_NotifyLinkAttack( ATTACK_NOTIFY Notify )
{
	for( LinkAttackList::iterator it = m_listLinkAttack.begin(); it != m_listLinkAttack.end(); ++it )
	{
		if( it->pAttack )
		{
			it->pAttack->OnNotify( Notify );
		}
	}
}