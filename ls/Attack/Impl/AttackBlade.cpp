#include "stdafx.h"
#include "AttackBlade.h"

#include "../AttackData.h"
#include "../AttackManager.h"

#include "World/World.h"

#include "Collide/Shape/CollideShapePoint.h"
#include "Collide/Shape/CollideShapeLine.h"

#include "Player/Player.h"
#include "Player/Equip/Blade.h"
#include "Player/Equip/Spear.h"
#include "Player/Equip/Ruler.h"
#include "Score/ScoreManager.h"
#include "Enemy/Enemy.h"
#include "Ground/Ground.h"


////////////////////////////////////////////////////////////////////////////////
//
//	敵の攻撃：ブレード
//
////////////////////////////////////////////////////////////////////////////////
AttackBlade::AttackBlade( AttackManager* pManager ) : Attack( pManager )
{
	m_pParam	= NULL;
	m_Energy	= 0.0f;
}
AttackBlade::~AttackBlade()
{
	
}

//******************************************************************************
//	オーナーの登録
//******************************************************************************
void AttackBlade::SetOwner( Enemy* pEnemy )
{
	m_pEnemy = pEnemy;
}

//******************************************************************************
//	通知
//******************************************************************************
void AttackBlade::OnNotify( ATTACK_NOTIFY Notify )
{
	switch( Notify )
	{
	case ATTACK_NOTIFY_DAMAGE:		_Crash();	return;
	}
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
void AttackBlade::_OnInit( void )
{
	//	パラメータ
	m_pParam	= &m_pData->GetParam().Blade;
	
	m_Energy	= m_pParam->Energy;
}

//------------------------------------------------------------------------------
//	更新
//------------------------------------------------------------------------------
void AttackBlade::_OnUpdate( void )
{
}

//------------------------------------------------------------------------------
//	当たり形状作成
//------------------------------------------------------------------------------
CollideShape* AttackBlade::_CreateCollideShape( void ) const
{
	return new CollideShapeLine( m_pParam->Length, m_pParam->Radius );
}

//------------------------------------------------------------------------------
//	当たりキャストに当たった
//------------------------------------------------------------------------------
bool AttackBlade::_OnCastHit( const COLLIDE_RESULT& Result )
{
	enum RESULT
	{
		RESULT_NONE,
		RESULT_HIT,
		RESULT_CRASH,
	};

	RESULT Ret = RESULT_NONE;
	Label HitEffect( m_pData->GetParam().HitEffectName );

	switch( Result.pOwner->Type )
	{
	case COLLIDE_OWNER_PLAYER:
		{
			Player* pPlayer = (Player*)Result.pOwner->pData;
			if( pPlayer->Damage( m_pData->GetParam().Damage, _GetHitVel(Result.Normal) * m_pData->GetParam().HitVel ) )
			{
				Ret = RESULT_HIT;
			}
		}
		break;
	case COLLIDE_OWNER_BLADE:
		{
			Blade* pBlade = (Blade*)Result.pOwner->pData;

			//	反発
			if( !pBlade->IsSmash() && IsRepulse() )
			{
				if( pBlade->OnHitAttack( m_pParam->RepulseExhaust, IsRepulse(), Result.HitPos ) )
				{
					//	ダメージ
					DecreaseZero( m_Energy, pBlade->GetRepulseAttackPower() );
				
					Ret = m_Energy <= 0.0f? RESULT_CRASH : RESULT_HIT;

					//	反発エフェクト設定
					if( !pBlade->IsCrash() ) HitEffect = m_pParam->RepulseEffectName;

					//	反発の通知
					if( m_pEnemy && m_Energy > 0.0f &&
						!m_pData->GetParam().bNotNotify )
					{
						m_pEnemy->OnAttackNotify( ENEMY_ATTACK_NOTIFY_REPULSE );
					}
				}
			}
			//	通常
			else
			{
				if( pBlade->OnHitAttack( m_pData->GetParam().Exhaust, IsRepulse(), Result.HitPos ) )
				{
					if( m_pParam->bSmash )
					{
						//	相手の剣がスマッシュなら折れる・それ以外は無事
						Ret = pBlade->IsSmash()? RESULT_CRASH : RESULT_HIT;
					}
					else
					{
						//	相手の剣が折れていればヒット・無事なら折れる
						Ret = pBlade->IsCrash()? RESULT_HIT : RESULT_CRASH;
					}
				}
			}
		}
		break;
	case COLLIDE_OWNER_SPEAR:
		{
			Spear* pSpear = (Spear*)Result.pOwner->pData;
			if( pSpear->OnHitAttack( m_pData->GetParam().Exhaust, Result.HitPos ) )
			{
				//	スピアが折れていればヒット・無事なら折れる
				Ret = pSpear->IsDelete()? RESULT_HIT : RESULT_CRASH;
			}
		}
		break;
	case COLLIDE_OWNER_RULER:
		{
			Ret = RESULT_CRASH;
		}
		break;
	case COLLIDE_OWNER_GROUND:
		{
			Ground* pGround = (Ground*)Result.pOwner->pData;

			//	ヒットエフェクト出すかどうか
			if( pGround->IsAttackHitEffect() )
			{
				Ret = RESULT_HIT;
			}

			//	地形側のエフェクト
			pGround->CreateHitEffect( Result.GetHitMatrix() );
		}
		break;
	}

	//	命中
	if( Ret == RESULT_HIT )
	{
		//	ヒットエフェクト出す
		if( !HitEffect.IsEmpty() )
		{	
			Matrix3 Mat = GetMatrix();
			Mat.SetPos( Result.HitPos );

			_AppearEffect( HitEffect, Mat );
		}
	}

	//	折れた
	else if( Ret == RESULT_CRASH )
	{
		_Crash();
	}

	return true;
}

//------------------------------------------------------------------------------
//	破損
//------------------------------------------------------------------------------
void AttackBlade::_Crash( void )
{
	//	通知
	if( m_pEnemy && !m_pData->GetParam().bNotNotify )
	{
		m_pEnemy->OnAttackNotify( ENEMY_ATTACK_NOTIFY_CRASH );
	}
	
	//	スコア通知
	m_pManager->GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_BLADE_ERASE, m_pData->GetParam().ScoreRate );

	//	破損エフェクト出す
	Label CrashEffect( m_pParam->CrashEffectName );
	if( !CrashEffect.IsEmpty() )
	{
		_AppearEffect( CrashEffect );
	}

	Erase();
}