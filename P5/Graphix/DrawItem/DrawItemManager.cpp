#include "P5.h"
#include "DrawItemManager.h"
#include "DrawItem.h"

#include "Graphix/DrawDevice.h"

namespace P5
{

//-----------------------------------------------------------------------------
//	���_��`
//-----------------------------------------------------------------------------
const dword FVF	= (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	
//-----------------------------------------------------------------------------
//	�\�[�g�p�v���f�B�P�[�^
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
//	�`��A�C�e���Ǘ�
//
///////////////////////////////////////////////////////////////////////////////
DrawItemManager::DrawItemManager( void )
{
}
DrawItemManager::~DrawItemManager()
{

}

//*****************************************************************************
//	�`��A�C�e���o�^
//*****************************************************************************
void DrawItemManager::AddDrawItem( const DrawItem* pItem )
{
	m_vDrawItem.push_back( pItem );
}

//******************************************************************************
//	�`��A�C�e���폜
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
//	�`��̎��s
//*****************************************************************************
void DrawItemManager::Render( void )
{
	//	�\�[�g
	std::stable_sort( m_vDrawItem.begin(), m_vDrawItem.end(), SortPred() );

	//	�`��
	for( VecDrawItem::iterator it = m_vDrawItem.begin(); it != m_vDrawItem.end(); ++it )
	{
		(*it)->Render();
	}
	m_vDrawItem.clear();
}

}
