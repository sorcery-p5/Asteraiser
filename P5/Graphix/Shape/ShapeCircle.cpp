#include "P5.h"
#include "ShapeCircle.h"


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	�~�V�F�C�v
//
///////////////////////////////////////////////////////////////////////////////
ShapeCircle::ShapeCircle( void ) : Shape( SEGMENT_NUM + 1 )
{
}
ShapeCircle::ShapeCircle( int Seg ) : Shape( Seg + 1 )
{
}
ShapeCircle::~ShapeCircle()
{
}

//******************************************************************************
//	�`��
//******************************************************************************
void ShapeCircle::Draw( Point Pos, float Rad )
{
	for( int i = 0; i < m_Num; i++ )
	{
		Point Pt = (Angle(((float)i / (float)(m_Num-1)) * PI2).GetDir() * Rad) + Pos;
		m_aVertex[i].x = Pt.x;	m_aVertex[i].y = Pt.y;
	}

	//	�o�^
	DrawItem::Draw();
}

}