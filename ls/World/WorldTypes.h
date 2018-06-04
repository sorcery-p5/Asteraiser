#pragma once


////////////////////////////////////////////////////////////////////////////////
//
//	ワールド：型定義
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	ワールドパラメータ
//------------------------------------------------------------------------------
struct WORLD_PARAM
{
	Rect		FieldRect;
	Point		InitPos;

	Label		PlayerName;

	Label		PauseMenuName;
	Label		ContinueMenuName;
	Label		ResultMenuName;
	Label		ReplayMenuName;

	bool		bPauseExit;
	bool		bContinueEnable;
	Label		ResultScoreSoundName;

	Label		DemoMenuName;
};