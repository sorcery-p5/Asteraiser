#include "P5.h"
#include "RingSprite.h"

#include "Graphix/Texture/Texture.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	�֌`�X�v���C�g
//
///////////////////////////////////////////////////////////////////////////////
RingSprite::RingSprite( float RadMin, float RadMax, float Ang, int Num, int Divide )
: LineSprite( Num, Divide )
{
	m_RadiusMin	= RadMin;
	m_RadiusMax	= RadMax;
	m_Angle		= Clamp( Ang, 0.0f, PI2 );
}

RingSprite::~RingSprite()
{

}

//******************************************************************************
//	�`��i���S�w��j
//******************************************************************************
void RingSprite::Draw( const Point& Center )
{
	Draw( Matrix3::Trans( Center ) );
}

//******************************************************************************
//	�`��i�}�g���N�X�w��j
//******************************************************************************
void RingSprite::Draw( const Matrix3& Mat )
{
	std::vector<Point> vPosMin, vPosMax;

	for( int i = 0; i < GetNum(); i++ )
	{
		Point MinPt( 0.0f, m_RadiusMin ), MaxPt( 0.0f, m_RadiusMax );
		
		//	�p�x����
		float Ang = m_Angle * ( (float)i / (float)(GetNum() - 1) );
		MinPt = MinPt.Rot( Ang );
		MaxPt = MaxPt.Rot( Ang );

		//	�ϊ����Ēǉ�
		vPosMin.push_back( Mat.Transform( MinPt ) );
		vPosMax.push_back( Mat.Transform( MaxPt ) );
	}

	LineSprite::Draw( &vPosMin[0], &vPosMax[0] );
}


}