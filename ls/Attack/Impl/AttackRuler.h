#pragma once

#include "../Attack.h"

class Player;


////////////////////////////////////////////////////////////////////////////////
//
//	“G‚ÌUŒ‚Fƒ‹[ƒ‰[
//
////////////////////////////////////////////////////////////////////////////////
class AttackRuler : public Attack
{
public:
	AttackRuler( AttackManager* pManager );
	virtual ~AttackRuler();

	virtual void				SetOwner( Enemy* pEnemy );

	virtual int					GetEndFrame( void )						const;

protected:
	virtual void				_OnInit( void );
	virtual void				_OnUpdate( void );
	virtual void				_OnEndFrame( void );
	
	virtual float				_GetOutRadius( void )					const	{ return m_pParam->Radius; }
	virtual CollideShape*		_CreateCollideShape( void )				const;
	virtual bool				_OnCastHit( const COLLIDE_RESULT& Result );

	virtual bool				IsHit( const COLLIDE_OWNER& Owner )		const;

private:
	const ATTACK_RULER*			m_pParam;
	RefPtr<Enemy>				m_pEnemy;
	RefPtr<Player>				m_pPlayer;
	bool						m_bHit;
	int							m_HitCount;
	Point						m_HitVel;
	bool						m_bChangeVolume;
};