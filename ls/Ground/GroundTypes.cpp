#include "stdafx.h"
#include "GroundTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	地形：型定義
//
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	地形パラメータ
//******************************************************************************
GROUND_PARAM::GROUND_PARAM( void )
{
	Time			= 0;
	bHitEnable		= false;
	
	bNotPlayerHit		= false;
	bNotEnemyHit		= false;
	bNotAttackHit		= false;
	bAttackErase		= true;	
	bAttackHitEffect	= true;
};