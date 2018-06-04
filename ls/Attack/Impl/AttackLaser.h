#pragma once

#include "../Attack.h"


////////////////////////////////////////////////////////////////////////////////
//
//	“G‚ÌUŒ‚FƒŒ[ƒU[
//
////////////////////////////////////////////////////////////////////////////////
class AttackLaser : public Attack
{
public:
	AttackLaser( AttackManager* pManager );
	virtual ~AttackLaser();
	
	virtual bool				IsAttach( void )							const	{ return m_pParam->bAttach; }
	virtual bool				IsRepulse( void )							const	{ return true; }

protected:
	virtual void				_OnInit( void );
	virtual void				_OnUpdate( void );
	
	virtual float				_GetRadius( void )							const	{ return m_pParam->Radius; }
	virtual CollideShape*		_CreateCollideShape( void )					const;
	virtual bool				_OnCastHit( const COLLIDE_RESULT& Result );

	void						_SetEffectLength( float Len );

private:
	const ATTACK_LASER*			m_pParam;
	float						m_HitLength;
};