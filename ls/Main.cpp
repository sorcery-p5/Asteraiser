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

//	�\������O����
int _OnException( _EXCEPTION_POINTERS* pExcept );
void _OutputStackTrace( CONTEXT* pContext );


////////////////////////////////////////////////////////////////////////////////
//
//	���C��
//
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	���C���֐�
//******************************************************************************
int APIENTRY _tWinMain( HINSTANCE Hinst, HINSTANCE, LPTSTR, int )
{
#ifdef _DEBUG
	//	���������[�N�`�F�b�N
	::_CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF );// | _CRTDBG_CHECK_ALWAYS_DF );
#endif

	//	���O������
	{
		InitLog( LOG_PATH );
	
		SYSTEMTIME t;
		::GetLocalTime(&t);

		Log( "--------Asteraiser�@�V�X�e���N��--------" );
		Log( "�@�@Version [%d], Build [%s %s]", VERSION_ID, __DATE__, __TIME__ );
		Log( "�@�@Time [%04d/%02d/%02d %02d:%02d:%02d]", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond );
		Log( "----------------------------------------" );
	}

	//	�R���t�B�O��������
	InitConfig();

	bool bFull = GetConfig()->GetSystemInfo().bFullScreen;

	//	������
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
			//	�R���t�B�O�̓K�p
			GetConfig()->Apply();

			//	�A�v���P�[�V����������
			InitApp();

			while( P5::Update() )
			{
				//	�X�V
				GetApp()->Update();

				//	�I���`�F�b�N
				if( GetApp()->IsEnd() )
				{
					break;
				}
			}

			//	�A�v���P�[�V�����I��
			ExitApp();

		}
		__except( _OnException( GetExceptionInformation() ) )
		{
			//	�A�v���P�[�V�����I��
			ExitApp();
		}
	}

	//	�R���t�B�O�I��
	ExitConfig();

	//	�I��
	P5::Exit();

	Log( "--------Asteraiser�@�V�X�e���I��--------" );

	return 0;
}


//------------------------------------------------------------------------------
//	�\������O����
//------------------------------------------------------------------------------
int _OnException( _EXCEPTION_POINTERS* pExcept )
{
	//	���O�̏o��
	Log( "--------Asteraiser�@�V�X�e����O--------" );
	Log( "�R�[�h�@�F%08x", pExcept->ExceptionRecord->ExceptionCode );
	Log( "�t���O�@�F%08x", pExcept->ExceptionRecord->ExceptionFlags );
	Log( "�A�h���X�F%px", pExcept->ExceptionRecord->ExceptionAddress );

	for( uint i = 0; i < pExcept->ExceptionRecord->NumberParameters; ++i )
	{
		Log( "�p�����^[%d]�F%08x", i, pExcept->ExceptionRecord->ExceptionInformation[i] );
	}

	//	�X�^�b�N�g���[�X
	Log( "------------�X�^�b�N�g���[�X------------" );

	_OutputStackTrace( pExcept->ContextRecord );

	Log( "----------------------------------------" );

	//	�ۑ��f�B���N�g��
	SYSTEMTIME t;
	::GetLocalTime(&t);

	String256 DirPath;
	DirPath.Format( CRASH_DIR_FORMAT, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond );

	//	�ۑ��f�B���N�g���𐶐�
	::CreateDirectory( CRASH_DIR, NULL );
	::CreateDirectory( DirPath.c_str(), NULL );

	//	�A�v���P�[�V�����֒ʒm
	GetApp()->OnException( DirPath.c_str() );

	//	�X�N���[���V���b�g�ۑ�
	if( GetGraphix() && GetGraphix()->GetDevice() )
	{
		String256 SnapshotPath;
		SnapshotPath.Format( SNAPSHOT_PATH_FORMAT, DirPath.c_str() );
	
		GetGraphix()->GetDevice()->SaveScreenShot( SnapshotPath.c_str() );
	}

	//	���O�t�@�C���̃R�s�[
	String256 LogPath;
	LogPath.Format( "%s/%s", DirPath.c_str(), LOG_PATH );
	::CopyFile( LOG_PATH, LogPath.c_str(), FALSE );

	return EXCEPTION_CONTINUE_SEARCH;
}

//------------------------------------------------------------------------------
//	�X�^�b�N�g���[�X�����O�֏o��
//------------------------------------------------------------------------------
void _OutputStackTrace( CONTEXT* pContext )
{
	if( !pContext ) return;

	//	�X�^�b�N�t���[���擾
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

	//	�X�^�b�N�t���[�����擾
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

		//	��������
		Log( "�@�@%016I64x", StackFrame.AddrReturn.Offset );
	}
}