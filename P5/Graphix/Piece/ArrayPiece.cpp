#include "P5.h"
#include "ArrayPiece.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	配列ピース
//
///////////////////////////////////////////////////////////////////////////////
ArrayPiece::ArrayPiece( const Label& TexName, const Rect* aRect, int RectNum )
{
	if( aRect && RectNum > 0 )
	{
		for( int i = 0; i < RectNum; i++ )
		{
			m_vRect.push_back( aRect[i] );
		}
	}

	m_TexName = TexName;
}

//*****************************************************************************
//	矩形域を取得
//*****************************************************************************
Rect ArrayPiece::GetRect( int Index ) const
{
	_ASSERT( !m_vRect.empty() );

	Index = Clamp<int>( Index, 0, (int)(m_vRect.size() - 1) );

	return m_vRect[ Index ];
}

}