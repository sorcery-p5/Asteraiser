#include "P5.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "Pad.h"
#include "PadManager.h"


namespace P5
{

//------------------------------------------------------------------------------
//	�萔
//------------------------------------------------------------------------------
const int	REPEAT_COUNT		= 20;
const int	REPEAT_INTERVAL		= 5;


///////////////////////////////////////////////////////////////////////////////
//
//	���͊Ǘ�
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
//	������
//*****************************************************************************
bool Sign::Init( HINSTANCE Hinst )
{
	Log( "�@�����͊Ǘ��������J�n" );

	//	�L�[�{�[�h
	m_pKeyboard = new Keyboard;
	if( !m_pKeyboard ) return false;

	//	�}�E�X
	m_pMouse = new Mouse;
	if( !m_pMouse ) return false;

	//	�p�b�h�Ǘ�
	m_pPadMng = new PadManager;
	if( !m_pPadMng ) return false;
	if( !m_pPadMng->Init( Hinst ) ) return false;

	Log( "�@�����͊Ǘ�����������" );
	Log( "�@----------------------------------------" );

	return true;
}

//*****************************************************************************
//	�X�V
//*****************************************************************************
bool Sign::Update( void )
{
	//	�L�[�{�[�h
	m_pKeyboard->Update();

	//	�}�E�X
	m_pMouse->Update();
	
	//	�p�b�h
	if( !m_pPadMng->Update() ) return false;

	return true;
}

//******************************************************************************
//	�p�b�h�擾
//******************************************************************************
Pad* Sign::GetPad( uint Index )
{
	return m_pPadMng->GetPad( Index );
}

//------------------------------------------------------------------------------
//	�J�E���g�����s�[�g�ɓK�����邩
//------------------------------------------------------------------------------
bool Sign::CountIsRepeat( int Count )
{
	return Count >= REPEAT_COUNT && (Count % REPEAT_INTERVAL) == 0;
}


}