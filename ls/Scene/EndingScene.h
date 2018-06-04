#pragma once

#include "Scene.h"
#include "Menu/MenuListener.h"
#include "Score/ScoreTypes.h"
#include "SaveData/SaveDataTypes.h"

class MenuResource;
class Menu;
class FadeScreen;


///////////////////////////////////////////////////////////////////////////////
//
//	エンディングシーン
//
///////////////////////////////////////////////////////////////////////////////
class EndingScene : public Scene, public MenuListener
{
public:
	EndingScene( Label Name, const SAVEDATA_SCORE& Score, const SCORE_STAGE_RECORD_VEC& vRecord );
	virtual ~EndingScene();

	virtual void				OnInit( void );
	virtual void				Update( void );

	virtual void				OnLayerNotify( MenuLayer* pLayer, Label Name, float Param );

private:
	MenuResource*				m_pMenuRes;
	Menu*						m_pMenu;
	FadeScreen*					m_pFade;

	SAVEDATA_SCORE				m_Score;
	SCORE_STAGE_RECORD_VEC		m_vRecord;
	bool						m_bExit;
};