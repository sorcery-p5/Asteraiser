#include "P5.h"
#include "ShapePolygon.h"


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	ëΩäpå`ÉVÉFÉCÉv
//
///////////////////////////////////////////////////////////////////////////////
ShapePolygon::ShapePolygon( int Seg ) : Shape( Seg + 1 )
{
}
ShapePolygon::~ShapePolygon()
{
}

//******************************************************************************
//	ï`âÊ
//******************************************************************************
void ShapePolygon::Draw( const Point aPt[] )
{
	for( int i = 0; i < (m_Num - 1); i++ )
	{
		m_aVertex[i].x = aPt[i].x;
		m_aVertex[i].y = aPt[i].y;
	}

	m_aVertex[m_Num - 1] = m_aVertex[0];

	//	ìoò^
	DrawItem::Draw();
}

//******************************************************************************
//	êFê›íË
//******************************************************************************
void ShapePolygon::SetColor( int Index, Color Col )
{
	m_aVertex[Index].color = Col;

	if( Index == 0 )			m_aVertex[m_Num - 1].color = Col;
	if( Index == m_Num - 1 )	m_aVertex[0].color = Col;
}

//******************************************************************************
//	êFê›íË
//******************************************************************************
void ShapePolygon::SetColor( const Color aCol[] )
{
	for( int i = 0; i < (m_Num - 1); i++ )
	{
		m_aVertex[i].color = aCol[i];
	}
	m_aVertex[m_Num - 1].color = aCol[0];
}

}