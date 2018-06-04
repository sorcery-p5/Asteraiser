#include "P5.h"

#include <limits.h>


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	�O�p�֐��e�[�u��
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
//	�Z�p
//
///////////////////////////////////////////////////////////////////////////////
static TriTable s_TriTable;
//*****************************************************************************
//	sin�̎擾
//*****************************************************************************
float	Sin( float angle )
{
	return s_TriTable.Sin( angle );
}

//*****************************************************************************
//	cos�̎擾
//*****************************************************************************
float	Cos( float angle )
{
	return s_TriTable.Cos( angle );
}

//*****************************************************************************
//	atan�̎擾
//*****************************************************************************
float	Atan( float x, float y )
{
	return s_TriTable.Atan( x, y );
}

//*****************************************************************************
//	atan�̎擾
//*****************************************************************************
float	Atan( const Point& pt )
{
	return s_TriTable.Atan( pt.x, pt.y );
}

//*****************************************************************************
//	�������̎擾
//*****************************************************************************
float	Sqrt( float c )
{
	static const int LOOP_MAX = 10;
	float	n;
	
	//	�������ɍł��߂��傫�����̐�����T��
	for( n = 1.0f ; n * n < c; n += 1.0f );

	for( int i = 0; i < LOOP_MAX; i++ )
	{
		if(n == 0) return 0.0f;

		n = (n + (c / n)) / 2.0f;	//	��_��x ���W�����߂�
	}

	return n;	//	���߂��ߎ��l��Ԃ�
}

///////////////////////////////////////////////////////////////////////////////
//
//	�O�p�֐��e�[�u��
//
///////////////////////////////////////////////////////////////////////////////
//*****************************************************************************
//	������
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
//	sin�̎擾
//*****************************************************************************
float	TriTable::Sin( const Angle& angle ) const
{
	return m_SinTable[ angle.Word() % TABLE_SIZE ];
}

//*****************************************************************************
//	cos�̎擾
//*****************************************************************************
float	TriTable::Cos( const Angle& angle ) const
{
	return m_SinTable[ (angle + PI05).Word() % TABLE_SIZE ];
}

//*****************************************************************************
//	atan�̎擾
//*****************************************************************************
Angle	TriTable::Atan( float x, float y ) const
{
	if( x == 0.0f && y == 0.0f )
	{
		return Angle( 0.0f );
	}

	bool rev = false;
	float ax = Abs(x), ay = Abs(y);

	//	�c��艡���傫����Ό������A�t���O���ĂĂ���
	if( ax < ay )
	{
		float w = ax;
		ax = ay;
		ay = w;
		rev = true;
	}
	
	//	�p�x�����o��
	float angle = m_AtanTable[ (int)(ay / ax * (TABLE_SIZE-1)) ];

	//	�������Ă����Ƃ��́A45�x���Ŕ��]
	if( rev )
	{
		angle = PI05 - angle;
	}

	if( x < 0.0f && y < 0.0f )
	{
		//	x�A�������ɕ��̎��A180�x��]
		return -Angle( angle + PI );
	}
	else if( x < 0.0f )
	{
		//	x�����̎��AY���Ŕ��]
		return -Angle( -angle );
	}
	else if( y < 0.0f )
	{
		//	�������̎��AX���Ŕ��]
		return -Angle( PI - angle );
	}
	else
	{
		return -Angle( angle );
	}
}

}