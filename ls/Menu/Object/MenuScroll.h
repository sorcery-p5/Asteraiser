#pragma once

#include "MenuObject.h"


////////////////////////////////////////////////////////////////////////////////
//
//	メニュースクロール
//
////////////////////////////////////////////////////////////////////////////////
class MenuScroll : public MenuObject
{
public:
	MenuScroll( MenuLayer* pParent );
	virtual ~MenuScroll();

	virtual void			OnInit( void );
	virtual void			OnUpdate( void );
	virtual void			OnUpdateSelect( void );

private:
	void					_UpdateScroll( void );
	void					_UpdateObject( void );

private:
	const MENU_SCROLL*		m_pScrollParam;
	Point					m_CurPos;
	Point					m_TargetPos;
	Point					m_CurScale;
	Point					m_TargetScale;
};