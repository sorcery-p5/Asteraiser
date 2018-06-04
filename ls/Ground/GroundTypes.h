#pragma once


////////////////////////////////////////////////////////////////////////////////
//
//	地形：型定義
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	地形パラメータ
//------------------------------------------------------------------------------
struct GROUND_PARAM
{
	GROUND_PARAM( void );

	Label		SpriteName;
	Label		AnimeName;
	Label		StartAnimeName;
	Label		EndAnimeName;

	Label		HitEffectName;

	Point		Speed;
	Angle		RotSpeed;
	int			Time;
	bool		bHitEnable;
	
	bool		bNotPlayerHit;
	bool		bNotEnemyHit;
	bool		bNotAttackHit;
	bool		bAttackErase;
	bool		bAttackHitEffect;
};