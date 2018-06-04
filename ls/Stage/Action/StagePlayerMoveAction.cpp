#include "stdafx.h"
#include "StagePlayerMoveAction.h"

#include "../Stage.h"

#include "World/World.h"
#include "Player/PlayerManager.h"
#include "Player/Player.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�X�e�[�W�A�N�V����
//
////////////////////////////////////////////////////////////////////////////////
StagePlayerMoveAction::StagePlayerMoveAction( Stage* pParent ) : StageAction( pParent )
{
}
StagePlayerMoveAction::~StagePlayerMoveAction( void )
{
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
void StagePlayerMoveAction::_OnInit( void )
{
	Player* pPlayer = m_pParent->GetWorld()->GetPlayerManager()->GetPlayer();
	if( !pPlayer )
	{
		Delete();
		return;
	}

	_ASSERT( m_pParam->ParamVal >= 1.0f );

	//	�����ʒu
	m_InitPos	= pPlayer->GetPos();
}

//------------------------------------------------------------------------------
//	�X�V
//------------------------------------------------------------------------------
void StagePlayerMoveAction::_OnUpdate( void )
{
	Player* pPlayer = m_pParent->GetWorld()->GetPlayerManager()->GetPlayer();
	if( !pPlayer )
	{
		Delete();
		return;
	}

	if( _GetFrameCount() > (int)m_pParam->ParamVal )
	{
		//	�I��
		Delete();
	}
	else
	{
		//	���`��Ԃňړ�
		float Val = (float)_GetFrameCount() / m_pParam->ParamVal;

		Point TargetPos = m_pParam->Pos + m_pParent->GetWorld()->GetFieldOffset();
		Point StartPos = Lerp( m_InitPos, TargetPos, Val );
		pPlayer->SetPos( Lerp( StartPos, TargetPos, Val ) );
		if( !m_pParam->ParamStr.IsEmpty() ) pPlayer->SetSide( m_pParam->ParamStr );
	}
}