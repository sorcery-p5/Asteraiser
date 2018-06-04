#include "P5.h"
#include "Sprite.h"

#include "Graphix/DrawDevice.h"
#include "Graphix/Texture/Texture.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	スプライト基底
//
///////////////////////////////////////////////////////////////////////////////
Sprite::Sprite( void ) : DrawItem()
{
	m_pTex		= NULL;
}
Sprite::~Sprite()
{
}

//-----------------------------------------------------------------------------
//	頂点列を描画する
//-----------------------------------------------------------------------------
void Sprite::_RenderVertex( const SPRITE_VERTEX* pVertex, int Num ) const
{
	DrawDevice* pDevice = GetGraphix()->GetDevice();

	//	テクスチャ登録
	pDevice->SetTexture( m_pTex );

	//	描画
	pDevice->GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, Num-2, pVertex, sizeof(SPRITE_VERTEX) );
}

//-----------------------------------------------------------------------------
//	頂点にUV値を反映
//-----------------------------------------------------------------------------
void Sprite::_ApplyVertexUv( SPRITE_VERTEX aVertex[], const Rect& Rc ) const
{
	if( !m_pTex ) return;

	//	UV算出
	const Point&	TexSize	= m_pTex->GetSize();
	float			l 		= Rc.l	/ TexSize.x;
	float			t 		= Rc.t	/ TexSize.y;
	float			r 		= Rc.r	/ TexSize.x;
	float			b 		= Rc.b	/ TexSize.y;

	//	頂点に設定
	aVertex[0].u	= l;	aVertex[0].v	= t;
	aVertex[1].u	= r;	aVertex[1].v	= t;
	aVertex[2].u	= l;	aVertex[2].v	= b;
	aVertex[3].u	= r;	aVertex[3].v	= b;
}

}