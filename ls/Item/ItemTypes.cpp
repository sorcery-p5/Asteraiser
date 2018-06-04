#include "stdafx.h"
#include "ItemTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	アイテム型定義
//
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	アイテムパラメータ
//******************************************************************************
ITEM_PARAM::ITEM_PARAM( void )
{
	Time				= 0;
	Speed				= 0.0f;
	Decel				= 0.0f;
	Gravity				= 0.0f;
	GravityMax			= 0.0f;
	GravityTime			= 0;

	CollideRadius		= 0.0f;
	CollectRadius		= 0.0f;
	AutoCollectRadius	= 0.0f;
	CollectAccel		= 0.0f;
	CollectSpeedMax		= 0.0f;
	CollectTime			= 0;
	DeleteHeight		= 0.0f;

	SoundVolume			= 1.0f;
};