#pragma once

#include "../EnemyTypes.h"

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

	void					Init( void );
	void					Update( void );

	void					ChangeHandlerState( Label Name );

	Label					GetCurStateName( void )								const	{ return m_CurStateName; }
	const Point*			GetMoveTarget( void )								const;
	const Point*			GetRotTarget( void )								const;

private:
	typedef std::list<EnemyAction*>	ActionList;
	typedef std::vector<bool>		VecBool;

private:
	void					_ChangeState( Label Name );
	void					_SetNextState( Label Name );
	void					_UpdateExecAction( const EnemyStateData* pStateData, VecBool& vExecAction );
	void					_ExecAction( const ENEMY_ACTION* pParam );
	void					_DeleteAction( ENEMY_ACTION_TYPE Type );
	void					_DeleteActionAll( void );
	bool					_IsEndAction( ENEMY_ACTION_TYPE Type )								const;
	bool					_CheckCondition( const ENEMY_CONDITION_LIST& ConditionList )		const;
	bool					_CheckCondition( const ENEMY_CONDITION& Condition )					const;

private:
	Enemy*					m_pEnemy;
	const EnemyData*		m_pData;
	const EnemyStateData*	m_pCurState;
	const EnemyStateData*	m_pResidentState;
	Label					m_CurStateName;
	Label					m_NextStateName;
	int						m_StateCount;

	ActionList				m_ActionList;
	VecBool					m_vExecAction;
	VecBool					m_vExecResidentAction;
};