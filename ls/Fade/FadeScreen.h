#pragma once


////////////////////////////////////////////////////////////////////////////////
//
//	フェードスクリーン
//
////////////////////////////////////////////////////////////////////////////////
class FadeScreen
{
public:
	FadeScreen( int Phase );
	~FadeScreen();

	void			Update( void );
	void			Draw( void );

	void			FadeIn( int Frame );
	void			FadeIn( int Frame, Color Col );
	void			FadeOut( int Frame, Color Col );

	bool			IsFade( void )						const		{ return m_State != STATE_NONE; }
	bool			IsActive( void )					const		{ return IsFade() && m_FrameCount <= m_TargetCount; }
	bool			IsFadeIn( void )					const		{ return m_State == STATE_IN; }
	bool			IsFadeOut( void )					const		{ return m_State == STATE_OUT; }

private:
	enum STATE
	{
		STATE_NONE,
		STATE_IN,
		STATE_OUT,
	};

private:
	RectSprite*		m_pSprite;

	STATE			m_State;

	Color			m_FadeColor;
	int				m_FrameCount;
	int				m_TargetCount;
};