#pragma once

#include "Controller.h"

class ReplayManager;


////////////////////////////////////////////////////////////////////////////////
//
//	���v���C����R���g���[��
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