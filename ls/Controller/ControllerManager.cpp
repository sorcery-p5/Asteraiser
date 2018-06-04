#include "stdafx.h"
#include "ControllerManager.h"
#include "Controller.h"
#include "ManualController.h"
#include "ReplayController.h"

#include "World/World.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�R���g���[���Ǘ�
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
//	�ʏ탂�[�h�ŏ�����
//******************************************************************************
void ControllerManager::Init( bool bReplay )
{
	m_bReplay = bReplay;

	//	�R���g���[���ǉ�
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
//	�X�V
//******************************************************************************
void ControllerManager::Update( uint FrameCount )
{
	SafePtr( m_pController )->Update( FrameCount );
}

//******************************************************************************
//	������}�j���A���ɐ؂�ւ���
//******************************************************************************
void ControllerManager::ChangeManual( void )
{
	if( !m_bReplay ) return;

	ManualController* pCtrl = new ManualController( m_pWorld->GetReplayManager() );

	//	��Ԃ��R�s�[
	pCtrl->CopyInfo( *m_pController );

	//	�����ւ�
	SafeDelete( m_pController );
	m_pController = pCtrl;

	m_bReplay = true;
}