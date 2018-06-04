#include "P5.h"


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	角度
//
///////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//	角度を360の間でループ
//*****************************************************************************
float Angle::Loop( float in )
{
	float diff = ( in < 0.0f )? PI2 : -PI2;
	while( in < 0.0f || in > PI2 )	{ in += diff; }
	return in;
}

//*****************************************************************************
//	角度の差分を得る
//*****************************************************************************
float Angle::Diff( Angle a, Angle b )
{
	float diff = (b.Rad() - a.Rad());	
	if( Abs(diff) > PI ){ diff += (diff < 0.0f)? PI2 : -PI2; }
	return diff;
}

//*****************************************************************************
//	角度を刻み幅分近づける
//*****************************************************************************
Angle Angle::NearTo( Angle a, Angle aim, float step )
{
	float diff = Diff( a, aim );
	if( diff < 0.0f ){ step = -step; }

	if( Abs(diff) < Abs(step) )	return aim;
	else						return a + step;
}

//*****************************************************************************
//	線形補間	
//*****************************************************************************
Angle Angle::Lerp( Angle a1, Angle a2, float t )
{
	return Angle( a1.angle + ( Diff( a1, a2 ) * t ) );
}

//*****************************************************************************
//	キャットマルロム補間
//*****************************************************************************
Angle Angle::CatmullRom( Angle pre, Angle a1, Angle a2, Angle next, float t )
{
	float a[] = { pre, a1, a2, next };
	for( int i = 1; i < 4; i++ )
	{
		float diff = a[i] - a[i - 1];
		if( Abs(diff) > PI )
		{
			a[i] += (diff > 0.0f)? -PI2 : PI2;
		}
	}

	return Angle( P5::CatmullRom( a[0], a[1], a[2], a[3], t ) );
}

}