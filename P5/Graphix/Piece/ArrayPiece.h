#pragma once

#include "Piece.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	配列ピース
//
///////////////////////////////////////////////////////////////////////////////
class ArrayPiece : public Piece
{
public:
	ArrayPiece( const Label& TexName, const Rect* aRect, int RectNum );

	void					AddRect( const Rect& Rc )			{ m_vRect.push_back(Rc); }

	virtual Rect			GetRect( int Index )		const;

private:
	typedef std::vector<Rect> VecRect;

private:
	VecRect					m_vRect;
};



}