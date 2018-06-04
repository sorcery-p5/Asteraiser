#include "stdafx.h"
#include "AttackShield.h"

#include "../AttackData.h"

#include "Collide/Shape/CollideShapePoint.h"
#include "Collide/Shape/CollideShapeLine.h"

#include "Player/Player.h"
#include "Player/Equip/Blade.h"
#include "Player/Equip/Spear.h"
#include "Enemy/Enemy.h"


////////////////////////////////////////////////////////////////////////////////
//
//	敵の攻撃：シールド
//
////////////////////////////////////////////////////////////////////////////////
AttackShield::AttackShield( AttackManager* pManager ) : Attack( pManager )
{
	m_pParam	= NULL;
	m_Energy	= 0.0f;
}
AttackShield::~AttackShield()
{
	
}

//******************************************************************************
//	オーナーの登録
//******************************************************************************
void AttackShield::SetOwner( Enemy* pEnemy )
{
	m_pEnemy = pEnemy;
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
void AttackShield::_OnInit( void )
{
	//	パラメータ
	m_pParam	= &m_pData->GetParam().Shield;
	m_Energy	= m_pParam->Energy;
}

//------------------------------------------------------------------------------
//	更新
//------------------------------------------------------------------------------
void AttackShield::_OnUpdate( void )
{
}

//------------------------------------------------------------------------------
//	当たり形状作成
//------------------------------------------------------------------------------
CollideShape* AttackShield::_CreateCollideShape( void ) const
{
	return new CollideShapePoint( m_pParam->Radius );
}

//------------------------------------------------------------------------------
//	当たりキャストに当たった
//------------------------------------------------------------------------------
bool AttackShield::_OnCastHit( const COLLIDE_RESULT& Result )
{
	enum RESULT
	{
		RESULT_NONE,
		RESULT_HIT,
		RESULT_BREAK,
	};

	RESULT Ret = RESULT_NONE;

	switch( Result.pOwner->Type )
	{
	case COLLIDE_OWNER_PLAYER:
		{
			//	自機
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

			//	スマッシュの場合
			if( pBlade->IsSmash() )
			{
				if( pBlade->OnHitAttack( m_pData->GetParam().Exhaust, false, Result.HitPos ) )
				{
					Ret = RESULT_BREAK;
				}
			}
			//	通常の場合
			else
			{
				if( pBlade->OnHitAttack( m_pData->GetParam().Exhaust, IsRepulse(), Result.HitPos ) )
				{
					//	ダメージ
					DecreaseZero( m_Energy, pBlade->GetRepulseAttackPower() * m_pParam->Defense );
					
					Ret = m_Energy <= 0.0f? RESULT_BREAK : RESULT_HIT;

					//	反発の通知
					if( m_pEnemy && m_Energy > 0.0f &&
						!m_pData->GetParam().bNotNotify )
					{
						m_pEnemy->OnAttackNotify( ENEMY_ATTACK_NOTIFY_REPULSE );
					}
				}
			}
		}
		break;
	case COLLIDE_OWNER_SPEAR:
		{
			//	スピア
			Spear* pSpear = (Spear*)Result.pOwner->pData;
			if( pSpear->OnHitAttack( m_pData->GetParam().Exhaust, Result.HitPos ) )
			{
				//	ダメージ
				DecreaseZero( m_Energy, pSpear->GetAttackPower() * m_pParam->Defense );
				Ret = m_Energy <= 0.0f? RESULT_BREAK : RESULT_HIT;
			}
		}
		break;
	case COLLIDE_OWNER_RULER:
		{
			//	ルーラー
			Ret = RESULT_BREAK;
		}
		break;
	}


	//	命中
	if( Ret == RESULT_HIT )
	{
		//	ヒットエフェクト出す
		Label HitEffect( m_pData->GetParam().HitEffectName );
		if( !HitEffect.IsEmpty() )
		{
			_AppearEffect( HitEffect, Result.GetHitMatrix() );
		}
	}

	//	ブレイク
	else if( Ret == RESULT_BREAK )
	{
		//	ブレイク通知
		if( m_pEnemy && !m_pData->GetParam().bNotNotify )
		{
			m_pEnemy->OnAttackNotify( ENEMY_ATTACK_NOTIFY_BREAK );
		}
		
		//	ブレイクエフェクト出す
		Label BreakEffect( m_pParam->BreakEffectName );
		if( !BreakEffect.IsEmpty() )
		{
			_AppearEffect( BreakEffect );
		}
		
		Erase();
	}

	return true;
}
