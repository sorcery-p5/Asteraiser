#include "stdafx.h"
#include "AttackLaser.h"

#include "../AttackData.h"

#include "Effect/Effect.h"

#include "Collide/Shape/CollideShapePoint.h"
#include "Collide/Shape/CollideShapeLine.h"

#include "Player/Player.h"
#include "Player/Equip/Blade.h"
#include "Player/Equip/Spear.h"
#include "Score/ScoreManager.h"
#include "Ground/Ground.h"


namespace
{
	const Label EFFECT_PARTICLE_NAME[]	= { "Laser", "Laser2" };
	const float	EFEFCT_BASE_LENGTH		= 1000.0f;	
};

////////////////////////////////////////////////////////////////////////////////
//
//	敵の攻撃：レーザー
//
////////////////////////////////////////////////////////////////////////////////
AttackLaser::AttackLaser( AttackManager* pManager ) : Attack( pManager )
{
	m_pParam	= NULL;
	m_HitLength	= 1000.0f;
}
AttackLaser::~AttackLaser()
{
	
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
void AttackLaser::_OnInit( void )
{
	//	パラメータ
	m_pParam		= &m_pData->GetParam().Laser;
	m_HitLength		= m_pParam->Length;
}

//------------------------------------------------------------------------------
//	更新
//------------------------------------------------------------------------------
void AttackLaser::_OnUpdate( void )
{
	//	エフェクト長さ設定
	_SetEffectLength( m_HitLength );

	//	当たり長さを初期化
	m_HitLength		= m_pParam->Length;
}

//------------------------------------------------------------------------------
//	当たり形状作成
//------------------------------------------------------------------------------
CollideShape* AttackLaser::_CreateCollideShape( void ) const
{
	return new CollideShapeLine( m_pParam->Length, m_pParam->Radius );
}

//------------------------------------------------------------------------------
//	当たりキャストに当たった
//------------------------------------------------------------------------------
bool AttackLaser::_OnCastHit( const COLLIDE_RESULT& Result )
{
	if( Result.Length > m_HitLength ) return false;

	enum RESULT
	{
		RESULT_NONE,
		RESULT_PIERCE,
		RESULT_HIT,
	};

	RESULT Ret = RESULT_NONE;

	switch( Result.pOwner->Type )
	{
	case COLLIDE_OWNER_PLAYER:
		{
			Player* pPlayer = (Player*)Result.pOwner->pData;
			if( pPlayer->Damage( m_pData->GetParam().Damage, _GetHitVel(Result.Normal) * m_pData->GetParam().HitVel ) )
			{
				Ret = m_pParam->bPierce? RESULT_PIERCE : RESULT_HIT;
			}
		}
		break;
	case COLLIDE_OWNER_BLADE:
		{
			Blade* pBlade = (Blade*)Result.pOwner->pData;
			if( pBlade->OnHitAttack( m_pData->GetParam().Exhaust, !pBlade->IsSmash() && IsRepulse(), Result.HitPos ) )
			{
				Ret = RESULT_HIT;
			}
		}
		break;
	case COLLIDE_OWNER_SPEAR:
		{
			Spear* pSpear = (Spear*)Result.pOwner->pData;
			if( pSpear->OnHitAttack( m_pData->GetParam().Exhaust, Result.HitPos ) )
			{
				Ret = RESULT_HIT;
			}
		}
		break;
	case COLLIDE_OWNER_RULER:
		{
			Ret = RESULT_HIT;

			//	根本までルーラーに進入している場合、消す
			if( Result.Length <= 0.0f )
			{
				Erase();
			}
		}
		break;
	case COLLIDE_OWNER_GROUND:
		{
			Ground* pGround = (Ground*)Result.pOwner->pData;

			//	貫通の有無
			if( pGround->IsAttackErase() )
			{
				Ret = RESULT_HIT;
			}
			//	ヒットエフェクト出すかどうか
			else if( pGround->IsAttackHitEffect() )
			{
				Ret = RESULT_PIERCE;
			}
		
			//	地形側のエフェクト
			pGround->CreateHitEffect( Result.GetHitMatrix() );
		}
		break;
	}

	if( Ret != RESULT_NONE )
	{
		//	ヒットエフェクト出す
		Label HitEffect( m_pData->GetParam().HitEffectName );
		if( !HitEffect.IsEmpty() )
		{
			_AppearEffect( HitEffect, Result.GetHitMatrix() );
		}

		if( Ret == RESULT_HIT )
		{
			//	当たり長さを更新
			m_HitLength = Result.Length;
		}
	}

	return Ret != RESULT_HIT;
}

//------------------------------------------------------------------------------
//	エフェクトの長さを設定
//------------------------------------------------------------------------------
void AttackLaser::_SetEffectLength( float Len )
{
	Effect* pEffect = _GetEffect();
	if( !pEffect ) return;

	Point Scale( 1.0f, Len / EFEFCT_BASE_LENGTH );

	for( int i = 0; i < ArrayNum(EFFECT_PARTICLE_NAME); i++ )
	{
		pEffect->SetParticleScale( EFFECT_PARTICLE_NAME[i], Scale );
	}
}