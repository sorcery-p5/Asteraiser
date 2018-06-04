#pragma once

#include "Sprite.h"


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	線形スプライト
//
///////////////////////////////////////////////////////////////////////////////
class LineSprite : public Sprite
{
public:
	LineSprite( int Num, int Divide );
	~LineSprite();
	
	void				SetPiece( const Piece* pPiece, const Texture* pTex );

	void				Draw( const Point aPt[] );
	void				Draw( const Point aPtA[], const Point aPtB[] );

	void				SetWidth( float Width )				{ m_Width = Width; }
	void				SetColor( Color Col );
	void				SetColor( int Index, Color Col );
	void				SetColor( const Color aCol[] );
	void				SetIndex( int PieceIndex );
	void				SetIndex( int Index, int PieceIndex );
	void				SetIndex( const int aIndex[] );
	
	int					GetNum( void )								const	{ return m_Num; }

protected:
	virtual void		_OnRender( void )							const;

private:
	void				_SetVertexUv( int Index, int PieceIndex );
	void				_SetVertexColor( int Index, Color Col );
	int					_GetVertexNum( void )						const	{ return (m_Num-1) * (4 + (m_Divide-1) * 2); }
	int					_GetIndexBase( int Index )					const	{ return (Index * (4 + (m_Divide-1) * 2) ); }

	Point				_CalcJointPoint( Point Pt, Point Prev, Point Next );
	Point				_CalcTipPoint( Point Pt, Point Next );
	void				_CalcDividePoint( void );
	void				_CalcDividePoint( int Base, int Prev1, int Prev0, int Next0, int Next1, float t );

private:
	SPRITE_VERTEX*		m_aVertex;
	int					m_Num;
	int					m_Divide;
	const Piece*		m_pPiece;
	float				m_Width;
};


}