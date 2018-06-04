#include "stdafx.h"
#include "EnemyTarget.h"

#include "../Enemy.h"
#include "../EnemyManager.h"

#include "Player/Player.h"
#include "Player/PlayerManager.h"

#include "World/World.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�F�ڕW�ʒu
//
////////////////////////////////////////////////////////////////////////////////
EnemyTarget::EnemyTarget( Enemy* pEnemy )
{
	_ASSERT( pEnemy );
	m_pEnemy		= pEnemy;
	m_pParam		= NULL;
	m_FrameCount	= 0;
}
EnemyTarget::~EnemyTarget()
{

}

//******************************************************************************
//	������
//******************************************************************************
void EnemyTarget::Init( const ENEMY_TARGET* pParam, Point InitPos )
{
	_ASSERT( pParam );
	m_pParam	= pParam;
	m_Pos		= InitPos;

	//	����
	_UpdatePos();
}

//******************************************************************************
//	�X�V
//******************************************************************************
void EnemyTarget::Update( void )
{
	if( m_pParam->Interval > 0 )
	{
		//	�X�V�Ԋu
		m_FrameCount++;
		if( m_FrameCount >= m_pParam->Interval )
		{
			_UpdatePos();
			m_FrameCount = 0;
		}
	}
}

//------------------------------------------------------------------------------
//	�ʒu�̍X�V
//------------------------------------------------------------------------------
void EnemyTarget::_UpdatePos( void )
{
	//	�I�t�Z�b�g�ʒu�v�Z
	m_OffsetPos	= _GetOffsetPos();

	switch( m_pParam->Type )
	{
	case ENEMY_TARGET_FIX:
		{
			m_Pos = m_OffsetPos + m_pEnemy->GetWorld()->GetFieldOffset();
		}
		break;
	case ENEMY_TARGET_PLAYER:
		{
			Player* pPlayer = m_pEnemy->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer )
			{
				if( m_pParam->AxisType == ENEMY_TARGET_AXIS_ABS )
				{
					m_Pos = pPlayer->GetPos() + m_OffsetPos;
				}
				else
				{
					m_Pos = pPlayer->GetPos() + m_OffsetPos.Rot( (pPlayer->GetPos() - m_pEnemy->GetPos()).GetAngle() );
				}
			}
		}
		break;
	case ENEMY_TARGET_SELF:
		{
			if( m_pParam->AxisType == ENEMY_TARGET_AXIS_ABS )
			{
				m_Pos = m_pEnemy->GetPos() + m_OffsetPos;
			}
			else
			{
				m_Pos = m_pEnemy->GetMatrix().Transform( m_OffsetPos );
			}
		}
		break;
	case ENEMY_TARGET_ENEMY:
		{
			Enemy* pEnemy = m_pEnemy->GetManager()->GetEnemy( Label(m_pParam->Name) );
			if( pEnemy )
			{
				if( m_pParam->AxisType == ENEMY_TARGET_AXIS_ABS )
				{
					m_Pos = pEnemy->GetPos() + m_OffsetPos;
				}
				else
				{
					m_Pos = pEnemy->GetPos() + m_OffsetPos.Rot( (pEnemy->GetPos() - m_pEnemy->GetPos()).GetAngle() );
				}
			}
		}
		break;
	}

	//	�t�B�[���h���Ɏ��߂�
	if( !m_pParam->bOutField )
	{
		m_Pos = m_pEnemy->GetWorld()->GetFieldRect().Inside( m_Pos );
	}
}

//------------------------------------------------------------------------------
//	�I�t�Z�b�g�̎擾
//------------------------------------------------------------------------------
Point EnemyTarget::_GetOffsetPos( void ) const
{
	Rand& Rnd = m_pEnemy->GetManager()->GetRand();

	//	����
	if( m_pParam->PosType == ENEMY_TARGET_POS_RECT )
	{
		return Point(	Rnd.GetRandRange( m_pParam->X, m_pParam->RandRange[0] ),
						Rnd.GetRandRange( m_pParam->Y, m_pParam->RandRange[1] ) );
	}
	//	��
	else if( m_pParam->PosType == ENEMY_TARGET_POS_POLER )
	{
		return Point( 0.0f, Rnd.GetRandRange( m_pParam->Len, m_pParam->RandRange[1] ) ).Rot( Rnd.GetRandRange( m_pParam->Ang, m_pParam->RandRange[0] ) );
	}

	return Point();
}