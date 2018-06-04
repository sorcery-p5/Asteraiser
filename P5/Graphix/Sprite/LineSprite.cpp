#include "P5.h"
#include "LineSprite.h"

#include "Graphix/Texture/Texture.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	線形スプライト
//
///////////////////////////////////////////////////////////////////////////////
LineSprite::LineSprite( int Num, int Divide ) : Sprite()
{
	_ASSERT( Num > 2 );

	m_Num		= Num;
	m_Divide	= Max( Divide, 1 );
	m_Width		= 1.0f;
	m_pPiece	= NULL;

	int VertexNum = _GetVertexNum();

	m_aVertex = new SPRITE_VERTEX[ VertexNum ];
	MEM_CHECK( m_aVertex );
	ZeroStructArray( m_aVertex, VertexNum );
	
	for( int i = 0; i < VertexNum; i++ )
	{
		m_aVertex[i].rhw	= 1.0f;
		m_aVertex[i].color	= Color(255,255,255);
	}
}
LineSprite::~LineSprite()
{
	SafeDeleteA( m_aVertex );
}

//*****************************************************************************
//	ピース設定
//*****************************************************************************
void LineSprite::SetPiece( const Piece* pPiece, const Texture* pTex )
{
	//	テクスチャ・ピース設定
	m_pPiece = pPiece;
	SetTex( pTex );
	
	//	UV適用
	SetIndex( 0 );
}

//*****************************************************************************
//	点列指定描画(中心点)
//*****************************************************************************
void LineSprite::Draw( const Point aPt[] )
{
	int VertexNum = _GetVertexNum();

	//	起点の設定
	{
		Point Tip = _CalcTipPoint( aPt[0], aPt[1] );

		m_aVertex[0].x = aPt[0].x + Tip.x; m_aVertex[0].y = aPt[0].y + Tip.y;
		m_aVertex[1].x = aPt[0].x - Tip.x; m_aVertex[1].y = aPt[0].y - Tip.y;
	}

	//	終点の設定
	{
		Point Tip = _CalcTipPoint( aPt[m_Num-2], aPt[m_Num-1] );

		m_aVertex[VertexNum-2].x = aPt[m_Num-1].x + Tip.x; m_aVertex[VertexNum-2].y = aPt[m_Num-1].y + Tip.y;
		m_aVertex[VertexNum-1].x = aPt[m_Num-1].x - Tip.x; m_aVertex[VertexNum-1].y = aPt[m_Num-1].y - Tip.y;
	}

	//	中間の頂点の設定
	for( int i = 1; i < (m_Num - 1); i++ )
	{
		int Base = _GetIndexBase( i ) - 2;
		
		Point Joint = _CalcJointPoint( aPt[i], aPt[i-1], aPt[i+1] );
		
		m_aVertex[Base+0].x = aPt[i].x + Joint.x; m_aVertex[Base+0].y = aPt[i].y + Joint.y;
		m_aVertex[Base+1].x = aPt[i].x - Joint.x; m_aVertex[Base+1].y = aPt[i].y - Joint.y;
		
		m_aVertex[Base+2].x = m_aVertex[Base+0].x; m_aVertex[Base+2].y = m_aVertex[Base+0].y;
		m_aVertex[Base+3].x = m_aVertex[Base+1].x; m_aVertex[Base+3].y = m_aVertex[Base+1].y;
	}
	
	//	分割
	_CalcDividePoint();

	//	ピクセル補正
	for( int i = 0; i < VertexNum; i++ )
	{
		m_aVertex[i].x += 0.1f;
		m_aVertex[i].y += 0.1f;
	}
	
	//	登録
	DrawItem::Draw();
}

//*****************************************************************************
//	点列指定描画（端点）
//*****************************************************************************
void LineSprite::Draw( const Point aPtA[], const Point aPtB[] )
{
	int VertexNum = _GetVertexNum();

	//	起点の設定
	m_aVertex[0].x = aPtA[0].x; m_aVertex[0].y = aPtA[0].y;
	m_aVertex[1].x = aPtB[0].x; m_aVertex[1].y = aPtB[0].y;

	//	終点の設定
	m_aVertex[VertexNum-2].x = aPtA[m_Num-1].x; m_aVertex[VertexNum-2].y = aPtA[m_Num-1].y;
	m_aVertex[VertexNum-1].x = aPtB[m_Num-1].x; m_aVertex[VertexNum-1].y = aPtB[m_Num-1].y;

	//	中間の頂点の設定
	for( int i = 1; i < m_Num - 1; i++ )
	{
		int Base = _GetIndexBase( i ) - 2;
		
		m_aVertex[Base+0].x = aPtA[i].x; m_aVertex[Base+0].y = aPtA[i].y;
		m_aVertex[Base+1].x = aPtB[i].x; m_aVertex[Base+1].y = aPtB[i].y;
		
		m_aVertex[Base+2].x = m_aVertex[Base+0].x; m_aVertex[Base+2].y = m_aVertex[Base+0].y;
		m_aVertex[Base+3].x = m_aVertex[Base+1].x; m_aVertex[Base+3].y = m_aVertex[Base+1].y;
	}

	//	分割
	_CalcDividePoint();

	//	ピクセル補正
	for( int i = 0; i < VertexNum; i++ )
	{
		m_aVertex[i].x += 0.1f;
		m_aVertex[i].y += 0.1f;
	}

	//	登録
	DrawItem::Draw();
}

//*****************************************************************************
//	色の設定(全体)
//*****************************************************************************
void LineSprite::SetColor( Color Col )
{
	for( int i = 0; i <= m_Num; i++ )
	{
		_SetVertexColor( i, Col );
	}
}

//*****************************************************************************
//	色の設定(一点指定)
//*****************************************************************************
void LineSprite::SetColor( int Index, Color Col )
{
	_SetVertexColor( Index, Col );
}

//*****************************************************************************
//	色の設定（配列）
//*****************************************************************************
void LineSprite::SetColor( const Color aCol[] )
{
	for( int i = 0; i < m_Num; i++ )
	{
		_SetVertexColor( i, aCol[i] );
	}
}

//******************************************************************************
//	インデクスの設定（全体）
//******************************************************************************
void LineSprite::SetIndex( int PieceIndex )
{
	//	頂点の反映
	for( int i = 0; i < m_Num - 1; i++ )
	{
		_SetVertexUv( i, PieceIndex );
	}
}

//******************************************************************************
//	インデクスの設定（一点指定）
//******************************************************************************
void LineSprite::SetIndex( int Index, int PieceIndex )
{
	//	頂点の反映
	_SetVertexUv( Index, PieceIndex );
}

//******************************************************************************
//	インデクスの設定（配列）
//******************************************************************************
void LineSprite::SetIndex( const int aIndex[] )
{
	//	頂点の反映
	for( int i = 0; i < m_Num - 1; i++ )
	{
		_SetVertexUv( i, aIndex[i] );
	}
}

//------------------------------------------------------------------------------
//	描画実行
//------------------------------------------------------------------------------
void LineSprite::_OnRender( void ) const
{
	_RenderVertex( m_aVertex, _GetVertexNum() );
}

//-----------------------------------------------------------------------------
//	頂点にUV値を反映
//-----------------------------------------------------------------------------
void LineSprite::_SetVertexUv( int Index, int PieceIndex )
{
	if( !m_pPiece || !m_pTex ) return;

	Rect Rc = m_pPiece->GetRect( PieceIndex );

	//	UV算出
	const Point&	TexSize	= m_pTex->GetSize();
	float			l 		= Rc.l	/ TexSize.x;
	float			t 		= Rc.t	/ TexSize.y;
	float			r 		= Rc.r	/ TexSize.x;
	float			b 		= Rc.b	/ TexSize.y;

	//	頂点に設定
	int Base = _GetIndexBase( Index );
	int Next = _GetIndexBase( Index + 1 ) - 2;
	m_aVertex[Base].u		= l;	m_aVertex[Base].v		= t;
	m_aVertex[Base+1].u		= r;	m_aVertex[Base+1].v		= t;
	m_aVertex[Next].u		= l;	m_aVertex[Next].v		= b;
	m_aVertex[Next+1].u		= r;	m_aVertex[Next+1].v		= b;
}

//-----------------------------------------------------------------------------
//	頂点に色を反映
//-----------------------------------------------------------------------------
void LineSprite::_SetVertexColor( int Index, Color Col )
{
	if( Index <= 0 )
	{
		m_aVertex[0].color	= Col;
		m_aVertex[1].color	= Col;
	}
	else if( Index >= m_Num - 1 )
	{
		int VertexNum = _GetVertexNum();
		m_aVertex[VertexNum-1].color	= Col;
		m_aVertex[VertexNum-2].color	= Col;
	}
	else
	{
		int Base = _GetIndexBase( Index ) - 2;
		m_aVertex[Base + 0].color	= Col;
		m_aVertex[Base + 1].color	= Col;
		m_aVertex[Base + 2].color	= Col;
		m_aVertex[Base + 3].color	= Col;
	}
}

//------------------------------------------------------------------------------
//	継ぎ目の頂点位置を計算
//------------------------------------------------------------------------------
Point LineSprite::_CalcJointPoint( Point Pt, Point Prev, Point Next )
{
	Point A = (Pt-Prev).GetNormal();
	Point B = (Next-Pt).GetNormal();

	//	90度回す
	A.Set( A.y, -A.x );
	B.Set( B.y, -B.x );

	//	両辺の合成を採る
	Point Ret = A + B;

	float Len = Ret.Length();
	if( Len < EPS )
	{
		return _CalcTipPoint( Pt, Next );
	}

	//	太さ分調整
	return Ret * ((m_Width*0.5f) / Len );
}

//------------------------------------------------------------------------------
//	端点の頂点位置を計算
//------------------------------------------------------------------------------
Point LineSprite::_CalcTipPoint( Point Pt, Point Next )
{
	Point Ret = (Next - Pt);

	//	90度回す
	Ret.Set( Ret.y, -Ret.x );

	float Len = Ret.Length();
	if( Len < EPS )
	{
		return Point();
	}

	//	太さ分調整
	return Ret * ((m_Width*0.5f) / Len);
}

//------------------------------------------------------------------------------
//	分割頂点の計算
//------------------------------------------------------------------------------
void LineSprite::_CalcDividePoint( void )
{
	int VertexNum = _GetVertexNum();

	for( int Div = 1; Div < m_Divide; Div++ )
	{
		float t = (float)Div / (float)m_Divide;

		//	起点
		_CalcDividePoint( Div*2, 0, 0, _GetIndexBase(1), _GetIndexBase(2), t );
		
		//	終点
		_CalcDividePoint( _GetIndexBase(m_Num-2) + (Div*2), _GetIndexBase(m_Num-3), _GetIndexBase(m_Num-2), VertexNum-2, VertexNum-2, t );
		_CalcDividePoint( _GetIndexBase(m_Num-3) + (Div*2), _GetIndexBase(m_Num-4), _GetIndexBase(m_Num-3), _GetIndexBase(m_Num-2), VertexNum-2, t );

		//	中間の頂点の設定
		for( int i = 1; i < m_Num - 3; i++ )
		{
			_CalcDividePoint( _GetIndexBase(i) + (Div*2), _GetIndexBase(i-1), _GetIndexBase(i), _GetIndexBase(i+1), _GetIndexBase(i+2), t );			
		}
	}
}

//------------------------------------------------------------------------------
//	分割頂点の計算
//------------------------------------------------------------------------------
void LineSprite::_CalcDividePoint( int Base, int Prev1, int Prev0, int Next0, int Next1, float t )
{
	m_aVertex[Base].x		= CatmullRom( m_aVertex[Prev1].x, m_aVertex[Prev0].x, m_aVertex[Next0].x, m_aVertex[Next1].x, t );
	m_aVertex[Base].y		= CatmullRom( m_aVertex[Prev1].y, m_aVertex[Prev0].y, m_aVertex[Next0].y, m_aVertex[Next1].y, t );
	m_aVertex[Base+1].x		= CatmullRom( m_aVertex[Prev1+1].x, m_aVertex[Prev0+1].x, m_aVertex[Next0+1].x, m_aVertex[Next1+1].x, t );
	m_aVertex[Base+1].y		= CatmullRom( m_aVertex[Prev1+1].y, m_aVertex[Prev0+1].y, m_aVertex[Next0+1].y, m_aVertex[Next1+1].y, t );
	m_aVertex[Base].color	= Color::Lerp( Color( m_aVertex[Prev0].color ), Color( m_aVertex[Next0].color ), t ).dw;
	m_aVertex[Base+1].color = m_aVertex[Base].color;
	m_aVertex[Base].u		= Lerp( m_aVertex[Prev0].u, m_aVertex[Next0].u, t );
	m_aVertex[Base].v		= Lerp( m_aVertex[Prev0].v, m_aVertex[Next0].v, t );
	m_aVertex[Base+1].u		= Lerp( m_aVertex[Prev0+1].u, m_aVertex[Next0+1].u, t );
	m_aVertex[Base+1].v		= Lerp( m_aVertex[Prev0+1].v, m_aVertex[Next0+1].v, t );
}


}