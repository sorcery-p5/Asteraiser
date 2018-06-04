#pragma once

#include "TimeKey/TimeKeyTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�^��`
//
////////////////////////////////////////////////////////////////////////////////

struct EFFECT_APPEAR;


//------------------------------------------------------------------------------
//	�G�t�F�N�g�p�����[�^
//------------------------------------------------------------------------------
struct EFFECT_PARAM
{
	EFFECT_PARAM( void );

	uint				EndFrame;		//	�I���t���[��
	uint				LoopFrame;		//	�I���t���[��
	bool				bLoop;			//	���[�v
};

//------------------------------------------------------------------------------
//	�p�[�e�B�N�����
//------------------------------------------------------------------------------
enum EFFECT_PARTICLE_TYPE
{
	EFFECT_PARTICLE_DUMMY,					//	�_�~�[
	EFFECT_PARTICLE_SPRITE,					//	�X�v���C�g
	EFFECT_PARTICLE_SPRITE_FRAME,			//	�X�v���C�g�t���[��
	EFFECT_PARTICLE_LINE,					//	���C��
	EFFECT_PARTICLE_FONT,					//	�t�H���g
	EFFECT_PARTICLE_SOUND,					//	�T�E���h
	EFFECT_PARTICLE_EFFECT,					//	�G�t�F�N�g
};

//------------------------------------------------------------------------------
//	���C���p�[�e�B�N���̎��
//------------------------------------------------------------------------------
enum EFFECT_PARTICLE_LINE_TYPE
{
	EFFECT_PARTICLE_LINE_CENTER,			//	���S�_�^
	EFFECT_PARTICLE_LINE_TIP,				//	�[�_�^
};

//------------------------------------------------------------------------------
//	�X�v���C�g�p�[�e�B�N���p�����[�^
//------------------------------------------------------------------------------
struct EFFECT_PARTICLE_SPRITE_PARAM
{
	Label						PieceName;	//	�s�[�X��
	Point						Center;		//	���S�ʒu
	int							Index;		//	�C���f�N�X
};

//------------------------------------------------------------------------------
//	�X�v���C�g�t���[���p�[�e�B�N���p�����[�^
//------------------------------------------------------------------------------
struct EFFECT_PARTICLE_SPRITE_FRAME_PARAM
{
	Label						Name;		//	�X�v���C�g�t���[����
	Label						AnimeName;	//	�A�j����
};

//------------------------------------------------------------------------------
//	���C���p�[�e�B�N���p�����[�^
//------------------------------------------------------------------------------
struct EFFECT_PARTICLE_LINE_PARAM
{
	EFFECT_PARTICLE_LINE_TYPE	Type;		//	���
	Label						PieceName;	//	�s�[�X��
	float						Width;		//	��
	int							Num;		//	���C����
	int							Divide;		//	���C��������
	int							Interval;	//	�_��X�V�p�x
	std::vector<Color>			vColor;		//	�F
	IntVec						vIndex;		//	�y�[�W�C���f�N�X
};

//------------------------------------------------------------------------------
//	�t�H���g�p�[�e�B�N���p�����[�^
//------------------------------------------------------------------------------
struct EFFECT_PARTICLE_FONT_PARAM
{
	Label						Name;		//	�t�H���g��
	std::string					Text;		//	�e�L�X�g
	FONT_ALIGN					Align;		//	�A���C��
};

//------------------------------------------------------------------------------
//	�T�E���h�p�[�e�B�N���p�����[�^
//------------------------------------------------------------------------------
struct EFFECT_PARTICLE_SOUND_PARAM
{
	Label						Name;		//	�T�E���h��
	float						Vol;		//	�{�����[��
	bool						bPan;		//	�ʒu�ɂ��킹�ăp������
	bool						bNoPause;	//	��~���Ȃ�
};

//------------------------------------------------------------------------------
//	�G�t�F�N�g�p�[�e�B�N���p�����[�^
//------------------------------------------------------------------------------
struct EFFECT_PARTICLE_EFFECT_PARAM
{
	Label						Name;		//	�G�t�F�N�g��
};

//------------------------------------------------------------------------------
//	�ړ��p�����[�^
//------------------------------------------------------------------------------
struct EFFECT_MOVE_VELOCITY_PARAM
{
	float						Decel;		//	�����W��
	float						Gravity;	//	�d�͉e��
	float						Force;		//	�͏�e��
	float						Flex;		//	�����W��
	bool						bHitEnable;	//	������e��
};

//------------------------------------------------------------------------------
//	�^�C���L�[�̎��
//------------------------------------------------------------------------------
enum EFFECT_TIME_KEY_TYPE
{
	//	�\��
	EFFECT_TIME_KEY_COLOR,			//	�F
	EFFECT_TIME_KEY_INDEX,			//	�C���f�N�X

	//	�t���O
	EFFECT_TIME_KEY_UPDATE_ENABLE,	//	�X�V

	//	�T�E���h
	EFFECT_TIME_KEY_VOL,			//	�{�����[��

	//	�ړ��ėp
	EFFECT_TIME_KEY_ROT,			//	�p�x
	EFFECT_TIME_KEY_ROTSPEED,		//	�p���x
	EFFECT_TIME_KEY_SCALE,			//	�g�k

	//	�ړ��^
	EFFECT_TIME_KEY_VEL,			//	�ړ���
	EFFECT_TIME_KEY_SPEED,			//	���x

	//	�Ǐ]�^
	EFFECT_TIME_KEY_POS,			//	�ʒu
	
	//	��]�^
	EFFECT_TIME_KEY_DIST,			//	����
	EFFECT_TIME_KEY_ANGLE,			//	�~����p�x
	EFFECT_TIME_KEY_ANGLE_SPEED,	//	�~����p���x
	EFFECT_TIME_KEY_AXIS_SCALE,		//	���g�k
	EFFECT_TIME_KEY_AXIS_ANGLE,		//	���p�x

	EFFECT_TIME_KEY_NUM,

	EFFECT_TIME_KEY_MOVE_BEGIN		= EFFECT_TIME_KEY_ROT,	//	�ړ��^
};

//------------------------------------------------------------------------------
//	�^�C���L�[�p�����[�^
//------------------------------------------------------------------------------
struct EFFECT_TIME_KEY_PARAM
{
	union
	{
		float				Val[2];
		byte				Col[4];
		byte				bFlag;

		struct
		{
			char			ID;
			char			Index;
		}
		Index;
	};
};
typedef TIME_KEY_PARAM<EFFECT_TIME_KEY_PARAM>	EFFECT_TIME_KEY;
typedef std::vector< EFFECT_TIME_KEY >			VecEffectTimeKey;

//------------------------------------------------------------------------------
//	�����^�C�v
//------------------------------------------------------------------------------
enum EFFECT_DIR_TYPE
{
	EFFECT_DIR_ROT,					//	��]�p�����[�^�ɏ]��
	EFFECT_DIR_FIX,					//	���[���h���W�ŌŒ�
	EFFECT_DIR_FOLLOW,				//	�ړ������֌�����
	EFFECT_DIR_CENTER,				//	�����֌�����

	EFFECT_DIR_TYPE_NUM,
};

//------------------------------------------------------------------------------
//	�p�[�e�B�N���p�����[�^
//------------------------------------------------------------------------------
struct EFFECT_PARTICLE
{
	EFFECT_PARTICLE( void );
	~EFFECT_PARTICLE();

	uint						EndFrame;	//	�I���t���[��
	bool						bLoop;		//	���[�v

	EFFECT_PARTICLE_TYPE		Type;		//	���
	Color						Col;		//	�F
	BLEND_MODE					Blend;		//	�u�����h
	int							Phase;		//	�`��t�F�[�Y

	union
	{
		EFFECT_PARTICLE_SPRITE_PARAM*		pSprite;		//	�X�v���C�g
		EFFECT_PARTICLE_SPRITE_FRAME_PARAM*	pSpriteFrame;	//	�X�v���C�g�t���[��
		EFFECT_PARTICLE_LINE_PARAM*			pLine;			//	���C��
		EFFECT_PARTICLE_FONT_PARAM*			pFont;			//	�t�H���g
		EFFECT_PARTICLE_SOUND_PARAM*		pSound;			//	�T�E���h
		EFFECT_PARTICLE_EFFECT_PARAM*		pEffect;		//	�G�t�F�N�g
	};

	EFFECT_MOVE_VELOCITY_PARAM*				pVelocity;		//	�ړ��p�����[�^

	VecEffectTimeKey						vTimeKey[EFFECT_TIME_KEY_NUM];		//	�^�C���L�[
};

//------------------------------------------------------------------------------
//	�G�t�F�N�g�ړ��̎��
//------------------------------------------------------------------------------
enum EFFECT_MOVE_TYPE
{
	EFFECT_MOVE_DEFAULT,				//	�Œ�
	EFFECT_MOVE_VELOCITY,				//	�ړ��^
	EFFECT_MOVE_FOLLOW,					//	�Ǐ]�^
	EFFECT_MOVE_CIRCULAR,				//	��]�^
};

//------------------------------------------------------------------------------
//	�����p�����[�^
//------------------------------------------------------------------------------
struct EFFECT_APPEAR
{
	uint				StartFrame;		//	�����J�n�t���[��
	uint				EndFrame;		//	�����I���t���[��
	int					Interval;		//	�C���^�[�o��
	uint				AppearNum;		//	����������

	Label				Name;			//	�������̌ŗL��
	Label				ParticleName;	//	�p�[�e�B�N���̖��O
	EFFECT_MOVE_TYPE	MoveType;		//	�ړ��^�C�v
	EFFECT_DIR_TYPE		DirType;		//	�����^�C�v

	union
	{
		//	�ړ��^
		struct VELOCITY
		{
			float		X, Y;			//	�ʒu
			float		XRange, YRange;	//	�ʒu�͈̔�
			float		Dir;			//	�����p�x
			float		DirRange;		//	�����p�x�̕�
			float		Dist;			//	��������
			float		DistRange;		//	���������̕�
			float		Speed;			//	��������
			float		SpeedRange;		//	���������̕�
			float		MoveRandomX;	//	�ʒu�̃����_�����ZX
			float		MoveRandomY;	//	�ʒu�̃����_�����ZY
			float		Rot;			//	��]
			float		RotRange;		//	��]�̕�
			float		RotSpeed;		//	��]���x
			float		RotSpeedRange;	//	��]���x�̕�
			float		RotRandom;		//	��]�̃����_�����Z
		}
		Velocity;

		//	�Ǐ]�^
		struct FOLLOW
		{
			float		X, Y;			//	�ʒu
			float		XRange, YRange;	//	�ʒu�͈̔�
			float		MoveRandomX;	//	�ʒu�̃����_�����ZX
			float		MoveRandomY;	//	�ʒu�̃����_�����ZY
			float		Rot;			//	��]
			float		RotRange;		//	��]�̕�
			float		RotSpeed;		//	��]���x
			float		RotSpeedRange;	//	��]���x�̕�
		}
		Follow;

		//	��]�^
		struct CIRCULAR
		{
			float		Dist;				//	����
			float		DistRange;			//	�����̕�
			float		Rot;				//	��]
			float		RotRange;			//	��]�̕�
			float		RotSpeed;			//	��]���x
			float		RotSpeedRange;		//	��]���x�̕�
			float		TrackAngle;			//	�~����p�x
			float		TrackAngleRange;	//	�~����p�x�̕�
			float		AngleSpeed;			//	�~����p���x
			float		AngleSpeedRange;	//	�~����p���x�̕�
			float		AxisScale[2];		//	���X�P�[��
			float		AxisScaleRange[2];	//	���X�P�[���̕�
			float		AxisAngle;			//	���p�x
			float		AxisAngleRange;		//	���p�x�̕�
		}
		Circular;
	};
};