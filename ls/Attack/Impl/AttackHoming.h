#pragma once

#include "AttackBullet.h"


////////////////////////////////////////////////////////////////////////////////
//
//	“G‚ÌUŒ‚F’Ç”ö
//
////////////////////////////////////////////////////////////////////////////////
class AttackHoming : public AttackBullet
{
public:
	AttackHoming( AttackManager* pManager );
	virtual ~AttackHoming();

	virtual void				ApplyTimeKey( ATTACK_TIME_KEY_TYPE Type, const ATTACK_TIME_KEY_PARAM& Key );
	virtual void				ApplyTimeKey( ATTACK_TIME_KEY_TYPE Type, const ATTACK_TIME_KEY_PARAM& Key, const ATTACK_TIME_KEY_PARAM& Next, float f );

protected:
	virtual void				_OnInit( void );
	virtual void				_OnUpdate( void );

	virtual float				_GetOutRadius( void )					const	{ return m_pParam->Radius + m_pParam->Length + m_pParam->OutRadius; }
	virtual Label				_GetAppearAttack( void )				const	{ return Label(m_pParam->AppearAttack); }
	virtual bool				_IsPierce( void )						const	{ return m_pParam->bPierce; }
	virtual float				_GetRadius( void )						const	{ return m_pParam->Radius; }
	virtual float				_GetLength( void )						const	{ return m_pParam->Length; }
	
	virtual float				_GetFrontLength( void )					const	{ return 0.0f; }
	virtual Label				_GetEndEffect( void )					const	{ return Label(); }

private:
	const ATTACK_HOMING*		m_pParam;
	float						m_Accel;
	float						m_RotAccel;
};