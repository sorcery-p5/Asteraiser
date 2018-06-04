#pragma once


////////////////////////////////////////////////////////////////////////////////
//
//	スコア：型定義
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	スコア：レートパラメータ
//------------------------------------------------------------------------------
struct SCORE_RATE
{
	SCORE_RATE( void );

	float		RateMin;
	float		RateMax;
	
	float		RateLow;
	float		RateHigh;
};

//------------------------------------------------------------------------------
//	スコア：ヒートパラメータ
//------------------------------------------------------------------------------
struct SCORE_HEAT
{
	SCORE_HEAT( void );

	float		HeatMin;
	float		HeatMax;
	float		RatePlusMin;
	float		RatePlusMax;
	float		RateMinusMin;
	float		RateMinusMax;
};

//------------------------------------------------------------------------------
//	スコアパラメータ
//------------------------------------------------------------------------------
struct SCORE_PARAM
{
	SCORE_PARAM( void );

	int				ScoreBase;
	int				ScoreMax;

	SCORE_RATE		Rate;
	SCORE_HEAT		Heat;
};

//------------------------------------------------------------------------------
//	スコアイベント種別
//------------------------------------------------------------------------------
enum SCORE_EVENT_TYPE
{
	SCORE_EVENT_HIT,
	SCORE_EVENT_COMBO,
	SCORE_EVENT_MISS,
	SCORE_EVENT_CRASH,
	SCORE_EVENT_CANCEL,

	SCORE_EVENT_BULLET_ERASE,
	SCORE_EVENT_BLADE_ERASE,

	SCORE_EVENT_DAMAGE,
	SCORE_EVENT_SERIAL_DAMAGE,
	SCORE_EVENT_BREAK,

	SCORE_EVENT_ENEMY_DESTROY,
	SCORE_EVENT_ENEMY_ESCAPE,

	SCORE_EVENT_GET_ITEM,
	SCORE_EVENT_LOST_ITEM,

	SCORE_EVENT_OVERDRIVE,
	SCORE_EVENT_SPEAR,
	SCORE_EVENT_RULER,
	
	SCORE_EVENT_TYPE_NUM,
	SCORE_EVENT_NULL		= -1,
};

//------------------------------------------------------------------------------
//	スコアイベント
//------------------------------------------------------------------------------
struct SCORE_EVENT
{
	float			Rate;
	float			Heat;
};

//------------------------------------------------------------------------------
//	スコア・時間変化値の種別
//------------------------------------------------------------------------------
enum SCORE_TIME_VALUE_TYPE
{
	SCORE_TIME_VALUE_DECAY_RATE,
	SCORE_TIME_VALUE_NO_DAMAGE,
	SCORE_TIME_VALUE_ENEMY_EXIST,
	SCORE_TIME_VALUE_ATTACK_EXIST,
	
	SCORE_TIME_VALUE_TYPE_NUM,
	SCORE_TIME_VALUE_NULL		= -1,
};

//------------------------------------------------------------------------------
//	スコア・時間変化値
//------------------------------------------------------------------------------
struct SCORE_TIME_VALUE
{
	int			Time;
	float		Rate;
	float		Heat;
};

//------------------------------------------------------------------------------
//	スコア・履歴記録
//------------------------------------------------------------------------------
struct SCORE_RECORD
{
	uint			Score;
	float			Rate;
	float			Heat;
};

//------------------------------------------------------------------------------
//	スコア・ステージ記録
//------------------------------------------------------------------------------
struct SCORE_STAGE_RECORD
{
	SCORE_STAGE_RECORD( void )			{ Reset(); }

	void			Reset( void );

	bool			operator !=( const SCORE_STAGE_RECORD& r );

	int				Score;
	float			Rate;
	float			Heat;

	uint			ItemCount;
	uint			DestroyCount;
	uint			FrameCount;

	uint			DamageCount;
	uint			BreakCount;
	uint			ContinueCount;
	uint			MissCount;
	uint			CrashCount;
	uint			ItemLostCount;
};

typedef std::vector< SCORE_RECORD >			SCORE_RECORD_VEC;
typedef std::vector< SCORE_STAGE_RECORD >	SCORE_STAGE_RECORD_VEC;