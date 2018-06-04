#include "stdafx.h"
#include "EnemyTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	ìGå^íËã`
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	ìGÉpÉâÉÅÅ[É^
//------------------------------------------------------------------------------
ENEMY_PARAM::ENEMY_PARAM( void )
{
	Life				= 0;
	Decel				= 0.0f;
	DamageInvincible	= 0;
	Defense				= 1.0f;
	ExhaustRate			= 1.0f;
	ReduceVel			= 1.0f;
	ReduceKnockback		= 1.0f;
	BodyDamage			= 0.0f;
	BodyDamageVel		= 0.0f;
	ItemNum				= 0;
	ItemSpeed			= 0.0f;
	ScoreRate			= 1.0f;
	CrashKnockback		= 0;
	CrashVel			= 0.0f;
	CollideRadius		= 0.0f;
	bCollideFrame		= false;
	bHitEnable			= true;
	bGroundHit			= true;
	bPlayerHit			= true;
	bRepulse			= true;
	bNotRepulseNotify	= false;
	bNoCount			= false;
	bBoss				= false;
	bOutField			= false;
	bNoIndicate			= false;
	DeadColor			= Color::White();
}
