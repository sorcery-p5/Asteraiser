#pragma once

#include "StageAction.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�X�e�[�W�A�N�V�����F���@�̈ړ�
//
////////////////////////////////////////////////////////////////////////////////
class StagePlayerMoveAction : public StageAction
{
public:
	StagePlayerMoveAction( Stage* pParent );
	virtual ~StagePlayerMoveAction( void );

protected:
	virtual void			_OnInit( void );
	virtual void			_OnUpdate( void );

private:
	Point					m_InitPos;
};