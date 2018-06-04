#pragma once

#include "Controller.h"

class ReplayManager;


////////////////////////////////////////////////////////////////////////////////
//
//	è“®‘€ìƒRƒ“ƒgƒ[ƒ‰
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