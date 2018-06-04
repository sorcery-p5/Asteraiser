#pragma once


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	キーボード
//
///////////////////////////////////////////////////////////////////////////////
class Keyboard
{
public:
	Keyboard( void );
	~Keyboard();

	void			Update( void );
	bool			IsKeyInput( byte Code, INPUT_TYPE Type )		const;
	int				GetInputCount( byte Code )						const	{ return m_KeyCount[Code]; }
	byte			GetInputKey( INPUT_TYPE Type )					const;

private:
	bool			m_CurrKey[UCHAR_MAX];
	bool			m_PrevKey[UCHAR_MAX];
	int				m_KeyCount[UCHAR_MAX];
};

}