#pragma once

#include "Menu/MenuListener.h"
#include "Score/ScoreTypes.h"

class World;
class MenuData;
class Menu;
class MenuLayer;


///////////////////////////////////////////////////////////////////////////////
//
//	ワールド：ステージリザルト
//
///////////////////////////////////////////////////////////////////////////////
class WorldResult : public MenuListener
{
public:
	WorldResult( World* pWorld );
	virtual ~WorldResult();

	void				Init( const MenuData* pMenuData, const SoundFile* pScoreSound );
	void				Open( void );

	void				Update( void );
	void				Draw( void );

	bool				IsActive( void )							const	{ return m_bActive; }

	virtual void		OnLayerNotify( MenuLayer* pLayer, Label Name, float Param );

private:
	enum STATE
	{
		STATE_NONE,
		STATE_OPEN,
		STATE_SCORE,
		STATE_ITEM,
		STATE_DESTROY,
		STATE_TIME,
		STATE_RATE,
		STATE_HEAT,
		STATE_GRAPH,
		STATE_DAMAGE,
		STATE_TOTAL,
		STATE_COMPLETE,
	};

private:
	void				_UpdateIndicate( MenuLayer* pLayer );
	void				_UpdateBGM( MenuLayer* pLayer );

private:
	World*				m_pWorld;
	Menu*				m_pMenu;

	const SoundFile*	m_pScoreSound;
	int					m_SoundInterval;

	bool				m_bActive;
	STATE				m_State;
	SCORE_STAGE_RECORD	m_Score;
	SCORE_STAGE_RECORD	m_CurScore;
	uint				m_TotalScore;
	FloatVec			m_vRate;
	FloatVec			m_vHeat;
};