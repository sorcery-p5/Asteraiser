#include "P5.h"


//	�E�B���h�E�R�[���o�b�N
BOOL	CALLBACK	CollectWindowPos( HWND hwnd, LPARAM lparam );
BOOL	CALLBACK	RestoreWindowPos( HWND hwnd, LPARAM lparam );
LRESULT CALLBACK	WindowProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

//	�E�B���h�E�ʒu�̊i�[
std::map<HWND, WINDOWPLACEMENT>	s_mapWindow;

//	�E�B���h�E�X�^�C��
const DWORD	s_WindowedStyle		= ( WS_OVERLAPPED | WS_CAPTION | WS_BORDER | WS_SYSMENU );
const DWORD	s_FullScreenStyle	= ( WS_POPUP | WS_VISIBLE );


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	�E�B���h�E
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
	//	�E�B���h�E���C��
	if( m_bFull ) _RestoreWindow();
}

//*****************************************************************************
//	������
//*****************************************************************************
bool Window::Init( const INIT_PARAM& InitParam )
{
	Log( "�@���E�B���h�E�쐬" );

	m_Name		= InitParam.pName;
	m_Size		= InitParam.Size;
	m_bFull		= InitParam.bFull;

	//	�E�B���h�E������
	if( !_CreateWindow( InitParam ) )
	{
		Log( "�@�~�E�B���h�E�쐬���s" );
		return false;
	}

	//	�E�B���h�E���i�[
	if( m_bFull ) _CollectWindow();

	Log( "�@���E�B���h�E�쐬����" );
	Log( "�@----------------------------------------" );

	return true;
}

//*****************************************************************************
//	�X�V
//*****************************************************************************
bool Window::Update( void )
{
	//	�E�B���h�E���b�Z�[�W����
	if( !_ProcessMsg() )
	{
		return false;
	}

	//	�V�X�e���X�V
	_UpdateSystem();

	return true;
}

//*****************************************************************************
//	�E�B���h�E�̕\��
//*****************************************************************************
void Window::Show( void )
{
	//	�E�B���h�E�X�^�C����ݒ�
	::SetWindowLong( m_Hwnd, GWL_EXSTYLE,	IsWindowed()? WS_EX_OVERLAPPEDWINDOW : WS_EX_TOPMOST );
	::SetWindowLong( m_Hwnd, GWL_STYLE,		IsWindowed()? s_WindowedStyle : s_FullScreenStyle );

	//	�E�B���h�E�̃T�C�Y������
	RECT rc = { 0, 0, (int)m_Size.x, (int)m_Size.y };
	::AdjustWindowRectEx( &rc, ::GetWindowLong(m_Hwnd, GWL_STYLE), GetMenu(m_Hwnd) != NULL, ::GetWindowLong(m_Hwnd, GWL_EXSTYLE) );
	
	//	�E�B���h�E���[�h�̏ꍇ�A�E�B���h�E�̏ꏊ�����߂�
	int x = 0, y = 0;
	if( IsWindowed() )
	{
		x = ( ::GetSystemMetrics(SM_CXSCREEN) - rc.right )  / 2;
		y = ( ::GetSystemMetrics(SM_CYSCREEN) - rc.bottom ) / 2;
	}

	//	�E�B���h�E�̃T�C�Y�ύX
	::SetWindowPos( m_Hwnd, IsWindowed()? HWND_NOTOPMOST : HWND_TOPMOST, x, y, rc.right - rc.left, rc.bottom - rc.top, 0 );

	//	�E�B���h�E��\��
	::ShowWindow( m_Hwnd, SW_RESTORE );
	::UpdateWindow( m_Hwnd );

	//	�h���b�O�h���b�v�󂯕t��
	::DragAcceptFiles( m_Hwnd, TRUE );
}

//-----------------------------------------------------------------------------
//	�E�B���h�E�̐���
//-----------------------------------------------------------------------------
bool Window::_CreateWindow( const INIT_PARAM& InitParam )
{
	//	�E�B���h�E�N���X�̓o�^
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

	//	�E�B���h�E�쐬
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
//	�E�B���h�E���b�Z�[�W�̏���
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

		//	�I�����b�Z�[�W���󂯎�����ꍇ�A
		if( msg.message == WM_QUIT ) return false;

		//	���b�Z�[�W�̑��o
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return true;
}

//-----------------------------------------------------------------------------
//	�V�X�e���X�V
//-----------------------------------------------------------------------------
void Window::_UpdateSystem( void )
{
	P5::Graphix* pGraphix = P5::GetGraphix();

	//	�X�N���[�����[�h�`�F���W
	if( m_bScreenChange ||
		GetAsyncKeyState( VK_MENU ) && GetAsyncKeyState( VK_RETURN ) )
	{
		_ChangeScreenMode();
		m_bScreenChange = false;
	}
	
	//	�X�N���[���V���b�g
	if( GetAsyncKeyState( VK_SNAPSHOT ) )
	{
		pGraphix->GetDevice()->SaveScreenShot();
	}
}

//-----------------------------------------------------------------------------
//	�X�N���[�����[�h�ύX
//-----------------------------------------------------------------------------
void Window::_ChangeScreenMode( void )
{
	Graphix* pGraphix = GetGraphix();
	if( !pGraphix->GetDevice()->IsScreenChange() ) return;

	m_bFull = !m_bFull;

	if( m_bFull )
	{
		//	�E�B���h�E�𕜋A���Ă���f�o�C�X�����Z�b�g
		_CollectWindow();
		Show();
		pGraphix->Reset();
	}
	else
	{
		//	�f�o�C�X�����Z�b�g���Ă���E�B���h�E�𕜋A
		pGraphix->Reset();
		_RestoreWindow();
		Show();

		//	�X�N���[���ĕ`��
		::InvalidateRect( NULL, NULL, FALSE );
	}
}

//-----------------------------------------------------------------------------
//	�E�B���h�E���̊i�[
//-----------------------------------------------------------------------------
void Window::_CollectWindow( void )
{
	s_mapWindow.clear();
	::EnumWindows( CollectWindowPos, (LPARAM)m_Hwnd );
}

//-----------------------------------------------------------------------------
//	�E�B���h�E���̊i�[
//-----------------------------------------------------------------------------
void Window::_RestoreWindow( void )
{
	::EnumWindows( RestoreWindowPos, (LPARAM)m_Hwnd );
	s_mapWindow.clear();
}


}



//*****************************************************************************
//	�E�B���h�E���̊i�[
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
//	�E�B���h�E�z�u���𕜋A
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
//	�E�B���h�E�R�[���o�b�N
//*****************************************************************************
LRESULT CALLBACK WindowProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	P5::Window*		pWindow		= P5::GetWindow();
	_ASSERT( pWindow );

	switch( msg )
	{
	//	�A�N�e�B�u�؂�ւ�
	case WM_ACTIVATE:
		pWindow->SetActive( LOWORD(wParam) != WA_INACTIVE );
		break;

	//	�E�B���h�E�j��
	case WM_DESTROY:
		::PostQuitMessage( 0 );
		return 0;

	//	�h���b�O�h���b�v
	case WM_DROPFILES:
		{
			HDROP			hDrop	= (HDROP)wParam;
			P5::String256	Path;
			
			::DragQueryFile( hDrop, 0, Path.c_str(), Path.Size() );
			pWindow->SetDragPath( Path.c_str() );

			::DragFinish(hDrop);
		}
		break;

	//	�L�[�_�E��
	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_MENU:		//	ALT�L�[�Ŏ~�߂Ȃ�
			return 0;

	//	case VK_ESCAPE:		//	�G�X�P�[�v�L�[�F�I��
	//		PostMessage(hwnd, WM_CLOSE, 0, 0);
	//		return 0;
		}
		break;

	//	�}�E�X�z�C�[��
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

