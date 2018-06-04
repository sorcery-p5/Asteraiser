#include "stdafx.h"
#include "SaveDataTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	セーブデータ型定義
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	セーブデータ
//------------------------------------------------------------------------------
void SAVEDATA::Reset( void )
{
	Arrival			= 0;
	PlayNum			= 0;
	ClearNum		= 0;

	TotalScore		= 0;
	ItemCount		= 0;
	DestroyCount	= 0;
	FrameCount		= 0;
	
	bNoDamage		= false;
	bNoBreak		= false;
	bNoContinue		= false;
	bNoMiss			= false;
	bNoCrash		= false;
	bNoLost			= false;
}

//------------------------------------------------------------------------------
//	セーブデータ：スコア
//------------------------------------------------------------------------------
SAVEDATA_SCORE::SAVEDATA_SCORE( void )
{
	Arrival			= 0;
	PlayID			= 0;

	Score			= 0;
	ItemCount		= 0;
	DestroyCount	= 0;
	FrameCount		= 0;

	DamageCount		= 0;
	BreakCount		= 0;
	ContinueCount	= 0;
	MissCount		= 0;
	CrashCount		= 0;
	ItemLostCount	= 0;

	RateAverage		= 0.0f;
	HeatAverage		= 0.0f;

	DateTime		= "0000/00/00 00:00:00";
}