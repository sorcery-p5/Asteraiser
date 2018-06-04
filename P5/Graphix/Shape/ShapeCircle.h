#pragma once

#include "Shape.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	円シェイプ
//
///////////////////////////////////////////////////////////////////////////////
class ShapeCircle : public Shape
{
public:
	ShapeCircle( void );
	ShapeCircle( int Seg );
	virtual ~ShapeCircle();

	void				Draw( Point Pos, float Rad );

	void				SetColor( Color Col )			{ _SetColor( Col ); }

private:
	enum
	{
		SEGMENT_NUM	= 16,
	};
};


}