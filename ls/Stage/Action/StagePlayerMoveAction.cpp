#include "stdafx.h"
#include "StagePlayerMoveAction.h"

#include "../Stage.h"

#include "World/World.h"
#include "Player/PlayerManager.h"
#include "Player/Player.h"


////////////////////////////////////////////////////////////////////////////////
//
//	ステージアクション
//
////////////////////////////////////////////////////////////////////////////////
StagePlayerMoveAction::StagePlayerMoveAction( Stage* pParent ) : StageAction( pParent )
{
}
StagePlayerMoveAction::~StagePlayerMoveAction( void )
{
}

//------------------------------------------------------------------------------
//	初期化
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

	//	初期位置
	m_InitPos	= pPlayer->GetPos();
}

//------------------------------------------------------------------------------
//	更新
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
		//	終了
		Delete();
	}
	else
	{
		//	線形補間で移動
		float Val = (float)_GetFrameCount() / m_pParam->ParamVal;

		Point TargetPos = m_pParam->Pos + m_pParent->GetWorld()->GetFieldOffset();
		Point StartPos = Lerp( m_InitPos, TargetPos, Val );
		pPlayer->SetPos( Lerp( StartPos, TargetPos, Val ) );
		if( !m_pParam->ParamStr.IsEmpty() ) pPlayer->SetSide( m_pParam->ParamStr );
	}
}