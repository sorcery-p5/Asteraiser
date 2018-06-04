#pragma once

#include "../EffectTypes.h"
#include "EffectMove.h"


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト移動物（回転型）
//
////////////////////////////////////////////////////////////////////////////////
class EffectMoveCircular : public EffectMove
{
public:
	EffectMoveCircular( EffectParticle* pParticle );
	virtual ~EffectMoveCircular();

	virtual void				Init( EffectManager* pManager, const Matrix3& Parent, const EFFECT_APPEAR* pAppear );
	virtual void				Update( const EffectManager* pManager );
	virtual void				ApplyTimeKey(  EFFECT_TIME_KEY_TYPE Type, Point Val );

private:
	void						_UpdateMatrix( void );

private:
	const Matrix3*				m_pParent;
	float						m_Dist;
	Angle						m_Angle;
	float						m_AngleSpeed;
	Point						m_AxisScale;
	Angle						m_AxisAngle;
	Angle						m_Rot;
	float						m_RotSpeed;
	Point						m_Scale;
	EFFECT_DIR_TYPE				m_DirType;
};