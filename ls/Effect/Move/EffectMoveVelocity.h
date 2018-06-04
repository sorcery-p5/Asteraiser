#pragma once

#include "EffectMove.h"


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト移動物（移動型）
//
////////////////////////////////////////////////////////////////////////////////
class EffectMoveVelocity : public EffectMove
{
public:
	EffectMoveVelocity( EffectParticle* pParticle, const EFFECT_MOVE_VELOCITY_PARAM* pParam );
	virtual ~EffectMoveVelocity();

	virtual void				Init( EffectManager* pManager, const Matrix3& Parent, const EFFECT_APPEAR* pAppear );
	virtual void				Update( const EffectManager* pManager );
	virtual void				ApplyTimeKey(  EFFECT_TIME_KEY_TYPE Type, Point Val );

private:
	void						_UpdateMatrix( void );

private:
	const EFFECT_MOVE_VELOCITY_PARAM*	m_pParam;
	const EFFECT_APPEAR*				m_pAppear;

	Point								m_Pos;
	Point								m_Vel;
	Angle								m_Rot;
	float								m_RotSpeed;
	Point								m_Scale;
};