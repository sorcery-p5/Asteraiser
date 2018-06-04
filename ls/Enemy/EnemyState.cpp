#include "stdafx.h"
#include "EnemyState.h"

#include "Enemy.h"


////////////////////////////////////////////////////////////////////////////////
//
//	敵ステート
//
////////////////////////////////////////////////////////////////////////////////
EnemyState::EnemyState( Enemy* pEnemy )
{
	_ASSERT( pEnemy && pEnemy->GetData() );

	m_pEnemy		= pEnemy;
	m_pData			= pEnemy->GetData();
	m_pCurState		= NULL;
	m_StateCount	= 0;
}
EnemyState::~EnemyState()
{
	DeleteVec( m_vAction );
}

//******************************************************************************
//	初期化
//******************************************************************************
void EnemyState::Init( void )
{
	
}

//******************************************************************************
//	更新
//******************************************************************************
void EnemyState::Update( void )
{

}