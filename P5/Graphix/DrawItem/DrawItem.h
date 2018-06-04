#pragma once

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	•`‰æƒAƒCƒeƒ€Šî’ê
//
///////////////////////////////////////////////////////////////////////////////
class DrawItem
{
public:
	DrawItem( void );
	virtual ~DrawItem();
	
	void			Draw( void )							const;
	void			Render( void )							const;

	void			SetBlend( BLEND_MODE Blend )					{ m_Blend = Blend; }
	void			SetPhase( int Phase )							{ m_Phase = Phase; }

	int				GetPhase( void )						const	{ return m_Phase; }

protected:
	virtual void	_OnRender( void )						const	= 0;

private:
	int				m_Phase;
	BLEND_MODE		m_Blend;
};


}