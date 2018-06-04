#pragma once


////////////////////////////////////////////////////////////////////////////////
//
//	�v���C���[�^��`
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	�萔
//------------------------------------------------------------------------------
const int OVERDRIVE_EFFECT_NUM			= 3;

//------------------------------------------------------------------------------
//	�u���[�h�F�G�l���M�[�p�����[�^
//------------------------------------------------------------------------------
struct BLADE_ENERGY_PARAM
{
	BLADE_ENERGY_PARAM( void );

	float			ExhaustEnergyRate;			//	�U���q�b�g���̃G�l���M�[���՗�
	float			ExhaustLimitRate;			//	�U���q�b�g���̑ϋv�l���՗�

	float			NormalRegen;				//	�G�l���M�[���R�񕜗�
	float			NormalLimitRegen;			//	�ϋv�l���R�񕜗�
	float			ItemRegen;					//	�A�C�e���擾�ɂ��G�l���M�[�񕜗�
	float			ItemLimitRegen;				//	�A�C�e���擾�ɂ��ϋv�l�񕜗�
	float			ReloadRegen;				//	�����[�h���̃G�l���M�[�E�ϋv�l�񕜗�
	float			CrashRegen;					//	�������̃G�l���M�[�E�ϋv�l�񕜗�
	float			CancelEnergyDecay;			//	�L�����Z���G�l���M�[�̌�����
	
	float			UseRegenEnergy;				//	�G�l���M�[�񕜎������
	float			UseLimitEnergy;				//	�ϋv�l�񕜎������
	float			UseReloadEnergy;			//	�����[�h��ԉ񕜎������
};

//------------------------------------------------------------------------------
//	�u���[�h�F�X�}�b�V���p�����[�^
//------------------------------------------------------------------------------
struct BLADE_SMASH_PARAM
{
	BLADE_SMASH_PARAM( void );

	Label				HitEffectName;				//	�q�b�g�G�t�F�N�g

	int					ChargeTime;					//	�^������
	int					ChargeStartTime;			//	�^���J�n����
	int					ChargeMaxTime;				//	�^���ő厞��
	int					RulerChargeTime;			//	���[���[�̃^������
	float				Length;						//	���̒���
	float				Radius;						//	���̑����i���a�j

	float				Attack;						//	�U����
	float				AttackVel;					//	�U����������
	int					Knockback;					//	�U�����m�b�N�o�b�N����

	float				ExhaustRate;				//	���Ք䗦
	float				ChargeExhaustEnergy;		//	�^�����̃G�l���M�[����
	float				ChargeExhaustLimit;			//	�^�����̑ϋv�l����
};

//------------------------------------------------------------------------------
//	�u���[�h�p�����[�^
//------------------------------------------------------------------------------
struct BLADE_PARAM
{
	BLADE_PARAM( void );

	Label				EffectName;					//	�G�t�F�N�g
	Label				HitEffectName;				//	�q�b�g�G�t�F�N�g
	Label				RepulseEffectName;			//	�����G�t�F�N�g
	Label				RepulseHitEffectName;		//	�����q�b�g�G�t�F�N�g
	Label				RepulseEndEffectName;		//	�����I���G�t�F�N�g
	Label				CrashEffectName;			//	�j���G�t�F�N�g
	Label				RulerEffectName;			//	���[���[���̃G�t�F�N�g
	Label				RulerHitEffectName;			//	���[���[���̃q�b�g�G�t�F�N�g

	float				Length;						//	���̒���
	float				Radius;						//	���̑����i���a�j
	int					NormalDelay;				//	�ʏ�U�����̃f�B���C
	int					HitDelay;					//	���𓖂Ă���̃f�B���C
	int					CancelTime;					//	�L�����Z����t����
	int					NotExhaustTime;				//	����Ȃ�����
	int					NotReloadTime;				//	�����[�h�ł��Ȃ�����

	float				Attack;						//	�U����
	float				RepulseAttack;				//	�������U����
	float				AttackEnergyRate;			//	�G�l���M�[�ɂ��U���͌�����
	float				AttackLimitRate;			//	�ϋv�ɂ��U���͌�����
	float				AttackVel;					//	�U����������
	int					Knockback;					//	�U�����m�b�N�o�b�N����
	int					RepulseInterval;			//	�����̔����Ԋu
	
	float				RulerAttack;				//	���[���[���̍U����
	int					RulerDelay;					//	���[���[���̃f�B���C
	float				RulerLength;				//	���[���[���̌�����
	float				RulerRadius;				//	���[���[���̌�����

	int					HitItemNum;					//	�������A�C�e��������
	int					RepulseItemNum;				//	�������A�C�e��������
	int					CrashItemNum;				//	�j�����A�C�e��������
	int					ReloadItemNum;				//	�����[�h���A�C�e��������
	float				ItemSpeed;					//	�A�C�e���������x

	BLADE_ENERGY_PARAM	EnergyParam;
	BLADE_SMASH_PARAM	SmashParam;	
};

//------------------------------------------------------------------------------
//	�X�s�A�p�����[�^
//------------------------------------------------------------------------------
struct SPEAR_PARAM
{
	SPEAR_PARAM( void );

	Label			EffectName;				//	�G�t�F�N�g
	Label			HitEffectName;			//	�q�b�g�G�t�F�N�g
	Label			CrashEffectName;		//	�j���G�t�F�N�g
	Label			LockEffectName;			//	���b�N�G�t�F�N�g

	float			Length;					//	����
	float			Radius;					//	�����i���a�j
	float			Speed;					//	�X�s�[�h
	int				Time;					//	���ʎ���
	float			AttackVel;				//	�U���̔���
	int				Knockback;				//	�m�b�N�o�b�N
	float			GroundExhaust;			//	�n�`�q�b�g�ɂ�����

	float			Energy;					//	�G�l���M�[
	float			EnergyMax;				//	�G�l���M�[�̍ő�l
	float			EnergyItemRate;			//	�A�C�e���擾�ɂ��G�l���M�[���Z��
	float			EnergyDecayRate;		//	�U���q�b�g�ɂ��G�l���M�[������

	float			Attack;					//	�U����
	float			AttackMax;				//	�U���͂̍ő�l
	float			AttackItemRate;			//	�A�C�e���擾�ɂ��U���͉��Z��
	float			AttackDecayRate;		//	�U���q�b�g�ɂ��U���͌�����
	
	int				HitItemNum;				//	�������A�C�e��������
	int				CrashItemNum;			//	�j�����A�C�e��������
	float			ItemSpeed;				//	�A�C�e���������x
};

//------------------------------------------------------------------------------
//	���[���[�p�����[�^
//------------------------------------------------------------------------------
struct RULER_PARAM
{
	RULER_PARAM( void );

	Label			EffectName;				//	�G�t�F�N�g
	Label			CrashEffectName;		//	�j���G�t�F�N�g
	Label			PlayerEffectName;		//	���@�ɕt���G�t�F�N�g

	float			Radius;					//	���a
	float			RadiusMax;				//	���a�̍ő�l
	float			RadiusItemRate;			//	�A�C�e���擾�ɂ�锼�a�̉��Z��
	int				Time;					//	���ʎ���
	int				TimeMax;				//	���ʎ��Ԃ̍ő�l
	float			TimeItemRate;			//	�A�C�e���擾�ɂ����ʎ��Ԃ̉��Z��
	
	int				CrashItemNum;			//	�j�����A�C�e��������
	float			ItemSpeed;				//	�A�C�e���������x
};

//------------------------------------------------------------------------------
//	�I�[���@�h���C���p�����[�^
//------------------------------------------------------------------------------
struct OVERDRIVE_PARAM
{
	OVERDRIVE_PARAM( void );

	float			NormalRegen;						//	�ʏ펞�G�l���M�[�񕜗�
	float			ItemRegen;							//	�A�C�e���擾�ɂ��G�l���M�[�񕜗�
	float			UseRegenEnergy;						//	�G�l���M�[�񕜎������

	Label			EffectName[OVERDRIVE_EFFECT_NUM];	//	�G�t�F�N�g�̖��O
	Label			ChargeEffectName;					//	�`���[�W�G�t�F�N�g�̖��O
	Label			MaxEffectName;						//	�ő�`���[�W�G�t�F�N�g�̖��O

	int				Delay;								//	�g�p��̃f�B���C

	SPEAR_PARAM		Spear;
	RULER_PARAM		Ruler;
};

//------------------------------------------------------------------------------
//	�_�b�V���F���������p�����[�^
//------------------------------------------------------------------------------
struct DASH_SPLASH_PARAM
{
	DASH_SPLASH_PARAM( void );

	Label			EffectName;					//	�G�t�F�N�g
	Label			HitEffectName;				//	�q�b�g�G�t�F�N�g

	float			CollideRadius;				//	���背�C�̑���
	float			RulerRadius;				//	���[���[�����背�C�̑���
	float			Attack;						//	�U����
	float			AttackVel;					//	�U����������
	int				Knockback;					//	�U�����m�b�N�o�b�N����
	float			ExhaustRate;				//	���Ք䗦
	int				HitInvincible;				//	�q�b�g���̖��G����
};

//------------------------------------------------------------------------------
//	�_�b�V���p�����[�^
//------------------------------------------------------------------------------
struct DASH_PARAM
{
	DASH_PARAM( void );

	Label				EffectName;					//	�G�t�F�N�g

	float				MoveLength;					//	�ړ�����
	int					Delay;						//	�f�B���C
	int					RulerDelay;					//	���[���[���̃f�B���C

	DASH_SPLASH_PARAM	Splash;
};

//------------------------------------------------------------------------------
//	�V�[���h�p�����[�^
//------------------------------------------------------------------------------
struct SHIELD_PARAM
{
	SHIELD_PARAM( void );
	
	Label			HitEffectName;				//	�q�b�g�G�t�F�N�g
	Label			BreakEffectName;			//	�j���G�t�F�N�g
	Label			RecoveryEffectName;			//	�񕜃G�t�F�N�g

	int				DamageInvincible;			//	��e�����G����
	int				BreakInvincible;			//	�u���C�N�����G����
	int				ReviveInvincible;			//	���������G����
	int				DamageNoMove;				//	��e���ړ��s����
	int				BreakTime;					//	�u���C�N��Ԏ���
	int				SerialDamageTime;			//	�A����e���莞��

	float			NormalRegen;				//	�ʏ펞�V�[���h�񕜗�
	float			ItemRegen;					//	�A�C�e���擾�ɂ��V�[���h�񕜗�
	float			NormalRecovery;				//	�ʏ펞�u���C�N���񕜗�
	float			ItemRecovery;				//	�A�C�e���擾�ɂ��u���C�N���񕜗�
	int				BreakNumMax;				//	�u���C�N�񐔂̍ő�l
	float			BreakNumDecayRate;			//	�u���C�N�񐔂ɂ��u���C�N���񕜗�
	float			UseRegenEnergy;				//	�V�[���h�񕜎������
	
	int				BreakItemNum;				//	�j�����A�C�e��������
	float			ItemSpeed;					//	�A�C�e���������x
};

//------------------------------------------------------------------------------
//	�v���C���[�p�����[�^
//------------------------------------------------------------------------------
struct PLAYER_PARAM
{
	PLAYER_PARAM( void );

	Label			SpriteName;					//	�X�v���C�g��

	float			MoveSpeed;					//	�ړ����x
	float			SlowSpeed;					//	�ᑬ��Ԃ̈ړ����x
	float			Decel;						//	�����W��
	float			CrashVel;					//	�u���[�h�������̔�����
	float			BreakVel;					//	�u���C�N���̔�����
	int				KnockbackTime;				//	�m�b�N�o�b�N����
	int				ReversalTime;				//	���o�[�T���\����
	int				RepulseInvincible;			//	������̖��G����

	float			RulerMoveSpeed;				//	���[���[���̈ړ����x

	float			HitRadius;					//	�����蔼�a
	float			ItemCollectRadius;			//	�A�C�e��������a

	float			RegenEnergy;				//	���R�񕜃G�l���M�[
	float			ItemEnergy;					//	�A�C�e���̃G�l���M�[

	BLADE_PARAM		Blade;
	OVERDRIVE_PARAM	Overdrive;
	DASH_PARAM		Dash;
	SHIELD_PARAM	Shield;
};