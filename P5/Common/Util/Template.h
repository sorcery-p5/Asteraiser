#pragma once


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	テンプレート
//
///////////////////////////////////////////////////////////////////////////////

template<typename T> void 	SafeDelete( T* &p )							{ delete p; p = NULL; }
template<typename T> void 	SafeDeleteA( T* &p )						{ delete[] p; p = NULL; }
template<typename T> void 	SafeRelease( T* &p )						{ if(p){ p->Release(); p = NULL; } }
#define						SafePtr( p )								if(p)(p)
template<typename T> void 	DeleteVec( T& a );
template<typename T> void 	DeleteMap( T& a );
template<typename T> void 	ReleaseVec( T& a );
template<typename T> void 	ReleaseMap( T& a );

template<typename T> void 	ZeroStruct( T& t )							{ ::ZeroMemory( &t, sizeof(T) ); }
#define						ZeroArray( a )								{ ::ZeroMemory( a,  sizeof(a) ); }
template<typename T> void 	ZeroStructArray( T* a, int n )				{ ::ZeroMemory( a,  sizeof(T) * n ); }
template<typename T> void 	CopyStruct( T& d, const T& s )				{ ::CopyMemory( &d, &s, sizeof(T) ); }
#define						CopyArray( d, s )							{ ::CopyMemory( d, s, sizeof(d) ); }
template<typename T> void 	CopyArrayNum( T* d, const T* s, int n )		{ ::CopyMemory( d, s, sizeof(T) * n ); }
#define						ArrayNum( a )								( sizeof(a) / sizeof(a[0]) )
template<typename T> void 	FillArray( T* a, int n, const T& d )		{ for(int i=0; i<n; i++) a[i] = d; }

#define						StdFind( vec, target )						( std::find( vec.begin(), vec.end(), target ) )
#define						StdFindIf( vec, comp )						( std::find_if( vec.begin(), vec.end(), comp ) )
#define						StdSort( vec )								( std::sort( vec.begin(), vec.end() ) )
#define						StdSortPred( vec, comp )					( std::sort( vec.begin(), vec.end(), comp ) )
#define						StdStableSort( vec )						( std::stable_sort( vec.begin(), vec.end() ) )
#define						StdStableSortPred( vec, comp )				( std::stable_sort( vec.begin(), vec.end(), comp ) )

template<typename T> T		Abs( const T& val )							{ return ( val < ((T)0) )? -val : val; }
template<typename T> T		Min( const T& a, const T& b )				{ return ( a < b )? a : b; }
template<typename T> T		Max( const T& a, const T& b )				{ return ( a > b )? a : b; }
template<typename T> T		Min( const T& a, const T& b, const T& c )	{ return ( a < b )? Min(a,c) : Min(b,c); }
template<typename T> T		Max( const T& a, const T& b, const T& c )	{ return ( a > b )? Max(a,c) : Max(b,c); }
template<typename T> T		Sq( const T& a )							{ return a * a; }
template<typename T> void	Swap( T& a, T& b )							{ T w = a; a = b; b = w; }
inline float				Coef( float n, float f1, float f2 )			{ return (f2 == f1)? 0.0f : (n - f1) / (f2 - f1); }
inline float				Round( float r )							{ return floorf(r + 0.5f); }
inline float				Modf( float r )								{ float i; return modff( r, &i ); }
inline float				EpsilonCut( float r )						{ return ( Abs(r) < EPS )? 0.0f : r; }

template<typename T> void	Increase( T& val, const T& aim, const T& step )			{ val = Min(val + step, aim); }
template<typename T> void	Decrease( T& val, const T& aim, const T& step )			{ val = Max(val - step, aim); }
template<typename T> void	DecreaseZero( T& val, const T& step )					{ val = Max(val - step, (T)0); }
template<typename T> T		NearTo( const T& val, const T& aim, const T& step );
template<typename T> T		Clamp( const T& val, const T& min, const T& max );
template<typename T> T		Loop( const T& val, const T& min, const T& max );
template<typename T> T		Lerp( const T& v1, const T& v2, float t );
template<typename T> T		CatmullRom( const T& pre, const T& v1, const T& v2, const T& next, float t );


//*****************************************************************************
//	ベクタ・リストの削除
//*****************************************************************************
template<typename T>
void DeleteVec( T& a )
{
	for( T::iterator it = a.begin(); it != a.end(); ++it )
	{
		SafeDelete( *it );
	}
	a.clear();
}

//*****************************************************************************
//	マップの削除
//*****************************************************************************
template<typename T>
void DeleteMap( T& a )
{
	for( T::iterator it = a.begin(); it != a.end(); ++it )
	{
		SafeDelete( it->second );
	}
	a.clear();
}

//*****************************************************************************
//	ベクタ・リストの解放
//*****************************************************************************
template<typename T>
void ReleaseVec( T& a )
{
	for( T::iterator it = a.begin(); it != a.end(); ++it )
	{
		SafeRelease( *it );
	}
	a.clear();
}

//*****************************************************************************
//	マップの解放
//*****************************************************************************
template<typename T>
void ReleaseMap( T& a )
{
	for( T::iterator it = a.begin(); it != a.end(); ++it )
	{
		SafeRelease( it->second );
	}
	a.clear();
}

//*****************************************************************************
//	目標値に近づけた値を取得
//*****************************************************************************
template<typename T>
T NearTo( const T& val, const T& aim, const T& step )
{
	if( val < aim ){	return (aim - val < step)? aim : val + step; }
	else{				return (val - aim < step)? aim : val - step; }
}

//*****************************************************************************
//	値を範囲の中に納める
//*****************************************************************************
template<typename T>
T Clamp( const T& val, const T& min, const T& max )
{
	if( val < min ) return min;
	if( val > max ) return max;
	return val;
}

//*****************************************************************************
//	値を範囲内でループ
//*****************************************************************************
template<typename T> 
T Loop( const T& val, const T& min, const T& max )
{
	T range = max - min;
	if( val < min )  return val + range;
	if( val >= max ) return val - range;
	return val;
}

//*****************************************************************************
//	値の線形補間
//*****************************************************************************
template<typename T>
T Lerp( const T& v1, const T& v2, float t )
{
	return	v1 + (T)( (v2 - v1) * t );	
}

//*****************************************************************************
//	値のキャットマルロム補間
//*****************************************************************************
template<typename T>
T CatmullRom( const T& pre, const T& v1, const T& v2, const T& next, float t )
{
	float t2 = t * t;
	float t3 = t2 * t;

	float B0 = (    -t3 + 2 * t2 - t ) * 0.5f;
	float B1 = ( 3 * t3 - 5 * t2 + 2 ) * 0.5f;
	float B2 = (-3 * t3 + 4 * t2 + t ) * 0.5f;
	float B3 = (     t3	-     t2     ) * 0.5f;
	return pre * B0 + v1 * B1 + v2 * B2 + next * B3;
}

}