///////////////////////////////////////////////////////////////////////////////
//
//	エラーと警告
//
///////////////////////////////////////////////////////////////////////////////

#include	"P5.h"
#include	"Alert.h"
#include	"HandlePtr.h"
#include	"String.h"

#include	<crtdbg.h>


namespace
{
	std::string	s_LogPath	= "Log.txt";	//	ログファイル名
}

namespace P5
{

//*****************************************************************************
//	ログの初期化
//*****************************************************************************
void InitLog( pstr pPath )
{
	//	パス登録
	s_LogPath = pPath;

	//	ファイル初期化
	FileHandle Handle( s_LogPath.c_str(), "w" );
}

//*****************************************************************************
//	ログの出力
//*****************************************************************************
void Log( pstr pMsg, ... )
{
	String1024 Str;

	Str.FormatV( pMsg, (char*)(&pMsg + 1) );
	Str += "\n";

	FileHandle Handle( s_LogPath.c_str(), "a" );
	if( !Handle ) return;

	fprintf( Handle, Str );
	OutputDebugString( Str );
}

//*****************************************************************************
//	エラー終了
//*****************************************************************************
void ErrorExit( pstr pMsg, ... )
{
	String1024 Str;
	Str.FormatV( pMsg, (char*)(&pMsg + 1) );

	Log( Str );
	MessageBox( NULL, Str, GetWindow()? GetWindow()->GetName() : "Error", MB_ICONERROR | MB_OK );

	if( GetWindow() )
	{
		PostMessage( GetWindow()->GetHwnd(), WM_CLOSE, 0, 0 );
	}
	else
	{
		PostQuitMessage( 0 );
	}
}

};