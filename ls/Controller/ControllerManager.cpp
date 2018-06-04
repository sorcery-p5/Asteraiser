#include "stdafx.h"
#include "ControllerManager.h"
#include "Controller.h"
#include "ManualController.h"
#include "ReplayController.h"

#include "World/World.h"


////////////////////////////////////////////////////////////////////////////////
//
//	コントローラ管理
//
////////////////////////////////////////////////////////////////////////////////
ControllerManager::ControllerManager( World* pWorld )
{
	_ASSERT(m_pWorld);

	m_pWorld		= pWorld;
	m_pController	= NULL;
	m_bReplay		= false;
}

ControllerManager::~ControllerManager()
{
	SafeDelete( m_pController );
}

//******************************************************************************
//	通常モードで初期化
//******************************************************************************
void ControllerManager::Init( bool bReplay )
{
	m_bReplay = bReplay;

	//	コントローラ追加
	if( bReplay )
	{
		m_pController = new ReplayController( m_pWorld->GetReplayManager() );
	}
	else
	{
		m_pController = new ManualController( m_pWorld->GetReplayManager() );
	}

	MEM_CHECK( m_pController );
}

//******************************************************************************
//	更新
//******************************************************************************
void ControllerManager::Update( uint FrameCount )
{
	SafePtr( m_pController )->Update( FrameCount );
}

//******************************************************************************
//	操作をマニュアルに切り替える
//******************************************************************************
void ControllerManager::ChangeManual( void )
{
	if( !m_bReplay ) return;

	ManualController* pCtrl = new ManualController( m_pWorld->GetReplayManager() );

	//	状態をコピー
	pCtrl->CopyInfo( *m_pController );

	//	差し替え
	SafeDelete( m_pController );
	m_pController = pCtrl;

	m_bReplay = true;
}