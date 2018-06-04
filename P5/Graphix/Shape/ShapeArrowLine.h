#pragma once

#include "Shape.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	矢印直線シェイプ
//
///////////////////////////////////////////////////////////////////////////////
class ShapeArrowLine : public Shape
{
public:
	ShapeArrowLine( void );
	virtual ~ShapeArrowLine();

	void				Draw( Point Start, Point End );
	void				Draw( const Point aPt[] );

	void				SetColor( Color Col )					{ _SetColor( Col ); }

private:
	enum
	{
		VERTEX_NUM	= 5,
	};

private:
	void				_SetVertex( Point Start, Point End );
};


}