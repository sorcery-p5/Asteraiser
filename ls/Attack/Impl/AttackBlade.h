#pragma once

#include "../Attack.h"

class Enemy;


////////////////////////////////////////////////////////////////////////////////
//
//	“G‚ÌUŒ‚FƒuƒŒ[ƒh
//
////////////////////////////////////////////////////////////////////////////////
class AttackBlade : public Attack
{
public:
	AttackBlade( AttackManager* pManager );
	virtual ~AttackBlade();

	virtual bool				IsAttach( void )							const	{ return true; }
	virtual bool				IsRepulse( void )							const	{ return m_pParam->bRepulse; }
	virtual void				SetOwner( Enemy* pEnemy );

	virtual	void				OnNotify( ATTACK_NOTIFY Notify );

protected:
	virtual void				_OnInit( void );
	virtual void				_OnUpdate( void );
	
	virtual float				_GetOutRadius( void )						const	{ return m_pParam->Radius + m_pParam->Length; }
	virtual CollideShape*		_CreateCollideShape( void )					const;
	virtual bool				_OnCastHit( const COLLIDE_RESULT& Result );

private:
	void						_Crash( void );

private:
	const ATTACK_BLADE*			m_pParam;
	RefPtr<Enemy>				m_pEnemy;
	float						m_Energy;
};