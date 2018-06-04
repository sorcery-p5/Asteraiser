#pragma once

#include "Controller.h"

class ReplayManager;


////////////////////////////////////////////////////////////////////////////////
//
//	�蓮����R���g���[��
//
////////////////////////////////////////////////////////////////////////////////
class ManualController : public Controller
{
public:
	ManualController( ReplayManager* pReplayManager );
	virtual ~ManualController();

	virtual void			Update( uint FrameCount );

private:
	ReplayManager*			m_pReplayManager;
};