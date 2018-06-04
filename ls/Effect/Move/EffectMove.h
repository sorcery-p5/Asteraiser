#pragma once

#include "../EffectTypes.h"

class EffectManager;
class EffectParticle;


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト移動物(基底)
//
////////////////////////////////////////////////////////////////////////////////
class EffectMove
{
public:
	EffectMove( EffectParticle*	pParticle )		{ _ASSERT(pParticle); m_pParticle = pParticle; }
	virtual ~EffectMove()						{}

	virtual void				Init( EffectManager* pManager, const Matrix3& Parent, const EFFECT_APPEAR* pAppear )	= 0;
	virtual void				Update( const EffectManager* pManager )													= 0;
	virtual void				ApplyTimeKey( EFFECT_TIME_KEY_TYPE Type, Point Val )		{}

	const Matrix3&				GetMatrix( void )									const	{ return m_Matrix; }

protected:
	Matrix3						m_Matrix;
	EffectParticle*				m_pParticle;
};