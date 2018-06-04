#pragma once


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	êF
//
///////////////////////////////////////////////////////////////////////////////
class Color
{
public:
	Color( void )										{ Set( 0, 0, 0 );		}
	Color( dword c )									{ Set( c );				}
	Color( int _r, int _g, int _b )						{ Set( _r, _g, _b );	}
	Color( int _r, int _g, int _b, int _a )				{ Set( _r, _g, _b, _a );}
	Color( const byte* p )								{ Set( p[0], p[1], p[2], p[3] );}
	Color( const int* p )								{ Set( p[0], p[1], p[2], p[3] );}

	Color& operator =( dword c )						{ Set( c );		return *this; }
	Color& operator =( const Color& c )					{ Set( c.dw );	return *this; }

	Color operator +( const Color& c ) 			const	{ Color result(dw); result += c; return result; }
	Color operator -( const Color& c ) 			const	{ Color result(dw); result -= c; return result; }
	Color operator *( const Color& c ) 			const	{ Color result(dw); result *= c; return result; }

	Color& operator +=( const Color& c )				{ Set( r + c.r, g + c.g, b + c.b, a + c.a ); return *this; }
	Color& operator -=( const Color& c )				{ Set( r - c.r, g - c.g, b - c.b, a - c.a ); return *this; }
	Color& operator *=( const Color& c )				{ Set( (r * c.r) / 255, (g * c.g) / 255, (b * c.b) / 255, (a * c.a) / 255 ); return *this; }

	bool operator ==( const Color& c ) 			const	{ return (dw == c.dw);}
	bool operator !=( const Color& c ) 			const	{ return (dw != c.dw);}
	
	void 		Set( dword c )							{ dw = c;	}
	void 		Set( int _r, int _g, int _b )			{ Set( _r, _g, _b, 255 ); }
	void 		Set( int _r, int _g, int _b, int _a )	{ r = Clamp(_r); g = Clamp(_g); b = Clamp(_b); a = Clamp(_a); }
	void		Set( const byte* p )					{ Set( p[0], p[1], p[2], p[3] );}
	void		Set( const int* p )						{ Set( p[0], p[1], p[2], p[3] );}

	void 		SetAlpha( int _a )						{ a = Clamp(_a); }
	void 		SetGray( int l )						{ SetGray( l, 255 ); }
	void 		SetGray( int l, int _a )				{ l = Clamp(l); r = l; g = l; b = l; a = Clamp(_a);	}
	void		SetZero( void )							{ dw = 0; }
	void		SetBlack( void )						{ Set( 0, 0, 0 ); }
	void		SetWhite( void )						{ Set( 255, 255, 255 ); }
	
	operator dword( void )						const	{ return dw; }

	float 		Rf( void )						const	{ return (float)r / 255.0f; }
	float 		Gf( void )						const	{ return (float)g / 255.0f; }
	float 		Bf( void )						const	{ return (float)b / 255.0f; }
	float 		Af( void )						const	{ return (float)a / 255.0f; }
	bool		IsOpaque( void )				const	{ return a == 255; }
	bool		IsZero( void )					const	{ return dw == 0; }

	void		GetArray( byte* p )				const	{ p[0] = r; p[1] = g; p[2] = b; p[3] = a; }

	static Color	Zero( void )						{ return Color( (dword)0 ); }
	static Color	Black( void )						{ return Color( 0,0,0 ); }
	static Color	White( void )						{ return Color( 255,255,255 ); }
	static Color	Gray( int l )						{ return Color( l, l, l, 255 ); }
	static Color	Gray( int l, int _a )				{ return Color( l, l, l, _a ); }
	
public:
	static int		Clamp( int in )								{ return P5::Clamp( in, 0, 255 ); }
	static Color	NearTo( Color a, Color aim, Color step );
	static Color	Lerp( Color c1, Color c2, float t );

public:
	union
	{
		struct
		{
			byte	b;
			byte	g;
			byte	r;
			byte	a;
		};
		DWORD	dw;
		byte	array[4];
	};
};

}