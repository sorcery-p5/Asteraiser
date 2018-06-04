#include "P5.h"
#include "Pad.h"

#include "PadManager.h"

namespace P5
{

const int s_AxisMax	= (1 << 15);

//-----------------------------------------------------------------------------
//	軸の列挙コールバック
//-----------------------------------------------------------------------------
static BOOL CALLBACK InitAxes( const DIDEVICEOBJECTINSTANCE* pdidoi, void* pContext );
static BOOL CALLBACK SetDeadzoneAxes( const DIDEVICEOBJECTINSTANCE* pdidoi, void* pContext );


///////////////////////////////////////////////////////////////////////////////
//
//	パッド
//
///////////////////////////////////////////////////////////////////////////////
Pad::Pad( PadManager* pManager )
{
	m_pManager	= pManager;
	m_pDevice	= NULL;
	ZeroStruct( m_JoyState );
	ZeroStruct( m_PrevState );
	m_bPolling	= false;
	m_Deadzone	= DEFAULT_DEADZONE;
	ZeroArray( m_ButtonCount );
	ZeroArray( m_AxisCount );
}
Pad::~Pad()
{
	SafeRelease( m_pDevice );
}

//*****************************************************************************
//	初期化
//*****************************************************************************
bool Pad::Init( const DIDEVICEINSTANCE* pdidInstance )
{
	int PadID = m_pManager->GetPadNum();
	Log( "　　　□パッド[%d]の初期化", PadID );

	HRESULT	hr;

	//	列挙されたジョイスティックへのインターフェイスを取得する。
	hr = m_pManager->GetDInput()->CreateDevice( pdidInstance->guidInstance, &m_pDevice, NULL );
	if( FAILED(hr) || !m_pDevice )
	{
		Log( "　　　×パッドのデバイス作成失敗" );
		return false;
	}

	//	データフォーマットの設定
	hr = m_pDevice->SetDataFormat( &c_dfDIJoystick );
	if( FAILED(hr) )
	{
		Log( "　　　×パッドのデータフォーマット設定失敗" );
		return false;
	}

	//	協調レベルの設定
	hr = m_pDevice->SetCooperativeLevel( P5::GetWindow()->GetHwnd(), DISCL_EXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY );
	if( FAILED(hr) )
	{
		Log( "　　　×パッドの協調レベル設定失敗" );
		return false;
	}

	//	レバー軸の設定
	hr = m_pDevice->EnumObjects( InitAxes, this, DIDFT_AXIS );
	if( FAILED(hr) )
	{
		Log( "　　　×パッドのレバー軸設定失敗" );
		return false;
	}

	// ポーリング有無を調べる
	DIDEVCAPS caps = { sizeof(DIDEVCAPS) };
	m_pDevice->GetCapabilities( &caps );

	m_bPolling = (caps.dwFlags & (DIDC_POLLEDDATAFORMAT | DIDC_POLLEDDEVICE)) != 0;

	Log( "　　　▽パッドのポーリング有無[%s]", m_bPolling? "必要" : "不要" );

	//	入力制御開始
	m_pDevice->Acquire();

	Log( "　　　○パッド[%d]の初期化完了", PadID );
	Log( "　　　----------------------------------------" );

	return true;
}

//*****************************************************************************
//	更新
//*****************************************************************************
bool Pad::Update( void )
{
	//	ポーリングを行う
	if( m_bPolling )
	{
		m_pDevice->Poll();
	}

	m_PrevState = m_JoyState;
	HRESULT hr = m_pDevice->GetDeviceState( sizeof(m_JoyState), &m_JoyState );
	if( FAILED(hr) )
	{
		//	デバイス再アクセス
		m_pDevice->Acquire();
		ZeroStruct( m_JoyState );
	}

	//	カウント
	for( int i = 0; i < PAD_BUTTON_MAX; i++ )
	{
		if( IsButtonInput( i, INPUT_PRESS ) )
		{
			m_ButtonCount[i]++;
		}
		else
		{
			m_ButtonCount[i] = 0;
		}
	}
	for( int i = 0; i < PAD_AXIS_NUM; i++ )
	{
		if( GetAxis( PAD_AXIS(i), INPUT_PRESS ) )
		{
			m_AxisCount[i]++;
		}
		else
		{
			m_AxisCount[i] = 0;
		}
	}

	return true;
}

//*****************************************************************************
//	軸の取得
//*****************************************************************************
float Pad::GetAxis( PAD_AXIS Axis, INPUT_TYPE Type )	const
{
	switch( Type )
	{
	case INPUT_PRESS:	return _GetAxis( m_JoyState, Axis );
	case INPUT_TRIG:	return (_GetAxis( m_PrevState, Axis ) == 0.0f)? _GetAxis( m_JoyState, Axis ) : 0.0f;
	case INPUT_OFF:		return (_GetAxis( m_JoyState, Axis ) == 0.0f)?	_GetAxis( m_PrevState, Axis ) : 0.0f;
	case INPUT_REPEAT:	return Sign::CountIsRepeat( m_AxisCount[Axis] )? GetAxis( Axis, INPUT_PRESS ) : GetAxis( Axis, INPUT_TRIG );
	}

	return 0.0f;
}

//*****************************************************************************
//	ボタンが押されているかどうか
//*****************************************************************************
bool Pad::IsButtonInput( byte Index, INPUT_TYPE Type ) const
{
	if( Index >= PAD_BUTTON_MAX ) return false;
	
	switch( Type )
	{
	case INPUT_PRESS:	return m_JoyState.rgbButtons[Index] != 0;
	case INPUT_TRIG:	return m_JoyState.rgbButtons[Index] != 0 && m_PrevState.rgbButtons[Index] == 0;
	case INPUT_OFF:		return m_JoyState.rgbButtons[Index] == 0 && m_PrevState.rgbButtons[Index] != 0;
	case INPUT_REPEAT:	return Sign::CountIsRepeat( m_ButtonCount[Index] )? IsButtonInput( Index, INPUT_PRESS ) : IsButtonInput( Index, INPUT_TRIG );
	}

	return false;
}

//******************************************************************************
//	入力されているボタンを得る
//******************************************************************************
byte Pad::GetInputButton( INPUT_TYPE Type ) const
{
	for( byte Index = 0; Index < PAD_BUTTON_MAX; Index++ )
	{
		if( IsButtonInput( Index, Type ) )
		{
			return Index;
		}
	}
	return UCHAR_MAX;
}

//******************************************************************************
//	レバーのデッドゾーンを設定
//******************************************************************************
void Pad::SetDeadzone( dword Deadzone )
{
	m_Deadzone = Deadzone;

	m_pDevice->EnumObjects( SetDeadzoneAxes, this, DIDFT_AXIS );
}

//------------------------------------------------------------------------------
//	軸の取得
//------------------------------------------------------------------------------
float Pad::_GetAxis( const DIJOYSTATE& State, PAD_AXIS Axis ) const
{
	if( Axis >= PAD_AXIS_POV_BEGIN && Axis <= PAD_AXIS_POV_END )
	{
		return _GetPovAxis( State, Axis );
	}

	switch( Axis )
	{
	case PAD_AXIS_X:	return (float)State.lX 			 / (float)s_AxisMax;
	case PAD_AXIS_Y:	return (float)State.lY 			 / (float)s_AxisMax;
	case PAD_AXIS_Z:	return (float)State.lZ 			 / (float)s_AxisMax;
	case PAD_AXIS_RX:	return (float)State.lRx			 / (float)s_AxisMax;
	case PAD_AXIS_RY:	return (float)State.lRy			 / (float)s_AxisMax;
	case PAD_AXIS_RZ:	return (float)State.lRz			 / (float)s_AxisMax;
	case PAD_AXIS_SL1:	return (float)State.rglSlider[0] / (float)s_AxisMax;
	case PAD_AXIS_SL2:	return (float)State.rglSlider[1] / (float)s_AxisMax;
	}

	return 0.0f;
}

//-----------------------------------------------------------------------------
//	POV軸の取得
//-----------------------------------------------------------------------------
float Pad::_GetPovAxis( const DIJOYSTATE& State, PAD_AXIS Axis )	const
{
	int Index	= (Axis - PAD_AXIS_POV_BEGIN) / 2;
	int	X		= (Axis - PAD_AXIS_POV_BEGIN) % 2;

	dword Val	= State.rgdwPOV[Index];

	//	中心にある？
	if( LOWORD(Val) == 0xFFFF ) return 0.0f;

	int n = ((int)Val/ 4500) % 8;
	float aVal[][2] =
	{
		{ 0.0f,		-1.0f },	//	0
		{ SQ2,		-SQ2 },		//	45
		{ 1.0f,		0.0f },		//	90
		{ SQ2,		SQ2 },		//	135
		{ 0.0f,		1.0f },		//	180
		{ -SQ2,		SQ2 },		//	225
		{ -1.0f,	0.0f },		//	270
		{ -SQ2,		-SQ2 },		//	315
	};

	return aVal[n][X];
}


//-----------------------------------------------------------------------------
//	軸の初期設定
//-----------------------------------------------------------------------------
BOOL CALLBACK InitAxes( const DIDEVICEOBJECTINSTANCE* pdidoi, void* pContext )
{
	Pad* pPad = (Pad*)pContext;

	// 軸の範囲設定
	DIPROPRANGE diprg	= { sizeof(DIPROPRANGE), sizeof(DIPROPHEADER) };
    diprg.diph.dwHow	= DIPH_BYID; 
    diprg.diph.dwObj	= pdidoi->dwType;
    diprg.lMin			= -s_AxisMax; 
    diprg.lMax			= s_AxisMax; 

	if FAILED( pPad->GetDevice()->SetProperty( DIPROP_RANGE, &diprg.diph ) ) return DIENUM_STOP;

	// デッドゾーン設定
	DIPROPDWORD	dipdw	= { sizeof(DIPROPDWORD), sizeof(DIPROPHEADER) };
	dipdw.diph.dwHow	= DIPH_BYID; 
	dipdw.diph.dwObj	= pdidoi->dwType;
	dipdw.dwData		= pPad->GetDeadzone();

	if FAILED( pPad->GetDevice()->SetProperty( DIPROP_DEADZONE, &dipdw.diph ) ) return DIENUM_STOP;

	// 軸モード設定
	dipdw.diph.dwHow	= DIPH_DEVICE; 
	dipdw.diph.dwObj	= 0;
	dipdw.dwData		= DIPROPAXISMODE_ABS;

	if FAILED( pPad->GetDevice()->SetProperty( DIPROP_AXISMODE, &dipdw.diph ) ) return DIENUM_STOP;

    return DIENUM_CONTINUE;
}

//-----------------------------------------------------------------------------
//	軸にデッドゾーン設定
//-----------------------------------------------------------------------------
static BOOL CALLBACK SetDeadzoneAxes( const DIDEVICEOBJECTINSTANCE* pdidoi, void* pContext )
{
	Pad* pPad = (Pad*)pContext;

	DIPROPDWORD	dipdw	= { sizeof(DIPROPDWORD), sizeof(DIPROPHEADER) };
	dipdw.diph.dwHow	= DIPH_BYID; 
	dipdw.diph.dwObj	= pdidoi->dwType;
	dipdw.dwData		= pPad->GetDeadzone();

	if FAILED( pPad->GetDevice()->SetProperty( DIPROP_DEADZONE, &dipdw.diph ) ) return DIENUM_STOP;
	
    return DIENUM_CONTINUE;
}

}