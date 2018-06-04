#pragma once

#include "Indicate/IndicateTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	���j���[�F�^��`
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	���j���[�F�J�[�\��
//------------------------------------------------------------------------------
struct MENU_CURSOR
{
	Label			BoneName;
	float			MoveSpeed;
	Label			MoveSound;
	float			MoveSoundVol;
};

//------------------------------------------------------------------------------
//	���j���[�F����
//------------------------------------------------------------------------------
enum MENU_DIR
{
	MENU_DIR_L,			//	��
	MENU_DIR_U,			//	��
	MENU_DIR_R,			//	�E
	MENU_DIR_D,			//	��

	MENU_DIR_NUM,
};

//------------------------------------------------------------------------------
//	���j���[�F�I�����
//------------------------------------------------------------------------------
enum MENU_SELECT_DIR
{
	MENU_SELECT_H,		//	����
	MENU_SELECT_V,		//	����
};

//------------------------------------------------------------------------------
//	���j���[�F�I�u�W�F�N�g���
//------------------------------------------------------------------------------
enum MENU_OBJECT_TYPE
{
	MENU_OBJECT_NONE,		//	�Ȃ�
	MENU_OBJECT_SELECTOR,	//	�Z���N�^�[
	MENU_OBJECT_SLIDER,		//	�X���C�_�[
	MENU_OBJECT_LIST,		//	���X�g
	MENU_OBJECT_SCROLL,		//	�X�N���[��
};

//------------------------------------------------------------------------------
//	���j���[�F�Z���N�^�[
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
//	���j���[�F�X���C�_�[
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
//	���j���[�F���X�g
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
//	���j���[�F�X�N���[��
//------------------------------------------------------------------------------
struct MENU_SCROLL
{
	MENU_SELECT_DIR		SelectDir;
	Label				TargetBone;
	Label				BaseBone;
	float				MoveSpeed;
};

//------------------------------------------------------------------------------
//	���j���[�F�A�N�V�������
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
//	���j���[�F�A�N�V����
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
//	���j���[�F�L�[�C�x���g���
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
//	���j���[�F�L�[�C�x���g
//------------------------------------------------------------------------------
struct MENU_KEY_EVENT
{
	MENU_KEY_EVENT_TYPE		Type;
	MENU_ACTION_VEC			vAction;
};
typedef std::vector<MENU_KEY_EVENT> MENU_KEY_EVENT_VEC;

//------------------------------------------------------------------------------
//	���j���[�F���ԃC�x���g
//------------------------------------------------------------------------------
struct MENU_TIME_EVENT
{
	int						Frame;
	MENU_ACTION				Action;
};
typedef std::vector<MENU_TIME_EVENT> MENU_TIME_EVENT_VEC;

//------------------------------------------------------------------------------
//	���j���[�F�I�u�W�F�N�g
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
//	���j���[�F���C���[
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
//	���j���[�p�����[�^
//------------------------------------------------------------------------------
struct MENU_PARAM
{
	Label				RootLayer;
	MENU_ACTION_VEC		vInitAction;
	MENU_ACTION_VEC		vCloseAction;
	MENU_KEY_EVENT_VEC	vKeyEvent;
	MENU_TIME_EVENT_VEC	vTimeEvent;
};