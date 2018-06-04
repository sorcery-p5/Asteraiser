#pragma once

#include "Scene.h"
#include "Menu/MenuListener.h"

class MenuResource;
class Menu;
class MenuObject;

class ReplayDirectory;


///////////////////////////////////////////////////////////////////////////////
//
//	リプレイ選択シーン
//
///////////////////////////////////////////////////////////////////////////////
class ReplaySelectScene : public Scene, public MenuListener
{
public:
	ReplaySelectScene( Label Name, pstr pWorldPath, pstr pReplayPath );
	virtual ~ReplaySelectScene();

	virtual void				OnInit( void );
	virtual void				Update( void );

private:
	virtual void				OnOpenLayer( MenuLayer* pLayer );
	virtual void				OnLayerNotify( MenuLayer* pLayer, Label Name, float Param );
	virtual void				OnObjectNotify( MenuObject* pObject, Label Name, float Param );
	virtual void				OnObjectChangeValue( MenuObject* pObject );

	void						_UpdateMenu( void );

private:
	MenuResource*				m_pMenuRes;
	Menu*						m_pMenu;
	
	std::string					m_WorldPath;
	Label						m_SelectNotify;
	
	ReplayDirectory*			m_pDirectory;
	int							m_DirIndex;
	int							m_SelectIndex;
	int							m_SelectStage;
	String256					m_ReplayPath;
};