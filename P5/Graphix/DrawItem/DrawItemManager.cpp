#include "P5.h"
#include "DrawItemManager.h"
#include "DrawItem.h"

#include "Graphix/DrawDevice.h"

namespace P5
{

//-----------------------------------------------------------------------------
//	頂点定義
//-----------------------------------------------------------------------------
const dword FVF	= (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	
//-----------------------------------------------------------------------------
//	ソート用プレディケータ
//-----------------------------------------------------------------------------
class SortPred
{
public:
	bool operator ()( const DrawItem* pA, const DrawItem* pB )
	{
		return pA->GetPhase() < pB->GetPhase();
	}
};

///////////////////////////////////////////////////////////////////////////////
//
//	描画アイテム管理
//
///////////////////////////////////////////////////////////////////////////////
DrawItemManager::DrawItemManager( void )
{
}
DrawItemManager::~DrawItemManager()
{

}

//*****************************************************************************
//	描画アイテム登録
//*****************************************************************************
void DrawItemManager::AddDrawItem( const DrawItem* pItem )
{
	m_vDrawItem.push_back( pItem );
}

//******************************************************************************
//	描画アイテム削除
//******************************************************************************
void DrawItemManager::RemoveDrawItem( const DrawItem* pItem )
{
	VecDrawItem::iterator it = StdFind( m_vDrawItem, pItem );

	if( it != m_vDrawItem.end() )
	{
		m_vDrawItem.erase( it );
	}
}

//*****************************************************************************
//	描画の実行
//*****************************************************************************
void DrawItemManager::Render( void )
{
	//	ソート
	std::stable_sort( m_vDrawItem.begin(), m_vDrawItem.end(), SortPred() );

	//	描画
	for( VecDrawItem::iterator it = m_vDrawItem.begin(); it != m_vDrawItem.end(); ++it )
	{
		(*it)->Render();
	}
	m_vDrawItem.clear();
}

}
