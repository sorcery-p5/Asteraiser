#pragma once

#include "Controller.h"

class ReplayManager;


////////////////////////////////////////////////////////////////////////////////
//
//	手動操作コントローラ
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