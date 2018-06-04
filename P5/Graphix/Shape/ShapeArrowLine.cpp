#include "P5.h"
#include "ShapeArrowLine.h"


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	ñÓàÛíºê¸ÉVÉFÉCÉv
//
///////////////////////////////////////////////////////////////////////////////
ShapeArrowLine::ShapeArrowLine( void ) : Shape( VERTEX_NUM )
{
}
ShapeArrowLine::~ShapeArrowLine()
{
}

//******************************************************************************
//	ï`âÊ
//******************************************************************************
void ShapeArrowLine::Draw( Point Start, Point End )
{
	_SetVertex( Start, End );

	//	ìoò^
	DrawItem::Draw();
}

//******************************************************************************
//	ï`âÊ
//******************************************************************************
void ShapeArrowLine::Draw( const Point aPt[] )
{
	_SetVertex( aPt[0], aPt[1] );

	//	ìoò^
	DrawItem::Draw();
}

//------------------------------------------------------------------------------
//	í∏ì_à íuÇÃçXêV
//------------------------------------------------------------------------------
void ShapeArrowLine::_SetVertex( Point Start, Point End )
{
	Point Dir = (End - Start) / 10.0f;
	Point Side = Dir.Rot90();

	Point aPt[] =
	{
		Start,
		Start + Dir + Side,
		End,
		Start + Dir - Side,
	};

	for( int i = 0; i < (VERTEX_NUM - 1); i++ )
	{
		m_aVertex[i].x = aPt[i].x;
		m_aVertex[i].y = aPt[i].y;
	}

	m_aVertex[4] = m_aVertex[0];
}


}