#pragma once

#include "EnemyTypes.h"

class Enemy;
class EnemyData;
class EnemyStateData;
class EnemyAction;


////////////////////////////////////////////////////////////////////////////////
//
//	敵ステート
//
////////////////////////////////////////////////////////////////////////////////
class EnemyState
{
public:
	EnemyState( Enemy* pEnemy );
	~EnemyState();

	void				Init( void );
	void				Update( void );

private:
	typedef std::vector<EnemyAction*>	VecAction;

private:

private:
	Enemy*				m_pEnemy;
	EnemyData*			m_pData;
	EnemyStateData*		m_pCurState;
	Label				m_CurStateName;
	uint				m_StateCount;
	
	VecAction			m_vAction;
};