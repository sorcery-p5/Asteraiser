#include "stdafx.h"
#include "PlayerTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	プレイヤー型定義
//
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	ブレード：エネルギーパラメータ
//******************************************************************************
BLADE_ENERGY_PARAM::BLADE_ENERGY_PARAM( void )
{
	ExhaustEnergyRate	= 0.0f;
	ExhaustLimitRate	= 0.0f;

	NormalRegen			= 0.0f;
	NormalLimitRegen	= 0.0f;
	ItemRegen			= 0.0f;
	ItemLimitRegen		= 0.0f;
	ReloadRegen			= 0.0f;
	CrashRegen			= 0.0f;
	CancelEnergyDecay	= 0.0f;

	UseRegenEnergy		= 0.0f;
	UseLimitEnergy		= 0.0f;
	UseReloadEnergy		= 0.0f;
}

//******************************************************************************
//	ブレード：スマッシュパラメータ
//******************************************************************************
BLADE_SMASH_PARAM::BLADE_SMASH_PARAM( void )
{
	ChargeStartTime		= 0;
	ChargeTime			= 0;
	ChargeMaxTime		= 0;
	RulerChargeTime		= 0;
	Length				= 0.0f;
	Radius				= 0.0f;

	Attack				= 0.0f;
	AttackVel			= 0.0f;
	Knockback			= 0;

	ExhaustRate			= 0.0f;
	ChargeExhaustEnergy	= 0.0f;
	ChargeExhaustLimit	= 0.0f;
}

//******************************************************************************
//	ブレードパラメータ
//******************************************************************************
BLADE_PARAM::BLADE_PARAM( void )
{
	Length				= 0.0f;
	Radius				= 0.0f;
	NormalDelay			= 0;
	HitDelay			= 0;
	RulerDelay			= 0;
	CancelTime			= 0;
	NotExhaustTime		= 0;
	NotReloadTime		= 0;

	Attack				= 0.0f;
	AttackEnergyRate	= 0.0f;
	AttackLimitRate		= 0.0f;
	RepulseAttack		= 0.0f;
	AttackVel			= 0.0f;
	Knockback			= 0;
	RepulseInterval		= 0;

	HitItemNum			= 0;
	RepulseItemNum		= 0;
	CrashItemNum		= 0;
	ReloadItemNum		= 0;
	ItemSpeed			= 0.0f;
}

//******************************************************************************
//	スピアパラメータ
//******************************************************************************
SPEAR_PARAM::SPEAR_PARAM( void )
{
	Length				= 0.0f;
	Radius				= 0.0f;
	Speed				= 0.0f;
	Time				= 0;
	AttackVel			= 0;
	Knockback			= 0;
	GroundExhaust		= 0.0f;

	Energy				= 0.0f;
	EnergyMax			= 0.0f;
	EnergyItemRate		= 0.0f;
	EnergyDecayRate		= 0.0f;

	Attack				= 0.0f;
	AttackMax			= 0.0f;
	AttackItemRate		= 0.0f;
	AttackDecayRate		= 0.0f;
	
	HitItemNum			= 0;
	CrashItemNum		= 0;
	ItemSpeed			= 0.0f;
}

//******************************************************************************
//	ルーラーパラメータ
//******************************************************************************
RULER_PARAM::RULER_PARAM( void )
{
	Radius				= 0.0f;
	RadiusMax			= 0.0f;
	RadiusItemRate		= 0.0f;
	Time				= 0;
	TimeMax				= 0;
	TimeItemRate		= 0.0f;
	
	CrashItemNum		= 0;
	ItemSpeed			= 0.0f;
}

//******************************************************************************
//	オーヴァドライヴパラメータ
//******************************************************************************
OVERDRIVE_PARAM::OVERDRIVE_PARAM( void )
{
	NormalRegen				= 0.0f;
	ItemRegen				= 0.0f;
	UseRegenEnergy			= 0.0f;

	Delay					= 0;
}

//******************************************************************************
//	ダッシュ：払い抜けパラメータ
//******************************************************************************
DASH_SPLASH_PARAM::DASH_SPLASH_PARAM( void )
{
	CollideRadius		= 0.0f;
	RulerRadius			= 0.0f;
	Attack				= 0.0;
	AttackVel			= 0.0f;
	Knockback			= 0;
	ExhaustRate			= 0.0f;
	HitInvincible		= 0;
}

//******************************************************************************
//	ダッシュパラメータ
//******************************************************************************
DASH_PARAM::DASH_PARAM( void )
{
	MoveLength			= 0.0f;
	Delay				= 0;
	RulerDelay			= 0;
}

//******************************************************************************
//	シールドパラメータ
//******************************************************************************
SHIELD_PARAM::SHIELD_PARAM( void )
{
	DamageInvincible	= 0;
	BreakInvincible		= 0;
	ReviveInvincible	= 0;
	DamageNoMove		= 0;
	BreakTime			= 0;
	SerialDamageTime	= 0;

	NormalRegen			= 0.0f;
	ItemRegen			= 0.0f;
	NormalRecovery		= 0.0f;
	ItemRecovery		= 0.0f;
	BreakNumMax			= 0;
	BreakNumDecayRate	= 0.0f;	
	UseRegenEnergy		= 0.0f;
	
	BreakItemNum		= 0;
	ItemSpeed			= 0.0f;
}

//******************************************************************************
//	プレイヤーパラメータ
//******************************************************************************
PLAYER_PARAM::PLAYER_PARAM( void )
{
	MoveSpeed			= 0.0f;
	SlowSpeed			= 0.0f;
	Decel				= 0.0f;
	CrashVel			= 0.0f;
	BreakVel			= 0.0f;
	KnockbackTime		= 0;
	ReversalTime		= 0;
	RepulseInvincible	= 0;
	
	RulerMoveSpeed		= 0.0f;

	RegenEnergy			= 0.0f;
	ItemEnergy			= 0.0f;

	HitRadius			= 0.0f;
	ItemCollectRadius	= 0.0f;
}