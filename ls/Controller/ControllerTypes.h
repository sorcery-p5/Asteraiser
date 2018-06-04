#pragma once


////////////////////////////////////////////////////////////////////////////////
//
//	�R���g���[���[�^��`
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	�R���g���[���F���͎��
//------------------------------------------------------------------------------
enum CONTROLLER_INPUT
{
	CONTROLLER_BLADE,		//	�u���[�h
	CONTROLLER_RELOAD,		//	�����[�h
	CONTROLLER_DASH,		//	�_�b�V��

	CONTROLLER_L,			//	��
	CONTROLLER_U,			//	��
	CONTROLLER_R,			//	�E
	CONTROLLER_D,			//	��

	CONTROLLER_BUTTON_BEGIN	= CONTROLLER_BLADE,
	CONTROLLER_BUTTON_END	= CONTROLLER_DASH,
	CONTROLLER_DIR_BEGIN	= CONTROLLER_L,
	CONTROLLER_DIR_END		= CONTROLLER_D,

	CONTROLLER_NONE			= UCHAR_MAX,

	CONTROLLER_BUTTON_NUM	= CONTROLLER_BUTTON_END - CONTROLLER_BUTTON_BEGIN + 1,
	CONTROLLER_DIR_NUM		= CONTROLLER_DIR_END	- CONTROLLER_DIR_BEGIN + 1,
};

//------------------------------------------------------------------------------
//	�R���g���[���F�V�X�e���L�[���
//------------------------------------------------------------------------------
enum CONTROLLER_SYSTEM_KEY
{
	CONTROLLER_SYSTEM_OK,		//	����
	CONTROLLER_SYSTEM_CANCEL,	//	�L�����Z��
	CONTROLLER_SYSTEM_PAUSE,	//	�|�[�Y
	CONTROLLER_SYSTEM_EXIT,		//	�I��

	CONTROLLER_SYSTEM_KEY_NUM,

	CONTROLLER_SYSTEM_NONE	= UCHAR_MAX,
};