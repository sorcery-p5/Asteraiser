#include "stdafx.h"
#include "EnemyRotAction.h"
#include "EnemyTarget.h"

#include "../Enemy.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�F����A�N�V����
//
////////////////////////////////////////////////////////////////////////////////
EnemyRotAction::EnemyRotAction( Enemy* pEnemy ) : EnemyAction( pEnemy )
{
	m_pTarget	= NULL;
	m_bRotEnd	= false;
}
EnemyRotAction::~EnemyRotAction()
{
	SafeDelete( m_pTarget );
}

//******************************************************************************
//	�ڕW�ʒu�̎擾
//******************************************************************************
const Point* EnemyRotAction::GetTargetPos( void ) const
{
	if( !m_pTarget ) return NULL;

	return &m_pTarget->GetPos();
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
void EnemyRotAction::_OnInit( void )
{
	const ENEMY_ROT_ACTION& Param = m_pParam->Rot;

	if( Param.Type == ENEMY_ROT_TARGET )
	{
		m_pTarget = new EnemyTarget( m_pEnemy );
		MEM_CHECK( m_pTarget );

		m_pTarget->Init( &Param.Target, m_pEnemy->GetPos() );
	}
	
	m_bRotEnd	= false;
}

//------------------------------------------------------------------------------
//	�X�V
//------------------------------------------------------------------------------
void EnemyRotAction::_OnUpdate( void )
{
	const ENEMY_ROT_ACTION& Param = m_pParam->Rot;

	bool bEnd	= false;
	Angle Ang	= 0.0f;
	Angle Rot	= m_pEnemy->GetRot();

	//	�ڕW����
	if( Param.Type == ENEMY_ROT_TARGET )
	{
		//	�ڕW�ʒu�̍X�V
		m_pTarget->Update();

		Point Diff = m_pTarget->GetPos() - m_pEnemy->GetPos();
		if( Diff.IsZero() )
		{
			bEnd = true;
		}
		else
		{
			Ang = Diff.GetAngle();
		}
	}
	//	�Œ����
	else if( Param.Type == ENEMY_ROT_FIX )
	{
		Ang = Param.Fix;
	}
	//	�ړ�����
	else if( Param.Type == ENEMY_ROT_MOVE )
	{
		const Point* pTarget = m_pEnemy->GetMoveTarget();
		if( pTarget )
		{
			Point Diff = *pTarget - m_pEnemy->GetPos();
			if( Diff.IsZero() )
			{
				bEnd = true;
			}
			else
			{
				Ang = Diff.GetAngle();
			}
		}
	}

	//	����ڕW�ɂ���
	if( bEnd )
	{
		m_bRotEnd = true;
		return;
	}

	//	�ݒ�
	Rot.NearTo( Ang, Param.Speed );
	m_pEnemy->SetRot( Rot );

	//	�ړ��I��
	if( Ang == Rot ) m_bRotEnd = true;
}