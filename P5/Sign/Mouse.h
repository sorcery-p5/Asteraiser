#pragma once


namespace P5
{

	
//-----------------------------------------------------------------------------
//	マウスボタン種別
//-----------------------------------------------------------------------------
enum MOUSE_BUTTON
{
	MOUSE_L,
	MOUSE_R,
	MOUSE_M,

	MOUSE_BUTTON_NUM,
};

///////////////////////////////////////////////////////////////////////////////
//
//	マウス
//
///////////////////////////////////////////////////////////////////////////////
class Mouse
{
public:
	Mouse( void );
	~Mouse();

	void			Update( void );

	bool			IsButtonInput( MOUSE_BUTTON Button, INPUT_TYPE Type )	const;
	int				GetButtonCount( MOUSE_BUTTON Button )					const	{ return m_ButtonCount[Button]; }
	Point			GetPos( void )											const	{ return m_Pos; }
	Point			GetPrevPos( void )										const	{ return m_PrevPos; }
	Point			GetSpeed( void )										const	{ return m_Pos - m_PrevPos; }
	int				GetWheel( void )										const	{ return m_Wheel; }

	void			AddWheel( int Val )												{ m_WheelBuff += Val; }

private:
	bool			m_CurrButton[MOUSE_BUTTON_NUM];
	bool			m_PrevButton[MOUSE_BUTTON_NUM];
	bool			m_ButtonCount[MOUSE_BUTTON_NUM];
	Point			m_Pos;
	Point			m_PrevPos;
	int				m_Wheel;
	int				m_WheelBuff;

	bool			m_bButtonSwap;
};

}