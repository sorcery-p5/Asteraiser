#pragma once

#include "EnemyAction.h"

class EnemyTarget;


////////////////////////////////////////////////////////////////////////////////
//
//	敵：移動アクション
//
////////////////////////////////////////////////////////////////////////////////
class EnemyMoveAction : public EnemyAction
{
public:
	EnemyMoveAction( Enemy* pEnemy );
	~EnemyMoveAction();

	virtual ENEMY_ACTION_TYPE	GetType( void )				const	{ return ENEMY_ACTION_MOVE; }
	virtual bool				IsActionEnd( void )			const	{ return m_bMoveEnd; }
	virtual	const Point*		GetTargetPos( void )		const;

protected:
	virtual void				_OnInit( void );
	virtual void				_OnUpdate( void );

private:
	float						_GetDecelRange( void )		const;

private:
	EnemyTarget*				m_pTarget;
	float						m_Speed;
	bool						m_bMoveEnd;
};