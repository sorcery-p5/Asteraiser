#pragma once

#include "DrawItem/DrawItem.h"
#include "Graphix/Piece/Piece.h"

namespace P5
{

class Texture;

//-----------------------------------------------------------------------------
//	�X�v���C�g�p���_
//-----------------------------------------------------------------------------
struct SPRITE_VERTEX
{
	float	x, y, z;
	float	rhw;
	dword	color;
	float	u, v;
};


///////////////////////////////////////////////////////////////////////////////
//
//	�X�v���C�g���
//
///////////////////////////////////////////////////////////////////////////////
class Sprite : public DrawItem
{
public:
	Sprite( void );
	virtual ~Sprite();

	void			SetTex( const Texture* pTex )					{ m_pTex = pTex; }

protected:
	void			_RenderVertex( const SPRITE_VERTEX* pVertex, int Num )		const;
	void			_ApplyVertexUv( SPRITE_VERTEX aVertex[], const Rect& Rc )	const;

protected:
	const Texture*	m_pTex;
};


}