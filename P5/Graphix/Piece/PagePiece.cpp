#include "P5.h"
#include "PagePiece.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	ページ設定ピース
//
///////////////////////////////////////////////////////////////////////////////
PagePiece::PagePiece( const Label& TexName, const Rect& Rc, const Point& Stride, int LineNum, int IndexMax )
{
	m_TexName	= TexName;
	m_Rect		= Rc;
	m_Stride	= Stride;
	m_LineNum	= LineNum;
	m_IndexMax	= IndexMax;

	if( m_Stride.IsZero() ) m_Stride = m_Rect.Size();
	if( m_LineNum <= 0 )	m_LineNum = 0;
	if( m_IndexMax <= 0 )	m_IndexMax = 0;
}

//*****************************************************************************
//	矩形域を取得
//*****************************************************************************
Rect PagePiece::GetRect( int Index ) const
{
	Index = Clamp<int>( Index, 0, m_IndexMax );

	Rect Rc = m_Rect;

	if( m_LineNum == 0 )
	{
		Rc.Shift( m_Stride.x * Index, 0.0f );
	}
	else
	{
		Rc.Shift( m_Stride.x * (Index % m_LineNum), m_Stride.y * (Index / m_LineNum) );
	}

	return Rc;
}

}