#include "stdafx.h"
#include "GroundTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�n�`�F�^��`
//
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	�n�`�p�����[�^
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