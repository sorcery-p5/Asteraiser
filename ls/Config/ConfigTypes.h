#pragma once

#include "Controller/ControllerTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	コンフィグ：型定義
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	定数
//------------------------------------------------------------------------------
enum
{
	//	入力
	INPUT_BUTTON_NUM		= CONTROLLER_BUTTON_NUM,		//	ボタンの数
	INPUT_SYSTEM_KEY_NUM	= CONTROLLER_SYSTEM_KEY_NUM,	//	システムキーの数
	INPUT_AXIS_NUM			= CONTROLLER_DIR_NUM,			//	方向の数
	CONCURRENT_NUM			= 4,							//	同時押し最大入力数
};

//------------------------------------------------------------------------------
//	システム設定
//------------------------------------------------------------------------------
struct CONFIG_SYSTEM
{
	CONFIG_SYSTEM( void )					{ Default(); }
	void Default( void );

	float			BGMVolume;
	float			SoundVolume;

	bool			bFullScreen;
};

//------------------------------------------------------------------------------
//	同時押し設定
//------------------------------------------------------------------------------
struct CONCURRENT
{
	CONCURRENT( byte b, const byte* pArray, uint Size );

	byte		Button;
	byte		Array[CONCURRENT_NUM];
};

//------------------------------------------------------------------------------
//	入力設定
//------------------------------------------------------------------------------
struct CONFIG_INPUT
{
	CONFIG_INPUT( void )					{ Default(0); }
	CONFIG_INPUT( int PlayerID )			{ Default(PlayerID); }
	void Default( int PlayerID );

	typedef	std::vector<CONCURRENT>			ConcurrentVec;

	//	キーボード
	byte			aKeyButton[INPUT_BUTTON_NUM];
	byte			aKeyDir[INPUT_AXIS_NUM];
	ConcurrentVec	vKeyConcurrent;
	byte			aKeySystem[INPUT_SYSTEM_KEY_NUM];

	//	パッド
	byte			aPadButton[INPUT_BUTTON_NUM];
	ConcurrentVec	vPadConcurrent;
	byte			aPadSystem[INPUT_SYSTEM_KEY_NUM];

	PAD_AXIS		PadAxisX;
	bool			bPadAxisXRev;
	PAD_AXIS		PadAxisY;
	bool			bPadAxisYRev;
	uint			PadDeadzone;
};

//------------------------------------------------------------------------------
//	デバッグ情報
//------------------------------------------------------------------------------
struct CONFIG_DEBUG
{
	CONFIG_DEBUG( void );

	bool			bDrawDummyBone;
	bool			bDrawCollideShape;
	bool			bOverdriveInfinite;
	bool			bShieldInfinite;
	bool			bBladeInfinite;
	bool			bPlayerInvincible;
	bool			bPlayerNodamage;
	bool			bPlayerHyperAttack;
	bool			bEnemyHyperAttack;
};