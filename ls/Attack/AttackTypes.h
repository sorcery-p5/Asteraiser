#pragma once

#include "TimeKey/TimeKeyTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�U���^��`
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	�U���^�C�v
//------------------------------------------------------------------------------
enum ATTACK_TYPE
{
	ATTACK_TYPE_BULLET,		//	�e��
	ATTACK_TYPE_HOMING,		//	�z�[�~���O
	ATTACK_TYPE_LASER,		//	���[�U�[
	ATTACK_TYPE_BLADE,		//	�u���[�h
	ATTACK_TYPE_SHIELD,		//	�V�[���h
	ATTACK_TYPE_RULER,		//	���[���[
};

//------------------------------------------------------------------------------
//	�U���ւ̒ʒm
//------------------------------------------------------------------------------
enum ATTACK_NOTIFY
{
	ATTACK_NOTIFY_DAMAGE,	//	�_���[�W���󂯂�
};

//------------------------------------------------------------------------------
//	���������^�C�v
//------------------------------------------------------------------------------
enum ATTACK_VEL_TYPE
{
	ATTACK_VEL_NORMAL,
	ATTACK_VEL_DIR,
};

//------------------------------------------------------------------------------
//	�U���F�e�ۃp�����[�^
//------------------------------------------------------------------------------
struct ATTACK_BULLET
{
	float		Speed;
	float		SpeedRange;
	float		RotSpeed;
	float		RotSpeedRange;

	float		Length;
	float		FrontLength;
	float		Radius;
	float		Decel;
	float		OutRadius;
	bool		bPierce;
	LabelStr	AppearAttack;
	LabelStr	EndEffectName;
};

//------------------------------------------------------------------------------
//	�U���F�U���e�p�����[�^
//------------------------------------------------------------------------------
struct ATTACK_HOMING
{
	float		Speed;
	float		MinSpeed;
	float		MaxSpeed;
	float		Accel;

	float		RotSpeed;
	float		MinRotSpeed;
	float		MaxRotSpeed;
	float		RotAccel;

	float		AccelAngle;
	float		RotAccelAngle;

	int			StartTime;
	int			SearchTime;
	float		Radius;
	float		Length;
	float		FrontLength;
	float		OutRadius;
	bool		bPierce;
	LabelStr	AppearAttack;
};

//------------------------------------------------------------------------------
//	�U���F���[�U�[�p�����[�^
//------------------------------------------------------------------------------
struct ATTACK_LASER
{
	float		Radius;
	float		Length;
	bool		bPierce;
	bool		bAttach;
};

//------------------------------------------------------------------------------
//	�U���F�u���[�h�p�����[�^
//------------------------------------------------------------------------------
struct ATTACK_BLADE
{
	float		Length;
	float		Radius;
	float		Energy;
	float		RepulseExhaust;
	bool		bRepulse;
	bool		bSmash;
	LabelStr	CrashEffectName;
	LabelStr	RepulseEffectName;
};

//------------------------------------------------------------------------------
//	�U���F�V�[���h�p�����[�^
//------------------------------------------------------------------------------
struct ATTACK_SHIELD
{
	float		Radius;
	float		Energy;
	float		Defense;
	bool		bAttach;
	bool		bHide;
	LabelStr	BreakEffectName;
};

//------------------------------------------------------------------------------
//	�U���F���[���[�p�����[�^
//------------------------------------------------------------------------------
struct ATTACK_RULER
{
	float		Radius;
	int			HitEndFrame;
	int			AttackFrame;
	LabelStr	AttackEffectName;
};

//------------------------------------------------------------------------------
//	�U���F���������p�����[�^
//------------------------------------------------------------------------------
struct ATTACK_CONCURRENT
{
	ATTACK_CONCURRENT( void );

	int			Num;
	float		OffsetAngle;
	float		AngleRange;
};

//------------------------------------------------------------------------------
//	�^�C���L�[�̎��
//------------------------------------------------------------------------------
enum ATTACK_TIME_KEY_TYPE
{
	ATTACK_TIME_KEY_EFFECT,
	ATTACK_TIME_KEY_ATTACK,
	ATTACK_TIME_KEY_COLOR,
	ATTACK_TIME_KEY_HIT,

	ATTACK_TIME_KEY_RADIUS,
	ATTACK_TIME_KEY_LENGTH,

	ATTACK_TIME_KEY_SPEED,
	ATTACK_TIME_KEY_ROTSPEED,
	ATTACK_TIME_KEY_ACCEL,

	ATTACK_TIME_KEY_TYPE_NUM,
};

//------------------------------------------------------------------------------
//	�^�C���L�[�p�����[�^
//------------------------------------------------------------------------------
struct ATTACK_TIME_KEY_PARAM
{
	union
	{
		bool				bFlag;
		float				Val[2];

		struct
		{
			float			Rot;
			LabelStr		Name;
		}
		Appear;

		byte				Col[4];
	};
};
typedef TIME_KEY_PARAM<ATTACK_TIME_KEY_PARAM>	ATTACK_TIME_KEY;
typedef std::vector< ATTACK_TIME_KEY >			VecAttackTimeKey;


//------------------------------------------------------------------------------
//	�U���p�����[�^
//------------------------------------------------------------------------------
struct ATTACK_PARAM
{
	ATTACK_PARAM( void );

	ATTACK_TYPE				Type;
	Label					EffectName;
	Label					HitEffectName;
	Label					AppearEffectName;

	float					AngleRange;
	float					Damage;
	float					Exhaust;
	float					HitVel;
	ATTACK_VEL_TYPE			HitVelType;
	int						EndFrame;
	int						TimeKeyLoopFrame;

	int						ItemNum;
	float					ItemSpeed;
	float					ScoreRate;

	bool					bNotPlayerHit;
	bool					bNotAttackHit;
	bool					bNotGroundHit;
	
	bool					bOwnerErase;
	bool					bNotNotify;

	Label					AppearSoundName;
	float					AppearSoundVol;

	ATTACK_CONCURRENT		Conncurrent;

	union
	{
		ATTACK_BULLET		Bullet;
		ATTACK_HOMING		Homing;
		ATTACK_LASER		Laser;
		ATTACK_BLADE		Blade;
		ATTACK_SHIELD		Shield;
		ATTACK_RULER		Ruler;
	};
};