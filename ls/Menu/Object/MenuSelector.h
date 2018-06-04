#pragma once

#include "MenuObject.h"


////////////////////////////////////////////////////////////////////////////////
//
//	メニューセレクター
//
////////////////////////////////////////////////////////////////////////////////
class MenuSelector : public MenuObject
{
public:
	MenuSelector( MenuLayer* pParent );
	virtual ~MenuSelector();

	virtual void			OnInit( void );
	virtual void			OnUpdate( void );
	virtual void			OnUpdateSelect( void );

	virtual float			GetValue( void )					const;
	virtual void			SetValue( float Value );
	virtual Label			GetSelectName( void )				const;
	virtual void			SetSelectName( Label Name );
	virtual int				GetSelectIndex( void )				const	{ return m_SelectIndex; }
	virtual void			SetSelectIndex( int Index )					{ m_SelectIndex = Index; }

private:
	Point					_GetSelectPos()						const;

private:
	const MENU_SELECTOR*	m_pSelectorParam;
	Point					m_CursorPos;
	int						m_SelectIndex;
};