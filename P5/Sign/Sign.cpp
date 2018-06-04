#include "P5.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "Pad.h"
#include "PadManager.h"


namespace P5
{

//------------------------------------------------------------------------------
//	定数
//------------------------------------------------------------------------------
const int	REPEAT_COUNT		= 20;
const int	REPEAT_INTERVAL		= 5;


///////////////////////////////////////////////////////////////////////////////
//
//	入力管理
//
///////////////////////////////////////////////////////////////////////////////
Sign::Sign( void )
{
	m_pKeyboard		= NULL;
	m_pMouse		= NULL;
	m_pPadMng		= NULL;
}
Sign::~Sign()
{
	SafeDelete( m_pKeyboard );
	SafeDelete( m_pMouse );
	SafeDelete( m_pPadMng );
}

//*****************************************************************************
//	初期化
//*****************************************************************************
bool Sign::Init( HINSTANCE Hinst )
{
	Log( "　□入力管理初期化開始" );

	//	キーボード
	m_pKeyboard = new Keyboard;
	if( !m_pKeyboard ) return false;

	//	マウス
	m_pMouse = new Mouse;
	if( !m_pMouse ) return false;

	//	パッド管理
	m_pPadMng = new PadManager;
	if( !m_pPadMng ) return false;
	if( !m_pPadMng->Init( Hinst ) ) return false;

	Log( "　○入力管理初期化完了" );
	Log( "　----------------------------------------" );

	return true;
}

//*****************************************************************************
//	更新
//*****************************************************************************
bool Sign::Update( void )
{
	//	キーボード
	m_pKeyboard->Update();

	//	マウス
	m_pMouse->Update();
	
	//	パッド
	if( !m_pPadMng->Update() ) return false;

	return true;
}

//******************************************************************************
//	パッド取得
//******************************************************************************
Pad* Sign::GetPad( uint Index )
{
	return m_pPadMng->GetPad( Index );
}

//------------------------------------------------------------------------------
//	カウントがリピートに適応するか
//------------------------------------------------------------------------------
bool Sign::CountIsRepeat( int Count )
{
	return Count >= REPEAT_COUNT && (Count % REPEAT_INTERVAL) == 0;
}


}