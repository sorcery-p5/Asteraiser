#include "stdafx.h"
#include "AttackTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�U���^��`
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	�U���F���������p�����[�^
//------------------------------------------------------------------------------
ATTACK_CONCURRENT::ATTACK_CONCURRENT( void )
{
	Num				= 0;
	OffsetAngle		= 0.0f;
	AngleRange		= 0.0f;
}

//------------------------------------------------------------------------------
//	�U���p�����[�^
//------------------------------------------------------------------------------
ATTACK_PARAM::ATTACK_PARAM( void )
{
	Type				= ATTACK_TYPE_BULLET;
	Damage				= 0.0f;
	AngleRange			= 0.0;
	Exhaust				= 0.0f;
	HitVel				= 1.0f;
	HitVelType			= ATTACK_VEL_NORMAL;
	EndFrame			= 0;
	ItemNum				= 0;
	ItemSpeed			= 0.0f;
	ScoreRate			= 1.0f;
	TimeKeyLoopFrame	= 0;

	bNotPlayerHit		= false;
	bNotAttackHit		= false;
	bNotGroundHit		= false;
	
	bOwnerErase			= false;
	bNotNotify			= false;

	AppearSoundVol	= 1.0f;
}
