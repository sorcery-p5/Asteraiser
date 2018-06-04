#include "P5.h"
#include "Mouse.h"


namespace P5
{

const int s_aButtonCode[] = { VK_LBUTTON, VK_RBUTTON, VK_MBUTTON };


///////////////////////////////////////////////////////////////////////////////
//
//	�}�E�X
//
///////////////////////////////////////////////////////////////////////////////
Mouse::Mouse( void )
{
	ZeroArray( m_CurrButton );
	ZeroArray( m_PrevButton );
	ZeroArray( m_ButtonCount );
	m_Wheel		= 0;
	m_WheelBuff	= 0;
	
	m_bButtonSwap = ::GetSystemMetrics( SM_SWAPBUTTON ) != 0;
}

Mouse::~Mouse()
{

}

//*****************************************************************************
//	�X�V
//*****************************************************************************
void Mouse::Update( void )
{
	//	���W�̎擾
	POINT Pt = { 0, 0 };
	GetCursorPos( &Pt );
	ScreenToClient( P5::GetWindow()->GetHwnd(), &Pt );

	m_PrevPos	= m_Pos;
	m_Pos		= Pt;

	//	�{�^���̍X�V
	for( int Button = 0; Button < MOUSE_BUTTON_NUM; Button++ )
	{
		m_PrevButton[Button] = m_CurrButton[Button];
		m_CurrButton[Button] = (GetAsyncKeyState( s_aButtonCode[Button] ) & 0x8000) != 0;

		if( m_CurrButton[Button] )
		{
			m_ButtonCount[Button]++;
		}
		else
		{
			m_ButtonCount[Button] = 0;
		}
	}

	//	�z�C�[���̍X�V
	m_Wheel		= m_WheelBuff;
	m_WheelBuff	= 0;
}

//*****************************************************************************
//	�}�E�X�{�^����������Ă��邩�ǂ���
//*****************************************************************************
bool Mouse::IsButtonInput( MOUSE_BUTTON Button, INPUT_TYPE Type )	const
{
	//	�}�E�X�{�^�����]�̗L�����`�F�b�N
	if( m_bButtonSwap )
	{
		if( Button == MOUSE_L )		{ Button = MOUSE_R; }
		else if( Button == MOUSE_R ){ Button = MOUSE_L; }
	}

	switch( Type )
	{
	case INPUT_PRESS:	return  m_CurrButton[Button];
	case INPUT_TRIG:	return  m_CurrButton[Button] && !m_PrevButton[Button];
	case INPUT_OFF:		return !m_CurrButton[Button] &&  m_PrevButton[Button];
	case INPUT_REPEAT:	return Sign::CountIsRepeat( m_ButtonCount[Button] )? m_CurrButton[Button] : (!m_CurrButton[Button] && m_PrevButton[Button]);
	}
	return false;
}

}