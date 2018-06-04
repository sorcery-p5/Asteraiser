#pragma once

#include "../Attack.h"


////////////////////////////////////////////////////////////////////////////////
//
//	“G‚ÌUŒ‚F’eŠÛ
//
////////////////////////////////////////////////////////////////////////////////
class AttackBullet : public Attack
{
public:
	AttackBullet( AttackManager* pManager );
	virtual ~AttackBullet();

	virtual void				ApplyTimeKey( ATTACK_TIME_KEY_TYPE Type, const ATTACK_TIME_KEY_PARAM& Key );
	virtual void				ApplyTimeKey( ATTACK_TIME_KEY_TYPE Type, const ATTACK_TIME_KEY_PARAM& Key, const ATTACK_TIME_KEY_PARAM& Next, float f );

protected:
	virtual void				_OnInit( void );
	virtual void				_OnUpdate( void );
	virtual void				_OnEndFrame( void );
	virtual void				_OnOutField( void );

	virtual float				_GetOutRadius( void )						const	{ return m_pParam->Radius + m_pParam->Length + m_pParam->OutRadius; }
	virtual CollideShape*		_CreateCollideShape( void )					const;
	virtual bool				_OnCastHit( const COLLIDE_RESULT& Result );

	virtual void				_SetHitLength( float Len );

	virtual Label				_GetAppearAttack( void )					const	{ return Label(m_pParam->AppearAttack); }
	virtual bool				_IsPierce( void )							const	{ return m_pParam->bPierce; }
	virtual float				_GetRadius( void )							const	{ return m_pParam->Radius; }
	virtual float				_GetLength( void )							const	{ return m_pParam->Length; }
	
	virtual float				_GetFrontLength( void )						const	{ return m_pParam->FrontLength; }
	virtual Label				_GetEndEffect( void )						const	{ return Label(m_pParam->EndEffectName); }
	
private:
	const ATTACK_BULLET*		m_pParam;

protected:
	float						m_Speed;
	float						m_RotSpeed;
};