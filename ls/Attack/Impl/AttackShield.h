#pragma once

#include "../Attack.h"


////////////////////////////////////////////////////////////////////////////////
//
//	“G‚ÌUŒ‚FƒV[ƒ‹ƒh
//
////////////////////////////////////////////////////////////////////////////////
class AttackShield : public Attack
{
public:
	AttackShield( AttackManager* pManager );
	virtual ~AttackShield();

	virtual bool				IsAttach( void )						const	{ return m_pParam->bAttach; }
	virtual bool				IsRepulse( void )						const	{ return true; }
	virtual bool				IsHide( void )							const	{ return m_pParam->bHide; }
	virtual void				SetOwner( Enemy* pEnemy );

protected:
	virtual void				_OnInit( void );
	virtual void				_OnUpdate( void );
	
	virtual float				_GetOutRadius( void )					const	{ return m_pParam->Radius; }
	virtual CollideShape*		_CreateCollideShape( void )				const;
	virtual bool				_OnCastHit( const COLLIDE_RESULT& Result );

private:
	const ATTACK_SHIELD*		m_pParam;
	RefPtr<Enemy>				m_pEnemy;
	float						m_Energy;
};