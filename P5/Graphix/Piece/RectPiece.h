#pragma once

#include "Piece.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	��`�s�[�X
//
///////////////////////////////////////////////////////////////////////////////
class RectPiece : public Piece
{
public:
	RectPiece( const Label& TexName, const Rect& Rc );

	virtual Rect			GetRect( int Index )		const	{ return m_Rect; }

private:
	Rect					m_Rect;
};


}