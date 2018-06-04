#include "stdafx.h"
#include "EnemyState.h"

#include "EnemyMoveAction.h"
#include "EnemyRotAction.h"
#include "EnemyAttackAction.h"
#include "EnemyTarget.h"

#include "../Enemy.h"
#include "../EnemyData.h"
#include "../EnemyStateData.h"

#include "World/World.h"
#include "Player/Player.h"
#include "Player/PlayerManager.h"
#include "Score/ScoreManager.h"
#include "Stage/StageManager.h"
#include "Stage/Stage.h"

#include "SpriteFrame/SpriteFrame.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�X�e�[�g
//
////////////////////////////////////////////////////////////////////////////////
EnemyState::EnemyState( Enemy* pEnemy )
{
	_ASSERT( pEnemy && pEnemy->GetData() );

	m_pEnemy			= pEnemy;
	m_pData				= pEnemy->GetData();
	m_pCurState			= NULL;
	m_pResidentState	= NULL;
	m_StateCount		= 0;
}
EnemyState::~EnemyState()
{
	DeleteVec( m_ActionList );
}

//******************************************************************************
//	������
//******************************************************************************
void EnemyState::Init( void )
{
	//	�����X�e�[�g
	_ChangeState( m_pData->GetHandlerStateName( ENEMY_HANDLER_STATE_ROOT ) );

	//	�풓�X�e�[�g
	m_pResidentState = m_pData->GetStateData( m_pData->GetHandlerStateName( ENEMY_HANDLER_STATE_RESIDENT ) );
	if( m_pResidentState )
	{
		m_vExecResidentAction.clear();
		m_vExecResidentAction.resize( m_pResidentState->GetActionNum(), false );
	}
}

//******************************************************************************
//	�X�V
//******************************************************************************
void EnemyState::Update( void )
{
	//	�X�e�[�g
	if( m_pCurState )
	{
		//	�A�N�V�������s
		_UpdateExecAction( m_pCurState, m_vExecAction );
		
		m_StateCount++;
	}

	//	�풓�X�e�[�g
	if( m_pResidentState )
	{
		//	�A�N�V�������s
		_UpdateExecAction( m_pResidentState, m_vExecResidentAction );
	}

	//	�A�N�V�����X�V
	for( ActionList::iterator it = m_ActionList.begin(); it != m_ActionList.end(); )
	{
		EnemyAction* pAction = *it;

		//	�폜
		if( pAction->IsDelete() )
		{
			SafeDelete( pAction );
			it = m_ActionList.erase( it );
			continue;
		}
		
		//	�X�V
		pAction->Update();

		++it;
	}
	
	//	�X�e�[�g�ύX
	if( !m_NextStateName.IsEmpty() )
	{
		_ChangeState( m_NextStateName );
		m_NextStateName.Clear();
	}
}

//******************************************************************************
//	�n���h���X�e�[�g�����s����
//******************************************************************************
void EnemyState::ChangeHandlerState( Label Name )
{
	Label StateName = m_pData->GetHandlerStateName( Name );

	if( !StateName.IsEmpty() && m_CurStateName != StateName )
	{
		_SetNextState( StateName );
	}
}

//******************************************************************************
//	�ړ��ڕW�ʒu���擾
//******************************************************************************
const Point* EnemyState::GetMoveTarget( void ) const
{
	for( ActionList::const_iterator it = m_ActionList.begin(); it != m_ActionList.end(); ++it )
	{
		if( (*it)->GetType() == ENEMY_ACTION_MOVE )
		{
			return (*it)->GetTargetPos();
		}
	}
	return NULL;
}

//******************************************************************************
//	����ڕW�ʒu���擾
//******************************************************************************
const Point* EnemyState::GetRotTarget( void ) const
{
	for( ActionList::const_iterator it = m_ActionList.begin(); it != m_ActionList.end(); ++it )
	{
		if( (*it)->GetType() == ENEMY_ACTION_ROT )
		{
			return (*it)->GetTargetPos();
		}
	}
	return NULL;
}

//------------------------------------------------------------------------------
//	�X�e�[�g�ύX
//------------------------------------------------------------------------------
void EnemyState::_ChangeState( Label Name )
{
	m_pCurState = m_pData->GetStateData( Name );

	if( m_pCurState )
	{
		m_CurStateName	= Name;
		m_StateCount	= 0;

		//	�A�N�V������~
		for( ActionList::iterator it = m_ActionList.begin(); it != m_ActionList.end(); ++it )
		{
			EnemyAction* pAction = *it;
			if( !pAction->IsContinue() )
			{
				pAction->Delete();
			}
		}

		m_vExecAction.clear();
		m_vExecAction.resize( m_pCurState->GetActionNum(), false );
	}
}

//------------------------------------------------------------------------------
//	�X�e�[�g�ύX
//------------------------------------------------------------------------------
void EnemyState::_SetNextState( Label Name )
{
	const EnemyStateData* pStateData = m_pData->GetStateData( Name );

	if( pStateData && Name != m_CurStateName )
	{
		m_NextStateName = Name;
	}
}

//------------------------------------------------------------------------------
//	�A�N�V�������s�̍X�V
//------------------------------------------------------------------------------
void EnemyState::_UpdateExecAction( const EnemyStateData* pStateData, VecBool& vExecAction )
{
	//	�A�N�V�����擾
	for( int Index = 0; Index < pStateData->GetActionNum(); Index++ )
	{
		const ENEMY_ACTION* pParam = pStateData->GetActionParam( Index );
		if( !vExecAction[Index] || !pParam->bOnce )
		{
			//	�����`�F�b�N
			if( _CheckCondition( pParam->ConditionList ) )
			{
				vExecAction[Index] = true;

				//	���s
				_ExecAction( pParam );
			}
		}
	}
}

//------------------------------------------------------------------------------
//	�A�N�V�������s
//------------------------------------------------------------------------------
void EnemyState::_ExecAction( const ENEMY_ACTION* pParam )
{
	switch( pParam->Type )
	{
	//	�ړ�
	case ENEMY_ACTION_MOVE:
		{
			EnemyMoveAction* pAction = new EnemyMoveAction( m_pEnemy );
			MEM_CHECK( pAction );
			pAction->Init( pParam );

			m_ActionList.push_back( pAction );
		}
		break;

	//	����
	case ENEMY_ACTION_ROT:
		{
			EnemyRotAction* pAction = new EnemyRotAction( m_pEnemy );
			MEM_CHECK( pAction );
			pAction->Init( pParam );

			m_ActionList.push_back( pAction );
		}
		break;

	//	�U��
	case ENEMY_ACTION_ATTACK:
		{
			EnemyAttackAction* pAction = new EnemyAttackAction( m_pEnemy );
			MEM_CHECK( pAction );
			pAction->Init( pParam );

			m_ActionList.push_back( pAction );
		}
		break;

	//	����
	case ENEMY_ACTION_SUMMON:
		{
			Matrix3 Mat = Matrix3::TransRotZ( Point(pParam->Summon.X, pParam->Summon.Y), pParam->Summon.Rot );

			m_pEnemy->AddChildEnemy( pParam->Summon.Name, pParam->Summon.BoneName, Mat, pParam->Summon.bAttach,  pParam->Summon.AppearName );
		}
		break;

	//	���[�v
	case ENEMY_ACTION_WARP:
		{
			EnemyTarget Target( m_pEnemy );
			Target.Init( &pParam->Warp.Target, m_pEnemy->GetPos() );

			m_pEnemy->SetPos( Target.GetPos() );
		}
		break;

	//	�ړ���~
	case ENEMY_ACTION_END_MOVE:
		{
			_DeleteAction( ENEMY_ACTION_MOVE );
		}
		break;
		
	//	�����~
	case ENEMY_ACTION_END_ROT:
		{
			_DeleteAction( ENEMY_ACTION_ROT );
		}
		break;
		
	//	�U����~
	case ENEMY_ACTION_END_ATTACK:
		{
			_DeleteAction( ENEMY_ACTION_ATTACK );

			//	�o�^�ς݂̍U��������
			m_pEnemy->DeleteLinkAttack( pParam->Name );
		}
		break;

	//	�S�A�N�V������~
	case ENEMY_ACTION_END_ACTION:
		{
			_DeleteActionAll();
		}
		break;

	//	�A�j���ύX
	case ENEMY_ACTION_CHANGE_ANIME:
		{
			if( m_pEnemy->GetSprite() )
			{
				m_pEnemy->GetSprite()->ChangeAnime( Label(pParam->Name), pParam->Param );
			}
		}
		break;

	//	�A�j����~
	case ENEMY_ACTION_PAUSE_ANIME:
		{
			m_pEnemy->SetPause( pParam->bFlag );
		}
		break;

	//	�X�e�[�g�ύX
	case ENEMY_ACTION_CHANGE_STATE:
		{
			if( pParam->bFlag || m_NextStateName.IsEmpty() )
			{
				_SetNextState( Label(pParam->Name) );
			}
		}
		break;
		
	//	�G�t�F�N�g
	case ENEMY_ACTION_EFFECT:
		{
			m_pEnemy->CreateEffect( Label(pParam->Name) );
		}
		break;
	
	//	�T�E���h
	case ENEMY_ACTION_SOUND:
		{
			m_pEnemy->PlaySound( Label(pParam->Name), pParam->Param );
		}
		break;
		
	//	�F�ύX
	case ENEMY_ACTION_COLOR:
		{
			if( m_pEnemy->GetSprite() ) m_pEnemy->GetSprite()->SetColor( pParam->Col );
		}
		break;

	//	�ϋv�͕ύX
	case ENEMY_ACTION_ADD_LIFE:
		{
			m_pEnemy->AddLife( pParam->Param );
		}
		break;
		
	//	�h��͕ύX
	case ENEMY_ACTION_ADD_DEFENSE:
		{
			m_pEnemy->AddDefense( pParam->Param );
		}
		break;

	//	���G
	case ENEMY_ACTION_INVINCIBLE:
		{
			m_pEnemy->SetInvincible( pParam->bFlag );
		}
		break;
		
	//	�n�C�h
	case ENEMY_ACTION_HIDE:
		{
			m_pEnemy->SetHide( pParam->bFlag );
		}
		break;

	//	�_���[�W����
	case ENEMY_ACTION_NO_DAMAGE:
		{
			m_pEnemy->SetNoDamage( pParam->bFlag );
		}
		break;

	//	������
	case ENEMY_ACTION_HIT:
		{
			m_pEnemy->SetHitEnable( pParam->bFlag );
		}
		break;
		
	//	�n�`������
	case ENEMY_ACTION_GROUND_HIT:
		{
			m_pEnemy->SetGroundHitEnable( pParam->bFlag );
		}
		break;

	//	��ʊO�ւ̈ړ���
	case ENEMY_ACTION_OUT_FIELD:
		{
			m_pEnemy->SetOutField( pParam->bFlag );
		}
		break;

	//	�J�E���g�ۂ̐ݒ�
	case ENEMY_ACTION_NO_COUNT:
		{
			m_pEnemy->SetNoCount( pParam->bFlag );
		}
		break;

	//	�����̗L��
	case ENEMY_ACTION_REPULSE:
		{
			m_pEnemy->SetRepulse( pParam->bFlag );
		}
		break;

	//	�X�R�A���b�N
	case ENEMY_ACTION_LOCK_SCORE:
		{
			m_pEnemy->GetWorld()->GetScoreManager()->SetReteLock( pParam->Param );
		}
		break;

	//	�e�ւ̒ʒm
	case ENEMY_ACTION_PARENT_NOTIFY:
		{
			m_pEnemy->NotifyParent( Label(pParam->Name) );
		}
		break;

	//	�q�ւ̒ʒm
	case ENEMY_ACTION_CHILD_NOTIFY:
		{
			m_pEnemy->NotifyChild( Label(pParam->ParamStr), Label(pParam->Name) );
		}
		break;

	//	�X�e�[�W�ւ̒ʒm
	case ENEMY_ACTION_STAGE_NOTIFY:
		{
			Stage* pStage = m_pEnemy->GetWorld()->GetStageManager()->GetCurStage();
			if( pStage )
			{
				pStage->AddNotify( Label(pParam->Name) );
			}
		}
		break;

	//	�j��
	case ENEMY_ACTION_DESTROY:
		{
			m_pEnemy->Destroy();
		}
		break;

	//	����
	case ENEMY_ACTION_ESCAPE:
		{
			m_pEnemy->Escape();
		}
		break;
	}
}

//------------------------------------------------------------------------------
//	�A�N�V�����̍폜
//------------------------------------------------------------------------------
void EnemyState::_DeleteAction( ENEMY_ACTION_TYPE Type )
{
	for( ActionList::iterator it = m_ActionList.begin(); it != m_ActionList.end(); ++it )
	{
		EnemyAction* pAction = *it;
		if( pAction->GetType() == Type )
		{
			pAction->Delete();
		}
	}
}

//------------------------------------------------------------------------------
//	�S�A�N�V�����̍폜
//------------------------------------------------------------------------------
void EnemyState::_DeleteActionAll( void )
{
	for( ActionList::iterator it = m_ActionList.begin(); it != m_ActionList.end(); ++it )
	{
		(*it)->Delete();
	}
}

//------------------------------------------------------------------------------
//	�A�N�V�������I�����Ă��邩�H
//------------------------------------------------------------------------------
bool EnemyState::_IsEndAction( ENEMY_ACTION_TYPE Type ) const
{
	for( ActionList::const_iterator it = m_ActionList.begin(); it != m_ActionList.end(); ++it )
	{
		EnemyAction* pAction = *it;
		if( pAction->GetType() == Type &&
			!pAction->IsDelete() &&
			!pAction->IsActionEnd() )
		{
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------
//	�����̃`�F�b�N
//------------------------------------------------------------------------------
bool EnemyState::_CheckCondition( const ENEMY_CONDITION_LIST& ConditionList ) const
{
	for( uint i = 0; i < ConditionList.size(); i++ )
	{
		if( !_CheckCondition( ConditionList[i] ) )
		{
			return false;
		}
	}

	return true;
}
	
//------------------------------------------------------------------------------
//	�����̃`�F�b�N
//------------------------------------------------------------------------------
bool EnemyState::_CheckCondition( const ENEMY_CONDITION& Condition ) const
{
	bool bRet = false;

	switch( Condition.Type )
	{
	case ENEMY_CONDITION_MOVE_END:			bRet = _IsEndAction( ENEMY_ACTION_MOVE );									break;
	case ENEMY_CONDITION_ROT_END:			bRet = _IsEndAction( ENEMY_ACTION_ROT );									break;
	case ENEMY_CONDITION_ATTACK_END:		bRet = _IsEndAction( ENEMY_ACTION_ATTACK );									break;
	case ENEMY_CONDITION_NOTIFY:			bRet = m_pEnemy->IsNotify( Condition.Name );								break;
	case ENEMY_CONDITION_CHILD_DEFEAT:		bRet = m_pEnemy->IsChildDefeat( Condition.Name );							break;
	case ENEMY_CONDITION_CHILD_ALL_DEFEAT:	bRet = m_pEnemy->IsChildDefeatAll();										break;
	case ENEMY_CONDITION_CHILD_EMPTY:		bRet = m_pEnemy->IsChildEmpty();											break;
	case ENEMY_CONDITION_ANIME_END:			bRet = m_pEnemy->GetSprite()->IsAnimeLast();								break;
	case ENEMY_CONDITION_ANIME_NOTIFY:		bRet = m_pEnemy->GetSprite()->GetAnimeNotify( Condition.Name ) != NULL;		break;
	case ENEMY_CONDITION_FRAME:				bRet = m_StateCount >= Condition.Param;										break;
	case ENEMY_CONDITION_RANDOM:			bRet = m_pEnemy->GetRand().GetRandFloat(100.0f) < Condition.Param;			break;
	case ENEMY_CONDITION_STATE_END:			bRet = !m_NextStateName.IsEmpty();											break;
	case ENEMY_CONDITION_LIFE_LESS:			bRet = m_pEnemy->GetLife() <= Condition.Param;								break;
	case ENEMY_CONDITION_DAMAGE:			bRet = m_pEnemy->IsDamage() == Condition.bFlag;								break;
	case ENEMY_CONDITION_KNOCKBACK:			bRet = m_pEnemy->IsKnockback() == Condition.bFlag;							break;
	case ENEMY_CONDITION_REPULSE:			bRet = m_pEnemy->IsRepulseDamage() == Condition.bFlag;						break;
	case ENEMY_CONDITION_DEAD:				bRet = m_pEnemy->IsDead() == Condition.bFlag;								break;

	case ENEMY_CONDITION_SELF_POS_X:
		{
			float Pos = m_pEnemy->GetPos().x - m_pEnemy->GetWorld()->GetFieldOffset().x;
			bRet = Pos >= Condition.Range[0] && Pos <= Condition.Range[1];
		}
		break;
	case ENEMY_CONDITION_SELF_POS_Y:
		{
			float Pos = m_pEnemy->GetPos().y - m_pEnemy->GetWorld()->GetFieldOffset().y;
			bRet = Pos >= Condition.Range[0] && Pos <= Condition.Range[1];
		}
		break;

	case ENEMY_CONDITION_PLAYER_RANGE:
		{
			Player* pPlayer = m_pEnemy->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer )
			{
				float LenSq = (pPlayer->GetPos() - m_pEnemy->GetPos()).LengthSq();
				bRet = LenSq >= Sq( Condition.Range[0] ) && LenSq <= Sq( Condition.Range[1] );
			}
		}
		break;
	case ENEMY_CONDITION_PLAYER_RANGE_X:
		{
			Player* pPlayer = m_pEnemy->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer )
			{
				float Len = pPlayer->GetPos().x - m_pEnemy->GetPos().x;
				bRet = Len >= Condition.Range[0] && Len <= Condition.Range[1];
			}
		}
		break;
	case ENEMY_CONDITION_PLAYER_RANGE_Y:
		{
			Player* pPlayer = m_pEnemy->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer )
			{
				float Len = pPlayer->GetPos().y - m_pEnemy->GetPos().y;
				bRet = Len >= Condition.Range[0] && Len <= Condition.Range[1];
			}
		}
		break;
	case ENEMY_CONDITION_PLAYER_ANGLE:
		{
			Player* pPlayer = m_pEnemy->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer )
			{
				Angle Ang = (pPlayer->GetPos() - m_pEnemy->GetPos()).GetAngle();
				float Diff = Abs( Angle::Diff( Ang, m_pEnemy->GetRot() ) );
				bRet = Diff >= Condition.Range[0] && Diff <= Condition.Range[1];
			}
		}
		break;
	case ENEMY_CONDITION_PLAYER_ANGLE_REL:
		{
			Player* pPlayer = m_pEnemy->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer )
			{
				Angle Ang = (pPlayer->GetPos() - m_pEnemy->GetPos()).GetAngle();
				float Diff = Angle::Diff( Ang, m_pEnemy->GetRot() );
				bRet = Diff >= Condition.Range[0] && Diff <= Condition.Range[1];
			}
		}
		break;
	case ENEMY_CONDITION_PLAYER_POS_X:
		{
			Player* pPlayer = m_pEnemy->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer )
			{
				float Pos = pPlayer->GetPos().x - m_pEnemy->GetWorld()->GetFieldOffset().x;
				bRet = Pos >= Condition.Range[0] && Pos <= Condition.Range[1];
			}
		}
		break;
	case ENEMY_CONDITION_PLAYER_POS_Y:
		{
			Player* pPlayer = m_pEnemy->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer )
			{
				float Pos = pPlayer->GetPos().y - m_pEnemy->GetWorld()->GetFieldOffset().y;
				bRet = Pos >= Condition.Range[0] && Pos <= Condition.Range[1];
			}
		}
		break;
	case ENEMY_CONDITION_PLAYER_STATE:
		{
			Player* pPlayer = m_pEnemy->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer )
			{
				bRet = pPlayer->IsState( Condition.Name );
			}
		}
		break;
	case ENEMY_CONDITION_PLAYER_STATE_TIME:
		{
			Player* pPlayer = m_pEnemy->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer )
			{
				bRet = pPlayer->GetStateCount() >= Condition.Param;
			}
		}
		break;
	}

	//	���]
	if( Condition.bNot )
	{
		bRet = !bRet;
	}

	return bRet;
}