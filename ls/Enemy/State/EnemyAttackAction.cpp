#include "stdafx.h"
#include "EnemyAttackAction.h"
#include "EnemyTarget.h"

#include "../Enemy.h"
#include "../EnemyData.h"

#include "World/World.h"

#include "Attack/Attack.h"
#include "Attack/AttackManager.h"
#include "Attack/AttackData.h"

#include "SpriteFrame/SpriteFrame.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�F�U���A�N�V����
//
////////////////////////////////////////////////////////////////////////////////
EnemyAttackAction::EnemyAttackAction( Enemy* pEnemy ) : EnemyAction( pEnemy )
{
	m_pAppearTarget	= NULL;
	m_pDirTarget	= NULL;

	m_AppearNum		= 0;
}
EnemyAttackAction::~EnemyAttackAction()
{
	SafeDelete( m_pAppearTarget );
	SafeDelete( m_pDirTarget );
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
void EnemyAttackAction::_OnInit( void )
{
	const ENEMY_ATTACK_ACTION& Param = m_pParam->Attack;

	//	�����ڕW
	if( Param.Pos.Type == ENEMY_ATTACK_POS_TARGET )
	{
		m_pAppearTarget = new EnemyTarget( m_pEnemy );
		MEM_CHECK( m_pAppearTarget );
		
		m_pAppearTarget->Init( &Param.Pos.Target, m_pEnemy->GetPos() );
	}

	//	�����ڕW
	if( Param.Dir.Type == ENEMY_ATTACK_DIR_TARGET )
	{
		m_pDirTarget = new EnemyTarget( m_pEnemy );
		MEM_CHECK( m_pDirTarget );
		
		m_pDirTarget->Init( &Param.Dir.Target, m_pEnemy->GetPos() );
	}
}

//------------------------------------------------------------------------------
//	�X�V
//------------------------------------------------------------------------------
void EnemyAttackAction::_OnUpdate( void )
{
	const ENEMY_ATTACK_ACTION& Param = m_pParam->Attack;

	//	�ڕW�̍X�V
	if( m_pAppearTarget )	m_pAppearTarget->Update();
	if( m_pDirTarget )		m_pDirTarget->Update();

	if( Param.Interval == 0	||
		_GetFrameCount() % Param.Interval == 0 )
	{
		//	�ʒu�ƌ������擾
		Point Pos = _GetAttackPos();
		float Rot = _GetAttackRot( Pos );
		Matrix3 Mat = Matrix3::TransRotZ( Pos, Rot );

		_Attack( Label(Param.Name), Mat );

		//	�񐔃J�E���g
		m_AppearNum++;
		if( Param.AppearNum != 0 &&
			Param.AppearNum <= m_AppearNum )
		{
			Delete();
		}
	}
}

//------------------------------------------------------------------------------
//	�U�������s
//------------------------------------------------------------------------------
void EnemyAttackAction::_Attack( Label Name, const Matrix3& Mat )
{
	//	�f�[�^�擾
	const AttackData* pData = m_pEnemy->GetData()->GetAttackData( Name );
	if( !pData ) return;

	//	����
	World* pWorld = m_pEnemy->GetWorld();
	VecAttack vAttack = pWorld->GetAttackManager()->AddAttack( pData, Mat );
	for( VecAttack::iterator it = vAttack.begin(); it != vAttack.end(); ++it )
	{
		//	�Ǐ]�o�^
		if( (*it)->IsAttach() )
		{
			//	�{�[��
			if( m_pParam->Attack.Pos.Type == ENEMY_ATTACK_POS_BONE )
			{
				m_pEnemy->RegisterAttack( (*it), Name, m_pParam->Attack.Pos.Bone, true );
			}
			else if( m_pParam->Attack.Pos.Type == ENEMY_ATTACK_POS_CENTER )
			{
				m_pEnemy->RegisterAttack( (*it), Name, "", true );
			}
		}
		//	�폜�o�^
		else if( (*it)->GetData()->GetParam().bOwnerErase )
		{
			m_pEnemy->RegisterAttack( (*it), Name, "", false );
		}
	}
}

//------------------------------------------------------------------------------
//	�U���ʒu�擾
//------------------------------------------------------------------------------
Point EnemyAttackAction::_GetAttackPos( void )
{
	const ENEMY_ATTACK_ACTION& Param = m_pParam->Attack;

	Point Pos = m_pEnemy->GetPos();

	switch( Param.Pos.Type )
	{
	//	�{�[��
	case ENEMY_ATTACK_POS_BONE:
		{
			SpriteFrame* pSprite = m_pEnemy->GetSprite();
			if( pSprite )
			{
				const Matrix3* pMat = pSprite->GetBoneMatrix( Param.Pos.Bone );
				if( pMat ) Pos = pMat->GetPos();
			}
		}
		break;
	//	�ڕW�w��
	case ENEMY_ATTACK_POS_TARGET:
		{
			_ASSERT( m_pAppearTarget );
			Pos = m_pAppearTarget->GetPos();
		}
		break;
	}

	return Pos;
}

//------------------------------------------------------------------------------
//	�U���p�x�擾
//------------------------------------------------------------------------------
float EnemyAttackAction::_GetAttackRot( Point Pos )
{
	const ENEMY_ATTACK_ACTION& Param = m_pParam->Attack;

	Angle Rot;

	//	�Œ�
	if( Param.Dir.Type == ENEMY_ATTACK_DIR_FIX )
	{
		Rot = Angle( Param.Dir.Fix );
	}

	//	���i
	else if( Param.Dir.Type == ENEMY_ATTACK_DIR_STRAIGHT )
	{
		//	���������Ɉˑ�
		switch( Param.Pos.Type )
		{
		//	���S
		case ENEMY_ATTACK_POS_CENTER:
			{
				Rot = m_pEnemy->GetRot();
			}
			break;
		//	�{�[��
		case ENEMY_ATTACK_POS_BONE:
			{
				Rot = m_pEnemy->GetBoneMatrix( Param.Pos.Bone ).GetRotZ();
			}
			break;
		}
	}
	
	//	�ڕW�w��
	else if( Param.Dir.Type == ENEMY_ATTACK_DIR_TARGET )
	{
		Rot = (m_pDirTarget->GetPos() - Pos).GetAngle();
	}

	//	�ړ��ڕW
	else if( Param.Dir.Type == ENEMY_ATTACK_DIR_MOVE )
	{
		const Point* pTarget = m_pEnemy->GetMoveTarget();
		if( pTarget )
		{
			Rot = (*pTarget - Pos).GetAngle();
		}
	}

	//	����ڕW
	else if( Param.Dir.Type == ENEMY_ATTACK_DIR_ROT )
	{
		const Point* pTarget = m_pEnemy->GetRotTarget();
		if( pTarget )
		{
			Rot = (*pTarget - Pos).GetAngle();
		}
	}

	return Rot;
}