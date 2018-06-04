#pragma once

#include "EnemyTypes.h"

class World;
class Enemy;
class EnemyData;
class EnemyIndicate;


////////////////////////////////////////////////////////////////////////////////
//
//	ìGä«óù
//
////////////////////////////////////////////////////////////////////////////////
class EnemyManager
{
public:
	EnemyManager( World* pWorld );
	~EnemyManager();

	void				Update( void );
	void				Draw( void );

	Enemy*				AddEnemy( const EnemyData* pData, const Matrix3& Mat, Label Name );
	Enemy*				GetEnemy( Label Name )			const;
	Enemy*				GetSpearTarget( Point Pos )		const;

	void				DestroyAllEnemy( void );
	void				EraseEnemy( Label Name );
	void				EraseAllEnemy( void );

	bool				IsDefeat( Label Name )			const;
	bool				IsDefeatAll( void )				const;
	bool				IsEmpty( void )					const;

	World*				GetWorld( void )						{ return m_pWorld; }
	EnemyIndicate*		GetIndicate( void )						{ return m_pIndicate; }
	Rand&				GetRand( void )							{ return m_Rand; }

private:
	typedef std::list< Enemy* >				EnemyList;

private:
	World*				m_pWorld;
	EnemyIndicate*		m_pIndicate;
	EnemyList			m_EnemyList;
	XORRand				m_Rand;
};