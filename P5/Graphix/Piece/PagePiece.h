#pragma once

#include "Piece.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	ページ設定ピース
//
///////////////////////////////////////////////////////////////////////////////
class PagePiece : public Piece
{
public:
	PagePiece( const Label& TexName, const Rect& Rc, const Point& Stride, int LineNum, int IndexMax );

	virtual Rect			GetRect( int Index )		const;

private:
	Rect					m_Rect;
	Point					m_Stride;
	int						m_LineNum;
	int						m_IndexMax;
};


}