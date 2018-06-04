#include "P5.h"
#include "FontSprite.h"

#include "Graphix/Texture/Texture.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	フォントスプライト
//
///////////////////////////////////////////////////////////////////////////////
FontSprite::FontSprite( const Font* pFont ) : Sprite()
{
	_ASSERT( pFont );

	m_pFont		= pFont;
	m_Align		= FONT_ALIGN_LT;
	m_Color.Set( 255, 255, 255 );

	SetTex( pFont->GetTex() );
}
FontSprite::~FontSprite()
{
}

//------------------------------------------------------------------------------
//	描画実行
//------------------------------------------------------------------------------
void FontSprite::_OnRender( void ) const
{
	//	開始位置を計算する
	Point Pos = _GetFontStart();
	float StartX = Pos.x;

	Point Size = m_pFont->GetLetterSize();

	pstr pStr = m_String.c_str();
	while( *pStr != '\0' )
	{
		//	全角
		if( IsEmQuad(*pStr) )
		{
			Rect Rc = m_pFont->GetRect( *pStr, *(pStr+1) );
			_RenderRect( Pos, Rc.Size(), Rc );

			pStr += 2;
			Pos.x += Rc.Width();
		}
		//	半角
		else
		{
			//	改行
			if( *pStr == '\n' )
			{
				Pos.y += Size.y;
				Pos.x = StartX;
			}
			//	非文字かどうか
			else if( !IsNotLetter( *pStr ) )
			{
				Rect Rc = m_pFont->GetRect( *pStr, '\0');
				_RenderRect( Pos, Rc.Size(), Rc );
				Pos.x += Rc.Width();
			}
			pStr += 1;
		}
	}	
}

//------------------------------------------------------------------------------
//	描画開始点を求める
//------------------------------------------------------------------------------
Point FontSprite::_GetFontStart( void ) const
{
	Point Pos = m_Rect.LT();

	//	左上開始
	if( m_Align == FONT_ALIGN_LT ) return Pos;

	Point Size = _GetFontSize();

	//	X:中央
	if( m_Align == FONT_ALIGN_CT || m_Align == FONT_ALIGN_CC || m_Align == FONT_ALIGN_CB )
	{
		Pos.x += (m_Rect.Width() - Size.x) / 2;
	}
	//	X:右
	else if( m_Align == FONT_ALIGN_RT || m_Align == FONT_ALIGN_RC || m_Align == FONT_ALIGN_RB )
	{
		Pos.x = m_Rect.r - Size.x;
	}

	//	Y:中央
	if( m_Align == FONT_ALIGN_LC || m_Align == FONT_ALIGN_CC || m_Align == FONT_ALIGN_RC )
	{
		Pos.y += (m_Rect.Height() - Size.y) / 2;
	}
	//	Y:下
	else if( m_Align == FONT_ALIGN_LB || m_Align == FONT_ALIGN_CB || m_Align == FONT_ALIGN_RB )
	{
		Pos.y = m_Rect.b - Size.y;
	}

	return Pos;
}

//------------------------------------------------------------------------------
//	フォントサイズ取得
//------------------------------------------------------------------------------
Point FontSprite::_GetFontSize( void ) const
{
	if( m_String.empty() ) return Point();

	Point Size = m_pFont->GetLetterSize();

	float x = 0;
	float w = 0, h = Size.y;
	for( uint i = 0; i < m_String.length(); i++ )
	{
		char Ch = m_String[i];
		
		//	全角
		if( IsEmQuad(Ch) )
		{
			x += m_pFont->GetLetterWidth( Ch, m_String[i+1] );
			i++;
		}
		//	半角
		else
		{
			//	改行
			if( Ch == '\n' )
			{
				w = Max( w, x );
				x = 0;
				h += Size.y;
			}
			else
			{
				if( !IsNotLetter(Ch) )
				{
					x += m_pFont->GetLetterWidth( Ch, '\0' );
				}
			}
		}
	}
	w = Max( w, x );
	
	return Point( w, h );
}

//------------------------------------------------------------------------------
//	矩形の描画
//------------------------------------------------------------------------------
void FontSprite::_RenderRect( Point Pt, Point Size, const Rect& Rc ) const
{
	SPRITE_VERTEX aVertex[VERTEX_NUM] = {0};

	Point aPt[] = 
	{
		Point( Pt.x, Pt.y ),			Point( Pt.x + Size.x, Pt.y ),
		Point( Pt.x, Pt.y + Size.y ),	Point( Pt.x + Size.x, Pt.y + Size.y ),
	};

	//	位置・色の設定
	bool bMat = m_Matrix != Matrix3::Identity();
	for( int i = 0; i < VERTEX_NUM; i++ )
	{
		if( bMat ) aPt[i] = m_Matrix.Transform( aPt[i] );

		aVertex[i].x	= Round( aPt[i].x ) + 0.1f;
		aVertex[i].y	= Round( aPt[i].y ) + 0.1f;
		aVertex[i].rhw	= 1.0f;
		aVertex[i].color = m_Color;
	}

	//	UV設定
	_ApplyVertexUv( aVertex, Rc );
	
	_RenderVertex( aVertex, VERTEX_NUM );
}

//******************************************************************************
//	文字列をブレンドモードへ変換
//******************************************************************************
FONT_ALIGN FontSprite::StrToAlign( pstr pStr )
{
	pstr aAlign[] =
	{
		"LT",	"CT",	"RT",
		"LC",	"CC",	"RC",
		"LB",	"CB",	"RB",
	};

	for( int i = 0; i < FONT_ALIGN_NUM; i++ )
	{
		if( stricmp( aAlign[i], pStr ) == 0 )
		{
			return FONT_ALIGN(i);
		}
	}

	return FONT_ALIGN_LT;
}

}