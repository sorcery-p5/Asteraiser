#pragma once

#include "Scene.h"

class World;
class WorldResource;


///////////////////////////////////////////////////////////////////////////////
//
//	�`���[�g���A���V�[��
//
///////////////////////////////////////////////////////////////////////////////
class TutorialScene : public Scene
{
public:
	TutorialScene( Label Name );
	virtual ~TutorialScene();

	virtual void			OnInit( void );
	virtual void			Update( void );

private:
	WorldResource*			m_pWorldRes;
	World*					m_pWorld;
};