#include "P5.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	P5
//
///////////////////////////////////////////////////////////////////////////////
Window*			s_pWindow		= NULL;
Graphix*		s_pGraphix		= NULL;
Sign*			s_pSign			= NULL;
Tone*			s_pTone			= NULL;

//*****************************************************************************
//	初期化
//*****************************************************************************
bool Init( HINSTANCE Hinst, const INIT_PARAM& InitParam )
{
	Log( "□システム初期化開始" );

	//	ウィンドウの初期化
	s_pWindow = new Window( Hinst );
	if( !s_pWindow ) return false;
	if( !s_pWindow->Init( InitParam ) ) return false;

	//	グラフィクス初期化
	s_pGraphix = new Graphix;
	if( !s_pGraphix ) return false;
	if( !s_pGraphix->Init( s_pWindow ) ) return false;
	
	//	ウィンドウ表示
	s_pWindow->Show();

	//	サウンド初期化
	s_pTone = new Tone;
	if( !s_pTone ) return false;
	s_pTone->Init();

	//	入力管理初期化
	s_pSign = new Sign;
	if( !s_pSign ) return false;
	if( !s_pSign->Init( Hinst ) ) return false;
	
	Log( "○システム初期化完了" );
	Log( "----------------------------------------" );

	return true;
}

//*****************************************************************************
//	終了
//*****************************************************************************
void Exit( void )
{
	SafeDelete( s_pSign );
	SafeDelete( s_pTone );
	SafeDelete( s_pGraphix );
	SafeDelete( s_pWindow );
}

//*****************************************************************************
//	更新
//*****************************************************************************
bool Update( void )
{
	//	無効
	if( !s_pWindow || !s_pGraphix ) return false;

	//	ウィンドウの更新
	if( !s_pWindow->Update() )
	{
		return false;
	}

	//	グラフィクスの更新
	if( !s_pGraphix->Update() )
	{
		return false;
	}

	//	サウンドの更新
	if( !s_pTone || !s_pTone->Update() )
	{
		return false;
	}

	//	入力管理の更新
	if( !s_pSign || !s_pSign->Update() )
	{
		return false;
	}

	return true;
}

//*****************************************************************************
//	ウィンドウの取得
//*****************************************************************************
Window* GetWindow( void )
{
	_ASSERT( s_pWindow != NULL );
	return s_pWindow;
}

//*****************************************************************************
//	グラフィクスの取得
//*****************************************************************************
Graphix* GetGraphix( void )
{
	_ASSERT( s_pGraphix != NULL );
	return s_pGraphix;
}

//*****************************************************************************
//	入力管理の取得
//***************************************************************************** 
Sign* GetSign( void )
{
	_ASSERT( s_pSign != NULL );
	return s_pSign;
}

//*****************************************************************************
//	サウンド管理の取得
//***************************************************************************** 
Tone* GetTone( void )
{
	_ASSERT( s_pTone != NULL );
	return s_pTone;
}

}