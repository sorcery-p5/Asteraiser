#include "stdafx.h"
#include "StageAction.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�X�e�[�W�A�N�V����
//
////////////////////////////////////////////////////////////////////////////////
StageAction::StageAction( Stage* pParent )
{
	_ASSERT( pParent );

	m_pParent		= pParent;
	m_pParam		= NULL;
	m_bDelete		= false;
	m_FrameCount	= 0;
}
StageAction::~StageAction( void )
{
}

//******************************************************************************
//	������
//******************************************************************************
void StageAction::Init( const STAGE_ACTION* pParam )
{
	_ASSERT( pParam );

	m_pParam		= pParam;
	m_bDelete		= false;
	m_FrameCount	= 0;

	//	�ʒm
	_OnInit();
}

//******************************************************************************
//	�X�V
//******************************************************************************
void StageAction::Update( void )
{
	//	�ʒm
	_OnUpdate();

	m_FrameCount++;
}