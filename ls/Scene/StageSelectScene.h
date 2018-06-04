#pragma once

#include "Scene.h"
#include "Menu/MenuListener.h"

class MenuResource;
class Menu;
class MenuObject;


///////////////////////////////////////////////////////////////////////////////
//
//	ステージ選択シーン
//
///////////////////////////////////////////////////////////////////////////////
class StageSelectScene : public Scene, public MenuListener
{
public:
	StageSelectScene( Label Name, pstr pWorldPath, int PrevStage, Label PrevState );
	virtual ~StageSelectScene();

	virtual void				OnInit( void );
	virtual void				Update( void );

private:
	virtual void				OnOpenLayer( MenuLayer* pLayer );
	virtual void				OnLayerNotify( MenuLayer* pLayer, Label Name, float Param );
	virtual void				OnObjectNotify( MenuObject* pObject, Label Name, float Param );
	virtual void				OnObjectSelect( MenuObject* pObject, MenuObject* pPrevObject );

	void						_UpdateMenu( void );

	Label						_StageStateToObjName( int Stage, Label State )						const;

private:
	MenuResource*				m_pMenuRes;
	Menu*						m_pMenu;
	
	std::string					m_WorldPath;
	Label						m_SelectNotify;
	int							m_PrevStage;
	Label						m_PrevState;
};