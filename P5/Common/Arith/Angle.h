#pragma once


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	Šp“x
//
///////////////////////////////////////////////////////////////////////////////
class Angle
{
public:
	Angle( void )									{ SetRad( 0.0f );	}
	Angle( float rad )								{ SetRad( rad );	}
	Angle( const Angle& a )							{ angle = a.angle;	}

	Angle& operator = ( float rad )					{ SetRad( rad );	return *this; }
	Angle& operator = ( const Angle& a )			{ angle = a.angle;	return *this; }

	float operator -  ( void ) const				{ return -angle;	}

	Angle operator +( float val ) 			const	{ Angle result( angle ); result += val; return result;	}
	Angle operator -( float val ) 			const	{ Angle result( angle ); result -= val; return result;	}
	Angle operator *( float val ) 			const	{ Angle result( angle ); result *= val; return result;	}
	Angle operator /( float val ) 			const	{ Angle result( angle ); result /= val; return result;	}

	Angle& operator +=( float val )					{ SetRad( angle + val ); return *this; }
	Angle& operator -=( float val )					{ SetRad( angle - val ); return *this; }
	Angle& operator *=( float val )					{ SetRad( angle * val ); return *this; }
	Angle& operator /=( float val )					{ SetRad( angle / val ); return *this; }

	bool operator ==( float val ) 			const	{ return ( angle == val );	}
	bool operator !=( float val ) 			const	{ return ( angle != val );	}

	bool operator < ( float val ) 			const	{ return ( angle <  val );	}
	bool operator > ( float val ) 			const	{ return ( angle >  val );	}
	bool operator <=( float val ) 			const	{ return ( angle <= val );	}
	bool operator >=( float val ) 			const	{ return ( angle >= val );	}

	void SetRad( float rad )						{ angle = Loop( rad );			}
	void SetDeg( float deg )						{ SetRad( ToRad( deg ) );		}
	void SetWord( word wod )						{ SetRad( WordToRad( wod ) );	}

	operator float( void )					const	{ return angle;	}
	operator word( void )					const	{ return Word(); }

	float	Rad( void ) 					const	{ return angle;				}
	float	Deg( void ) 					const	{ return ToDeg( angle );	}
	word	Word( void )					const	{ return RadToWord( angle );}
	Point	GetDir( void )					const	{ return Point( Sin(angle), Cos(angle) ); }

	void			NearTo( Angle aim, float step )	{ *this = NearTo( angle, aim, step ); }

public:
	static float 	ToDeg( float rad )				{ return rad * DIV_180_PI; }
	static float 	ToRad( float deg ) 				{ return deg * DIV_PI_180; }
	static word  	RadToWord( float rad )			{ if(rad < 0.0f) rad += PI2; return (word)(rad * DIV_WORD_PI2); }
	static float 	WordToRad( word wod )			{ return wod * DIV_PI2_WORD; }

	static float	Loop( float in );
	static float	Diff( Angle a, Angle b );
	static Angle	NearTo( Angle a, Angle aim, float step );
	static Angle	Lerp( Angle a1, Angle a2, float t );
	static Angle	CatmullRom( Angle pre, Angle a1, Angle a2, Angle next, float t );

public:
	float	angle;
};

}