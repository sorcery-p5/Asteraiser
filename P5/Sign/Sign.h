#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include	<Dinput.h>


namespace P5
{

//-----------------------------------------------------------------------------
//	�^��`
//-----------------------------------------------------------------------------
class Keyboard;
class Mouse;
class PadManager;
class Pad;


//-----------------------------------------------------------------------------
//	���̓^�C�v
//-----------------------------------------------------------------------------
enum INPUT_TYPE
{
	INPUT_PRESS,		//	���݉�����Ă���
	INPUT_TRIG,			//	�����ꂽ�u��
	INPUT_OFF,			//	�����ꂽ�u��
	INPUT_REPEAT,		//	���s�[�g

	INPUT_TYPE_NUM,
};

///////////////////////////////////////////////////////////////////////////////
//
//	���͊Ǘ�
//
///////////////////////////////////////////////////////////////////////////////
class Sign
{
public:
	Sign( void );
	~Sign();

	bool			Init( HINSTANCE Hinst );
	bool			Update( void );

	Keyboard*		GetKeyboard( void )			{ return m_pKeyboard; }
	Mouse*			GetMouse( void )			{ return m_pMouse; }
	PadManager*		GetPadMng( void )			{ return m_pPadMng; }
	Pad*			GetPad( uint PadID );

	static bool		CountIsRepeat( int Count );

private:
	Keyboard*		m_pKeyboard;
	Mouse*			m_pMouse;
	PadManager*		m_pPadMng;
};

}

#include "Keyboard.h"
#include "Mouse.h"
#include "Pad.h"
