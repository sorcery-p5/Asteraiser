#pragma once

#include "EffectMove.h"


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト移動物（追従型）
//
////////////////////////////////////////////////////////////////////////////////
class EffectMoveFollow : public EffectMove
{
public:
	EffectMoveFollow( EffectParticle* pParticle );
	virtual ~EffectMoveFollow();

	virtual void				Init( EffectManager* pManager, const Matrix3& Parent, const EFFECT_APPEAR* pAppear );
	virtual void				Update( const EffectManager* pManager );
	virtual void				ApplyTimeKey( EFFECT_TIME_KEY_TYPE Type, Point Val );

private:
	void						_UpdateMatrix( void );

private:
	const Matrix3*				m_pParent;
	const EFFECT_APPEAR*		m_pAppear;
	Point						m_Pos;
	Angle						m_Rot;
	float						m_RotSpeed;
	Point						m_Scale;
};