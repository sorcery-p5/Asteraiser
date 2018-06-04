#include "P5.h"

#include <limits.h>


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	三角関数テーブル
//
///////////////////////////////////////////////////////////////////////////////
class TriTable
{
public:
	TriTable( void )						{ Init(); }

	float	Sin( const Angle& angle ) const;
	float	Cos( const Angle& angle ) const;
	Angle	Atan( float x, float y )  const;

private:
	void	Init( void );

private:
	enum
	{
		TABLE_SIZE	= (1 << 16),
	};

private:
	float m_SinTable[TABLE_SIZE];
	float m_AtanTable[TABLE_SIZE];
};


///////////////////////////////////////////////////////////////////////////////
//
//	算術
//
///////////////////////////////////////////////////////////////////////////////
static TriTable s_TriTable;
//*****************************************************************************
//	sinの取得
//*****************************************************************************
float	Sin( float angle )
{
	return s_TriTable.Sin( angle );
}

//*****************************************************************************
//	cosの取得
//*****************************************************************************
float	Cos( float angle )
{
	return s_TriTable.Cos( angle );
}

//*****************************************************************************
//	atanの取得
//*****************************************************************************
float	Atan( float x, float y )
{
	return s_TriTable.Atan( x, y );
}

//*****************************************************************************
//	atanの取得
//*****************************************************************************
float	Atan( const Point& pt )
{
	return s_TriTable.Atan( pt.x, pt.y );
}

//*****************************************************************************
//	平方根の取得
//*****************************************************************************
float	Sqrt( float c )
{
	static const int LOOP_MAX = 10;
	float	n;
	
	//	平方根に最も近い大きい方の整数を探す
	for( n = 1.0f ; n * n < c; n += 1.0f );

	for( int i = 0; i < LOOP_MAX; i++ )
	{
		if(n == 0) return 0.0f;

		n = (n + (c / n)) / 2.0f;	//	交点のx 座標を求める
	}

	return n;	//	求めた近似値を返す
}

///////////////////////////////////////////////////////////////////////////////
//
//	三角関数テーブル
//
///////////////////////////////////////////////////////////////////////////////
//*****************************************************************************
//	初期化
//*****************************************************************************
void TriTable::Init( void )
{
	for( uint i = 0; i < TABLE_SIZE; i++ )
	{
		m_SinTable[i] = sinf( Angle::WordToRad(i) );
		m_AtanTable[i] = atan2f( 1.0f, (float)i / (float)(TABLE_SIZE-1) );
	}
}

//*****************************************************************************
//	sinの取得
//*****************************************************************************
float	TriTable::Sin( const Angle& angle ) const
{
	return m_SinTable[ angle.Word() % TABLE_SIZE ];
}

//*****************************************************************************
//	cosの取得
//*****************************************************************************
float	TriTable::Cos( const Angle& angle ) const
{
	return m_SinTable[ (angle + PI05).Word() % TABLE_SIZE ];
}

//*****************************************************************************
//	atanの取得
//*****************************************************************************
Angle	TriTable::Atan( float x, float y ) const
{
	if( x == 0.0f && y == 0.0f )
	{
		return Angle( 0.0f );
	}

	bool rev = false;
	float ax = Abs(x), ay = Abs(y);

	//	縦より横が大きければ交換し、フラグたてておく
	if( ax < ay )
	{
		float w = ax;
		ax = ay;
		ay = w;
		rev = true;
	}
	
	//	角度引き出す
	float angle = m_AtanTable[ (int)(ay / ax * (TABLE_SIZE-1)) ];

	//	交換していたときは、45度線で反転
	if( rev )
	{
		angle = PI05 - angle;
	}

	if( x < 0.0f && y < 0.0f )
	{
		//	x、ｙが共に負の時、180度回転
		return -Angle( angle + PI );
	}
	else if( x < 0.0f )
	{
		//	xが負の時、Y軸で反転
		return -Angle( -angle );
	}
	else if( y < 0.0f )
	{
		//	ｙが負の時、X軸で反転
		return -Angle( PI - angle );
	}
	else
	{
		return -Angle( angle );
	}
}

}