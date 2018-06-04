#include "P5.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	P5
//
///////////////////////////////////////////////////////////////////////////////
Window*			s_pWindow		= NULL;
Graphix*		s_pGraphix		= NULL;
Sign*			s_pSign			= NULL;
Tone*			s_pTone			= NULL;

//*****************************************************************************
//	������
//*****************************************************************************
bool Init( HINSTANCE Hinst, const INIT_PARAM& InitParam )
{
	Log( "���V�X�e���������J�n" );

	//	�E�B���h�E�̏�����
	s_pWindow = new Window( Hinst );
	if( !s_pWindow ) return false;
	if( !s_pWindow->Init( InitParam ) ) return false;

	//	�O���t�B�N�X������
	s_pGraphix = new Graphix;
	if( !s_pGraphix ) return false;
	if( !s_pGraphix->Init( s_pWindow ) ) return false;
	
	//	�E�B���h�E�\��
	s_pWindow->Show();

	//	�T�E���h������
	s_pTone = new Tone;
	if( !s_pTone ) return false;
	s_pTone->Init();

	//	���͊Ǘ�������
	s_pSign = new Sign;
	if( !s_pSign ) return false;
	if( !s_pSign->Init( Hinst ) ) return false;
	
	Log( "���V�X�e������������" );
	Log( "----------------------------------------" );

	return true;
}

//*****************************************************************************
//	�I��
//*****************************************************************************
void Exit( void )
{
	SafeDelete( s_pSign );
	SafeDelete( s_pTone );
	SafeDelete( s_pGraphix );
	SafeDelete( s_pWindow );
}

//*****************************************************************************
//	�X�V
//*****************************************************************************
bool Update( void )
{
	//	����
	if( !s_pWindow || !s_pGraphix ) return false;

	//	�E�B���h�E�̍X�V
	if( !s_pWindow->Update() )
	{
		return false;
	}

	//	�O���t�B�N�X�̍X�V
	if( !s_pGraphix->Update() )
	{
		return false;
	}

	//	�T�E���h�̍X�V
	if( !s_pTone || !s_pTone->Update() )
	{
		return false;
	}

	//	���͊Ǘ��̍X�V
	if( !s_pSign || !s_pSign->Update() )
	{
		return false;
	}

	return true;
}

//*****************************************************************************
//	�E�B���h�E�̎擾
//*****************************************************************************
Window* GetWindow( void )
{
	_ASSERT( s_pWindow != NULL );
	return s_pWindow;
}

//*****************************************************************************
//	�O���t�B�N�X�̎擾
//*****************************************************************************
Graphix* GetGraphix( void )
{
	_ASSERT( s_pGraphix != NULL );
	return s_pGraphix;
}

//*****************************************************************************
//	���͊Ǘ��̎擾
//***************************************************************************** 
Sign* GetSign( void )
{
	_ASSERT( s_pSign != NULL );
	return s_pSign;
}

//*****************************************************************************
//	�T�E���h�Ǘ��̎擾
//***************************************************************************** 
Tone* GetTone( void )
{
	_ASSERT( s_pTone != NULL );
	return s_pTone;
}

}