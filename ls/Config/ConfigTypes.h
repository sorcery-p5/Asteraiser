#pragma once

#include "Controller/ControllerTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�R���t�B�O�F�^��`
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	�萔
//------------------------------------------------------------------------------
enum
{
	//	����
	INPUT_BUTTON_NUM		= CONTROLLER_BUTTON_NUM,		//	�{�^���̐�
	INPUT_SYSTEM_KEY_NUM	= CONTROLLER_SYSTEM_KEY_NUM,	//	�V�X�e���L�[�̐�
	INPUT_AXIS_NUM			= CONTROLLER_DIR_NUM,			//	�����̐�
	CONCURRENT_NUM			= 4,							//	���������ő���͐�
};

//------------------------------------------------------------------------------
//	�V�X�e���ݒ�
//------------------------------------------------------------------------------
struct CONFIG_SYSTEM
{
	CONFIG_SYSTEM( void )					{ Default(); }
	void Default( void );

	float			BGMVolume;
	float			SoundVolume;

	bool			bFullScreen;
};

//------------------------------------------------------------------------------
//	���������ݒ�
//------------------------------------------------------------------------------
struct CONCURRENT
{
	CONCURRENT( byte b, const byte* pArray, uint Size );

	byte		Button;
	byte		Array[CONCURRENT_NUM];
};

//------------------------------------------------------------------------------
//	���͐ݒ�
//------------------------------------------------------------------------------
struct CONFIG_INPUT
{
	CONFIG_INPUT( void )					{ Default(0); }
	CONFIG_INPUT( int PlayerID )			{ Default(PlayerID); }
	void Default( int PlayerID );

	typedef	std::vector<CONCURRENT>			ConcurrentVec;

	//	�L�[�{�[�h
	byte			aKeyButton[INPUT_BUTTON_NUM];
	byte			aKeyDir[INPUT_AXIS_NUM];
	ConcurrentVec	vKeyConcurrent;
	byte			aKeySystem[INPUT_SYSTEM_KEY_NUM];

	//	�p�b�h
	byte			aPadButton[INPUT_BUTTON_NUM];
	ConcurrentVec	vPadConcurrent;
	byte			aPadSystem[INPUT_SYSTEM_KEY_NUM];

	PAD_AXIS		PadAxisX;
	bool			bPadAxisXRev;
	PAD_AXIS		PadAxisY;
	bool			bPadAxisYRev;
	uint			PadDeadzone;
};

//------------------------------------------------------------------------------
//	�f�o�b�O���
//------------------------------------------------------------------------------
struct CONFIG_DEBUG
{
	CONFIG_DEBUG( void );

	bool			bDrawDummyBone;
	bool			bDrawCollideShape;
	bool			bOverdriveInfinite;
	bool			bShieldInfinite;
	bool			bBladeInfinite;
	bool			bPlayerInvincible;
	bool			bPlayerNodamage;
	bool			bPlayerHyperAttack;
	bool			bEnemyHyperAttack;
};