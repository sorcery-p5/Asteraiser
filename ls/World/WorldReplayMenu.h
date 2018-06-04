#pragma once

#include "Menu/MenuListener.h"

class World;
class MenuData;
class Menu;


///////////////////////////////////////////////////////////////////////////////
//
//	ワールド：リプレイメニュー管理
//
///////////////////////////////////////////////////////////////////////////////
class WorldReplayMenu : public MenuListener
{
public:
	WorldReplayMenu( World* pWorld );
	virtual ~WorldReplayMenu();

	void				Init( const MenuData* pMenuData );
	void				Open( bool bCancelEnable );

	void				Update( void );
	void				Draw( void );

	bool				IsActive( void )							const	{ return m_bActive; }

	virtual void		OnLayerNotify( MenuLayer* pLayer, Label Name, float Param );
	virtual void		OnObjectNotify( MenuObject* pObject, Label Name, float Param );

private:
	World*				m_pWorld;
	Menu*				m_pMenu;

	bool				m_bActive;
	bool				m_bCancelEnable;

	int					m_FrameSkip;
};