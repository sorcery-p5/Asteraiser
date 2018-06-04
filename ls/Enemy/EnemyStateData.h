#pragma once

#include "EnemyTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	敵ステートデータ
//
////////////////////////////////////////////////////////////////////////////////
class EnemyStateData
{
public:
	EnemyStateData( void );
	~EnemyStateData();

	void					AddAction( ENEMY_ACTION* pAction );

	const ENEMY_ACTION*		GetActionParam( int Index )			const;
	int						GetActionNum( void )				const	{ return (int)m_vAction.size(); }

private:
	typedef std::vector<ENEMY_ACTION*>	VecAction;

private:
	VecAction				m_vAction;
};