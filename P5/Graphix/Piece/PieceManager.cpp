#include "P5.h"
#include "PieceManager.h"


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	ピース管理
//
///////////////////////////////////////////////////////////////////////////////
//*****************************************************************************
//	ピース追加
//*****************************************************************************
void PieceManager::AddPiece( const Piece* pPic, const Label& Name )
{
	m_PieceMap.insert( PieceMap::value_type(Name, pPic) );
}

//*****************************************************************************
//	ピースの取得
//*****************************************************************************
const Piece* PieceManager::GetPiece( const Label& Name ) const
{
	PieceMap::const_iterator it = m_PieceMap.find( Name );
	if( it != m_PieceMap.end() )
	{
		return it->second;
	}
	return NULL;
}


}