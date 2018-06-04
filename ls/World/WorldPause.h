#pragma once

#include "Menu/MenuListener.h"

class World;
class MenuData;
class Menu;


///////////////////////////////////////////////////////////////////////////////
//
//	ワールド：ポーズ管理
//
///////////////////////////////////////////////////////////////////////////////
class WorldPause : public MenuListener
{
public:
	WorldPause( World* pWorld );
	virtual ~WorldPause();

	void				Init( const MenuData* pMenuData, bool bPauseExit );
	void				Update( void );
	void				Draw( void );

	bool				IsPause( void )								const	{ return m_bPause; }

	virtual void		OnLayerNotify( MenuLayer* pLayer, Label Name, float Param );
	virtual void		OnObjectNotify( MenuObject* pObject, Label Name, float Param );

private:
	World*				m_pWorld;
	Menu*				m_pMenu;
	bool				m_bPauseExit;

	bool				m_bPause;
	bool				m_bHide;
};