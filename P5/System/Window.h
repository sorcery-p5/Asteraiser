#pragma once


namespace P5
{

struct INIT_PARAM;

///////////////////////////////////////////////////////////////////////////////
//
//	ウィンドウ
//
///////////////////////////////////////////////////////////////////////////////
class Window
{
public:
	Window( HINSTANCE Hinstance );
	~Window();

	bool			Init( const INIT_PARAM& InitParam );
	bool			Update( void );

	void			Show( void );
	void			ChangeScreenMode( void )			{ m_bScreenChange = true; }
	void			SetActive( bool bActive )			{ m_bActive = bActive; }
	void			SetWindowed( void )					{ m_bFull	= FALSE; }
	void			SetDragPath( pstr pPath )			{ m_DragPath = pPath; }

	HINSTANCE		GetHinstance( void )		const	{ return m_Hinstance; }
	HWND			GetHwnd( void )				const	{ return m_Hwnd; }
	pstr			GetName( void )				const	{ return m_Name; }
	const Point&	GetSize( void )				const	{ return m_Size; }
	bool			IsWindowed( void )			const	{ return !m_bFull; }
	bool			IsActive( void )			const	{ return m_bActive; }
	pstr			GetDragPath( void )			const	{ return m_DragPath.c_str(); }	

private:
	bool			_CreateWindow( const INIT_PARAM& InitParam );
	bool			_ProcessMsg( void );
	void			_UpdateSystem( void );
	void			_ChangeScreenMode( void );
	void			_CollectWindow( void );
	void			_RestoreWindow( void );

private:
	HINSTANCE		m_Hinstance;
	HWND			m_Hwnd;
	
	String256		m_Name;
	Point			m_Size;
	bool			m_bFull;
	bool			m_bActive;
	bool			m_bScreenChange;
	String256		m_DragPath;
};


};