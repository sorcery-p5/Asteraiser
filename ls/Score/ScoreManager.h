#pragma once

#include "ScoreTypes.h"

class World;
class ScoreData;
struct SAVEDATA_SCORE;


////////////////////////////////////////////////////////////////////////////////
//
//	ÉXÉRÉAä«óù
//
////////////////////////////////////////////////////////////////////////////////
class ScoreManager
{
public:
	ScoreManager( World* pWorld, const ScoreData* pData );
	~ScoreManager();

	void							Update( void );

	void							OnGetItem( void );
	void							OnEvent( SCORE_EVENT_TYPE Type, float Rate = 1.0f );
	void							OnContinue( void );

	void							StopTimeValue( SCORE_TIME_VALUE_TYPE Type );
	void							SetReteLock( int Time )									{ m_RateLockCount = Max( Time, m_RateLockCount ); }

	void							Arrive( int Arrival );
	void							EndStage( void );
	void							GetScoreData( SAVEDATA_SCORE& ScoreData )		const;
	void							GetStageResult( SCORE_STAGE_RECORD& Result )	const;
	const SCORE_RECORD_VEC&			GetRecord( void )								const	{ return m_vRecord; }
	const SCORE_STAGE_RECORD_VEC&	GetStageRecord( void )							const	{ return m_vStageRecord; }
	const IntVec&					GetArrivalIndex( void )							const	{ return m_vArrivalIndex; }

	const ScoreData*				GetData( void )									const	{ return m_pData; }
	uint							GetScore( void )								const	{ return m_Score; }
	float							GetRate( void )									const	{ return m_Rate; }
	float							GetHeat( void )									const	{ return m_Heat; }

private:
	void							_UpdateTimeValue( void );
	void							_AddRate( float Rate );
	void							_AddHeat( float Heat );
	float							_GetHeatRate( void )							const;

private:
	World*							m_pWorld;
	const ScoreData*				m_pData;

	int								m_Score;
	float							m_Rate;
	float							m_Heat;

	int								m_aTimeValueCount[ SCORE_TIME_VALUE_TYPE_NUM ];
	int								m_RateLockCount;

	int								m_Arrival;

	uint							m_FrameCount;
	uint							m_ItemCount;
	uint							m_DestroyCount;

	uint							m_DamageCount;
	uint							m_BreakCount;
	uint							m_ContinueCount;
	uint							m_MissCount;
	uint							m_CrashCount;
	uint							m_ItemLostCount;

	SCORE_STAGE_RECORD_VEC			m_vStageRecord;
	SCORE_RECORD_VEC				m_vRecord;
	IntVec							m_vArrivalIndex;
};