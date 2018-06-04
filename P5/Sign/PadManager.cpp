#include "P5.h"
#include "PadManager.h"


namespace P5
{

//-----------------------------------------------------------------------------
//	列挙コールバック
//-----------------------------------------------------------------------------
static BOOL CALLBACK EnumJoysticks( const DIDEVICEINSTANCE* pdidInstance, void* pContext );

///////////////////////////////////////////////////////////////////////////////
//
//	パッド管理
//
///////////////////////////////////////////////////////////////////////////////
PadManager::PadManager( void )
{
	m_pDinput	= NULL;
}
PadManager::~PadManager()
{
	DeleteVec( m_vPad );
	SafeRelease( m_pDinput );
}

//*****************************************************************************
//	初期化
//*****************************************************************************
bool PadManager::Init( HINSTANCE Hinst )
{
	Log( "　　□パッドの初期化" );

	HRESULT hr;

	//	DInput初期化
	hr = DirectInput8Create( Hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDinput, NULL ); 
	if( FAILED(hr) || !m_pDinput )
	{
		Log( "　　×DirectInputの初期化失敗" );
		return false;
	}

	//	ジョイスティックの数を調べる
	uint Num = joyGetNumDevs();
	if( Num == JOYERR_UNPLUGGED )
	{
		Log( "　　△パッドを未検出" );
		return true;
	}

	// ジョイスティックを列挙する
	hr = m_pDinput->EnumDevices( DI8DEVCLASS_GAMECTRL, EnumJoysticks, this, DIEDFL_ATTACHEDONLY );
	if( FAILED(hr) )
	{
		Log( "　　△パッドの列挙に失敗" );
		return false;
	}

	Log( "　　○パッドの初期化完了" );
	Log( "　　----------------------------------------" );

	return true;
}

//*****************************************************************************
//	更新
//*****************************************************************************
bool PadManager::Update( void )
{
	for( VecPad::iterator it = m_vPad.begin(); it != m_vPad.end(); ++it )
	{
		if( !(*it)->Update() ) return false;
	}

	return true;
}


//-----------------------------------------------------------------------------
//	ジョイスティック列挙コールバック
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumJoysticks( const DIDEVICEINSTANCE* pdidInstance, void* pContext )
{
	PadManager* pPadMng = (PadManager*)(pContext);

	Pad* pPad = new Pad( pPadMng );
	if( pPad )
	{
		if( pPad->Init( pdidInstance ) )
		{
			//	パッド追加
			pPadMng->AddPad( pPad );
		}
		else
		{
			SafeDelete( pPad );
		}
	}

	return DIENUM_CONTINUE;
}

}