#pragma once

#include "EffectMove.h"


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト移動物（固定型）
//
////////////////////////////////////////////////////////////////////////////////
class EffectMoveDefault : public EffectMove
{
public:
	EffectMoveDefault( EffectParticle* pParent );
	virtual ~EffectMoveDefault();

	virtual void				Init( EffectManager* pManager, const Matrix3& Parent, const EFFECT_APPEAR* pAppear );
	virtual void				Update( const EffectManager* pManager );
	virtual void				ApplyTimeKey(  EFFECT_TIME_KEY_TYPE Type, Point Val );

private:
	void						_UpdateMatrix( void );

private:
	Matrix3						m_BaseMat;
	Point						m_Pos;
	Angle						m_Rot;
	Point						m_Scale;
};