#pragma once

#include "../PlayerTypes.h"
class SpriteFrame;
class CollideCaster;
class CollideObject;
class Effect;
class Player;
class Enemy;
class Item;
class Attack;
class Ground;
class BladeEnergy;


////////////////////////////////////////////////////////////////////////////////
//
//	ブレード
//
////////////////////////////////////////////////////////////////////////////////
class Blade
{
public:
	Blade( void );
	~Blade();

	void					Init( Player* pParent, const BLADE_PARAM* pParam );
	void					Reset( void );
	void					Update( void );

	void					Charge( Point Dir );
	void					Slash( bool bSmash );
	void					Reload( void );
	void					CancelReload( void );
	void					CancelState( void );
	void					Delay( void );

	bool					OnHitEnemy( Enemy* pEnemy, float Power, Point Vel, int Knockback, bool bRepulse, bool bAntiHide, const Matrix3& HitMat, Label HitEffect );
	bool					OnHitAttack( float Exhaust, bool bRepulse, Point HitPos );
	void					OnHitItem( Item* pItem, Point HitPos );
	void					OnSplash( void );
	void					OnCrashRuler( void );
	void					OnDisableSlash( void );

	float					Regenerate( float Energy );
	float					OnGetItem( float Energy );

	void					SetVisible( bool bVisible )				{ m_bVisible = bVisible; }
	
	bool					IsActive( void )				const	{ return m_State != STATE_NORMAL; }
	bool					IsCharge( void )				const	{ return m_State == STATE_CHARGE; }
	bool					IsRepulse( void )				const	{ return m_bRepulse; }
	bool					IsCrash( void )					const;
	bool					IsSlashEnable( void )			const;
	bool					IsReloadEnable( void )			const;
	bool					IsCancelEnable( void )			const;
	bool					IsHit( void )					const	{ return m_bHit; }
	bool					IsSmash( void )					const	{ return m_bSmash; }

	float					GetAttackRate( void )			const;
	float					GetAttackPower( void )			const;
	float					GetRepulseAttackPower( void )	const;
	float					GetAttackVel( void )			const;
	int						GetKnockback( void )			const;

	Rand&					GetRand( void )					const;

private:
	enum STATE
	{
		STATE_NORMAL,
		STATE_CHARGE,
		STATE_SLASH,
	};

private:
	void					_UpdateNormal( void );
	void					_UpdateCharge( void );
	void					_UpdateSlash( void );

	void					_UpdateMatrix( const Matrix3* Mat );
	void					_UpdateHitSize( void );
	void					_UpdateEffect( void );
	void					_UpdateIndicate( void );
	void					_UpdateRuler( void );
	void					_OnHitEnemy( Enemy* pEnemy, Point HitPos );
	void					_OnHitGround( Ground* pGround, Point HitPos );
	void					_Exhaust( float Damage, bool bRepulse, Point HitPos );

	float					_GetExhaustRate( void )			const;
	Matrix3					_GetHitMatrix( Point HitPos )	const;

private:
	Player*					m_pParent;
	const BLADE_PARAM*		m_pParam;

	Matrix3					m_Matrix;
	STATE					m_State;
	int						m_StateCount;
	bool					m_bVisible;

	Point					m_KnockbackDir;
	bool					m_bRepulse;
	int						m_RepulseCount;
	bool					m_bAttackRepulse;
	int						m_Delay;
	int						m_CancelCount;
	int						m_NotExhaustCount;
	int						m_NotReloadCount;
	int						m_DisableSlashCount;
	float					m_ExhaustMax;
	bool					m_bHit;
	bool					m_bSmash;
	bool					m_bRuler;
	bool					m_bEndNotify;

	RefPtr<Effect>			m_pEffect;
	CollideObject*			m_pCollide;
	CollideCaster*			m_pCaster;
	BladeEnergy*			m_pEnergy;
};


////////////////////////////////////////////////////////////////////////////////
//
//	ブレードエネルギー
//
////////////////////////////////////////////////////////////////////////////////
class BladeEnergy
{
public:
	BladeEnergy( Blade* pParent, const BLADE_ENERGY_PARAM* pParam );
	~BladeEnergy();

	void						Reset( void );
	void						Update( void );

	void						Reload( void );
	void						CancelReload( void );

	void						Exhaust( float Damage );
	void						ChargeExhaust( float Energy, float Limit );
	float						Regenerate( float Energy );
	float						OnGetItem( float Energy );
	void						Crash( void );

	float						GetEnergyRate( void )			const;
	float						GetNormalEnergyRate( void )		const;
	float						GetRedEnergyRate( void )		const;
	float						GetCancelEnergyRate( void )		const;
	float						GetLimitRate( void )			const;

	bool						IsNormalState( void )			const	{ return m_State == STATE_NORMAL; }
	bool						IsCrash( void )					const	{ return m_State == STATE_CRASH; }
	bool						IsReload( void )				const	{ return m_State == STATE_RELOAD; }
	bool						IsSlashEnable( void )			const;
	bool						IsNormalRedzone( void )			const;
	
	bool						IsEnergyFull( void )			const;
	bool						IsEnergyLimit( void )			const;
	
private:
	enum STATE
	{
		STATE_NORMAL,
		STATE_RELOAD,
		STATE_CRASH,
	};

private:
	void						_UpdateNormal( void );
	void						_UpdateCharge( void );
	void						_UpdateReload( void );
	void						_UpdateCrash( void );

	void						_ExhaustEnergy( float Energy, float Limit );

private:
	Blade*						m_pParent;
	const BLADE_ENERGY_PARAM*	m_pParam;
	STATE						m_State;

	float						m_EnergyLimit;
	float						m_Energy;
	float						m_CancelEnergy;
};