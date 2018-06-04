#pragma once

#include "LineSprite.h"


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	輪形スプライト
//
///////////////////////////////////////////////////////////////////////////////
class RingSprite : public LineSprite
{
public:
	RingSprite( float RadMin, float RadMax, float Ang, int Num, int Divide );
	~RingSprite();
	
	void				Draw( const Point& Center );
	void				Draw( const Matrix3& Mat );

	void				SetAngle( float Ang )					{ m_Angle = Clamp( Ang, 0.0f, PI2 ); }

	float				GetRadiusMax( void )			const	{ return m_RadiusMax; }

private:
	float				m_RadiusMin;
	float				m_RadiusMax;
	float				m_Angle;
};


}