#include "P5.h"
#include "ShapeSolidLine.h"


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	ëæÇ≥ïtÇ´íºê¸ÉVÉFÉCÉv
//
///////////////////////////////////////////////////////////////////////////////
ShapeSolidLine::ShapeSolidLine( void ) : Shape( VERTEX_NUM )
{
}
ShapeSolidLine::~ShapeSolidLine()
{
}

//******************************************************************************
//	ï`âÊ
//******************************************************************************
void ShapeSolidLine::Draw( Point Start, Point End, float Rad )
{
	_SetVertex( Start, End, Rad );

	//	ìoò^
	DrawItem::Draw();
}

//******************************************************************************
//	ï`âÊ
//******************************************************************************
void ShapeSolidLine::Draw( const Point aPt[], float Rad  )
{
	_SetVertex( aPt[0], aPt[1], Rad );

	//	ìoò^
	DrawItem::Draw();
}

//******************************************************************************
//	êFê›íË
//******************************************************************************
void ShapeSolidLine::SetColor( int Index, Color Col )
{
	if( Index == 0 )
	{
		m_aVertex[0].color = Col;
		m_aVertex[1].color = Col;
		m_aVertex[2].color = Col;
		m_aVertex[6].color = Col;
	}
	else
	{
		m_aVertex[3].color = Col;
		m_aVertex[4].color = Col;
		m_aVertex[5].color = Col;
	}
}

//******************************************************************************
//	êFê›íË
//******************************************************************************
void ShapeSolidLine::SetColor( const Color aCol[] )
{
	m_aVertex[0].color = aCol[0];
	m_aVertex[1].color = aCol[0];
	m_aVertex[2].color = aCol[0];
	m_aVertex[6].color = aCol[0];

	m_aVertex[3].color = aCol[1];
	m_aVertex[4].color = aCol[1];
	m_aVertex[5].color = aCol[1];
}

//------------------------------------------------------------------------------
//	í∏ì_à íuÇÃçXêV
//------------------------------------------------------------------------------
void ShapeSolidLine::_SetVertex( Point Start, Point End, float Rad )
{
	Point Dir = (End - Start).GetNormal() * Rad;
	Point Side = Dir.Rot90();

	Point aPt[] =
	{
		Start + Side,	Start - Dir,	Start - Side,
		End - Side,		End + Dir,		End + Side,		
	};

	for( int i = 0; i < (VERTEX_NUM - 1); i++ )
	{
		m_aVertex[i].x = aPt[i].x;
		m_aVertex[i].y = aPt[i].y;
	}

	m_aVertex[6] = m_aVertex[0];
}


}