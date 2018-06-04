#pragma once

#include "../PlayerTypes.h"

class SpriteFrame;
class CollideCaster;
class Player;


////////////////////////////////////////////////////////////////////////////////
//
//	ƒ_ƒbƒVƒ…
//
////////////////////////////////////////////////////////////////////////////////
class Dash
{
public:
	Dash( void );
	~Dash();

	void					Init( Player* pPlayer, const DASH_PARAM* pParam );
	void					Reset( void );
	void					Update( void );

	void					Start( Point Dir );
	void					Splash( Point Dir );
	void					CancelState( void );
	
	bool					IsActive( void )		const			{ return m_State != STATE_NORMAL; }
	bool					IsEnd( void )			const			{ return m_bEndNotify; }
	bool					IsDelay( void )			const			{ return m_Delay > 0; }
	bool					IsInvincible( void )	const			{ return m_bInvincible || m_InvincibleCount > 0; }

private:
	enum STATE
	{
		STATE_NORMAL,
		STATE_READY,
		STATE_AFTER,
	};

private:
	void					_UpdateNormal( void );
	void					_UpdateReady( void );
	void					_UpdateAfter( void );

	void					_ExecWarp( void );
	void					_CastSplash( Point Pos );

private:
	Player*					m_pParent;
	const DASH_PARAM*		m_pParam;
	CollideCaster*			m_pCaster;

	STATE					m_State;
	int						m_Delay;
	bool					m_bInvincible;
	int						m_InvincibleCount;

	Point					m_MoveDir;
	bool					m_bSplash;
	bool					m_bEndNotify;
};