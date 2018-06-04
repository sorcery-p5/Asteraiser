#pragma once


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	ìÒéüå≥ç¿ïW
//
///////////////////////////////////////////////////////////////////////////////
class Point
{
public:
	Point( void )												{ Set( 0.0f, 0.0f ); }
	Point( int _x, int _y )										{ Set( _x, _y );	 }
	Point( float _x, float _y )									{ Set( _x, _y );	 }
	Point( const Point& pt )									{ Set( pt.x, pt.y ); }
	Point( const POINT& pt )									{ Set( pt.x, pt.y ); }
	Point( const int* a )										{ Set( a );	 }
	Point( const float* a )										{ Set( a );	 }

	Point& operator = ( const Point& pt )						{ Set( pt.x, pt.y ); return *this; }
	Point& operator = ( const POINT& pt )						{ Set( pt.x, pt.y ); return *this; }

	Point operator -  ( void ) const							{ return Point( -x, -y ); }

	Point operator +( const Point& pt ) 				const	{ Point result( x, y ); result += pt;	return result; }
	Point operator -( const Point& pt ) 				const	{ Point result( x, y ); result -= pt;	return result; }
	Point operator *( const Point& pt ) 				const	{ Point result( x, y ); result *= pt;	return result; }
	Point operator /( const Point& pt ) 				const	{ Point result( x, y ); result /= pt;	return result; }
	Point operator +( float f )							const	{ Point result( x, y ); result += f;	return result; }
	Point operator -( float f )							const	{ Point result( x, y ); result -= f;	return result; }
	Point operator *( float f )							const	{ Point result( x, y ); result *= f;	return result; }
	Point operator /( float f )							const	{ Point result( x, y ); result /= f;	return result; }

	Point& operator +=( const Point& pt )						{ Set( x + pt.x, y + pt.y ); return *this; }
	Point& operator -=( const Point& pt )						{ Set( x - pt.x, y - pt.y ); return *this; }
	Point& operator *=( const Point& pt )						{ Set( x * pt.x, y * pt.y ); return *this; }
	Point& operator /=( const Point& pt )						{ Set( x / pt.x, y / pt.y ); return *this; }
	Point& operator +=( float f )								{ Set( x + f, y + f );		 return *this; }
	Point& operator -=( float f )								{ Set( x - f, y - f );		 return *this; }
	Point& operator *=( float f )								{ Set( x * f, y * f );		 return *this; }
	Point& operator /=( float f )								{ Set( x / f, y / f );		 return *this; }

	bool operator ==( const Point& pt )					const	{ return ( x == pt.x && y == pt.y ); }
	bool operator !=( const Point& pt )					const	{ return ( x != pt.x || y != pt.y ); }

	void 	Set( int _x, int _y )								{ x = (float)_x; y = (float)_y; }
	void 	Set( float _x, float _y )							{ x = _x; y = _y; }
	void 	Set( const POINT& pt )								{ x = (float)pt.x; y = (float)pt.y; }
	void 	Set( const int* a )									{ x = (float)a[0]; y = (float)a[1]; }
	void 	Set( const float* a )								{ x = a[0]; y = a[1]; }
	void	Zero( void )										{ x = 0.0f; y = 0.0f; }
	void	One( void )											{ x = 1.0f; y = 1.0f; }

	operator POINT( void )								const	{ POINT pt = { (int)x, (int)y }; return pt; }
	operator qword( void )								const	{ return qw; }

	void	Shift( float sx, float sy )							{ x += sx;	y += sy; }
	void	Scale( float sx, float sy )							{ x *= sx;	y *= sy; }
	void	Shift( float s )									{ x += s;	y += s; }
	void	Scale( float s )									{ x *= s;	y *= s; }

	bool	IsZero( void )								const	{ return x == 0.0f && y == 0.0f; }
	bool	IsEpsilon( void )							const	{ return x < EPS && y < EPS && x > -EPS && y > -EPS; }
	bool	IsOne( void )								const	{ return x == 1.0f && y == 1.0f; }

	float	Length( void )								const	{ return Sqrt( x*x + y*y ); }
	float	LengthSq( void )							const	{ return x*x + y*y; }
	float	Area( void )								const	{ return x * y; }
	float	Dot( const Point& r )						const	{ return x * r.x + y * r.y; }
	float	Cross( const Point& r )						const	{ return x * r.y - y * r.x; }
	Point	Reflect( const Point& n )					const	{ float t = -Dot(n)/n.LengthSq(); return Point( x + t*n.x * 2.0f, y + t*n.y * 2.0f ); }
	Point	Slide( const Point& n )						const	{ float t = -Dot(n)/n.LengthSq(); return Point( x + t*n.x, y + t*n.y ); }
	Point	Rot( float a )								const	{ return Point( Cos(a) * x - Sin(a) * y, Sin(a) * x + Cos(a) * y); }
	Point	Rot90( void )								const	{ return Point( -y, x ); }
	Point	RotMinus90( void )							const	{ return Point( y, -x ); }

	void	Normalize( void )									{ float l = Length(); if(l>0.0f) Scale(1.0f/l); }
	void	Floor( void )										{ x = floorf(x); y = floorf(y); }
	void	Ceil( void )										{ x = ceilf(x); y = ceilf(y); }
	void	Round( void )										{ x = P5::Round(x); y = P5::Round(y); }
	Point	GetNormal( void )							const	{ Point result( x, y ); result.Normalize();	return result; }
	Point	GetFloor( void )							const	{ Point result( x, y ); result.Floor();		return result; }
	Point	GetCeil( void )								const	{ Point result( x, y ); result.Ceil();		return result; }
	Point	GetRound( void )							const	{ Point result( x, y ); result.Round();		return result; }
	qword	GetQword( void )							const	{ return qw; }
	float	GetAngle( void )							const	{ return Atan( *this ); }

	static Point Min( const Point& a, const Point& b )			{ Point pt( P5::Min(a.x,b.x), P5::Min(a.y,b.y) ); return pt; }
	static Point Max( const Point& a, const Point& b )			{ Point pt( P5::Max(a.x,b.x), P5::Max(a.y,b.y) ); return pt; }
	static Point EpsilonCut( Point Pt )							{ return Point( P5::EpsilonCut(Pt.x), P5::EpsilonCut(Pt.y) ); }

	static Point NearTo( const Point& val, const Point& aim, float step )
	{
		Point dir = (aim - val);
		if( dir.LengthSq() < Sq(step) ) return aim;

		return val + dir.GetNormal() * step;
	}

public:
	union
	{
		struct
		{
			float	x, y;
		};
		qword		qw;
	};
};

}