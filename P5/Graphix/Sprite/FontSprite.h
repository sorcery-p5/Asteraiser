#pragma once

#include "Sprite.h"


namespace P5
{

//------------------------------------------------------------------------------
//	�����̃A���C���w��
//------------------------------------------------------------------------------
enum FONT_ALIGN
{
	FONT_ALIGN_LT,	//	����
	FONT_ALIGN_CT,	//	����
	FONT_ALIGN_RT,	//	�E��
	FONT_ALIGN_LC,	//	����
	FONT_ALIGN_CC,	//	����
	FONT_ALIGN_RC,	//	�E��
	FONT_ALIGN_LB,	//	����
	FONT_ALIGN_CB,	//	����
	FONT_ALIGN_RB,	//	�E��

	FONT_ALIGN_NUM,
};

///////////////////////////////////////////////////////////////////////////////
//
//	�t�H���g�X�v���C�g
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