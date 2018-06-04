#pragma once

namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	óêêîäÓíÍ
//
////////////////////////////////////////////////////////////////////////////////
class Rand
{
public:
	virtual ~Rand()											{}
	
public:
	virtual void	Reset( void )							= 0;
	virtual void	Reset( dword Seed )						= 0;

	dword			GetRand( void )							{ return _ExtRand(); }
	int				GetRand( int Max )						{ return GetRand( 0, Max ); }
	int				GetRand( int Min, int Max )				{ uint R = Max - Min + 1; return R? (GetRand() % R + Min) : Min; }
	float			GetRandFloat( float Max )				{ return GetRandFloat( 0, Max ); }
	float			GetRandFloat( float Min, float Max )	{ float R = (Max - Min); return R != 0.0f? GetRandFloat() * R + Min : Min; }
	int				GetRandRange( int Val, int Range )		{ return GetRand(Val - Range, Val + Range ); }
	float			GetRandRange( float Val, float Range )	{ return GetRandFloat( Val - Range, Val + Range ); }

	float			GetRandFloat( void )
	{
		union
		{
			uint		u32;
			float		f32;
		} cx;

		uint	 r	= GetRand() >> 8UL;
		cx.u32	= ONE_F32 + ( r >> 1UL ) + ( r & 1UL );
		return cx.f32 - 1.0f;
	}

protected:
	virtual dword	_ExtRand( void )		= 0;

private:
	static const uint		ONE_F32 = 0x3f800000UL;
};


////////////////////////////////////////////////////////////////////////////////
//
//	XORóêêî
//
////////////////////////////////////////////////////////////////////////////////
class XORRand : public Rand
{
public:
	XORRand( void )										{ Reset(); }
	XORRand( dword Seed )								{ Reset( Seed ); }

	virtual void			Reset( void );
	virtual void			Reset( dword Seed );

private:
	void					_SetSeed( dword s );
	virtual dword			_ExtRand( void );

private:
	static const dword		DEFAULT_X = 123456789;
	static const dword		DEFAULT_Y = 362436069;
	static const dword		DEFAULT_Z = 521288629;
	static const dword		DEFAULT_W = 88675123;

private:
	dword					x;
	dword 					y;
	dword 					z;
	dword 					w;
};


}