#include "stdafx.h"
#include "ConfigTypes.h"

#include "Controller/Controller.h"

namespace
{
	const byte					s_DefaultKeyButton[][INPUT_BUTTON_NUM]		= { {'Z','X','C' }, };
	const byte					s_DefaultKeySystem[][INPUT_SYSTEM_KEY_NUM]	= { {'Z','X',VK_SPACE,VK_ESCAPE }, };
	const byte					s_DefaultKeyDir[][INPUT_AXIS_NUM]			= { {VK_LEFT,VK_UP,VK_RIGHT,VK_DOWN}, };

#ifdef DEMO_VER

	const byte					s_DefaultPadButton[INPUT_BUTTON_NUM]		= { 0,1,3 };
	const byte					s_DefaultPadSystem[INPUT_SYSTEM_KEY_NUM]	= { 0,1,9,6 };

	struct DEFAULT_CONCURRENT
	{
		byte			Key;
		byte			Button;
		byte			Array[2];
	};

	const DEFAULT_CONCURRENT	s_DefaultConcurrent[]						= {
																				{	'A',	2,	{ CONTROLLER_BLADE, CONTROLLER_DASH } },
																				{	'S',	5,	{ CONTROLLER_RELOAD, CONTROLLER_DASH } },
																			};

#else

	const byte					s_DefaultPadButton[INPUT_BUTTON_NUM]		= { 0,1,2 };
	const byte					s_DefaultPadSystem[INPUT_SYSTEM_KEY_NUM]	= { 0,1,8,9 };

	struct DEFAULT_CONCURRENT
	{
		byte			Key;
		byte			Button;
		byte			Array[2];
	};

	const DEFAULT_CONCURRENT	s_DefaultConcurrent[]						= {
																				{	'A',	4,	{ CONTROLLER_BLADE, CONTROLLER_DASH } },
																				{	'S',	5,	{ CONTROLLER_RELOAD, CONTROLLER_DASH } },
																			};

#endif

};


////////////////////////////////////////////////////////////////////////////////
//
//	コンフィグ：型定義
//
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	システム設定
//******************************************************************************
void CONFIG_SYSTEM::Default( void )
{
	BGMVolume			= 0.7f;
	SoundVolume			= 0.3f;

	bFullScreen			= false;
}

//******************************************************************************
//	同時押し
//******************************************************************************
CONCURRENT::CONCURRENT( byte b, const byte* pArray, uint Size )
{
	Button = b;
	
	FillArray( Array, ArrayNum(Array), (byte)CONTROLLER_NONE );
	CopyArrayNum( Array, pArray, Min( (uint)CONCURRENT_NUM, Size ) );
}

//******************************************************************************
//	入力設定
//******************************************************************************
void CONFIG_INPUT::Default( int PlayerID )
{
	//	キーボード
	CopyArray( aKeyButton,	s_DefaultKeyButton[ PlayerID >= ArrayNum(s_DefaultKeyButton)? 0 : PlayerID ] );
	CopyArray( aKeyDir,		s_DefaultKeyDir[ PlayerID >= ArrayNum(s_DefaultKeyDir)? 0 : PlayerID ] );
	CopyArray( aKeySystem,	s_DefaultKeySystem[ PlayerID >= ArrayNum(s_DefaultKeySystem)? 0 : PlayerID ] );

	//	パッド
	CopyArray( aPadButton,	s_DefaultPadButton );
	CopyArray( aPadSystem,	s_DefaultPadSystem );

	//	同時押し
	vKeyConcurrent.clear();
	vPadConcurrent.clear();

	for( int i = 0; i < ArrayNum(s_DefaultConcurrent); i++ )
	{
		DEFAULT_CONCURRENT DefCon = s_DefaultConcurrent[i];

		vKeyConcurrent.push_back( CONCURRENT( DefCon.Key,		DefCon.Array, ArrayNum(DefCon.Array) ) );
		vPadConcurrent.push_back( CONCURRENT( DefCon.Button,	DefCon.Array, ArrayNum(DefCon.Array) ) );
	}

	PadAxisX		= PAD_AXIS_X;
	bPadAxisXRev	= false;
	PadAxisY		= PAD_AXIS_Y;
	bPadAxisYRev	= false;

	PadDeadzone		= DEFAULT_DEADZONE;
}

//******************************************************************************
//	デバッグ用
//******************************************************************************
CONFIG_DEBUG::CONFIG_DEBUG( void )
{
	bDrawDummyBone		= false;
	bDrawCollideShape	= false;
	bOverdriveInfinite	= false;
	bShieldInfinite		= false;
	bBladeInfinite		= false;
	bPlayerInvincible	= false;
	bPlayerNodamage		= false;
	bPlayerHyperAttack	= false;
	bEnemyHyperAttack	= false;
}