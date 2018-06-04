#include "stdafx.h"
#include "EnemyStateData.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�X�e�[�g�f�[�^
//
////////////////////////////////////////////////////////////////////////////////
EnemyStateData::EnemyStateData( void )
{

}
EnemyStateData::~EnemyStateData()
{
	DeleteVec( m_vAction );
}

//******************************************************************************
//	�A�N�V�����p�����[�^�̒ǉ�
//******************************************************************************
void EnemyStateData::AddAction( ENEMY_ACTION* pAction )
{
	m_vAction.push_back( pAction );
}

//******************************************************************************
//	�A�N�V�����p�����[�^�擾
//******************************************************************************
const ENEMY_ACTION* EnemyStateData::GetActionParam( int Index ) const
{
	_ASSERT( Index < GetActionNum() );

	return m_vAction[Index];
}