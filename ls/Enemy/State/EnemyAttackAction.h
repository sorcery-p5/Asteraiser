#pragma once

#include "EnemyAction.h"

class EnemyTarget;


////////////////////////////////////////////////////////////////////////////////
//
//	敵：攻撃アクション
//
////////////////////////////////////////////////////////////////////////////////
class EnemyAttackAction : public EnemyAction
{
public:
	EnemyAttackAction( Enemy* pEnemy );
	~EnemyAttackAction();

	virtual ENEMY_ACTION_TYPE	GetType( void )				const	{ return ENEMY_ACTION_ATTACK; }
	virtual bool				IsActionEnd( void )			const	{ return false; }

protected:
	virtual void				_OnInit( void );
	virtual void				_OnUpdate( void );

private:
	void						_Attack( Label Name, const Matrix3& Mat );
	Point						_GetAttackPos( void );
	float						_GetAttackRot( Point Pos );

private:
	EnemyTarget*				m_pAppearTarget;
	EnemyTarget*				m_pDirTarget;

	int							m_AppearNum;
};