#include "stdafx.h"
#include "EnemyAction.h"


////////////////////////////////////////////////////////////////////////////////
//
//	敵アクション（基底）
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
//	初期化
//******************************************************************************
void EnemyAction::Init( const ENEMY_ACTION* pParam )
{
	_ASSERT( pParam );

	m_pParam		= pParam;
	m_FrameCount	= 0;

	//	派生先
	_OnInit();
}

//******************************************************************************
//	更新
//******************************************************************************
void EnemyAction::Update( void )
{
	if( m_bDelete ) return;

	//	派生先
	_OnUpdate();

	//	フレームカウント
	m_FrameCount++;
	if( m_pParam->Time != 0 &&
		m_pParam->Time <= m_FrameCount )
	{
		Delete();
	}
}
