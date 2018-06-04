#include "stdafx.h"
#include "EnemyStateData.h"


////////////////////////////////////////////////////////////////////////////////
//
//	敵ステートデータ
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
//	アクションパラメータの追加
//******************************************************************************
void EnemyStateData::AddAction( ENEMY_ACTION* pAction )
{
	m_vAction.push_back( pAction );
}

//******************************************************************************
//	アクションパラメータ取得
//******************************************************************************
const ENEMY_ACTION* EnemyStateData::GetActionParam( int Index ) const
{
	_ASSERT( Index < GetActionNum() );

	return m_vAction[Index];
}