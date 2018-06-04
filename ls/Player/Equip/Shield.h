#pragma once

#include "../PlayerTypes.h"

class Player;
class Effect;


////////////////////////////////////////////////////////////////////////////////
//
//	ƒV[ƒ‹ƒh
//
////////////////////////////////////////////////////////////////////////////////
class Shield
{
public:
	Shield( void );
	~Shield();

	void					Init( Player* pParent, const SHIELD_PARAM* pParam );
	void					Reset( void );
	void					Update( void );

	void					Damage( float Damage );
	float					Regenerate( float Energy );
	float					OnGetItem( float Energy );
	void					CancelBreak( void );
	void					OnRevive( void );
	
	bool					IsBreak( void )					const	{ return m_State == STATE_BREAK; }
	bool					IsInvincible( void )			const	{ return m_InvincibleCount > 0; }
	bool					IsNoMove( void )				const	{ return m_NoMoveCount > 0; }
	bool					IsDeadly( void )				const	{ return m_Sector == 0; }

private:
	enum STATE
	{
		STATE_NORMAL,
		STATE_BREAK,
	};

private:
	void					_UpdateNormal( void );
	void					_UpdateBreak( void );

	void					_UpdateRecovery( void );
	void					_UpdateIndicate( void );

	void					_Normal( void );
	void					_Break( void );

	float					_GetShieldRate( void )		const;
	float					_GetRecoveryRate( void )	const;

private:
	Player*					m_pParent;
	const SHIELD_PARAM*		m_pParam;
	STATE					m_State;

	RefPtr<Effect>			m_pEffect;

	float					m_Shield;
	int						m_Sector;
	float					m_Recovery;

	int						m_BreakNum;
	int						m_BreakCount;
	int						m_NoMoveCount;
	int						m_InvincibleCount;
	int						m_SerialDamageCount;
};