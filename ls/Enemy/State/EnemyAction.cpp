#include "stdafx.h"
#include "EnemyAction.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�A�N�V�����i���j
//
////////////////////////////////////////////////////////////////////////////////
EnemyAction::EnemyAction( Enemy* pEnemy )
{
	_ASSERT( pEnemy );

	m_pEnemy		= pEnemy;
	m_pParam		= NULL;
	m_bDelete		= false;
	m_FrameCount	= 0;
}
EnemyAction::~EnemyAction()
{
}

//******************************************************************************
//	������
//******************************************************************************
void EnemyAction::Init( const ENEMY_ACTION* pParam )
{
	_ASSERT( pParam );

	m_pParam		= pParam;
	m_FrameCount	= 0;

	//	�h����
	_OnInit();
}

//******************************************************************************
//	�X�V
//******************************************************************************
void EnemyAction::Update( void )
{
	if( m_bDelete ) return;

	//	�h����
	_OnUpdate();

	//	�t���[���J�E���g
	m_FrameCount++;
	if( m_pParam->Time != 0 &&
		m_pParam->Time <= m_FrameCount )
	{
		Delete();
	}
}
