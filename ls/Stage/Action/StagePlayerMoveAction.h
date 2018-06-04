#pragma once

#include "StageAction.h"


////////////////////////////////////////////////////////////////////////////////
//
//	ステージアクション：自機の移動
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