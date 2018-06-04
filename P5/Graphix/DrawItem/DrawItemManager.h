#pragma once


namespace P5
{

class DrawItem;


///////////////////////////////////////////////////////////////////////////////
//
//	ï`âÊÉAÉCÉeÉÄä«óù
//
///////////////////////////////////////////////////////////////////////////////
class DrawItemManager
{
public:
	DrawItemManager( void );
	~DrawItemManager();

	void			AddDrawItem( const DrawItem* pItem );
	void			RemoveDrawItem( const DrawItem* pItem );
	void			Render( void );
	void			ClearDrawItem( void )						{ m_vDrawItem.clear(); }

private:
	typedef std::vector< const DrawItem* >	VecDrawItem;

private:
	VecDrawItem		m_vDrawItem;
};


}