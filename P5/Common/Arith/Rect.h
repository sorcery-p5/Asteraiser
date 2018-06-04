#pragma once


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	‹éŒ`
//
///////////////////////////////////////////////////////////////////////////////
class Rect
{
public:
	Rect( void )													{ Set( 0.0f, 0.0f, 0.0f, 0.0f ); }
	Rect( int _l, int _t, int _r, int _b )							{ Set( _l, _t, _r, _b ); }
	Rect( float _l, float _t, float _r, float _b )					{ Set( _l, _t, _r, _b ); }
	Rect( const Point& lt, const Point& rb )						{ Set( lt.x, lt.y, rb.x, rb.y ); }
	Rect( const Rect& rc )											{ Set( rc.l, rc.t, rc.r, rc.b ); }
	Rect( const RECT& rc )											{ Set( rc ); }

	Rect& operator = ( const Rect& rc )								{ Set( rc ); return *this; }
	Rect& operator = ( const RECT& rc )								{ Set( rc ); return *this; }
	
	Rect& operator += ( const Point& pt )							{ Shift( pt ); return *this; }
	Rect& operator *= ( const Point& pt )							{ Scale( pt ); return *this; }
	Rect  operator + ( const Point& pt )							{ Rect rc = *this; rc.Shift( pt ); return rc; }
	Rect  operator * ( const Point& pt )							{ Rect rc = *this; rc.Scale( pt ); return rc; }

	bool operator ==( const Rect& rc ) 						const	{ return ( l == rc.l && t == rc.t && r == rc.r && b == rc.b ); }
	bool operator !=( const Rect& rc ) 						const	{ return ( l != rc.l || t != rc.t || r != rc.r || b != rc.b ); }

	void 	Set( int _l, int _t, int _r, int _b )					{ l = (float)_l; t = (float)_t; r = (float)_r; b = (float)_b; }
	void 	Set( float _l, float _t, float _r, float _b )			{ l = _l; t = _t; r = _r; b = _b; }
	void 	Set( const RECT& rc )									{ Set( rc.left, rc.top, rc.right, rc.bottom); }
	void 	SetXYWH( int x, int y, int w, int h )					{ l = (float)x; t = (float)y; r = l + (float)w; b = t + (float)h; }
	void	SetXYWH( float x, float y, float w, float h )			{ l = x; t = y; r = l + w; b = t + h; }
	void	Zero( void )											{ Set( 0.0f, 0.0f, 0.0f, 0.0f ); }

	operator RECT( void ) 									const	{ RECT rc = { (int)l, (int)t, (int)r, (int)b }; return rc; }

	float	Width( void ) 									const	{ return r - l; }
	float	Height( void )									const	{ return b - t; }
	float	CenterX( void ) 								const	{ return (l+r)/2.0f; }
	float	CenterY( void )									const	{ return (t+b)/2.0f; }

	Point	Size( void )									const	{ return Point( Width(), Height() ); }
	Point	Center( void )									const	{ return Point( (l+r)/2.0f, (t+b)/2.0f ); }
	Point	LT( void )										const	{ return Point( l, t ); }
	Point	RT( void ) 										const	{ return Point( r, t ); }
	Point	LB( void ) 										const	{ return Point( l, b ); }
	Point	RB( void ) 										const	{ return Point( r, b ); }

	bool	IsEmpty( void )									const	{ return ( l == r && t == b ); }
	bool	IsZero( void )									const	{ return l == 0.0f && r == 0.0f && t == 0.0f && b == 0.0f; }
	bool	IsReverse( void )								const	{ return ( l > r || t > b ); }
	bool	IsInside( const Point& pt )						const	{ return l <= pt.x && r >= pt.x && t <= pt.y && b >= pt.y; }
	bool	IsIntersect( const Rect& rc )					const	{ return l <= rc.r && r >= rc.l && t <= rc.b && b >= rc.t; }

	void	SetWidth( float w ) 									{ r = l + w; }
	void	SetHeight( float h )									{ b = t + h; }

	void	SetPos( float x, float y )								{ l = x; t = y; }
	void	SetSize( float w, float h )								{ r = l + w; b = t + h; }
	void	SetRB( float x, float y ) 								{ r = x; b = y; }
	void	Shift( float x, float y )								{ l += x; t += y; r += x; b += y; }
	void	Scale( float x, float y )								{ r = (r-l) * x + l; b = (b-t) * y + t; }
	void	Inflate( float x, float y )								{ r += x; b += y; }
	void	Widen( float rad )										{ l -= rad; t -= rad; r += rad; b += rad; }
	void	Widen( float x, float y )								{ l -= x; t -= x; r += y; b += y; }

	void	SetPos( const Point& pt )								{ SetPos( pt.x, pt.y ); }
	void	SetSize( const Point& pt )								{ SetSize( pt.x, pt.y ); }
	void	SetRB( const Point& pt ) 								{ SetRB( pt.x, pt.y ); }
	void	Shift( const Point& pt )								{ Shift( pt.x, pt.y ); } 
	void	Scale( const Point& pt )								{ Scale( pt.x, pt.y ); } 
	void	Inflate( const Point& pt )								{ Inflate( pt.x, pt.y ); }

	Point	Inside( const Point& pt )								{ return Point( Clamp(pt.x, l, r), Clamp(pt.y, t, b) ); }

	static Rect	Union( const Rect& a, const Rect& b )				{ Rect rc( Point::Min(a.LT(), b.LT()), Point::Max(a.RB(), b.RB()) ); return rc; }
	static Rect	Intersect( const Rect& a, const Rect& b );

public:
	float	l, t, r, b;
};


}