#pragma once

#include "ControllerTypes.h"

class Controller;
class World;


////////////////////////////////////////////////////////////////////////////////
//
//	�R���g���[���Ǘ�
//
////////////////////////////////////////////////////////////////////////////////
class ControllerManager
{
public:
	ControllerManager( World* pWorld );
	~ControllerManager();

	void				Init( bool bReplay );
	void				Update( uint FrameCount );

	void				ChangeManual( void );

	const Controller*	GetController( void )				const	{ return m_pController; }

private:
	World*				m_pWorld;
	Controller*			m_pController;

	bool				m_bReplay;
};