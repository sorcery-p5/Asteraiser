#include "P5.h"


//	ウィンドウコールバック
BOOL	CALLBACK	CollectWindowPos( HWND hwnd, LPARAM lparam );
BOOL	CALLBACK	RestoreWindowPos( HWND hwnd, LPARAM lparam );
LRESULT CALLBACK	WindowProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

//	ウィンドウ位置の格納
std::map<HWND, WINDOWPLACEMENT>	s_mapWindow;

//	ウィンドウスタイル
const DWORD	s_WindowedStyle		= ( WS_OVERLAPPED | WS_CAPTION | WS_BORDER | WS_SYSMENU );
const DWORD	s_FullScreenStyle	= ( WS_POPUP | WS_VISIBLE );


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	ウィンドウ
//
///////////////////////////////////////////////////////////////////////////////
Window::Window( HINSTANCE Hinstance )
{
	m_Hinstance		= Hinstance;
	m_Hwnd			= NULL;
	
	m_bFull			= false;
	m_bActive		= false;
	m_bScreenChange	= false;
}

Window::~Window()
{
	//	ウィンドウ情報修復
	if( m_bFull ) _RestoreWindow();
}

//*****************************************************************************
//	初期化
//*****************************************************************************
bool Window::Init( const INIT_PARAM& InitParam )
{
	Log( "　□ウィンドウ作成" );

	m_Name		= InitParam.pName;
	m_Size		= InitParam.Size;
	m_bFull		= InitParam.bFull;

	//	ウィンドウ初期化
	if( !_CreateWindow( InitParam ) )
	{
		Log( "　×ウィンドウ作成失敗" );
		return false;
	}

	//	ウィンドウ情報格納
	if( m_bFull ) _CollectWindow();

	Log( "　○ウィンドウ作成完了" );
	Log( "　----------------------------------------" );

	return true;
}

//*****************************************************************************
//	更新
//*****************************************************************************
bool Window::Update( void )
{
	//	ウィンドウメッセージ処理
	if( !_ProcessMsg() )
	{
		return false;
	}

	//	システム更新
	_UpdateSystem();

	return true;
}

//*****************************************************************************
//	ウィンドウの表示
//*****************************************************************************
void Window::Show( void )
{
	//	ウィンドウスタイルを設定
	::SetWindowLong( m_Hwnd, GWL_EXSTYLE,	IsWindowed()? WS_EX_OVERLAPPEDWINDOW : WS_EX_TOPMOST );
	::SetWindowLong( m_Hwnd, GWL_STYLE,		IsWindowed()? s_WindowedStyle : s_FullScreenStyle );

	//	ウィンドウのサイズを決定
	RECT rc = { 0, 0, (int)m_Size.x, (int)m_Size.y };
	::AdjustWindowRectEx( &rc, ::GetWindowLong(m_Hwnd, GWL_STYLE), GetMenu(m_Hwnd) != NULL, ::GetWindowLong(m_Hwnd, GWL_EXSTYLE) );
	
	//	ウィンドウモードの場合、ウィンドウの場所を決める
	int x = 0, y = 0;
	if( IsWindowed() )
	{
		x = ( ::GetSystemMetrics(SM_CXSCREEN) - rc.right )  / 2;
		y = ( ::GetSystemMetrics(SM_CYSCREEN) - rc.bottom ) / 2;
	}

	//	ウィンドウのサイズ変更
	::SetWindowPos( m_Hwnd, IsWindowed()? HWND_NOTOPMOST : HWND_TOPMOST, x, y, rc.right - rc.left, rc.bottom - rc.top, 0 );

	//	ウィンドウを表示
	::ShowWindow( m_Hwnd, SW_RESTORE );
	::UpdateWindow( m_Hwnd );

	//	ドラッグドロップ受け付け
	::DragAcceptFiles( m_Hwnd, TRUE );
}

//-----------------------------------------------------------------------------
//	ウィンドウの生成
//-----------------------------------------------------------------------------
bool Window::_CreateWindow( const INIT_PARAM& InitParam )
{
	//	ウィンドウクラスの登録
	WNDCLASSEX	wc;
	ZeroStruct( wc );
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.lpfnWndProc		= WindowProc;
	wc.hInstance		= m_Hinstance;
	wc.hbrBackground	= (HBRUSH)GetStockObject( BLACK_BRUSH );
	wc.lpszClassName	= m_Name;
	wc.hCursor			= LoadCursor( NULL, IDC_CROSS );
	wc.hIcon			= LoadIcon( m_Hinstance, (LPCSTR)InitParam.IconID );
	wc.hIconSm			= LoadIcon( m_Hinstance, (LPCSTR)InitParam.IconSmallID );

	ATOM ret = RegisterClassEx( &wc );
	if( ret == 0 ) return false;

	//	ウィンドウ作成
	m_Hwnd = ::CreateWindowEx(	IsWindowed()? WS_EX_OVERLAPPEDWINDOW : WS_EX_TOPMOST,
								m_Name,
								m_Name,
								IsWindowed()? s_WindowedStyle : s_FullScreenStyle,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								(int)m_Size.x,
								(int)m_Size.y,
								NULL,
								NULL,
								m_Hinstance,
								NULL );

	if( m_Hwnd == NULL ) return false;

	return true;
}

//-----------------------------------------------------------------------------
//	ウィンドウメッセージの処理
//-----------------------------------------------------------------------------
bool Window::_ProcessMsg( void )
{
	MSG		msg = {0};
	while( !IsActive() || PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
	{
		if( !IsActive() )
		{
			int ret = GetMessage( &msg, NULL, 0, 0 );
			if( ret == -1 ) return false;
		}

		//	終了メッセージを受け取った場合、
		if( msg.message == WM_QUIT ) return false;

		//	メッセージの送出
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return true;
}

//-----------------------------------------------------------------------------
//	システム更新
//-----------------------------------------------------------------------------
void Window::_UpdateSystem( void )
{
	P5::Graphix* pGraphix = P5::GetGraphix();

	//	スクリーンモードチェンジ
	if( m_bScreenChange ||
		GetAsyncKeyState( VK_MENU ) && GetAsyncKeyState( VK_RETURN ) )
	{
		_ChangeScreenMode();
		m_bScreenChange = false;
	}
	
	//	スクリーンショット
	if( GetAsyncKeyState( VK_SNAPSHOT ) )
	{
		pGraphix->GetDevice()->SaveScreenShot();
	}
}

//-----------------------------------------------------------------------------
//	スクリーンモード変更
//-----------------------------------------------------------------------------
void Window::_ChangeScreenMode( void )
{
	Graphix* pGraphix = GetGraphix();
	if( !pGraphix->GetDevice()->IsScreenChange() ) return;

	m_bFull = !m_bFull;

	if( m_bFull )
	{
		//	ウィンドウを復帰してからデバイスをリセット
		_CollectWindow();
		Show();
		pGraphix->Reset();
	}
	else
	{
		//	デバイスをリセットしてからウィンドウを復帰
		pGraphix->Reset();
		_RestoreWindow();
		Show();

		//	スクリーン再描画
		::InvalidateRect( NULL, NULL, FALSE );
	}
}

//-----------------------------------------------------------------------------
//	ウィンドウ情報の格納
//-----------------------------------------------------------------------------
void Window::_CollectWindow( void )
{
	s_mapWindow.clear();
	::EnumWindows( CollectWindowPos, (LPARAM)m_Hwnd );
}

//-----------------------------------------------------------------------------
//	ウィンドウ情報の格納
//-----------------------------------------------------------------------------
void Window::_RestoreWindow( void )
{
	::EnumWindows( RestoreWindowPos, (LPARAM)m_Hwnd );
	s_mapWindow.clear();
}


}



//*****************************************************************************
//	ウィンドウ情報の格納
//*****************************************************************************
BOOL CALLBACK CollectWindowPos( HWND hwnd, LPARAM lparam )
{
	if( IsWindowVisible(hwnd) && (hwnd != (HWND)(lparam)) )
	{
		WINDOWPLACEMENT wp;
		wp.length = sizeof(WINDOWPLACEMENT);
		::GetWindowPlacement( hwnd, &wp );
		s_mapWindow[hwnd] = wp;
	}

	return TRUE;
}

//*****************************************************************************
//	ウィンドウ配置情報を復帰
//*****************************************************************************
BOOL CALLBACK RestoreWindowPos( HWND hwnd, LPARAM lparam )
{
	if( hwnd != (HWND)(lparam) )
	{
		::SetWindowPlacement( hwnd, &s_mapWindow[hwnd] );
	}
	return TRUE;
}


//*****************************************************************************
//	ウィンドウコールバック
//*****************************************************************************
LRESULT CALLBACK WindowProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	P5::Window*		pWindow		= P5::GetWindow();
	_ASSERT( pWindow );

	switch( msg )
	{
	//	アクティブ切り替え
	case WM_ACTIVATE:
		pWindow->SetActive( LOWORD(wParam) != WA_INACTIVE );
		break;

	//	ウィンドウ破棄
	case WM_DESTROY:
		::PostQuitMessage( 0 );
		return 0;

	//	ドラッグドロップ
	case WM_DROPFILES:
		{
			HDROP			hDrop	= (HDROP)wParam;
			P5::String256	Path;
			
			::DragQueryFile( hDrop, 0, Path.c_str(), Path.Size() );
			pWindow->SetDragPath( Path.c_str() );

			::DragFinish(hDrop);
		}
		break;

	//	キーダウン
	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_MENU:		//	ALTキーで止めない
			return 0;

	//	case VK_ESCAPE:		//	エスケープキー：終了
	//		PostMessage(hwnd, WM_CLOSE, 0, 0);
	//		return 0;
		}
		break;

	//	マウスホイール
	case WM_MOUSEWHEEL:
		if( pWindow->IsActive() )
		{
			int Delta = (short)HIWORD( wParam );
			P5::GetSign()->GetMouse()->AddWheel( (Delta > 0)? 1 : -1 );
		}
		break;
	}
	return ::DefWindowProc( hwnd, msg, wParam, lParam );
}

