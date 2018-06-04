#pragma once

#include "../PlayerTypes.h"

class Effect;
class SpriteFrame;
class Player;
class OverdriveEnergy;
class Spear;
class Ruler;


////////////////////////////////////////////////////////////////////////////////
//
//	オーヴァドライヴ
//
////////////////////////////////////////////////////////////////////////////////
class Overdrive
{
public:
	Overdrive( void );
	~Overdrive();

	void					Init( Player* pParent, const OVERDRIVE_PARAM* pParam );
	void					Reset( void );
	void					Update( void );

	void					DriveSpear( void );
	void					DriveRuler( void );
	void					CancelState( void );
	void					AllErase( void );
	
	float					Regenerate( float Energy );
	float					OnGetItem( float Energy );

	bool					IsActive( void )			const	{ return m_State != STATE_NONE; }
	bool					IsReady( void )				const	{ return m_State == STATE_READY; }
	bool					IsInvincible( void )		const	{ return m_bInvincible; }
	bool					IsSpearEnable( void )		const;
	bool					IsRulerEnable( void )		const;

private:
	enum TYPE
	{
		TYPE_SPEAR,
		TYPE_RULER,
	};

	enum STATE
	{
		STATE_NONE,
		STATE_READY,
		STATE_AFTER,
	};

	typedef std::list< Spear* >		SpearList;
	typedef std::list< Ruler* >		RulerList;

private:
	void					_UpdateReady( void );
	void					_UpdateAfter( void );
	void					_UpdateInvincible( void );
	void					_UpdateIndicate( void );

	void					_ExecSpear( void );
	void					_ExecRuler( void );
	void					_UpdateSpear( void );
	void					_UpdateRuler( void );

private:
	Player*					m_pParent;
	const OVERDRIVE_PARAM*	m_pParam;

	TYPE					m_Type;
	STATE					m_State;
	float					m_ReadyEnergy;
	bool					m_bInvincible;
	int						m_Delay;

	OverdriveEnergy*		m_pEnergy;

	SpearList				m_SpearList;
	RulerList				m_RulerList;
	
	RefPtr<Effect>			m_pIndicateEffect;
	RefPtr<Effect>			m_pSpearLockEffect;
};


////////////////////////////////////////////////////////////////////////////////
//
//	オーヴァドライヴエネルギー
//
////////////////////////////////////////////////////////////////////////////////
class OverdriveEnergy
{
public:
	OverdriveEnergy( const OVERDRIVE_PARAM*	pParam );
	
	void					Reset( void );
	void					Update( void );

	void					Drive( float Use );
	float					Regenerate( float Energy );
	float					OnGetItem( float Energy );

	float					GetEnergy( void )				const	{ return m_Energy; }
	float					GetEnergyRate( void )			const;
	float					GetEnergySectorRate( void )		const;
	int						GetEnergySectorNum( void )		const;
	bool					IsDriveEnable( float Use )		const;
	bool					IsEnergyFull( void )			const;
	
private:
	const OVERDRIVE_PARAM*	m_pParam;
	float					m_Energy;
};