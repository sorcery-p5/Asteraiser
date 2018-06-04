#pragma once

#include "Sprite.h"


namespace P5
{

//------------------------------------------------------------------------------
//	文字のアライン指定
//------------------------------------------------------------------------------
enum FONT_ALIGN
{
	FONT_ALIGN_LT,	//	左上
	FONT_ALIGN_CT,	//	中上
	FONT_ALIGN_RT,	//	右上
	FONT_ALIGN_LC,	//	左中
	FONT_ALIGN_CC,	//	中中
	FONT_ALIGN_RC,	//	右中
	FONT_ALIGN_LB,	//	左下
	FONT_ALIGN_CB,	//	中下
	FONT_ALIGN_RB,	//	右下

	FONT_ALIGN_NUM,
};

///////////////////////////////////////////////////////////////////////////////
//
//	フォントスプライト
//
///////////////////////////////////////////////////////////////////////////////
class FontSprite : public Sprite
{
public:
	FontSprite( const Font* pFont );
	~FontSprite();
	
	void				SetText( pstr pStr )							{ m_String = pStr; }
	void				SetColor( Color Col )							{ m_Color = Col; }
	void				SetAlign( FONT_ALIGN Align )					{ m_Align = Align; }

	void				SetRect( const Rect& Rc )						{ m_Rect = Rc; }
	void				SetMatrix( const Matrix3& Mat )					{ m_Matrix = Mat; }

	static FONT_ALIGN	StrToAlign( pstr pStr );

protected:
	virtual void		_OnRender( void )						const;

private:
	enum
	{
		VERTEX_NUM	= 4,
	};

private:
	Point				_GetFontStart( void )								const;
	Point				_GetFontSize( void )								const;
	void				_RenderRect( Point Pt, Point Size, const Rect& Rc )	const;

private:
	const Font*			m_pFont;
	std::string			m_String;
	Rect				m_Rect;
	Matrix3				m_Matrix;
	Color				m_Color;
	FONT_ALIGN			m_Align;
};


}