#pragma once

#include "Indicate/IndicateTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	メニュー：型定義
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	メニュー：カーソル
//------------------------------------------------------------------------------
struct MENU_CURSOR
{
	Label			BoneName;
	float			MoveSpeed;
	Label			MoveSound;
	float			MoveSoundVol;
};

//------------------------------------------------------------------------------
//	メニュー：方向
//------------------------------------------------------------------------------
enum MENU_DIR
{
	MENU_DIR_L,			//	左
	MENU_DIR_U,			//	上
	MENU_DIR_R,			//	右
	MENU_DIR_D,			//	下

	MENU_DIR_NUM,
};

//------------------------------------------------------------------------------
//	メニュー：選択方向
//------------------------------------------------------------------------------
enum MENU_SELECT_DIR
{
	MENU_SELECT_H,		//	水平
	MENU_SELECT_V,		//	垂直
};

//------------------------------------------------------------------------------
//	メニュー：オブジェクト種別
//------------------------------------------------------------------------------
enum MENU_OBJECT_TYPE
{
	MENU_OBJECT_NONE,		//	なし
	MENU_OBJECT_SELECTOR,	//	セレクター
	MENU_OBJECT_SLIDER,		//	スライダー
	MENU_OBJECT_LIST,		//	リスト
	MENU_OBJECT_SCROLL,		//	スクロール
};

//------------------------------------------------------------------------------
//	メニュー：セレクター
//------------------------------------------------------------------------------
struct MENU_SELECTOR
{
	MENU_SELECT_DIR					SelectDir;
	Label							CursorBone;
	float							MoveSpeed;
	Label							MoveSound;
	float							MoveSoundVol;
	std::vector<LabelPair<float>>	vSelect;
};

//------------------------------------------------------------------------------
//	メニュー：スライダー
//------------------------------------------------------------------------------
struct MENU_SLIDER
{
	MENU_SELECT_DIR		SelectDir;
	float				MinValue;
	float				MaxValue;
	float				Step;
	Label				MoveSound;
	float				MoveSoundVol;
};

//------------------------------------------------------------------------------
//	メニュー：リスト
//------------------------------------------------------------------------------
struct MENU_LIST
{
	int					NumH;
	int					NumV;
	int					LineNum;
	Label				CellSprite;
	Label				CursorBone;
	float				MoveSpeed;
	Label				MoveSound;
	float				MoveSoundVol;
};

//------------------------------------------------------------------------------
//	メニュー：スクロール
//------------------------------------------------------------------------------
struct MENU_SCROLL
{
	MENU_SELECT_DIR		SelectDir;
	Label				TargetBone;
	Label				BaseBone;
	float				MoveSpeed;
};

//------------------------------------------------------------------------------
//	メニュー：アクション種別
//------------------------------------------------------------------------------
enum MENU_ACTION_TYPE
{
	MENU_ACTION_OPEN,
	MENU_ACTION_CLOSE,

	MENU_ACTION_ANIME,
	MENU_ACTION_NOTIFY,
	MENU_ACTION_SELECT,

	MENU_ACTION_EFFECT,
	MENU_ACTION_SOUND,
	MENU_ACTION_BGM_PLAY,
	MENU_ACTION_BGM_STOP,
	MENU_ACTION_BGM_FADE,

	MENU_ACTION_NULL	= -1,
};

//------------------------------------------------------------------------------
//	メニュー：アクション
//------------------------------------------------------------------------------
struct MENU_ACTION
{
	MENU_ACTION_TYPE		Type;
	Label					Name;
	float					Param;
	std::string				Text;
};
typedef std::vector<MENU_ACTION> MENU_ACTION_VEC;

//------------------------------------------------------------------------------
//	メニュー：キーイベント種別
//------------------------------------------------------------------------------
enum MENU_KEY_EVENT_TYPE
{
	MENU_KEY_EVENT_DECIDE,
	MENU_KEY_EVENT_CANCEL,
	MENU_KEY_EVENT_PAUSE,
	MENU_KEY_EVENT_EXIT,
	MENU_KEY_EVENT_ANY,

	MENU_KEY_EVENT_NULL		= -1,
};

//------------------------------------------------------------------------------
//	メニュー：キーイベント
//------------------------------------------------------------------------------
struct MENU_KEY_EVENT
{
	MENU_KEY_EVENT_TYPE		Type;
	MENU_ACTION_VEC			vAction;
};
typedef std::vector<MENU_KEY_EVENT> MENU_KEY_EVENT_VEC;

//------------------------------------------------------------------------------
//	メニュー：時間イベント
//------------------------------------------------------------------------------
struct MENU_TIME_EVENT
{
	int						Frame;
	MENU_ACTION				Action;
};
typedef std::vector<MENU_TIME_EVENT> MENU_TIME_EVENT_VEC;

//------------------------------------------------------------------------------
//	メニュー：オブジェクト
//------------------------------------------------------------------------------
struct MENU_OBJECT
{
	Label					BoneName;
	Label					DirBoneName[MENU_DIR_NUM];
	MENU_OBJECT_TYPE		Type;
	std::string				HelpText;

	MENU_SELECTOR			Selector;
	MENU_SLIDER				Slider;
	MENU_LIST				List;
	MENU_SCROLL				Scroll;

	MENU_KEY_EVENT_VEC		vKeyEvent;
};
typedef std::vector<MENU_OBJECT> MENU_OBJECT_VEC;

//------------------------------------------------------------------------------
//	メニュー：レイヤー
//------------------------------------------------------------------------------
struct MENU_LAYER
{
	Label				Name;
	Label				SpriteName;
	Label				OpenAnime;
	Label				WaitAnime;
	Label				CloseAnime;
	Label				RootObject;
	Label				HelpBoneName;
	Point				Pos;
	Color				BackColor;
	bool				bSkipOpen;
	bool				bAnimeWaitEnd;
	bool				bInactive;
	int					CloseIntp;
	std::string			HelpText;

	MENU_CURSOR			Cursor;
	MENU_OBJECT_VEC		vObject;
	
	MENU_ACTION_VEC		vInitAction;
	MENU_ACTION_VEC		vCloseAction;
	MENU_KEY_EVENT_VEC	vKeyEvent;
	MENU_TIME_EVENT_VEC	vTimeEvent;

	std::map< Label, INDICATE_MARKER >		mapMarker;
	std::map< Label, INDICATE_SPIN >		mapSpin;
	std::map< Label, INDICATE_LINE_GRAPH >	mapLineGraph;
};

//------------------------------------------------------------------------------
//	メニューパラメータ
//------------------------------------------------------------------------------
struct MENU_PARAM
{
	Label				RootLayer;
	MENU_ACTION_VEC		vInitAction;
	MENU_ACTION_VEC		vCloseAction;
	MENU_KEY_EVENT_VEC	vKeyEvent;
	MENU_TIME_EVENT_VEC	vTimeEvent;
};