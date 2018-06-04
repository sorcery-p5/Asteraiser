#pragma once

#include "Scene.h"
#include "Menu/MenuListener.h"

class MenuResource;
class Menu;
class MenuObject;
class ScoreResource;


///////////////////////////////////////////////////////////////////////////////
//
//	プレイ記録シーン
//
///////////////////////////////////////////////////////////////////////////////
class RecordScene : public Scene, public MenuListener
{
public:
	RecordScene( Label Name );
	virtual ~RecordScene();

	virtual void				OnInit( void );
	virtual void				Update( void );

private:
	void						_UpdateMenu( void );

private:
	MenuResource*				m_pMenuRes;
	Menu*						m_pMenu;
	ScoreResource*				m_pScoreRes;
};