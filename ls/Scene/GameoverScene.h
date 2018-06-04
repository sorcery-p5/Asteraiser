#pragma once

#include "Scene.h"
#include "Menu/MenuListener.h"
#include "Score/ScoreTypes.h"
#include "SaveData/SaveDataTypes.h"

class MenuResource;
class Menu;
class MenuObject;


///////////////////////////////////////////////////////////////////////////////
//
//	ゲームオーバーシーン
//
///////////////////////////////////////////////////////////////////////////////
class GameoverScene : public Scene
{
public:
	GameoverScene( Label Name, const SAVEDATA_SCORE& Score, const SCORE_STAGE_RECORD_VEC& vRecord );
	virtual ~GameoverScene();

	virtual void				OnInit( void );
	virtual void				Update( void );

private:
	MenuResource*				m_pMenuRes;
	Menu*						m_pMenu;

	SAVEDATA_SCORE				m_Score;
	SCORE_STAGE_RECORD_VEC		m_vRecord;
};