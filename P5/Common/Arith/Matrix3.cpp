#include "P5.h"


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	3x3行列
//
///////////////////////////////////////////////////////////////////////////////
const Matrix3 Matrix3::ident = Matrix3(	1.0f, 0.0f, 0.0f,
										0.0f, 1.0f, 0.0f,
										0.0f, 0.0f, 1.0f );

//******************************************************************************
//	X回転の設定
//******************************************************************************
void Matrix3::SetRotX( float a )
{
	m[0][0] = Cos( a );
}

//******************************************************************************
//	Y回転の設定
//******************************************************************************
void Matrix3::SetRotY( float a )
{
	m[1][1] = Cos( a );
}

//******************************************************************************
//	Z回転の設定
//******************************************************************************
void Matrix3::SetRotZ( float a )
{
	m[0][0] =  Cos( a ); m[0][1] = Sin( a );
	m[1][0] = -Sin( a ); m[1][1] = Cos( a );
}

//******************************************************************************
//	A地点からB地点に向かう行列を設定
//******************************************************************************
void Matrix3::SetFromTo( Point a, Point b )
{
	SetRotZ( (b - a).GetAngle() );
	SetPos( a );
}

//*****************************************************************************
//	逆行列の算出
//*****************************************************************************
void Matrix3::Inverse( void )
{
	Matrix3	result;
	if( GetInverse( result ) )
	{
		Set( result );
	}
}

//*****************************************************************************
//	乗算
//*****************************************************************************
void Matrix3::Multiply( const Matrix3& mat )
{
	Matrix3	result;

	result.m[0][0] = m[0][0] * mat.m[0][0] + m[0][1] * mat.m[1][0] + m[0][2] * mat.m[2][0];
	result.m[0][1] = m[0][0] * mat.m[0][1] + m[0][1] * mat.m[1][1] + m[0][2] * mat.m[2][1];
	result.m[0][2] = m[0][0] * mat.m[0][2] + m[0][1] * mat.m[1][2] + m[0][2] * mat.m[2][2];

	result.m[1][1] = m[1][0] * mat.m[0][1] + m[1][1] * mat.m[1][1] + m[1][2] * mat.m[2][1];
	result.m[1][0] = m[1][0] * mat.m[0][0] + m[1][1] * mat.m[1][0] + m[1][2] * mat.m[2][0];
	result.m[1][2] = m[1][0] * mat.m[0][2] + m[1][1] * mat.m[1][2] + m[1][2] * mat.m[2][2];

	result.m[2][0] = m[2][0] * mat.m[0][0] + m[2][1] * mat.m[1][0] + m[2][2] * mat.m[2][0];
	result.m[2][1] = m[2][0] * mat.m[0][1] + m[2][1] * mat.m[1][1] + m[2][2] * mat.m[2][1];
	result.m[2][2] = m[2][0] * mat.m[0][2] + m[2][1] * mat.m[1][2] + m[2][2] * mat.m[2][2];

	Set( result );
}

//******************************************************************************
//	逆行列の取得
//******************************************************************************
bool Matrix3::GetInverse( Matrix3& Mat ) const
{
	float det =	m[0][0] * m[1][1] * m[2][2] +
				m[1][0] * m[2][1] * m[0][2] +
				m[2][0] * m[0][1] * m[1][2] -
				m[0][0] * m[2][1] * m[1][2] -
				m[2][0] * m[1][1] * m[0][2] -
				m[1][0] * m[0][1] * m[2][2];

	if( det == 0.0f ) return false;

	det = 1.0f / det;

	Mat.m[0][0] = det * (m[1][1] * m[2][2] - m[1][2] * m[2][1]);
	Mat.m[0][1] = det * (m[0][2] * m[2][1] - m[0][1] * m[2][2]);
	Mat.m[0][2] = 0.0f;

	Mat.m[1][0] = det * (m[1][2] * m[2][0] - m[1][0] * m[2][2]);
	Mat.m[1][1] = det * (m[0][0] * m[2][2] - m[0][2] * m[2][0]);
	Mat.m[1][2] = 0.0f;

	Mat.m[2][0] = det * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
	Mat.m[2][1] = det * (m[0][1] * m[2][0] - m[0][0] * m[2][1]);
	Mat.m[2][2] = 1.0f;

	return true;
}

//*****************************************************************************
//	変換
//*****************************************************************************
Point Matrix3::Transform( const Point& pt ) const
{
	return Point( m[0][0] * pt.x + m[1][0] * pt.y + m[2][0],
				  m[0][1] * pt.x + m[1][1] * pt.y + m[2][1] );
}

//******************************************************************************
//	変換（移動なし）
//******************************************************************************
Point Matrix3::TransformNormal( const Point& pt ) const
{
	return Point( m[0][0] * pt.x + m[1][0] * pt.y, 
				  m[0][1] * pt.x + m[1][1] * pt.y );
}

}