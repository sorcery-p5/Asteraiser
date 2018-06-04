#pragma once

#include "../MenuTypes.h"

class Menu;
class MenuLayer;
class SpriteFrame;


////////////////////////////////////////////////////////////////////////////////
//
//	メニューオブジェクト
//
////////////////////////////////////////////////////////////////////////////////
class MenuObject
{
public:
	MenuObject( MenuLayer* pParent );
	virtual ~MenuObject();

	void				Init( const MENU_OBJECT* pParam );
	void				Update( void );
	void				UpdateSelect( void );

	bool				IsSelect( void )									const;
	Label				GetName( void )										const	{ return m_pParam->BoneName; }
	Label				GetDirObjectName( MENU_DIR Dir )					const	{ return m_pParam->DirBoneName[Dir]; }
	pstr				GetHelpText( void )									const	{ return m_pParam->HelpText.c_str(); }
	MenuLayer*			GetParent( void )									const	{ return m_pParent; }
	Menu*				GetMenu( void )										const;
	bool				IsEnable( void )									const	{ return m_bEnable; }

	void				SetEnable( bool bEnable )									{ m_bEnable = bEnable; }

	virtual void		OnInit( void )												{}
	virtual void		OnUpdate( void )											{}
	virtual void		OnUpdateSelect( void )										{}
	virtual void		Draw( const Matrix3& OffsetMat )							{}
	virtual void		OnChangeSelect( MENU_DIR Dir )								{}
	virtual bool		IsMoveEnable( MENU_DIR Dir )								{ return true; }
	virtual bool		IsCursorVisible( void )										{ return true; }

	virtual float		GetValue( void )									const	{ return 0.0f; }
	virtual void		SetValue( float Value )										{}
	virtual void		SetMinMax( float MinVal, float MaxVal )						{}
	virtual Label		GetSelectName( void )								const	{ return Label(); }
	virtual void		SetSelectName( Label Name )									{}
	virtual int			GetSelectIndex( void )								const	{ return 0; }
	virtual void		SetSelectIndex( int Index )									{}
	virtual int			GetCellNum( void )									const	{ return 0; }
	virtual void		SetCellNum( int Num )										{}
	virtual int			GetCellTopIndex( MENU_SELECT_DIR Dir )				const	{ return 0; }
	virtual int			GetCellNum( MENU_SELECT_DIR Dir )					const	{ return 0; }
	virtual int			GetListNum( MENU_SELECT_DIR Dir )					const	{ return 0; }
	virtual void		SetCellText( int Index, Label Bone, pstr pText, ... )		{}
	virtual void		SetCellColor( int Index, Label Bone, Color Col )			{}

protected:
	SpriteFrame*		_GetSprite( void )									const;
	const MENU_OBJECT*	_GetParam( void )									const	{ return m_pParam; }

private:
	void				_ExecAction( const MENU_ACTION& Action );

private:
	MenuLayer*			m_pParent;
	const MENU_OBJECT*	m_pParam;
	bool				m_bEnable;
	int					m_FrameCount;
};