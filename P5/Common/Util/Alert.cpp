///////////////////////////////////////////////////////////////////////////////
//
//	�G���[�ƌx��
//
///////////////////////////////////////////////////////////////////////////////

#include	"P5.h"
#include	"Alert.h"
#include	"HandlePtr.h"
#include	"String.h"

#include	<crtdbg.h>


namespace
{
	std::string	s_LogPath	= "Log.txt";	//	���O�t�@�C����
}

namespace P5
{

//*****************************************************************************
//	���O�̏�����
//*****************************************************************************
void InitLog( pstr pPath )
{
	//	�p�X�o�^
	s_LogPath = pPath;

	//	�t�@�C��������
	FileHandle Handle( s_LogPath.c_str(), "w" );
}

//*****************************************************************************
//	���O�̏o��
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
//	�G���[�I��
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