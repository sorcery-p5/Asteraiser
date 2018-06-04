#pragma once


////////////////////////////////////////////////////////////////////////////////
//
//	セーブデータ型定義
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	セーブデータ
//------------------------------------------------------------------------------
struct SAVEDATA
{
	SAVEDATA( void )					{ Reset(); }
	void			Reset( void );

	int				Arrival;
	uint			PlayNum;
	uint			ClearNum;

	uint			TotalScore;
	uint			ItemCount;
	uint			DestroyCount;
	uint			FrameCount;

	bool			bNoDamage;
	bool			bNoBreak;
	bool			bNoContinue;
	bool			bNoMiss;
	bool			bNoCrash;
	bool			bNoLost;
	
	std::string		LastDateTime;
};

//------------------------------------------------------------------------------
//	セーブデータ：スコア
//------------------------------------------------------------------------------
struct SAVEDATA_SCORE
{
	SAVEDATA_SCORE( void );

	int				Arrival;
	uint			PlayID;

	uint			Score;
	uint			ItemCount;
	uint			DestroyCount;
	uint			FrameCount;

	uint			DamageCount;
	uint			BreakCount;
	uint			ContinueCount;
	uint			MissCount;
	uint			CrashCount;
	uint			ItemLostCount;

	float			RateAverage;
	float			HeatAverage;

	std::string		DateTime;
};