#pragma once

#include "Scene.h"

class World;
class WorldResource;


///////////////////////////////////////////////////////////////////////////////
//
//	ワールドシーン
//
///////////////////////////////////////////////////////////////////////////////
class WorldScene : public Scene
{
public:
	WorldScene( Label Name, pstr pPath );
	virtual ~WorldScene();

	virtual void			OnInit( void );
	virtual void			Update( void );

	virtual void			OnException( pstr pDirPath );

private:
	WorldResource*			m_pWorldRes;
	World*					m_pWorld;

	std::string				m_Path;
};