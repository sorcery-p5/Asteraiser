#include "stdafx.h"
#include "EnemyState.h"

#include "Enemy.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�X�e�[�g
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
//	������
//******************************************************************************
void EnemyState::Init( void )
{
	
}

//******************************************************************************
//	�X�V
//******************************************************************************
void EnemyState::Update( void )
{

}