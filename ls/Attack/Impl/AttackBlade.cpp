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
//	�G�̍U���F�u���[�h
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
//	�I�[�i�[�̓o�^
//******************************************************************************
void AttackBlade::SetOwner( Enemy* pEnemy )
{
	m_pEnemy = pEnemy;
}

//******************************************************************************
//	�ʒm
//******************************************************************************
void AttackBlade::OnNotify( ATTACK_NOTIFY Notify )
{
	switch( Notify )
	{
	case ATTACK_NOTIFY_DAMAGE:		_Crash();	return;
	}
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
void AttackBlade::_OnInit( void )
{
	//	�p�����[�^
	m_pParam	= &m_pData->GetParam().Blade;
	
	m_Energy	= m_pParam->Energy;
}

//------------------------------------------------------------------------------
//	�X�V
//------------------------------------------------------------------------------
void AttackBlade::_OnUpdate( void )
{
}

//------------------------------------------------------------------------------
//	������`��쐬
//------------------------------------------------------------------------------
CollideShape* AttackBlade::_CreateCollideShape( void ) const
{
	return new CollideShapeLine( m_pParam->Length, m_pParam->Radius );
}

//------------------------------------------------------------------------------
//	������L���X�g�ɓ�������
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

			//	����
			if( !pBlade->IsSmash() && IsRepulse() )
			{
				if( pBlade->OnHitAttack( m_pParam->RepulseExhaust, IsRepulse(), Result.HitPos ) )
				{
					//	�_���[�W
					DecreaseZero( m_Energy, pBlade->GetRepulseAttackPower() );
				
					Ret = m_Energy <= 0.0f? RESULT_CRASH : RESULT_HIT;

					//	�����G�t�F�N�g�ݒ�
					if( !pBlade->IsCrash() ) HitEffect = m_pParam->RepulseEffectName;

					//	�����̒ʒm
					if( m_pEnemy && m_Energy > 0.0f &&
						!m_pData->GetParam().bNotNotify )
					{
						m_pEnemy->OnAttackNotify( ENEMY_ATTACK_NOTIFY_REPULSE );
					}
				}
			}
			//	�ʏ�
			else
			{
				if( pBlade->OnHitAttack( m_pData->GetParam().Exhaust, IsRepulse(), Result.HitPos ) )
				{
					if( m_pParam->bSmash )
					{
						//	����̌����X�}�b�V���Ȃ�܂��E����ȊO�͖���
						Ret = pBlade->IsSmash()? RESULT_CRASH : RESULT_HIT;
					}
					else
					{
						//	����̌����܂�Ă���΃q�b�g�E�����Ȃ�܂��
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
				//	�X�s�A���܂�Ă���΃q�b�g�E�����Ȃ�܂��
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

			//	�q�b�g�G�t�F�N�g�o�����ǂ���
			if( pGround->IsAttackHitEffect() )
			{
				Ret = RESULT_HIT;
			}

			//	�n�`���̃G�t�F�N�g
			pGround->CreateHitEffect( Result.GetHitMatrix() );
		}
		break;
	}

	//	����
	if( Ret == RESULT_HIT )
	{
		//	�q�b�g�G�t�F�N�g�o��
		if( !HitEffect.IsEmpty() )
		{	
			Matrix3 Mat = GetMatrix();
			Mat.SetPos( Result.HitPos );

			_AppearEffect( HitEffect, Mat );
		}
	}

	//	�܂ꂽ
	else if( Ret == RESULT_CRASH )
	{
		_Crash();
	}

	return true;
}

//------------------------------------------------------------------------------
//	�j��
//------------------------------------------------------------------------------
void AttackBlade::_Crash( void )
{
	//	�ʒm
	if( m_pEnemy && !m_pData->GetParam().bNotNotify )
	{
		m_pEnemy->OnAttackNotify( ENEMY_ATTACK_NOTIFY_CRASH );
	}
	
	//	�X�R�A�ʒm
	m_pManager->GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_BLADE_ERASE, m_pData->GetParam().ScoreRate );

	//	�j���G�t�F�N�g�o��
	Label CrashEffect( m_pParam->CrashEffectName );
	if( !CrashEffect.IsEmpty() )
	{
		_AppearEffect( CrashEffect );
	}

	Erase();
}