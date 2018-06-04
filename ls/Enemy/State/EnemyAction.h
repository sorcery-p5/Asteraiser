#pragma once

#include "../EnemyTypes.h"

class Enemy;


////////////////////////////////////////////////////////////////////////////////
//
//	敵アクション（基底）
//
////////////////////////////////////////////////////////////////////////////////
class EnemyAction
{
public:
	EnemyAction( Enemy* pEnemy );
	virtual ~EnemyAction();

	void						Init( const ENEMY_ACTION* pParam );
	void						Update( void );
	void						Delete( void )								{ m_bDelete = true; }

	bool						IsDelete( void )					const	{ return m_bDelete; }
	bool						IsContinue( void )					const	{ return m_pParam->bContinue; }

	virtual ENEMY_ACTION_TYPE	GetType( void )						const	= 0;
	virtual bool				IsActionEnd( void )					const	= 0;
	virtual	const Point*		GetTargetPos( void )				const	{ return NULL; }

protected:
	virtual void				_OnInit( void )								= 0;
	virtual void				_OnUpdate( void )							= 0;

	int							_GetFrameCount( void )				const	{ return m_FrameCount; }

protected:
	Enemy*						m_pEnemy;
	const ENEMY_ACTION*			m_pParam;

private:
	bool						m_bDelete;
	int							m_FrameCount;
};