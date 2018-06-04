#include "P5.h"
#include "ShapeLine.h"


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	�����V�F�C�v
//
///////////////////////////////////////////////////////////////////////////////
ShapeLine::ShapeLine( void ) : Shape( VERTEX_NUM )
{
}
ShapeLine::~ShapeLine()
{
}

//******************************************************************************
//	�`��
//******************************************************************************
void ShapeLine::Draw( Point Start, Point End )
{
	m_aVertex[0].x = Start.x;	m_aVertex[0].y = Start.y;
	m_aVertex[1].x = End.x;		m_aVertex[1].y = End.y;
	
	//	�o�^
	DrawItem::Draw();
}

//******************************************************************************
//	�`��
//******************************************************************************
void ShapeLine::Draw( const Point aPt[] )
{
	m_aVertex[0].x = aPt[0].x;	m_aVertex[0].y = aPt[0].y;
	m_aVertex[1].x = aPt[1].x;	m_aVertex[1].y = aPt[1].y;

	//	�o�^
	DrawItem::Draw();
}

//******************************************************************************
//	�F�ݒ�
//******************************************************************************
void ShapeLine::SetColor( int Index, Color Col )
{
	m_aVertex[Index].color = Col;
}

//******************************************************************************
//	�F�ݒ�
//******************************************************************************
void ShapeLine::SetColor( const Color aCol[] )
{
	for( int i = 0; i < m_Num; i++ )
	{
		m_aVertex[i].color = aCol[i];
	}
}

}