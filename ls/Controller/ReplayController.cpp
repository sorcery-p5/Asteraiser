#include "stdafx.h"
#include "ReplayController.h"

#include "App/App.h"
#include "Config/Config.h"

#include "Replay/ReplayManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	���v���C����R���g���[��
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
//	�X�V
//******************************************************************************
void ReplayController::Update( uint FrameCount )
{
	byte Info = m_pReplayManager->GetReplayData( FrameCount );

	//	�o�^
	_UpdateCurInfo( Info );
}