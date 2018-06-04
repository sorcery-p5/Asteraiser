#pragma once


class PadManager;

namespace P5
{

//-----------------------------------------------------------------------------
//	パッド軸
//-----------------------------------------------------------------------------
enum PAD_AXIS
{
	PAD_AXIS_X,
	PAD_AXIS_Y,
	PAD_AXIS_Z,
	PAD_AXIS_RX,
	PAD_AXIS_RY,
	PAD_AXIS_RZ,	
	PAD_AXIS_SL1,
	PAD_AXIS_SL2,

	PAD_AXIS_POV1X,
	PAD_AXIS_POV1Y,
	PAD_AXIS_POV2X,
	PAD_AXIS_POV2Y,
	PAD_AXIS_POV3X,
	PAD_AXIS_POV3Y,
	PAD_AXIS_POV4X,
	PAD_AXIS_POV4Y,

	PAD_AXIS_NUM,

	PAD_AXIS_POV_BEGIN	= PAD_AXIS_POV1X,
	PAD_AXIS_POV_END	= PAD_AXIS_POV4Y,
	PAD_AXIS_POV_MAX	= 4,
};

enum
{
	PAD_BUTTON_MAX		= 32,
	DEFAULT_DEADZONE	= 2000,
};

///////////////////////////////////////////////////////////////////////////////
//
//	パッド
//
///////////////////////////////////////////////////////////////////////////////
class Pad
{
public:
	Pad( PadManager* pManager );
	~Pad();

	bool					Init( const DIDEVICEINSTANCE* pdidInstance );
	bool					Update( void );

	PadManager*				GetManager( void )						{ return m_pManager; }
	LPDIRECTINPUTDEVICE8	GetDevice( void )						{ return m_pDevice; }

	float					GetAxis( PAD_AXIS Axis, INPUT_TYPE Type = INPUT_PRESS )			const;
	int						GetAxisCount( PAD_AXIS Axis )									const	{ return m_AxisCount[Axis]; }
	bool					IsButtonInput( byte Index, INPUT_TYPE Type = INPUT_PRESS )		const;
	byte					GetInputButton( INPUT_TYPE Type = INPUT_PRESS )					const;
	int						GetButtonCount( byte Index )									const	{ return m_ButtonCount[Index]; }

	void					SetDeadzone( dword Deadzone );
	dword					GetDeadzone( void )												const	{ return m_Deadzone; }

private:
	float					_GetAxis( const DIJOYSTATE& State, PAD_AXIS Axis )				const;
	float					_GetPovAxis( const DIJOYSTATE& State, PAD_AXIS Axis )			const;

private:	
	PadManager*				m_pManager;

	LPDIRECTINPUTDEVICE8	m_pDevice;
	DIJOYSTATE 				m_JoyState;
	DIJOYSTATE 				m_PrevState;
	bool					m_bPolling;
	dword					m_Deadzone;

	int						m_ButtonCount[ PAD_BUTTON_MAX ];
	int						m_AxisCount[ PAD_AXIS_NUM ];
};

}