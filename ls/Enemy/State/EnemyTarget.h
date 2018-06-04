#pragma once

#include "../EnemyTypes.h"

class Enemy;


////////////////////////////////////////////////////////////////////////////////
//
//	ìGÅFñ⁄ïWà íu
//
////////////////////////////////////////////////////////////////////////////////
class EnemyTarget
{
public:
	EnemyTarget( Enemy* pEnemy );
	~EnemyTarget();

	void					Init( const ENEMY_TARGET* pParam, Point InitPos );
	void					Update( void );

	const Point&			GetPos( void )				const	{ return m_Pos; }
	bool					IsUpdatePos( void )			const	{ return m_FrameCount == 0; }

private:
	void					_UpdatePos( void );
	Point					_GetOffsetPos( void )		const;

private:
	Enemy*					m_pEnemy;
	const ENEMY_TARGET*		m_pParam;

	Point					m_Pos;
	Point					m_OffsetPos;
	int						m_FrameCount;
};


