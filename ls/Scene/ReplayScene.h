#pragma once

#include "Scene.h"

class World;
class WorldResource;


///////////////////////////////////////////////////////////////////////////////
//
//	リプレイシーン
//
///////////////////////////////////////////////////////////////////////////////
class ReplayScene : public Scene
{
public:
	ReplayScene( Label Name, pstr pWorldPath, pstr pReplayPath, int Stage );
	virtual ~ReplayScene();

	virtual void			OnInit( void );
	virtual void			Update( void );

private:
	WorldResource*			m_pWorldRes;
	World*					m_pWorld;

	std::string				m_WorldPath;
	std::string				m_ReplayPath;
	int						m_InitStage;
};