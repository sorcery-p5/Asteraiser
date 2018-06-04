#include "stdafx.h"
#include "ScoreTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�X�R�A�F�^��`
//
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	�X�R�A�F���[�g�p�����[�^
//******************************************************************************
SCORE_RATE::SCORE_RATE( void )
{
	RateMin		= 0.0f;
	RateMax		= 0.0f;
	
	RateLow		= 0.0f;
	RateHigh	= 0.0f;
}

//******************************************************************************
//	�X�R�A�F�q�[�g�p�����[�^
//******************************************************************************
SCORE_HEAT::SCORE_HEAT( void )
{
	HeatMin			= 0.0f;
	HeatMax			= 0.0f;
	RatePlusMin		= 0.0f;
	RatePlusMax		= 0.0f;
	RateMinusMin	= 0.0f;
	RateMinusMax	= 0.0f;
}

//******************************************************************************
//	�X�R�A�p�����[�^
//******************************************************************************
SCORE_PARAM::SCORE_PARAM( void )
{
	ScoreBase	= 100;
	ScoreMax	= 999999999;
}

//******************************************************************************
//	�X�R�A�E�X�e�[�W�L�^
//******************************************************************************
void SCORE_STAGE_RECORD::Reset( void )
{
	Score			= 0;
	Rate			= 0.0f;
	Heat			= 0.0f;

	ItemCount		= 0;
	DestroyCount	= 0;
	FrameCount		= 0;

	DamageCount		= 0;
	BreakCount		= 0;
	ContinueCount	= 0;
	MissCount		= 0;
	CrashCount		= 0;
	ItemLostCount	= 0;
}

//------------------------------------------------------------------------------
//	��r
//------------------------------------------------------------------------------
bool SCORE_STAGE_RECORD::operator !=( const SCORE_STAGE_RECORD& r )
{
	return	Score			!= r.Score			||
			Rate			!= r.Rate			||
			Heat			!= r.Heat			||

			ItemCount		!= r.ItemCount		||
			DestroyCount	!= r.DestroyCount	||
			FrameCount		!= r.FrameCount;
}
