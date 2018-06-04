#pragma once

#include "Sprite.h"


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	矩形スプライト
//
///////////////////////////////////////////////////////////////////////////////
class RectSprite : public Sprite
{
public:
	RectSprite( void );
	
	void				SetPiece( const Piece* pPiece, const Texture* pTex );

	void				Draw( const Point& Center );
	void				Draw( const Matrix3& Mat );
	void				Draw( const Rect& Rc );
	void				Draw( const Point aPt[] );

	void				SetSize( const Point& Size )		{ m_Size = Size; }
	void				SetColor( Color Col );
	void				SetColor( const Color aCol[] );
	void				SetIndex( int Index );
	
	const Point&		GetSize( void )				const	{ return m_Size; }

protected:
	virtual void		_OnRender( void )			const;

private:
	enum
	{
		VERTEX_NUM	= 4,
	};

private:
	void				_ApplyVertexUv( void );

private:
	SPRITE_VERTEX		m_aVertex[VERTEX_NUM];
	
	const Piece*		m_pPiece;
	Point				m_Size;
	int					m_Index;
};


}