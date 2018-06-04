#pragma once

#include "ItemTypes.h"

class Item;
class ItemData;
class World;


////////////////////////////////////////////////////////////////////////////////
//
//	ÉAÉCÉeÉÄä«óù
//
////////////////////////////////////////////////////////////////////////////////
class ItemManager
{
public:
	ItemManager( World* pWorld, const ItemData* pData );
	~ItemManager();

	void				Update( void );
	void				AppearItem( int Num, Point Pos, float Speed );

	void				OnGet( void );
	void				ForceCollect( void );
	void				AutoCollect( void );

	float				GetCollectRadius( void )		const	{ return m_CollectRadius; }
	float				GetCollectTimeRate( void )		const;

	World*				GetWorld( void )						{ return m_pWorld; }
	Rand&				GetRand( void )							{ return m_Rand; }

private:
	typedef std::list< Item* >		ItemList;

private:	
	void				_AddItem( Point Pos, Point Vel );

private:
	World*				m_pWorld;
	const ItemData*		m_pData;

	int					m_CollectTime;
	float				m_CollectRadius;

	ItemList			m_ItemList;
	XORRand				m_Rand;

	bool				m_bPlaySound;
};