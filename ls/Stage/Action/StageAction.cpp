#include "stdafx.h"
#include "StageAction.h"


////////////////////////////////////////////////////////////////////////////////
//
//	ステージアクション
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
//	初期化
//******************************************************************************
void StageAction::Init( const STAGE_ACTION* pParam )
{
	_ASSERT( pParam );

	m_pParam		= pParam;
	m_bDelete		= false;
	m_FrameCount	= 0;

	//	通知
	_OnInit();
}

//******************************************************************************
//	更新
//******************************************************************************
void StageAction::Update( void )
{
	//	通知
	_OnUpdate();

	m_FrameCount++;
}