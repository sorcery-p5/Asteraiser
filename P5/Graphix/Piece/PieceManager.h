#pragma once

#include "Piece.h"

namespace P5
{

class Texture;


///////////////////////////////////////////////////////////////////////////////
//
//	ÉsÅ[ÉXä«óù
//
///////////////////////////////////////////////////////////////////////////////
class PieceManager
{
public:
	void			AddPiece( const Piece* pPic, const Label& Name );
	const Piece*	GetPiece( const Label& Name )					const;

private:
	typedef std::map<Label, const Piece*> PieceMap;

private:
	PieceMap		m_PieceMap;
};


}