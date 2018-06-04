#pragma once

#include "Shape.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	太さ付き直線シェイプ
//
///////////////////////////////////////////////////////////////////////////////
class ShapeSolidLine : public Shape
{
public:
	ShapeSolidLine( void );
	virtual ~ShapeSolidLine();

	void				Draw( Point Start, Point End, float Rad );
	void				Draw( const Point aPt[], float Rad  );

	void				SetColor( Color Col )					{ _SetColor( Col ); }
	void				SetColor( int Index, Color Col );
	void				SetColor( const Color aCol[] );

private:
	enum
	{
		VERTEX_NUM	= 7,
	};

private:
	void				_SetVertex( Point Start, Point End, float Rad );
};


}