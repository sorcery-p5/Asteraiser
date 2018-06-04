#pragma once

#include "EnemyAction.h"

class EnemyTarget;


////////////////////////////////////////////////////////////////////////////////
//
//	敵：旋回アクション
//
////////////////////////////////////////////////////////////////////////////////
class EnemyRotAction : public EnemyAction
{
public:
	EnemyRotAction( Enemy* pEnemy );
	~EnemyRotAction();

	virtual ENEMY_ACTION_TYPE	GetType( void )			const	{ return ENEMY_ACTION_ROT; }
	virtual bool				IsActionEnd( void )		const	{ return m_bRotEnd; }
	virtual	const Point*		GetTargetPos( void )	const;

protected:
	virtual void				_OnInit( void );
	virtual void				_OnUpdate( void );

private:
	EnemyTarget*				m_pTarget;
	bool						m_bRotEnd;
};