#include "P5.h"
#include "DrawItem.h"
#include "DrawItemManager.h"

#include "Graphix/DrawDevice.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	•`‰æƒAƒCƒeƒ€Šî’ê
//
///////////////////////////////////////////////////////////////////////////////
DrawItem::DrawItem( void )
{
	m_Phase		= 0;
	m_Blend		= BLEND_ALPHA;
}
DrawItem::~DrawItem()
{
	//	“o˜^‰ğœ
	GetGraphix()->GetDrawItemMng()->RemoveDrawItem( this );
}

//*****************************************************************************
//	•`‰æ‚Ì—\–ñ
//*****************************************************************************
void DrawItem::Draw( void ) const
{
	//	“o˜^
	GetGraphix()->GetDrawItemMng()->AddDrawItem( this );
}

//*****************************************************************************
//	•`‰æ‚ÌÀs
//*****************************************************************************
void DrawItem::Render( void ) const
{
	//	ƒuƒŒƒ“ƒhƒ‚[ƒh“o˜^
	GetGraphix()->GetDevice()->SetBlend( m_Blend );

	//	Àsˆ—
	_OnRender();
}

}