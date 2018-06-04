#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include	<Dinput.h>


namespace P5
{

//-----------------------------------------------------------------------------
//	型定義
//-----------------------------------------------------------------------------
class Keyboard;
class Mouse;
class PadManager;
class Pad;


//-----------------------------------------------------------------------------
//	入力タイプ
//-----------------------------------------------------------------------------
enum INPUT_TYPE
{
	INPUT_PRESS,		//	現在押されている
	INPUT_TRIG,			//	押された瞬間
	INPUT_OFF,			//	離された瞬間
	INPUT_REPEAT,		//	リピート

	INPUT_TYPE_NUM,
};

///////////////////////////////////////////////////////////////////////////////
//
//	入力管理
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
