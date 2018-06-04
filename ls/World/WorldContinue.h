#pragma once

#include "Menu/MenuListener.h"

class World;
class MenuData;
class Menu;


///////////////////////////////////////////////////////////////////////////////
//
//	ワールド：コンティニュー管理
//
///////////////////////////////////////////////////////////////////////////////
class WorldContinue : public MenuListener
{
public:
	WorldContinue( World* pWorld );
	virtual ~WorldContinue();

	void				Init( const MenuData* pMenuData, int Credit );
	void				Open( void );
	void				AutoDecide( void );

	void				Update( void );
	void				Draw( void );

	bool				IsCountinueEnable( void )					const	{ return m_Credit > 0; }
	int					GetContinueCount( void )					const	{ return m_Credit - m_InitCredit; }
	bool				IsActive( void )							const	{ return m_bActive; }

	virtual void		OnLayerNotify( MenuLayer* pLayer, Label Name, float Param );
	virtual void		OnObjectNotify( MenuObject* pObject, Label Name, float Param );

private:
	World*				m_pWorld;
	Menu*				m_pMenu;

	int					m_Credit;
	int					m_InitCredit;
	bool				m_bActive;
};