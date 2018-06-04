#pragma once

#include "Scene.h"
#include "Menu/MenuListener.h"

class MenuResource;
class Menu;
class MenuObject;


///////////////////////////////////////////////////////////////////////////////
//
//	タイトルシーン
//
///////////////////////////////////////////////////////////////////////////////
class TitleScene : public Scene, public MenuListener
{
public:
	TitleScene( Label Name, Label PrevScene = Label() );
	virtual ~TitleScene();

	virtual void				OnInit( void );
	virtual void				Update( void );

private:
	virtual void				OnObjectNotify( MenuObject* pObject, Label Name, float Param );

	void						_UpdateMenuEnable( void );
	void						_UpdateDemoCount( void );
	void						_ChangeDemoScene( void );

private:
	MenuResource*				m_pMenuRes;
	Menu*						m_pMenu;
	
	Label						m_SelectNotify;
	Label						m_PrevScene;

	int							m_DemoCount;
};