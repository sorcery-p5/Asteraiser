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
//	�G�̍U���F���[�U�[
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
//	������
//------------------------------------------------------------------------------
void AttackLaser::_OnInit( void )
{
	//	�p�����[�^
	m_pParam		= &m_pData->GetParam().Laser;
	m_HitLength		= m_pParam->Length;
}

//------------------------------------------------------------------------------
//	�X�V
//------------------------------------------------------------------------------
void AttackLaser::_OnUpdate( void )
{
	//	�G�t�F�N�g�����ݒ�
	_SetEffectLength( m_HitLength );

	//	�����蒷����������
	m_HitLength		= m_pParam->Length;
}

//------------------------------------------------------------------------------
//	������`��쐬
//------------------------------------------------------------------------------
CollideShape* AttackLaser::_CreateCollideShape( void ) const
{
	return new CollideShapeLine( m_pParam->Length, m_pParam->Radius );
}

//------------------------------------------------------------------------------
//	������L���X�g�ɓ�������
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

			//	���{�܂Ń��[���[�ɐi�����Ă���ꍇ�A����
			if( Result.Length <= 0.0f )
			{
				Erase();
			}
		}
		break;
	case COLLIDE_OWNER_GROUND:
		{
			Ground* pGround = (Ground*)Result.pOwner->pData;

			//	�ђʂ̗L��
			if( pGround->IsAttackErase() )
			{
				Ret = RESULT_HIT;
			}
			//	�q�b�g�G�t�F�N�g�o�����ǂ���
			else if( pGround->IsAttackHitEffect() )
			{
				Ret = RESULT_PIERCE;
			}
		
			//	�n�`���̃G�t�F�N�g
			pGround->CreateHitEffect( Result.GetHitMatrix() );
		}
		break;
	}

	if( Ret != RESULT_NONE )
	{
		//	�q�b�g�G�t�F�N�g�o��
		Label HitEffect( m_pData->GetParam().HitEffectName );
		if( !HitEffect.IsEmpty() )
		{
			_AppearEffect( HitEffect, Result.GetHitMatrix() );
		}

		if( Ret == RESULT_HIT )
		{
			//	�����蒷�����X�V
			m_HitLength = Result.Length;
		}
	}

	return Ret != RESULT_HIT;
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�̒�����ݒ�
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