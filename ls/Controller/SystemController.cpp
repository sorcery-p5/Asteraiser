#include "stdafx.h"
#include "SystemController.h"
#include "Controller.h"

#include "Config/Config.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�V�X�e���p�R���g���[��
//
////////////////////////////////////////////////////////////////////////////////
SystemController::SystemController( void )
{
}
SystemController::~SystemController()
{
}

//******************************************************************************
//	�L�[���͂̎擾
//******************************************************************************
bool SystemController::IsInput( CONTROLLER_SYSTEM_KEY Input, INPUT_TYPE Type ) const
{
	for( int PlayerID = 0; PlayerID < GetConfig()->GetInputInfoNum(); PlayerID++ )
	{
		const CONFIG_INPUT& ConfigInput = GetConfig()->GetInputInfo(PlayerID);

		//	�L�[�{�[�h
		Keyboard* pKeyboard	= GetSign()->GetKeyboard();
		if( pKeyboard->IsKeyInput( ConfigInput.aKeySystem[Input], Type ) ) return true;
		
		//	�p�b�h
		Pad* pPad = GetSign()->GetPad( PlayerID );
		if( pPad )
		{
			if( pPad->IsButtonInput( ConfigInput.aPadSystem[Input], Type ) ) return true;
		}
	}

	return false;
}

//******************************************************************************
//	�ǂꂩ�̃L�[�����͂���Ă��邩
//******************************************************************************
bool SystemController::IsAnyInput( INPUT_TYPE Type ) const
{
	for( int i = 0; i < CONTROLLER_SYSTEM_KEY_NUM; i++ )
	{
		if( IsInput( CONTROLLER_SYSTEM_KEY(i), Type ) )
		{
			return true;
		}
	}
	return false;
}

//******************************************************************************
//	�����L�[�����͂���Ă��邩
//******************************************************************************
bool SystemController::IsDirInput( CONTROLLER_INPUT Input, INPUT_TYPE Type ) const
{
	byte Info = GetDirBit(Type);

	return (Info & (1 << Input) ) != 0;
}

//******************************************************************************
//	�����L�[�̎擾
//******************************************************************************
Point SystemController::GetDir( INPUT_TYPE Type ) const
{
	return Controller::InfoToDir( GetDirBit(Type) );
}

//******************************************************************************
//	�����L�[�̃r�b�g�擾
//******************************************************************************
byte SystemController::GetDirBit( INPUT_TYPE Type ) const
{
	byte Info = 0;

	for( int PlayerID = 0; PlayerID < GetConfig()->GetInputInfoNum(); PlayerID++ )
	{
		const CONFIG_INPUT& ConfigInput = GetConfig()->GetInputInfo(PlayerID);

		Pad*		pPad		= GetSign()->GetPad( PlayerID );
		Keyboard*	pKeyboard	= GetSign()->GetKeyboard();

		//	�����L�[�F�L�[�{�[�h
		for( int Button = CONTROLLER_DIR_BEGIN; Button <= CONTROLLER_DIR_END; Button++ )
		{
			byte Mask = (1 << Button);
			int Index = Button - CONTROLLER_DIR_BEGIN;

			if( pKeyboard->IsKeyInput( ConfigInput.aKeyDir[Index], Type ) ) Info |= Mask;
		}
		
		//	�����L�[�F�p�b�h
		if( pPad )
		{			
			float X = pPad->GetAxis( ConfigInput.PadAxisX, Type );
			float Y = pPad->GetAxis( ConfigInput.PadAxisY, Type );

			if( X < 0.0f ) Info |= ConfigInput.bPadAxisXRev? (1 << CONTROLLER_R) : (1 << CONTROLLER_L);
			if( X > 0.0f ) Info |= ConfigInput.bPadAxisXRev? (1 << CONTROLLER_L) : (1 << CONTROLLER_R);
			if( Y < 0.0f ) Info |= ConfigInput.bPadAxisYRev? (1 << CONTROLLER_D) : (1 << CONTROLLER_U);
			if( Y > 0.0f ) Info |= ConfigInput.bPadAxisYRev? (1 << CONTROLLER_U) : (1 << CONTROLLER_D);
		}
	}

	return Info;
}