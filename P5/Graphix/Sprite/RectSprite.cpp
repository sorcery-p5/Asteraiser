#include "P5.h"
#include "RectSprite.h"

#include "Graphix/Texture/Texture.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	矩形スプライト
//
///////////////////////////////////////////////////////////////////////////////
RectSprite::RectSprite( void ) : Sprite()
{
	m_Index		= 0;
	m_pPiece	= NULL;

	ZeroArray( m_aVertex );
	
	for( int i = 0; i < VERTEX_NUM; i++ )
	{
		m_aVertex[i].rhw	= 1.0f;
		m_aVertex[i].color	= Color(255,255,255);
	}
}

//*****************************************************************************
//	ピース設定
//*****************************************************************************
void RectSprite::SetPiece( const Piece* pPiece, const Texture* pTex )
{
	//	サイズ抽出
	if( pPiece ) m_Size = pPiece->GetRect(m_Index).Size();

	//	テクスチャ設定
	m_pPiece = pPiece;
	SetTex( pTex );
	
	//	UV適用
	_ApplyVertexUv();
}

//*****************************************************************************
//	中央指定描画
//*****************************************************************************
void RectSprite::Draw( const Point& Center )
{
	const Point Half	= (m_Size * 0.5f).GetFloor();
	const Point LT		= Center - Half;
	const Point RB		= LT + m_Size;

	const Point aPt[] = 
	{
		Point( LT.x, LT.y ),	Point( RB.x, LT.y ),
		Point( LT.x, RB.y ),	Point( RB.x, RB.y ),
	};
	Draw( aPt );
}

//*****************************************************************************
//	マトリクス指定描画
//*****************************************************************************
void RectSprite::Draw( const Matrix3& Mat )
{
	Point aPt[] = 
	{
		Point( 0.0f, 0.0f ),		Point( m_Size.x, 0.0f ),
		Point( 0.0f, m_Size.y ),	m_Size,
	};

	//	マトリクス変換
	for( int i = 0; i < VERTEX_NUM; i++ )
	{
		aPt[i] = Mat.Transform( aPt[i] );
	}

	Draw( aPt );
}

//*****************************************************************************
//	矩形指定描画
//*****************************************************************************
void RectSprite::Draw( const Rect& Rc )
{
	Point aPt[] = { Rc.LT(), Rc.RT(), Rc.LB(), Rc.RB() };

	Draw( aPt );
}

//*****************************************************************************
//	四点指定描画
//*****************************************************************************
void RectSprite::Draw( const Point aPt[] )
{
	//	頂点の設定
	for( int i = 0; i < VERTEX_NUM; i++ )
	{
		m_aVertex[i].x = Round( aPt[i].x ) + 0.1f;
		m_aVertex[i].y = Round( aPt[i].y ) + 0.1f;
	}
	
	//	登録
	DrawItem::Draw();
}

//*****************************************************************************
//	色の設定
//*****************************************************************************
void RectSprite::SetColor( Color Col )
{
	for( int i = 0; i < VERTEX_NUM; i++ )
	{
		m_aVertex[i].color = Col;
	}
}

//*****************************************************************************
//	色の設定（４点）
//*****************************************************************************
void RectSprite::SetColor( const Color aCol[] )
{
	for( int i = 0; i < VERTEX_NUM; i++ )
	{
		m_aVertex[i].color = aCol[i];
	}
}

//******************************************************************************
//	インデクスの設定
//******************************************************************************
void RectSprite::SetIndex( int Index )
{
	if( m_Index != Index )
	{
		m_Index = Index;

		//	サイズ変更
		if( m_pPiece ) m_Size = m_pPiece->GetRect(m_Index).Size();

		//	頂点の反映
		_ApplyVertexUv();
	}
}

//------------------------------------------------------------------------------
//	描画実行
//------------------------------------------------------------------------------
void RectSprite::_OnRender( void ) const
{
	_RenderVertex( m_aVertex, VERTEX_NUM );
}

//-----------------------------------------------------------------------------
//	頂点にUV値を反映
//-----------------------------------------------------------------------------
void RectSprite::_ApplyVertexUv( void )
{
	if( !m_pPiece ) return;

	Sprite::_ApplyVertexUv( m_aVertex, m_pPiece->GetRect( m_Index ) );
}

}