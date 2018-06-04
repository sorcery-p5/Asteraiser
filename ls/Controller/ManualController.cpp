#include "stdafx.h"
#include "ManualController.h"

#include "App/App.h"
#include "Config/Config.h"

#include "Sign/Keyboard.h"
#include "Sign/Pad.h"

#include "Replay/ReplayManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�蓮����R���g���[��
//
////////////////////////////////////////////////////////////////////////////////
ManualController::ManualController( ReplayManager* pReplayManager )
:Controller()
{
	_ASSERT( pReplayManager );

	m_pReplayManager = pReplayManager;
}
ManualController::~ManualController()
{
	
}

//******************************************************************************
//	�X�V
//******************************************************************************
void ManualController::Update( uint FrameCount )
{
	const CONFIG_INPUT& InputConfig = GetConfig()->GetInputInfo( 0 );
	Keyboard*	pKeyboard	= GetSign()->GetKeyboard();
	Pad*		pPad		= GetSign()->GetPad( 0 );

	byte Info = 0;

	//	�����L�[�F�L�[�{�[�h
	for( int Button = CONTROLLER_DIR_BEGIN; Button <= CONTROLLER_DIR_END; Button++ )
	{
		byte Mask = (1 << Button);
		int Index = Button - CONTROLLER_DIR_BEGIN;

		byte KeyCode = InputConfig.aKeyDir[Index];
		if( pKeyboard->IsKeyInput( KeyCode, INPUT_PRESS ) ) Info |= Mask;
	}
	
	//	�����L�[�F�p�b�h
	if( pPad )
	{			
		float X = pPad->GetAxis( InputConfig.PadAxisX );
		float Y = pPad->GetAxis( InputConfig.PadAxisY );

		if( X < 0.0f ) Info |= InputConfig.bPadAxisXRev? (1 << CONTROLLER_R) : (1 << CONTROLLER_L);
		if( X > 0.0f ) Info |= InputConfig.bPadAxisXRev? (1 << CONTROLLER_L) : (1 << CONTROLLER_R);
		if( Y < 0.0f ) Info |= InputConfig.bPadAxisYRev? (1 << CONTROLLER_D) : (1 << CONTROLLER_U);
		if( Y > 0.0f ) Info |= InputConfig.bPadAxisYRev? (1 << CONTROLLER_U) : (1 << CONTROLLER_D);
	}

	//	�{�^��
	for( int Button = CONTROLLER_BUTTON_BEGIN; Button <= CONTROLLER_BUTTON_END; Button++ )
	{
		byte Mask = (1 << Button);
		int Index = Button - CONTROLLER_BUTTON_BEGIN;

		//	�L�[�{�[�h
		byte KeyCode = InputConfig.aKeyButton[Index];
		if( pKeyboard->IsKeyInput( KeyCode, INPUT_PRESS ) ) Info |= Mask;

		//	�p�b�h
		if( pPad )
		{
			uint ButtonIndex = InputConfig.aPadButton[Index];
			if( pPad->IsButtonInput( ButtonIndex ) ) Info |= Mask;
		}
	}
	
	//	���������̌��o
	for( uint i = 0; i < InputConfig.vKeyConcurrent.size(); i++ )
	{
		const CONCURRENT& Conc = InputConfig.vKeyConcurrent[i];
		
		if( pKeyboard->IsKeyInput( Conc.Button, INPUT_PRESS ) )
		{
			for( int j = 0; j < CONCURRENT_NUM; j++ )
			{
				if( Conc.Array[j] != CONTROLLER_NONE ) Info |= (1 << Conc.Array[j]);
			}
		}
	}
	for( uint i = 0; i < InputConfig.vPadConcurrent.size(); i++ )
	{
		if( pPad )
		{
			const CONCURRENT& Conc = InputConfig.vPadConcurrent[i];
			
			if( pPad->IsButtonInput( Conc.Button ) )
			{
				for( int j = 0; j < CONCURRENT_NUM; j++ )
				{
					if( Conc.Array[j] != CONTROLLER_NONE ) Info |= (1 << Conc.Array[j]);
				}
			}
		}
	}

	//	���v���C�ۑ�
	m_pReplayManager->RecordData( Info );

	//	�o�^
	_UpdateCurInfo( Info );
}