#include "stdafx.h"
#include "ScoreManager.h"
#include "ScoreData.h"

#include "World/World.h"
#include "SaveData/SaveDataTypes.h"

#include "Indicate/Indicate.h"
#include "Indicate/IndicateManager.h"

namespace
{
	const Label	INDICATE_SCORE				= "Score";
	const Label	INDICATE_RATE				= "Rate";
	const Label	INDICATE_HEAT				= "Heat";
	const Label	INDICATE_BONE_SCORE			= "Score";
	const Label	INDICATE_BONE_RATE			= "Rate";
	const Label	INDICATE_BONE_HEAT			= "Marker";

	const Label	INDICATE_ANIME_RATE_ZERO	= "Zero";
	const Label	INDICATE_ANIME_RATE_LOW		= "Low";
	const Label	INDICATE_ANIME_RATE_MID		= "Mid";
	const Label	INDICATE_ANIME_RATE_HIGH	= "High";
	const Label	INDICATE_ANIME_RATE_MAX		= "Max";

	const float DEFAULT_HEAT				= 0.5f;

	const uint	COUNT_MAX					= 999999999;

	const uint	RECORD_INTERVAL				= 60;
};


////////////////////////////////////////////////////////////////////////////////
//
//	�X�R�A�Ǘ�
//
////////////////////////////////////////////////////////////////////////////////
ScoreManager::ScoreManager( World* pWorld, const ScoreData* pData )
{
	_ASSERT( pWorld && pData );

	m_pWorld	= pWorld;
	m_pData		= pData;

	m_Score		= 0;
	m_Rate		= pData->GetParam().Rate.RateMin;
	m_Heat		= DEFAULT_HEAT;

	ZeroArray( m_aTimeValueCount );

	m_RateLockCount	= 0;
	m_Arrival		= 0;
	m_FrameCount	= 0;
	m_ItemCount		= 0;
	m_DestroyCount	= 0;
	
	m_DamageCount	= 0;
	m_BreakCount	= 0;
	m_ContinueCount	= 0;
	m_MissCount		= 0;
	m_CrashCount	= 0;
	m_ItemLostCount	= 0;
}
ScoreManager::~ScoreManager()
{
	
}

//******************************************************************************
//	�X�V
//******************************************************************************
void ScoreManager::Update( void )
{
	//	���ԃp�����[�^�̍X�V
	_UpdateTimeValue();

	//	�C���W�P�[�g�̍X�V
	Indicate* pScoreIndicate	= m_pWorld->GetIndicateManager()->GetIndicate( INDICATE_SCORE );
	Indicate* pRateIndicate		= m_pWorld->GetIndicateManager()->GetIndicate( INDICATE_RATE );
	Indicate* pHeatIndicate		= m_pWorld->GetIndicateManager()->GetIndicate( INDICATE_HEAT );

	//	�X�R�A
	if( pScoreIndicate )	pScoreIndicate->SetText( INDICATE_BONE_SCORE,	"%9d", m_Score );

	//	���[�g
	if( pRateIndicate )
	{
		Label AnimeName = INDICATE_ANIME_RATE_ZERO;

		if( m_Rate >= m_pData->GetParam().Rate.RateMax )		AnimeName = INDICATE_ANIME_RATE_MAX;
		else if( m_Rate >= m_pData->GetParam().Rate.RateHigh )	AnimeName = INDICATE_ANIME_RATE_HIGH;
		else if( m_Rate >= m_pData->GetParam().Rate.RateLow )	AnimeName = INDICATE_ANIME_RATE_MID;
		else if( m_Rate > 0.0f )								AnimeName = INDICATE_ANIME_RATE_LOW;

		if( pRateIndicate->GetAnimeName() != AnimeName ) pRateIndicate->ChangeAnime( AnimeName, 0 );

		pRateIndicate->SetText( INDICATE_BONE_RATE,		"%1.2f", m_Rate );
	}

	//	�q�[�g
	if( pHeatIndicate )		pHeatIndicate->SetMarker( INDICATE_BONE_HEAT,	_GetHeatRate() );

	//	�J�E���g�_�E��
	DecreaseZero( m_RateLockCount, 1 );

	//	�t���[���v��
	Increase( m_FrameCount, COUNT_MAX, 1u );
	
	//	�������L�^
	if( m_FrameCount % RECORD_INTERVAL == 0 )
	{
		SCORE_RECORD Record;
		Record.Score	= m_Score;
		Record.Rate		= m_Rate;
		Record.Heat		= m_Heat;

		m_vRecord.push_back( Record );
	}
}

//******************************************************************************
//	�A�C�e���擾
//******************************************************************************
void ScoreManager::OnGetItem( void )
{
	//	�X�R�A�㏸
	int Add = (int)floor( m_pData->GetParam().ScoreBase * m_Rate );
	Increase( m_Score, m_pData->GetParam().ScoreMax, Add );

	//	���J�E���g
	Increase( m_ItemCount, COUNT_MAX, 1u );
}

//******************************************************************************
//	�X�R�A�C�x���g
//******************************************************************************
void ScoreManager::OnEvent( SCORE_EVENT_TYPE Type, float Rate )
{
	const SCORE_EVENT* pEvent = m_pData->GetEvent( Type );
	if( !pEvent ) return;

	_AddHeat( pEvent->Heat );
	_AddRate( pEvent->Rate * Rate );

	//	���j���J�E���g
	switch( Type )
	{
	case SCORE_EVENT_ENEMY_DESTROY:		Increase( m_DestroyCount,	COUNT_MAX, 1u );		break;
	case SCORE_EVENT_DAMAGE:			Increase( m_DamageCount,	COUNT_MAX, 1u );		break;
	case SCORE_EVENT_BREAK:				Increase( m_BreakCount,		COUNT_MAX, 1u );		break;
	case SCORE_EVENT_MISS:				Increase( m_MissCount,		COUNT_MAX, 1u );		break;
	case SCORE_EVENT_CRASH:				Increase( m_CrashCount,		COUNT_MAX, 1u );		break;
	case SCORE_EVENT_LOST_ITEM:			Increase( m_ItemLostCount,	COUNT_MAX, 1u );		break;
	}
}

//******************************************************************************
//	�R���e�B�j���[�ʒm
//******************************************************************************
void ScoreManager::OnContinue( void )
{
	//	�X�R�A�͔���
	m_Score		/= 2;

	//	���[�g�n�͏����l
	m_Rate		= m_pData->GetParam().Rate.RateMin;
	m_Heat		= DEFAULT_HEAT;

	//	�񐔂��J�E���g
	Increase( m_ContinueCount,	COUNT_MAX, 1u );
}

//******************************************************************************
//	���ԕω��p�����[�^�̍X�V�𒆒f
//******************************************************************************
void ScoreManager::StopTimeValue( SCORE_TIME_VALUE_TYPE Type )
{
	m_aTimeValueCount[Type] = 0;
}

//******************************************************************************
//	���B�n�_���X�V
//******************************************************************************
void ScoreManager::Arrive( int Arrival )
{
	m_Arrival = Max( m_Arrival, Arrival );

	//	�������L�^
	m_vArrivalIndex.push_back( m_vRecord.size() );
}

//******************************************************************************
//	�X�e�[�W�̏I��
//******************************************************************************
void ScoreManager::EndStage( void )
{
	if( m_vRecord.empty() ) return;

	SCORE_STAGE_RECORD Record;
	ZeroStruct( Record );
	
	//	���Ԃ̒l���L�^
	Record.Score			= m_Score;

	Record.ItemCount		= m_ItemCount;
	Record.DestroyCount		= m_DestroyCount;
	Record.FrameCount		= m_FrameCount;

	Record.DamageCount		= m_DamageCount;
	Record.BreakCount		= m_BreakCount;
	Record.ContinueCount	= m_ContinueCount;
	Record.MissCount		= m_MissCount;
	Record.CrashCount		= m_CrashCount;
	Record.ItemLostCount	= m_ItemLostCount;

	//	�X�e�[�W���ς��Z�o
	for( int i = 0; i < (int)m_vRecord.size(); i++ )
	{
		Record.Rate	+= m_vRecord[i].Rate;
		Record.Heat	+= m_vRecord[i].Heat;
	}

	Record.Rate	/= m_vRecord.size();	
	Record.Heat	/= m_vRecord.size();
		
	m_vStageRecord.push_back( Record );

	//	�������N���A	
	m_vRecord.clear();
	m_vArrivalIndex.clear();
}

//******************************************************************************
//	�X�R�A�f�[�^�̎擾
//******************************************************************************
void ScoreManager::GetScoreData( SAVEDATA_SCORE& ScoreData ) const
{
	//	����
	String256 DateTime;
	SYSTEMTIME t;
	::GetLocalTime(&t); 
	DateTime.Format( "%04d/%02d/%02d %02d:%02d:%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond );

	ScoreData.DateTime = DateTime;

	//	���B
	ScoreData.Arrival		= m_Arrival;

	//	�J�E���g
	ScoreData.Score			= m_Score;
	ScoreData.ItemCount		= m_ItemCount;
	ScoreData.DestroyCount	= m_DestroyCount;
	ScoreData.FrameCount	= m_FrameCount;

	ScoreData.DamageCount	= m_DamageCount;
	ScoreData.BreakCount	= m_BreakCount;
	ScoreData.ContinueCount	= m_ContinueCount;
	ScoreData.MissCount		= m_MissCount;
	ScoreData.CrashCount	= m_CrashCount;
	ScoreData.ItemLostCount	= m_ItemLostCount;

	//	����
	if( !m_vStageRecord.empty() )
	{
		for( int i = 0; i < (int)m_vStageRecord.size(); i++ )
		{
			ScoreData.RateAverage	+= m_vStageRecord[i].Rate;
			ScoreData.HeatAverage	+= m_vStageRecord[i].Heat;
		}

		ScoreData.RateAverage /= m_vStageRecord.size();	
		ScoreData.HeatAverage /= m_vStageRecord.size();
	}
}

//******************************************************************************
//	�X�e�[�W���ʂ��擾
//******************************************************************************
void ScoreManager::GetStageResult( SCORE_STAGE_RECORD& Result )	const
{
	//	�J�E���g�擾
	Result.Score		= m_Score;
	Result.ItemCount	= m_ItemCount;
	Result.DestroyCount	= m_DestroyCount;
	Result.FrameCount	= m_FrameCount;

	Result.DamageCount		= m_DamageCount;
	Result.BreakCount		= m_BreakCount;
	Result.ContinueCount	= m_ContinueCount;
	Result.MissCount		= m_MissCount;
	Result.CrashCount		= m_CrashCount;
	Result.ItemLostCount	= m_ItemLostCount;

	//	��O�Ɣ�r
	if( !m_vStageRecord.empty() )
	{
		const SCORE_STAGE_RECORD& Last = m_vStageRecord.back();

		Result.Score		-= Last.Score;
		Result.ItemCount	-= Last.ItemCount;
		Result.DestroyCount	-= Last.DestroyCount;
		Result.FrameCount	-= Last.FrameCount;
			
		Result.DamageCount		-= Last.DamageCount;
		Result.BreakCount		-= Last.BreakCount;
		Result.ContinueCount	-= Last.ContinueCount;
		Result.MissCount		-= Last.MissCount;
		Result.CrashCount		-= Last.CrashCount;
		Result.ItemLostCount	-= Last.ItemLostCount;
	}

	//	�X�e�[�W���ς��Z�o
	if( !m_vRecord.empty() )
	{
		for( int i = 0; i < (int)m_vRecord.size(); i++ )
		{
			Result.Rate	+= m_vRecord[i].Rate;
			Result.Heat	+= m_vRecord[i].Heat;
		}

		Result.Rate	/= m_vRecord.size();	
		Result.Heat	/= m_vRecord.size();
	}
}

//------------------------------------------------------------------------------
//	���ԃp�����[�^�̍X�V
//------------------------------------------------------------------------------
void ScoreManager::_UpdateTimeValue( void )
{
	for( int i = 0; i < SCORE_TIME_VALUE_TYPE_NUM; i++ )
	{
		SCORE_TIME_VALUE_TYPE Type = SCORE_TIME_VALUE_TYPE(i);

		const SCORE_TIME_VALUE* pTimeValue = m_pData->GetTimeValue( Type );
		if( !pTimeValue ) continue;

		if( m_aTimeValueCount[Type] >= pTimeValue->Time )
		{
			_AddHeat( pTimeValue->Heat );
			_AddRate( pTimeValue->Rate );
		}
		m_aTimeValueCount[Type]++;
	}
}

//------------------------------------------------------------------------------
//	���[�g��ω�
//------------------------------------------------------------------------------
void ScoreManager::_AddRate( float Rate )
{
	//	�q�[�g�ɂ��ω�
	float t = Coef( m_Heat, m_pData->GetParam().Heat.HeatMin, m_pData->GetParam().Heat.HeatMax );

	if( Rate > 0.0f )
	{
		Rate *= Lerp( m_pData->GetParam().Heat.RatePlusMin, m_pData->GetParam().Heat.RatePlusMax, t );

		StopTimeValue( SCORE_TIME_VALUE_DECAY_RATE );
	}
	else if( Rate < 0.0f )
	{
		//	���b�N
		if( m_RateLockCount > 0 )
		{
			return;
		}

		Rate *= Lerp( m_pData->GetParam().Heat.RateMinusMin, m_pData->GetParam().Heat.RateMinusMax, t );
	}
	else
	{
		return;
	}

	m_Rate = Clamp( m_Rate + Rate, m_pData->GetParam().Rate.RateMin, m_pData->GetParam().Rate.RateMax );
}

//------------------------------------------------------------------------------
//	�q�[�g��ω�
//------------------------------------------------------------------------------
void ScoreManager::_AddHeat( float Heat )
{
	m_Heat = Clamp( m_Heat + Heat, m_pData->GetParam().Heat.HeatMin, m_pData->GetParam().Heat.HeatMax );
}

//------------------------------------------------------------------------------
//	�q�[�g���擾
//------------------------------------------------------------------------------
float ScoreManager::_GetHeatRate( void ) const
{
	return Coef( m_Heat, m_pData->GetParam().Heat.HeatMin, m_pData->GetParam().Heat.HeatMax );
}