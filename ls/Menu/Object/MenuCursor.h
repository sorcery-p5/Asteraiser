#pragma once

#include "../MenuTypes.h"

class MenuLayer;
class SpriteFrame;


////////////////////////////////////////////////////////////////////////////////
//
//	メニューカーソル
//
////////////////////////////////////////////////////////////////////////////////
class MenuCursor
{
public:
	MenuCursor( MenuLayer* pParent );
	~MenuCursor();

	void				Init( const MENU_CURSOR* pParam, Label Root );
	void				Update( void );
	void				UpdateCurrent( void );

	void				SelectObject( Label Name, bool bSound );
	Label				GetSelectObject( void )							const	{ return m_SelectObject; }

private:
	Point				_GetSelectPos( void )							const;

private:
	MenuLayer*			m_pParent;
	const MENU_CURSOR*	m_pParam;

	Point				m_Pos;
	Label				m_SelectObject;
};