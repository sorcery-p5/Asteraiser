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
//	�G�̍U���F�V�[���h
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
//	�I�[�i�[�̓o�^
//******************************************************************************
void AttackShield::SetOwner( Enemy* pEnemy )
{
	m_pEnemy = pEnemy;
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
void AttackShield::_OnInit( void )
{
	//	�p�����[�^
	m_pParam	= &m_pData->GetParam().Shield;
	m_Energy	= m_pParam->Energy;
}

//------------------------------------------------------------------------------
//	�X�V
//------------------------------------------------------------------------------
void AttackShield::_OnUpdate( void )
{
}

//------------------------------------------------------------------------------
//	������`��쐬
//------------------------------------------------------------------------------
CollideShape* AttackShield::_CreateCollideShape( void ) const
{
	return new CollideShapePoint( m_pParam->Radius );
}

//------------------------------------------------------------------------------
//	������L���X�g�ɓ�������
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
			//	���@
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

			//	�X�}�b�V���̏ꍇ
			if( pBlade->IsSmash() )
			{
				if( pBlade->OnHitAttack( m_pData->GetParam().Exhaust, false, Result.HitPos ) )
				{
					Ret = RESULT_BREAK;
				}
			}
			//	�ʏ�̏ꍇ
			else
			{
				if( pBlade->OnHitAttack( m_pData->GetParam().Exhaust, IsRepulse(), Result.HitPos ) )
				{
					//	�_���[�W
					DecreaseZero( m_Energy, pBlade->GetRepulseAttackPower() * m_pParam->Defense );
					
					Ret = m_Energy <= 0.0f? RESULT_BREAK : RESULT_HIT;

					//	�����̒ʒm
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
			//	�X�s�A
			Spear* pSpear = (Spear*)Result.pOwner->pData;
			if( pSpear->OnHitAttack( m_pData->GetParam().Exhaust, Result.HitPos ) )
			{
				//	�_���[�W
				DecreaseZero( m_Energy, pSpear->GetAttackPower() * m_pParam->Defense );
				Ret = m_Energy <= 0.0f? RESULT_BREAK : RESULT_HIT;
			}
		}
		break;
	case COLLIDE_OWNER_RULER:
		{
			//	���[���[
			Ret = RESULT_BREAK;
		}
		break;
	}


	//	����
	if( Ret == RESULT_HIT )
	{
		//	�q�b�g�G�t�F�N�g�o��
		Label HitEffect( m_pData->GetParam().HitEffectName );
		if( !HitEffect.IsEmpty() )
		{
			_AppearEffect( HitEffect, Result.GetHitMatrix() );
		}
	}

	//	�u���C�N
	else if( Ret == RESULT_BREAK )
	{
		//	�u���C�N�ʒm
		if( m_pEnemy && !m_pData->GetParam().bNotNotify )
		{
			m_pEnemy->OnAttackNotify( ENEMY_ATTACK_NOTIFY_BREAK );
		}
		
		//	�u���C�N�G�t�F�N�g�o��
		Label BreakEffect( m_pParam->BreakEffectName );
		if( !BreakEffect.IsEmpty() )
		{
			_AppearEffect( BreakEffect );
		}
		
		Erase();
	}

	return true;
}
