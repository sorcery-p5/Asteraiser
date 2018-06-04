#pragma once

#include "MenuTypes.h"
#include "Indicate/IndicateBase.h"

class Menu;
class MenuCursor;
class MenuObject;
class SpriteFrame;


////////////////////////////////////////////////////////////////////////////////
//
//	メニューレイヤー
//
////////////////////////////////////////////////////////////////////////////////
class MenuLayer : public IndicateBase
{
public:
	MenuLayer( Menu* pParent );
	virtual ~MenuLayer();

	void								Init( const MENU_LAYER* pParam );
	virtual void						Update( void );
	void								UpdateCurrent( void );
	virtual void						Draw( const Matrix3& OffsetMat );

	void								Delete( void )										{ m_bDelete = true; }
	void								Close( void );
	void								OnMoveCurrent( void );
	void								OnMoveBack( void );

	bool								IsDelete( void )							const	{ return m_bDelete; }
	bool								IsCurrent( void )							const;
	bool								IsWait( void )								const	{ return m_State == STATE_WAIT; }
	Label								GetName( void )								const	{ return m_pParam->Name; }
	Menu*								GetParent( void )							const	{ return m_pParent; }
	SpriteFrame*						GetSprite( void )							const	{ return m_pSprite; }

	bool								IsVisible( void )							const	{ return m_bVisible; }
	void								SetVisible( bool bVisible )							{ m_bVisible = bVisible; UpdateVisible(); }
	void								UpdateVisible( void );

	MenuObject*							GetObject( Label Name )						const;
	MenuObject*							GetSelectObject( void )						const;
	void								SelectObject( Label Name );
	void								SetObjectSelect( Label Name, Label Value );
	void								SetObjectValue( Label Name, float Value );
	float								GetObjectValue( Label Name )				const;
	void								SetObjectEnable( Label Name, bool bEnable );

	void								UpdateHelpText( void );
	void								ReplaceHelpText( pstr pText );

protected:
	virtual SpriteFrame*				_GetSprite( void )							const	{ return m_pSprite; }
	virtual const Piece*				_GetPiece( Label Name )						const;
	virtual const Texture*				_GetTex( Label Name )						const;
	virtual const INDICATE_MARKER*		_GetMarker( Label BoneName )				const;
	virtual const INDICATE_SPIN*		_GetSpin( Label BoneName )					const;
	virtual const INDICATE_LINE_GRAPH*	_GetLineGraph( Label BoneName )				const;

private:	
	enum STATE
	{
		STATE_OPEN,
		STATE_WAIT,
		STATE_CLOSE,
	};

	typedef std::map< Label, MenuObject* >	MenuObjectMap;

private:
	void							_UpdateTimeEvent( void );
	void							_ExecActionList( const MENU_ACTION_VEC& vAction );
	void							_ExecAction( const MENU_ACTION& Action );
	MenuObject*						_CreateObject( const MENU_OBJECT& Param );
	void							_UpdateHelpTextLength( void );

private:
	Menu*				m_pParent;
	const MENU_LAYER*	m_pParam;

	SpriteFrame*		m_pSprite;
	MenuCursor*			m_pCursor;
	MenuObjectMap		m_mapObject;

	bool				m_bDelete;
	bool				m_bVisible;
	STATE				m_State;
	int					m_FrameCount;

	std::string			m_HelpText;
	std::string			m_ReplaceHelpText;
	int					m_HelpTextLength;
};