#pragma once

#include "MenuObject.h"


////////////////////////////////////////////////////////////////////////////////
//
//	メニュースライダー
//
////////////////////////////////////////////////////////////////////////////////
class MenuSlider : public MenuObject
{
public:
	MenuSlider( MenuLayer* pParent );
	virtual ~MenuSlider();

	virtual void			OnInit( void );
	virtual void			OnUpdate( void );
	virtual void			OnUpdateSelect( void );

	virtual float			GetValue( void )					const	{ return m_Value; }
	virtual void			SetValue( float Value )						{ m_Value = Value; }
	virtual void			SetMinMax( float MinVal, float MaxVal )		{ m_MinValue = MinVal; m_MaxValue = MaxVal; }

private:
	const MENU_SLIDER*		m_pSliderParam;
	float					m_Value;
	float					m_MinValue;
	float					m_MaxValue;
};