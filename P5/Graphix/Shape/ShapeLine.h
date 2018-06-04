#pragma once

#include "Shape.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	直線シェイプ
//
///////////////////////////////////////////////////////////////////////////////
class ShapeLine : public Shape
{
public:
	ShapeLine( void );
	virtual ~ShapeLine();

	void				Draw( Point Start, Point End );
	void				Draw( const Point aPt[] );

	void				SetColor( Color Col )					{ _SetColor( Col ); }
	void				SetColor( int Index, Color Col );
	void				SetColor( const Color aCol[] );

private:
	enum
	{
		VERTEX_NUM	= 2,
	};
};


}