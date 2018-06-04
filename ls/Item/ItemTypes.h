#pragma once 


////////////////////////////////////////////////////////////////////////////////
//
//	�A�C�e���^��`
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	�A�C�e���p�����[�^
//------------------------------------------------------------------------------
struct ITEM_PARAM
{
	ITEM_PARAM( void );

	Label			EffectName;
	
	int				Time;
	float			Speed;
	float			Decel;
	float			Gravity;
	float			GravityMax;
	int				GravityTime;

	float			CollideRadius;
	float			CollectRadius;
	float			AutoCollectRadius;
	float			CollectAccel;
	float			CollectSpeedMax;
	int				CollectTime;
	float			DeleteHeight;
	float			FieldOutRadius;

	float			SoundVolume;
};