#include "stdafx.h"
#include "Main.h"

#include "App/App.h"
#include "Config/Config.h"

namespace
{
	pstr		APP_NAME			= "Asteraiser";
	const Point	WINDOW_SIZE			= Point( 640, 480 );
	pstr		LOG_PATH			= "Log.txt";

	int			VERSION_ID			= 1;

	pstr		CRASH_DIR				= "Crash/";
	pstr		CRASH_DIR_FORMAT		= "Crash/%04d_%02d_%02d_%02d_%02d_%02d";
	pstr		SNAPSHOT_PATH_FORMAT	= "%s/Snapshot.bmp";
}

//	構造化例外処理
int _OnException( _EXCEPTION_POINTERS* pExcept );
void _OutputStackTrace( CONTEXT* pContext );


////////////////////////////////////////////////////////////////////////////////
//
//	メイン
//
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	メイン関数
//******************************************************************************
int APIENTRY _tWinMain( HINSTANCE Hinst, HINSTANCE, LPTSTR, int )
{
#ifdef _DEBUG
	//	メモリリークチェック
	::_CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF );// | _CRTDBG_CHECK_ALWAYS_DF );
#endif

	//	ログ初期化
	{
		InitLog( LOG_PATH );
	
		SYSTEMTIME t;
		::GetLocalTime(&t);

		Log( "--------Asteraiser　システム起動--------" );
		Log( "　　Version [%d], Build [%s %s]", VERSION_ID, __DATE__, __TIME__ );
		Log( "　　Time [%04d/%02d/%02d %02d:%02d:%02d]", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond );
		Log( "----------------------------------------" );
	}

	//	コンフィグを初期化
	InitConfig();

	bool bFull = GetConfig()->GetSystemInfo().bFullScreen;

	//	初期化
	INIT_PARAM InitParam;
	InitParam.pName			= APP_NAME;
	InitParam.bFull			= bFull;
	InitParam.Size			= WINDOW_SIZE;
	InitParam.IconID		= IDI_ICON;
	InitParam.IconSmallID	= IDI_ICON_S;

	if( P5::Init( Hinst, InitParam ) )
	{
		__try
		{
			//	コンフィグの適用
			GetConfig()->Apply();

			//	アプリケーション初期化
			InitApp();

			while( P5::Update() )
			{
				//	更新
				GetApp()->Update();

				//	終了チェック
				if( GetApp()->IsEnd() )
				{
					break;
				}
			}

			//	アプリケーション終了
			ExitApp();

		}
		__except( _OnException( GetExceptionInformation() ) )
		{
			//	アプリケーション終了
			ExitApp();
		}
	}

	//	コンフィグ終了
	ExitConfig();

	//	終了
	P5::Exit();

	Log( "--------Asteraiser　システム終了--------" );

	return 0;
}


//------------------------------------------------------------------------------
//	構造化例外処理
//------------------------------------------------------------------------------
int _OnException( _EXCEPTION_POINTERS* pExcept )
{
	//	ログの出力
	Log( "--------Asteraiser　システム例外--------" );
	Log( "コード　：%08x", pExcept->ExceptionRecord->ExceptionCode );
	Log( "フラグ　：%08x", pExcept->ExceptionRecord->ExceptionFlags );
	Log( "アドレス：%px", pExcept->ExceptionRecord->ExceptionAddress );

	for( uint i = 0; i < pExcept->ExceptionRecord->NumberParameters; ++i )
	{
		Log( "パラメタ[%d]：%08x", i, pExcept->ExceptionRecord->ExceptionInformation[i] );
	}

	//	スタックトレース
	Log( "------------スタックトレース------------" );

	_OutputStackTrace( pExcept->ContextRecord );

	Log( "----------------------------------------" );

	//	保存ディレクトリ
	SYSTEMTIME t;
	::GetLocalTime(&t);

	String256 DirPath;
	DirPath.Format( CRASH_DIR_FORMAT, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond );

	//	保存ディレクトリを生成
	::CreateDirectory( CRASH_DIR, NULL );
	::CreateDirectory( DirPath.c_str(), NULL );

	//	アプリケーションへ通知
	GetApp()->OnException( DirPath.c_str() );

	//	スクリーンショット保存
	if( GetGraphix() && GetGraphix()->GetDevice() )
	{
		String256 SnapshotPath;
		SnapshotPath.Format( SNAPSHOT_PATH_FORMAT, DirPath.c_str() );
	
		GetGraphix()->GetDevice()->SaveScreenShot( SnapshotPath.c_str() );
	}

	//	ログファイルのコピー
	String256 LogPath;
	LogPath.Format( "%s/%s", DirPath.c_str(), LOG_PATH );
	::CopyFile( LOG_PATH, LogPath.c_str(), FALSE );

	return EXCEPTION_CONTINUE_SEARCH;
}

//------------------------------------------------------------------------------
//	スタックトレースをログへ出力
//------------------------------------------------------------------------------
void _OutputStackTrace( CONTEXT* pContext )
{
	if( !pContext ) return;

	//	スタックフレーム取得
	STACKFRAME64 StackFrame;
	ZeroStruct( StackFrame );

#if defined(_M_IX86)
	StackFrame.AddrPC.Offset = pContext->Eip;
	StackFrame.AddrPC.Mode = AddrModeFlat;
	StackFrame.AddrFrame.Offset = pContext->Ebp;
	StackFrame.AddrFrame.Mode = AddrModeFlat;
	StackFrame.AddrStack.Offset = pContext->Esp;
	StackFrame.AddrStack.Mode = AddrModeFlat;
	const DWORD Type = IMAGE_FILE_MACHINE_I386;
#elif defined(_M_AMD64)
	StackFrame.AddrPC.Offset = pContext->Rip;
	StackFrame.AddrPC.Mode = AddrModeFlat;
	StackFrame.AddrFrame.Offset = pContext->Rbp;
	StackFrame.AddrFrame.Mode = AddrModeFlat;
	StackFrame.AddrStack.Offset = pContext->Rsp;
	StackFrame.AddrStack.Mode = AddrModeFlat;
	const DWORD Type = IMAGE_FILE_MACHINE_AMD64;
#elif defined(_M_IA64)
	StackFrame.AddrPC.Offset = pContext->StIIP;
	StackFrame.AddrPC.Mode = AddrModeFlat;
	StackFrame.AddrStack.Offset = pContext->IntSp;
	StackFrame.AddrStack.Mode = AddrModeFlat;
	StackFrame.AddrBStore.Offset = pContext->RsBSP;
	StackFrame.AddrBStore.Mode = AddrModeFlat;
	const DWORD Type = IMAGE_FILE_MACHINE_IA64;
#endif

	//	スタックフレームを取得
	while( ::StackWalk64( Type, ::GetCurrentProcess(), ::GetCurrentThread(), &StackFrame, pContext, NULL, NULL, NULL, NULL ) )
	{
		if( StackFrame.AddrPC.Offset == StackFrame.AddrReturn.Offset )
		{
			break;
		}
		if( 0 == StackFrame.AddrPC.Offset )
		{
			break;
		}       
		if( 0 == StackFrame.AddrReturn.Offset )
		{
			break;
		}

		//	書き込み
		Log( "　　%016I64x", StackFrame.AddrReturn.Offset );
	}
}