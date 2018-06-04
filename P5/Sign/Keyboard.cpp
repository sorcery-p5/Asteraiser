#include "P5.h"
#include "Keyboard.h"


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	キーボード
//
///////////////////////////////////////////////////////////////////////////////
Keyboard::Keyboard( void )
{
	ZeroArray( m_CurrKey );
	ZeroArray( m_PrevKey );
	ZeroArray( m_KeyCount );
}

Keyboard::~Keyboard()
{
}

//*****************************************************************************
//	更新
//*****************************************************************************
void Keyboard::Update( void )
{
	for( int Code = 0; Code < UCHAR_MAX; Code++ )
	{
		m_PrevKey[Code] = m_CurrKey[Code];
		m_CurrKey[Code] = (::GetAsyncKeyState( Code ) & 0x8000) != 0;

		if( m_CurrKey[Code] )
		{
			m_KeyCount[Code]++;
		}
		else
		{		
			m_KeyCount[Code] = 0;
		}
	}
}

//*****************************************************************************
//	指定のキーが押されたかどうか
//*****************************************************************************
bool Keyboard::IsKeyInput( byte Code, INPUT_TYPE Type ) const
{
	switch( Type )
	{
	case INPUT_PRESS:	return  m_CurrKey[Code];
	case INPUT_TRIG:	return  m_CurrKey[Code] && !m_PrevKey[Code];
	case INPUT_OFF:		return !m_CurrKey[Code] &&  m_PrevKey[Code];
	case INPUT_REPEAT:	return Sign::CountIsRepeat( m_KeyCount[Code] )? IsKeyInput( Code, INPUT_PRESS ) : IsKeyInput( Code, INPUT_TRIG );
	}
	return false;
}

//******************************************************************************
//	押されたキーの取得
//******************************************************************************
byte Keyboard::GetInputKey( INPUT_TYPE Type ) const
{
	for( int Code = 0; Code < UCHAR_MAX; Code++ )
	{
		if( IsKeyInput( Code, Type ) ) return Code;
	}
	return 0;
}

}