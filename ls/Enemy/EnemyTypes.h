#pragma once


////////////////////////////////////////////////////////////////////////////////
//
//	�G�^��`
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	�G�p�����[�^
//------------------------------------------------------------------------------
struct ENEMY_PARAM
{
	ENEMY_PARAM( void );

	Label			SpriteName;
	std::string		PreviewName;
	int				Life;
	float			Decel;
	int				DamageInvincible;
	float			Defense;
	float			ExhaustRate;
	float			ReduceVel;
	float			ReduceKnockback;
	float			BodyDamage;
	float			BodyDamageVel;
	int				ItemNum;
	float			ItemSpeed;
	float			ScoreRate;
	int				CrashKnockback;
	float			CrashVel;
	float			CollideRadius;
	bool			bCollideFrame;
	bool			bHitEnable;
	bool			bGroundHit;
	bool			bPlayerHit;
	bool			bRepulse;
	bool			bNotRepulseNotify;
	bool			bNoCount;
	bool			bBoss;
	bool			bOutField;
	bool			bNoIndicate;
	Color			DeadColor;
};

//------------------------------------------------------------------------------
//	�G�F�ڕW�^�C�v
//------------------------------------------------------------------------------
enum ENEMY_TARGET_TYPE
{
	ENEMY_TARGET_FIX,			//	�Œ���W
	ENEMY_TARGET_PLAYER,		//	���@
	ENEMY_TARGET_SELF,			//	����
	ENEMY_TARGET_ENEMY,			//	����̓G

	ENEMY_TARGET_NONE	= -1,	//	����
};

//------------------------------------------------------------------------------
//	�G�F�ڕW�ʒu�^�C�v
//------------------------------------------------------------------------------
enum ENEMY_TARGET_POS_TYPE
{
	ENEMY_TARGET_POS_NONE,		//	�Ȃ�
	ENEMY_TARGET_POS_RECT,		//	����
	ENEMY_TARGET_POS_POLER,		//	��
};

//------------------------------------------------------------------------------
//	�G�F�ڕW���^�C�v
//------------------------------------------------------------------------------
enum ENEMY_TARGET_AXIS_TYPE
{
	ENEMY_TARGET_AXIS_ABS,		//	���
	ENEMY_TARGET_AXIS_REL,		//	����
};


//------------------------------------------------------------------------------
//	�G�F�ڕW�p�����[�^
//------------------------------------------------------------------------------
struct ENEMY_TARGET
{
	ENEMY_TARGET_TYPE		Type;
	ENEMY_TARGET_POS_TYPE	PosType;
	ENEMY_TARGET_AXIS_TYPE	AxisType;
	union
	{
		struct
		{
			float			X, Y;
		};
		
		struct
		{
			float			Ang, Len;
		};
		float				Pos[2];
	};
	float					RandRange[2];
	LabelStr				Name;
	int						Interval;
	bool					bOutField;
};

//------------------------------------------------------------------------------
//	�G�F�ړ��^�C�v
//------------------------------------------------------------------------------
enum ENEMY_MOVE_TYPE
{
	ENEMY_MOVE_TARGET,				//	�ڕW
	ENEMY_MOVE_TARGET_X,			//	�ڕW�F���݈̂ړ�
	ENEMY_MOVE_TARGET_Y,			//	�ڕW�F�c�݈̂ړ�

	ENEMY_MOVE_NONE			= -1,	//	����
};

//------------------------------------------------------------------------------
//	�G�F�ړ��A�N�V�����p�����[�^
//------------------------------------------------------------------------------
struct ENEMY_MOVE_ACTION
{
	ENEMY_MOVE_TYPE		Type;
	ENEMY_TARGET		Target;
	float				Accel;
	float				Decel;
	float				MaxSpeed;
};

//------------------------------------------------------------------------------
//	�G�F��]�^�C�v
//------------------------------------------------------------------------------
enum ENEMY_ROT_TYPE
{
	ENEMY_ROT_FIX,				//	�Œ�
	ENEMY_ROT_TARGET,			//	�^�[�Q�b�g
	ENEMY_ROT_MOVE,				//	�ړ��ڕW
	
	ENEMY_ROT_NONE		= -1,	//	����
};

//------------------------------------------------------------------------------
//	�G�F��]�A�N�V�����p�����[�^
//------------------------------------------------------------------------------
struct ENEMY_ROT_ACTION
{
	ENEMY_ROT_TYPE		Type;
	float				Speed;

	union
	{
		ENEMY_TARGET		Target;
		float				Fix;
	};
};

//------------------------------------------------------------------------------
//	�G�F�U�������ʒu�^�C�v
//------------------------------------------------------------------------------
enum ENEMY_ATTACK_POS_TYPE
{
	ENEMY_ATTACK_POS_CENTER,			//	���S
	ENEMY_ATTACK_POS_BONE,				//	�{�[�����甭��
	ENEMY_ATTACK_POS_TARGET,			//	�ڕW�ʒu
	
	ENEMY_ATTACK_POS_NONE		= -1,	//	����
};

//------------------------------------------------------------------------------
//	�G�F�U�������^�C�v
//------------------------------------------------------------------------------
enum ENEMY_ATTACK_DIR_TYPE
{
	ENEMY_ATTACK_DIR_FIX,			//	�Œ�
	ENEMY_ATTACK_DIR_STRAIGHT,		//	���i
	ENEMY_ATTACK_DIR_TARGET,		//	�ڕW�w��
	ENEMY_ATTACK_DIR_MOVE,			//	�ړ��ڕW
	ENEMY_ATTACK_DIR_ROT,			//	����ڕW
	
	ENEMY_ATTACK_DIR_NONE	= -1,	//	����
};

//------------------------------------------------------------------------------
//	�G�F�U���ʒu
//------------------------------------------------------------------------------
struct ENEMY_ATTACK_POS 
{
	ENEMY_ATTACK_POS_TYPE		Type;
	union
	{
		ENEMY_TARGET			Target;
		LabelStr				Bone;
	};
};

//------------------------------------------------------------------------------
//	�G�F�U������
//------------------------------------------------------------------------------
struct ENEMY_ATTACK_DIR
{
	ENEMY_ATTACK_DIR_TYPE		Type;
	union
	{
		ENEMY_TARGET			Target;
		float					Fix;
	};
};

//------------------------------------------------------------------------------
//	�G�F�U���A�N�V�����p�����[�^
//------------------------------------------------------------------------------
struct ENEMY_ATTACK_ACTION
{
	int							Interval;
	int							AppearNum;

	ENEMY_ATTACK_POS			Pos;
	ENEMY_ATTACK_DIR			Dir;

	LabelStr					Name;
};

//------------------------------------------------------------------------------
//	�G�F�����A�N�V�����p�����[�^
//------------------------------------------------------------------------------
struct ENEMY_SUMMON_ACTION
{
	LabelStr					Name;
	LabelStr					AppearName;
	LabelStr					BoneName;
	float						X, Y;
	float						Rot;
	bool						bAttach;
};

//------------------------------------------------------------------------------
//	�G�F���[�v�A�N�V�����p�����[�^
//------------------------------------------------------------------------------
struct ENEMY_WARP_ACTION
{
	ENEMY_TARGET		Target;
};

//------------------------------------------------------------------------------
//	�G�F�������
//------------------------------------------------------------------------------
enum ENEMY_CONDITION_TYPE
{
	ENEMY_CONDITION_MOVE_END,			//	�ړ�����
	ENEMY_CONDITION_ROT_END,			//	���񊮗�
	ENEMY_CONDITION_ATTACK_END,			//	�U������
	ENEMY_CONDITION_ANIME_END,			//	�A�j���I��
	ENEMY_CONDITION_ANIME_NOTIFY,		//	�A�j���ʒm
	ENEMY_CONDITION_NOTIFY,				//	�ʒm

	ENEMY_CONDITION_CHILD_DEFEAT,		//	�q�����S
	ENEMY_CONDITION_CHILD_ALL_DEFEAT,	//	�q���S��
	ENEMY_CONDITION_CHILD_EMPTY,		//	�q�����Ȃ�

	ENEMY_CONDITION_SELF_POS_X,			//	���g�̈ʒu(X��)
	ENEMY_CONDITION_SELF_POS_Y,			//	���g�̈ʒu(Y��)

	ENEMY_CONDITION_PLAYER_RANGE,		//	���@�ւ̋���
	ENEMY_CONDITION_PLAYER_RANGE_X,		//	���@�ւ̋���(X��)
	ENEMY_CONDITION_PLAYER_RANGE_Y,		//	���@�ւ̋���(Y��)
	ENEMY_CONDITION_PLAYER_ANGLE,		//	���@�ւ̊p�x
	ENEMY_CONDITION_PLAYER_ANGLE_REL,	//	���@�ւ̊p�x(����)
	ENEMY_CONDITION_PLAYER_POS_X,		//	���@�̈ʒu(X��)
	ENEMY_CONDITION_PLAYER_POS_Y,		//	���@�̈ʒu(Y��)
	ENEMY_CONDITION_PLAYER_STATE,		//	���@�̏��
	ENEMY_CONDITION_PLAYER_STATE_TIME,	//	���@�̏�Ԏ���

	ENEMY_CONDITION_FRAME,				//	�t���[���o��
	ENEMY_CONDITION_RANDOM,				//	�����_��
	ENEMY_CONDITION_STATE_END,			//	�X�e�[�g�I��
	ENEMY_CONDITION_LIFE_LESS,			//	���C�t����

	ENEMY_CONDITION_DAMAGE,				//	�_���[�W
	ENEMY_CONDITION_KNOCKBACK,			//	�m�b�N�o�b�N
	ENEMY_CONDITION_REPULSE,			//	����
	ENEMY_CONDITION_DEAD,				//	���S

	ENEMY_CONDITION_NONE		= -1,	//	����
};

//------------------------------------------------------------------------------
//	�G�F����
//------------------------------------------------------------------------------
struct ENEMY_CONDITION
{
	ENEMY_CONDITION_TYPE	Type;
	bool					bNot;

	union
	{
		float				Param;
		float				Range[2];
		LabelStr			Name;
		bool				bFlag;
	};
};
typedef std::vector< ENEMY_CONDITION >	ENEMY_CONDITION_LIST;

//------------------------------------------------------------------------------
//	�G�F�A�N�V�����^�C�v
//------------------------------------------------------------------------------
enum ENEMY_ACTION_TYPE
{
	ENEMY_ACTION_MOVE,					//	�ړ�
	ENEMY_ACTION_ROT,					//	����
	ENEMY_ACTION_ATTACK,				//	�U��
	ENEMY_ACTION_SUMMON,				//	����
	ENEMY_ACTION_WARP,					//	���[�v

	ENEMY_ACTION_END_MOVE,				//	�ړ��L�����Z��
	ENEMY_ACTION_END_ROT,				//	�����~
	ENEMY_ACTION_END_ATTACK,			//	�U����~
	ENEMY_ACTION_END_ACTION,			//	�S�A�N�V������~

	ENEMY_ACTION_CHANGE_ANIME,			//	�A�j���ύX
	ENEMY_ACTION_PAUSE_ANIME,			//	�A�j���̒�~
	ENEMY_ACTION_CHANGE_STATE,			//	�X�e�[�g�ύX
	ENEMY_ACTION_EFFECT,				//	�G�t�F�N�g
	ENEMY_ACTION_SOUND,					//	�T�E���h
	ENEMY_ACTION_COLOR,					//	�F�ύX

	ENEMY_ACTION_ADD_LIFE,				//	�ϋv�͉��Z
	ENEMY_ACTION_ADD_DEFENSE,			//	�h��͉��Z

	ENEMY_ACTION_INVINCIBLE,			//	���G
	ENEMY_ACTION_HIDE,					//	�n�C�h���
	ENEMY_ACTION_NO_DAMAGE,				//	�_���[�W����
	ENEMY_ACTION_HIT,					//	�����蔻��
	ENEMY_ACTION_GROUND_HIT,			//	�n�`�Ƃ̓����蔻��
	ENEMY_ACTION_OUT_FIELD,				//	��ʊO�ւ̈ړ���
	ENEMY_ACTION_NO_COUNT,				//	�J�E���g��
	ENEMY_ACTION_REPULSE,				//	�����̗L��
	
	ENEMY_ACTION_PARENT_NOTIFY,			//	�e�֒ʒm
	ENEMY_ACTION_CHILD_NOTIFY,			//	�q�֒ʒm

	ENEMY_ACTION_STAGE_NOTIFY,			//	�X�e�[�W�֒ʒm
	ENEMY_ACTION_LOCK_SCORE,			//	�X�R�A���b�N

	ENEMY_ACTION_DESTROY,				//	�j��
	ENEMY_ACTION_ESCAPE,				//	����
	
	ENEMY_ACTION_NONE			= -1,	//	����
};

//------------------------------------------------------------------------------
//	�G�F�A�N�V�����p�����[�^
//------------------------------------------------------------------------------
struct ENEMY_ACTION
{
	ENEMY_ACTION_TYPE		Type;
	ENEMY_CONDITION_LIST	ConditionList;
	int						Time;
	bool					bContinue;
	bool					bOnce;

	union
	{
		ENEMY_MOVE_ACTION	Move;
		ENEMY_ROT_ACTION	Rot;
		ENEMY_ATTACK_ACTION	Attack;
		ENEMY_SUMMON_ACTION	Summon;
		ENEMY_WARP_ACTION	Warp;
	};

	LabelStr				Name;
	LabelStr				ParamStr;
	float					Param;
	bool					bFlag;
	Color					Col;
};

//------------------------------------------------------------------------------
//	�U������̒ʒm
//------------------------------------------------------------------------------
const Label		ENEMY_ATTACK_NOTIFY_CRASH		= Label( "Crash" );		//	�u���[�h�N���b�V��
const Label		ENEMY_ATTACK_NOTIFY_BREAK		= Label( "Break" );		//	�V�[���h�u���C�N
const Label		ENEMY_ATTACK_NOTIFY_REPULSE		= Label( "Repulse" );	//	����

//------------------------------------------------------------------------------
//	�G�F�n���h���X�e�[�g���
//------------------------------------------------------------------------------
const Label		ENEMY_HANDLER_STATE_ROOT		= Label( "Root" );
const Label		ENEMY_HANDLER_STATE_RESIDENT	= Label( "Resident" );
const Label		ENEMY_HANDLER_STATE_DAMAGE		= Label( "Damage" );
const Label		ENEMY_HANDLER_STATE_DEAD		= Label( "Dead" );
const Label		ENEMY_HANDLER_STATE_REPULSE		= Label( "Repulse" );
const Label		ENEMY_HANDLER_STATE_CRASH		= Label( "Crash" );
const Label		ENEMY_HANDLER_STATE_BREAK		= Label( "Break" );