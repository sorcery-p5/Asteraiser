#pragma once

#include "ItemTypes.h"

class ItemManager;
class ItemData;
class Effect;
class CollideObject;


////////////////////////////////////////////////////////////////////////////////
//
//	ƒAƒCƒeƒ€
//
////////////////////////////////////////////////////////////////////////////////
class Item
{
public:
	Item( ItemManager* pManager );
	~Item();

	void				Init( const ItemData* pData, Point Pos, Point Vel );
	void				Update( void );

	void				OnGet( void );
	void				Delete( void )													{ m_bDelete = true; }
	void				AutoCollect( void );

	bool				IsDelete( void )										const	{ return m_bDelete; }
	Point				GetPos( void )											const	{ return m_Pos; }
	
private:
	ItemManager*		m_pManager;
	const ItemData*		m_pData;

	RefPtr<Effect>		m_pEffect;
	CollideObject*		m_pCollide;

	int					m_FrameCount;
	bool				m_bDelete;
	bool				m_bAutoCollect;
	Point				m_Pos;
	Point				m_Vel;
	float				m_Gravity;
	float				m_CollectSpeed;
};