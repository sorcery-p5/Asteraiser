#pragma once

#include "Scene.h"
#include "Menu/MenuListener.h"
#include "Score/ScoreTypes.h"
#include "SaveData/SaveDataTypes.h"

class MenuResource;
class Menu;
class MenuObject;
class ScoreResource;


///////////////////////////////////////////////////////////////////////////////
//
//	プレイ結果シーン
//
///////////////////////////////////////////////////////////////////////////////
class PlayResultScene : public Scene, public MenuListener
{
public:
	PlayResultScene( Label Name, const SAVEDATA_SCORE& Score, const SCORE_STAGE_RECORD_VEC& vRecord );
	virtual ~PlayResultScene();

	virtual void				OnInit( void );
	virtual void				Update( void );

private:
	void						_UpdateMenu( void );

private:
	MenuResource*				m_pMenuRes;
	Menu*						m_pMenu;
	ScoreResource*				m_pScoreRes;

	SAVEDATA_SCORE				m_Score;
	SCORE_STAGE_RECORD_VEC		m_vRecord;
};