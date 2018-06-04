#include "stdafx.h"
#include "ReplayController.h"

#include "App/App.h"
#include "Config/Config.h"

#include "Replay/ReplayManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	リプレイ操作コントローラ
//
////////////////////////////////////////////////////////////////////////////////
ReplayController::ReplayController( ReplayManager* pReplayManager )
:Controller()
{
	_ASSERT( pReplayManager );

	m_pReplayManager = pReplayManager;
}
ReplayController::~ReplayController()
{
	
}

//******************************************************************************
//	更新
//******************************************************************************
void ReplayController::Update( uint FrameCount )
{
	byte Info = m_pReplayManager->GetReplayData( FrameCount );

	//	登録
	_UpdateCurInfo( Info );
}