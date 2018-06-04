#pragma once


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	3x3çsóÒ
//
///////////////////////////////////////////////////////////////////////////////
class Matrix3
{
public:
	Matrix3( void )											{ Set( ident );	}
	Matrix3( const float* p )								{ Set( p );		}
	Matrix3( const Matrix3& m )								{ Set( m );		}
	Matrix3( float _11, float _12, float _13, 
			 float _21, float _22, float _23,
			 float _31, float _32, float _33 )
	{
		m[0][0] = _11;	m[0][1] = _12;	m[0][2] = _13;
		m[1][0] = _21;	m[1][1] = _22;	m[1][2] = _23;
		m[2][0] = _31;	m[2][1] = _32;	m[2][2] = _33;
	}

	void Inverse( void );
	void Multiply( const Matrix3& mat );

	Matrix3& operator =( const float* p )					{ Set( p );	return *this; }
	Matrix3& operator =( const Matrix3& m )					{ Set( m );	return *this; }

	Matrix3 operator *( const Matrix3& m )			const	{ Matrix3 result(a); result *= m; return result; }

	Matrix3& operator *=( const Matrix3& m )				{ Multiply( m ); return *this; }

	void Set( const float* p )								{ CopyArray( a, p );  }
	void Set( const Matrix3& m )							{ Set( m.a );	}
	void SetPos( const Point& pt )							{ m[2][0] = pt.x;	m[2][1] = pt.y; }
	void SetPos( float x, float y )							{ m[2][0] = x;		m[2][1] = y; }
	void SetScale( const Point& pt )						{ m[0][0] = pt.x;	m[1][1] = pt.y; }
	void SetScale( float x, float y )						{ m[0][0] = x;		m[1][1] = y; }
	void SetRotX( float a );
	void SetRotY( float a );
	void SetRotZ( float a );
	void SetFromTo( Point a, Point b );

	operator float*( void )									{ return a; }
	operator const float*( void )					const	{ return a; }

	Point GetPos( void )							const	{ return Point(m[2][0], m[2][1]); }
	Point GetScale( void )							const	{ return Point(m[0][0], m[1][1]); }
	Point GetAxisX( void )							const	{ return Point(m[0][0], m[0][1]); }
	Point GetAxisY( void )							const	{ return Point(m[1][0], m[1][1]); }
	float GetRotZ( void )							const	{ return Atan( m[1][0], m[1][1] ); }
	bool  GetInverse( Matrix3& Mat )				const;

	Point Transform( const Point& pt )				const;
	Point TransformNormal( const Point& pt )		const;

	static const Matrix3& Identity( void )					{ return ident; }

	static Matrix3 Trans( const Point& pt )					{ Matrix3 mat; mat.SetPos(pt);		return mat; }
	static Matrix3 Trans( float x, float y )				{ Matrix3 mat; mat.SetPos(x,y);		return mat; }
	static Matrix3 Scale( const Point& pt )					{ Matrix3 mat; mat.SetScale(pt);	return mat; }
	static Matrix3 Scale( float x, float y )				{ Matrix3 mat; mat.SetScale(x,y);	return mat; }
	static Matrix3 RotX( float a )							{ Matrix3 mat; mat.SetRotX(a);		return mat; }
	static Matrix3 RotY( float a )							{ Matrix3 mat; mat.SetRotY(a);		return mat; }
	static Matrix3 RotZ( float a )							{ Matrix3 mat; mat.SetRotZ(a);		return mat; }
	static Matrix3 FromTo( Point a, Point b )				{ Matrix3 mat; mat.SetFromTo(a,b);	return mat; }
	static Matrix3 TransRotZ( const Point& pt, float a )	{ Matrix3 mat; mat.SetRotZ(a); mat.SetPos(pt); return mat; }

public:
	union
	{
		float m[3][3];
		float a[9];
	};

private:
	static const Matrix3 ident;
};


}