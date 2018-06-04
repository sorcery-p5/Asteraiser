#pragma once

#include "Shape.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	多角形シェイプ
//
///////////////////////////////////////////////////////////////////////////////
class ShapePolygon : public Shape
{
public:
	ShapePolygon( int Seg );
	virtual ~ShapePolygon();

	void				Draw( const Point aPt[] );

	void				SetColor( Color Col )					{ _SetColor( Col ); }
	void				SetColor( int Index, Color Col );
	void				SetColor( const Color aCol[] );
};

}