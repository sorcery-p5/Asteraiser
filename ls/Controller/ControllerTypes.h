#pragma once


////////////////////////////////////////////////////////////////////////////////
//
//	コントローラー型定義
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	コントローラ：入力種別
//------------------------------------------------------------------------------
enum CONTROLLER_INPUT
{
	CONTROLLER_BLADE,		//	ブレード
	CONTROLLER_RELOAD,		//	リロード
	CONTROLLER_DASH,		//	ダッシュ

	CONTROLLER_L,			//	左
	CONTROLLER_U,			//	上
	CONTROLLER_R,			//	右
	CONTROLLER_D,			//	下

	CONTROLLER_BUTTON_BEGIN	= CONTROLLER_BLADE,
	CONTROLLER_BUTTON_END	= CONTROLLER_DASH,
	CONTROLLER_DIR_BEGIN	= CONTROLLER_L,
	CONTROLLER_DIR_END		= CONTROLLER_D,

	CONTROLLER_NONE			= UCHAR_MAX,

	CONTROLLER_BUTTON_NUM	= CONTROLLER_BUTTON_END - CONTROLLER_BUTTON_BEGIN + 1,
	CONTROLLER_DIR_NUM		= CONTROLLER_DIR_END	- CONTROLLER_DIR_BEGIN + 1,
};

//------------------------------------------------------------------------------
//	コントローラ：システムキー種別
//------------------------------------------------------------------------------
enum CONTROLLER_SYSTEM_KEY
{
	CONTROLLER_SYSTEM_OK,		//	決定
	CONTROLLER_SYSTEM_CANCEL,	//	キャンセル
	CONTROLLER_SYSTEM_PAUSE,	//	ポーズ
	CONTROLLER_SYSTEM_EXIT,		//	終了

	CONTROLLER_SYSTEM_KEY_NUM,

	CONTROLLER_SYSTEM_NONE	= UCHAR_MAX,
};