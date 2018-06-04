#pragma once

#include "TimeKey/TimeKeyTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	攻撃型定義
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	攻撃タイプ
//------------------------------------------------------------------------------
enum ATTACK_TYPE
{
	ATTACK_TYPE_BULLET,		//	弾丸
	ATTACK_TYPE_HOMING,		//	ホーミング
	ATTACK_TYPE_LASER,		//	レーザー
	ATTACK_TYPE_BLADE,		//	ブレード
	ATTACK_TYPE_SHIELD,		//	シールド
	ATTACK_TYPE_RULER,		//	ルーラー
};

//------------------------------------------------------------------------------
//	攻撃への通知
//------------------------------------------------------------------------------
enum ATTACK_NOTIFY
{
	ATTACK_NOTIFY_DAMAGE,	//	ダメージを受けた
};

//------------------------------------------------------------------------------
//	反動方向タイプ
//------------------------------------------------------------------------------
enum ATTACK_VEL_TYPE
{
	ATTACK_VEL_NORMAL,
	ATTACK_VEL_DIR,
};

//------------------------------------------------------------------------------
//	攻撃：弾丸パラメータ
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
//	攻撃：誘導弾パラメータ
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
//	攻撃：レーザーパラメータ
//------------------------------------------------------------------------------
struct ATTACK_LASER
{
	float		Radius;
	float		Length;
	bool		bPierce;
	bool		bAttach;
};

//------------------------------------------------------------------------------
//	攻撃：ブレードパラメータ
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
//	攻撃：シールドパラメータ
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
//	攻撃：ルーラーパラメータ
//------------------------------------------------------------------------------
struct ATTACK_RULER
{
	float		Radius;
	int			HitEndFrame;
	int			AttackFrame;
	LabelStr	AttackEffectName;
};

//------------------------------------------------------------------------------
//	攻撃：同時発生パラメータ
//------------------------------------------------------------------------------
struct ATTACK_CONCURRENT
{
	ATTACK_CONCURRENT( void );

	int			Num;
	float		OffsetAngle;
	float		AngleRange;
};

//------------------------------------------------------------------------------
//	タイムキーの種類
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
//	タイムキーパラメータ
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
//	攻撃パラメータ
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