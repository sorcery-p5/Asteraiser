#pragma once

#include "Scene.h"

class World;
class WorldResource;


///////////////////////////////////////////////////////////////////////////////
//
//	�v���N�e�B�X�V�[��
//
///////////////////////////////////////////////////////////////////////////////
class PracticeScene : public Scene
{
public:
	PracticeScene( Label Name, pstr pPath, int Stage, Label State );
	virtual ~PracticeScene();

	virtual void			OnInit( void );
	virtual void			Update( void );

private:
	WorldResource*			m_pWorldRes;
	World*					m_pWorld;

	std::string				m_Path;
	int						m_InitStage;
	Label					m_InitState;
};