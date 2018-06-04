#include "stdafx.h"
#include "AttackRuler.h"

#include "../AttackData.h"

#include "App/App.h"

#include "Collide/Shape/CollideShapePoint.h"
#include "Collide/Shape/CollideShapeLine.h"

#include "BGM/BGMManager.h"

#include "Player/Player.h"
#include "Player/Equip/Blade.h"
#include "Player/Equip/Spear.h"
#include "Player/Equip/Ruler.h"
#include "Enemy/Enemy.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�̍U���F���[���[
//
////////////////////////////////////////////////////////////////////////////////
AttackRuler::AttackRuler( AttackManager* pManager ) : Attack( pManager )
{
	m_pParam		= NULL;
	m_bHit			= false;
	m_HitCount		= 0;
	m_bChangeVolume	= false;
}
AttackRuler::~AttackRuler()
{
	//	�S��������
	if( m_pPlayer )	m_pPlayer->SetEnemyRulerHit( false );
	
	//	BGM�߂�
	if( m_bChangeVolume ) GetApp()->GetBGMMng()->SetVolume( 1.0f );
}

//******************************************************************************
//	�I�[�i�[�̓o�^
//******************************************************************************
void AttackRuler::SetOwner( Enemy* pEnemy )
{
	m_pEnemy = pEnemy;
}

//******************************************************************************
//	�I���t���[���擾
//******************************************************************************
int AttackRuler::GetEndFrame( void ) const
{
	return m_bHit? 0 : m_pData->GetParam().EndFrame;
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
void AttackRuler::_OnInit( void )
{
	//	�p�����[�^
	m_pParam	= &m_pData->GetParam().Ruler;
}

//------------------------------------------------------------------------------
//	�X�V
//------------------------------------------------------------------------------
void AttackRuler::_OnUpdate( void )
{
	//	���������ꍇ
	if( m_bHit )
	{
		m_HitCount++;

		if( !m_pPlayer ) return;

		if( m_HitCount == m_pParam->AttackFrame )
		{
			//	�U���G�t�F�N�g����
			Label AttackEffect( m_pParam->AttackEffectName );
			if( !AttackEffect.IsEmpty() )
			{
				_AppearEffect( AttackEffect, Matrix3::TransRotZ( m_pPlayer->GetPos(), m_HitVel.GetAngle() ) );
			}
		}

		if( m_HitCount >= m_pParam->HitEndFrame )
		{
			//	�_���[�W��^����
			m_pPlayer->Damage( m_pData->GetParam().Damage, m_HitVel * m_pData->GetParam().HitVel, true );

			//	�j���ʒm
			if( m_pEnemy && !m_pData->GetParam().bNotNotify )
			{
				m_pEnemy->OnAttackNotify( ENEMY_ATTACK_NOTIFY_CRASH );
			}

			//	����
			Label BreakEffect( m_pData->GetParam().HitEffectName );
			if( !BreakEffect.IsEmpty() )
			{
				_AppearEffect( BreakEffect );
			}

			Erase();
		}
	}
}

//------------------------------------------------------------------------------
//	�I���t���[���ɓ��B
//------------------------------------------------------------------------------
void AttackRuler::_OnEndFrame( void )
{
	//	�j���ʒm
	if( m_pEnemy && !m_pData->GetParam().bNotNotify )
	{
		m_pEnemy->OnAttackNotify( ENEMY_ATTACK_NOTIFY_CRASH );
	}

	//	����
	Label BreakEffect( m_pData->GetParam().HitEffectName );
	if( !BreakEffect.IsEmpty() )
	{
		_AppearEffect( BreakEffect );
	}
}

//------------------------------------------------------------------------------
//	������`��쐬
//------------------------------------------------------------------------------
CollideShape* AttackRuler::_CreateCollideShape( void ) const
{
	return new CollideShapePoint( m_pParam->Radius );
}

//------------------------------------------------------------------------------
//	������L���X�g�ɓ�������
//------------------------------------------------------------------------------
bool AttackRuler::_OnCastHit( const COLLIDE_RESULT& Result )
{
	enum RESULT
	{
		RESULT_NONE,
		RESULT_BREAK,
	};

	RESULT Ret = RESULT_NONE;

	switch( Result.pOwner->Type )
	{
	case COLLIDE_OWNER_PLAYER:
		{
			//	���@
			Player* pPlayer = (Player*)Result.pOwner->pData;

			//	���q�b�g
			if( !m_bHit )
			{
				//	���@���S��
				pPlayer->SetEnemyRulerHit( true );

				//	�o�^
				m_bHit		= true;
				m_HitCount	= 0;
				m_pPlayer	= pPlayer;
				m_HitVel	= _GetHitVel( Result.Normal );

				//	BGM����
				GetApp()->GetBGMMng()->SetVolume( 0.0f );
				m_bChangeVolume = true;
			}
		}
		break;
	case COLLIDE_OWNER_BLADE:
		{
			//	�u���[�h�E�܂��
			Blade* pBlade = (Blade*)Result.pOwner->pData;
			pBlade->OnCrashRuler();
		}
		break;
	case COLLIDE_OWNER_SPEAR:
		{
			//	�X�s�A�E�܂��
			Spear* pSpear = (Spear*)Result.pOwner->pData;
			pSpear->Crash();
		}
		break;
	case COLLIDE_OWNER_RULER:
		{
			//	���[���[�E�Ώ���
			Ruler* pRuler = (Ruler*)Result.pOwner->pData;
			pRuler->Crash();
			Ret = RESULT_BREAK;
		}
		break;
	}

	//	�j��
	if( Ret == RESULT_BREAK )
	{
		//	�ʒm
		if( m_pEnemy && !m_pData->GetParam().bNotNotify )
		{
			m_pEnemy->OnAttackNotify( ENEMY_ATTACK_NOTIFY_CRASH );
		}
		
		//	�u���C�N�G�t�F�N�g�o��
		Label BreakEffect( m_pData->GetParam().HitEffectName );
		if( !BreakEffect.IsEmpty() )
		{
			_AppearEffect( BreakEffect );
		}
		
		Erase();
	}

	return true;
}

//------------------------------------------------------------------------------
//	�����蔻������邩
//------------------------------------------------------------------------------
bool AttackRuler::IsHit( const COLLIDE_OWNER& Owner ) const
{
	switch( Owner.Type )
	{
	case COLLIDE_OWNER_PLAYER:
		{
			if( m_pData->GetParam().bNotPlayerHit ) return false;
			
			//	���G���
			Player* pPlayer = (Player*)Owner.pData;
			if( pPlayer->IsInvincible( true ) ) return false;
		}
		return true;
	case COLLIDE_OWNER_BLADE:
	case COLLIDE_OWNER_SPEAR:	return !m_pData->GetParam().bNotAttackHit;
	case COLLIDE_OWNER_GROUND:	return !m_pData->GetParam().bNotGroundHit;
	}
	return true;
}