#include "P5.h"
#include "DrawItem.h"
#include "DrawItemManager.h"

#include "Graphix/DrawDevice.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	�`��A�C�e�����
//
///////////////////////////////////////////////////////////////////////////////
DrawItem::DrawItem( void )
{
	m_Phase		= 0;
	m_Blend		= BLEND_ALPHA;
}
DrawItem::~DrawItem()
{
	//	�o�^����
	GetGraphix()->GetDrawItemMng()->RemoveDrawItem( this );
}

//*****************************************************************************
//	�`��̗\��
//*****************************************************************************
void DrawItem::Draw( void ) const
{
	//	�o�^
	GetGraphix()->GetDrawItemMng()->AddDrawItem( this );
}

//*****************************************************************************
//	�`��̎��s
//*****************************************************************************
void DrawItem::Render( void ) const
{
	//	�u�����h���[�h�o�^
	GetGraphix()->GetDevice()->SetBlend( m_Blend );

	//	���s����
	_OnRender();
}

}