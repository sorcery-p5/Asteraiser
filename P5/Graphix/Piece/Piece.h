#pragma once


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	ÉsÅ[ÉXäÓíÍ
//
///////////////////////////////////////////////////////////////////////////////
class Piece
{
public:
	virtual ~Piece()											{}

	const Label&			GetTexName( void )			const	{ return m_TexName; }

	virtual Rect			GetRect( int Index )		const	= 0;

protected:	
	Label					m_TexName;
};

typedef std::map< Label, Piece* > PieceMap;

}