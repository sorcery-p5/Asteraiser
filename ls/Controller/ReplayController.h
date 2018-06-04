#pragma once

#include "Controller.h"

class ReplayManager;


////////////////////////////////////////////////////////////////////////////////
//
//	リプレイ操作コントローラ
//
////////////////////////////////////////////////////////////////////////////////
class ReplayController : public Controller
{
public:
	ReplayController( ReplayManager* pReplayManager );
	virtual ~ReplayController();

	virtual void			Update( uint FrameCount );

private:
	ReplayManager*			m_pReplayManager;
};