#pragma once

#include "EnemyTypes.h"

class Enemy;
class World;
class Indicate;


////////////////////////////////////////////////////////////////////////////////
//
//	敵のインジケート管理
//
////////////////////////////////////////////////////////////////////////////////
class EnemyIndicate
{
public:
	EnemyIndicate( World* pWorld );
	~EnemyIndicate();

	void				Update( void );

	void				AddEnemy( Enemy* pEnemy );

private:
	struct INFO 
	{
		RefPtr<Enemy>	pEnemy;
		int				Life;
		int				DamageLife;
		int				MaxLife;
		Label			IndicateName;
		int				IndicateID;
		bool			bBoss;
		bool			bUp;
		bool			bClose;
	};
	typedef std::list<INFO>		InfoList;

	//	検索用比較関数
	class EnemyComp
	{
	public:
		EnemyComp( const Enemy* pEnemy )		{ m_pEnemy = pEnemy; }
		bool operator()( const INFO& r ) const	{ return r.pEnemy == m_pEnemy; }

		const Enemy* m_pEnemy;
	};
	class IndicateIDComp
	{
	public:
		IndicateIDComp( int ID )				{ IndicateID = ID; }
		bool operator()( const INFO& r ) const	{ return r.IndicateID == IndicateID; }

		int IndicateID;
	};
	class SortComp
	{
	public:
		bool operator()( const INFO& a, const INFO& b ) const
		{
			if( a.bBoss && !b.bBoss ) return true;

			if( a.Life > 0 && b.Life <= 0 ) return true;

			return false;
		}
	};

private:
	int					_GetUnuseIndicateID( void );
	bool				_UpdateInfo( INFO& Info );
	void				_UpdateIndicate( Indicate* pIndicate, const INFO& Info );
	void				_DeleteInfo( INFO& Info );
	void				_UpdatePosition( void );

private:
	World*				m_pWorld;
	InfoList			m_InfoList;
};