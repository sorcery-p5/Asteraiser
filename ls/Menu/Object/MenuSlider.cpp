#include "stdafx.h"
#include "MenuSlider.h"
#include "../Menu.h"
#include "../MenuLayer.h"
#include "../MenuData.h"

#include "Controller/SystemController.h"
#include "SpriteFrame/SpriteFrame.h"


////////////////////////////////////////////////////////////////////////////////
//
//	メニュースライダー
//
////////////////////////////////////////////////////////////////////////////////
MenuSlider::MenuSlider( MenuLayer* pParent ) : MenuObject( pParent )
{
	m_pSliderParam	= NULL;
	m_Value			= 0.0f;
	m_MinValue		= 0.0f;
	m_MaxValue		= 0.0f;
}

MenuSlider::~MenuSlider()
{

}

//******************************************************************************
//	初期化
//******************************************************************************
void MenuSlider::OnInit( void )
{
	m_pSliderParam = &_GetParam()->Slider;
	_ASSERT( m_pSliderParam );

	m_Value		= m_pSliderParam->MinValue;
	m_MinValue	= m_pSliderParam->MinValue;
	m_MaxValue	= m_pSliderParam->MaxValue;
}

//******************************************************************************
//	更新
//******************************************************************************
void MenuSlider::OnUpdate( void )
{
}

//******************************************************************************
//	選択中の更新
//******************************************************************************
void MenuSlider::OnUpdateSelect( void )
{
	SystemController Ctrl;

	float Step = 0.0f;
	if( m_pSliderParam->SelectDir == MENU_SELECT_H )
	{
		//	横方向
		if( Ctrl.IsDirInput( CONTROLLER_L, INPUT_REPEAT ) )			Step = -m_pSliderParam->Step;
		else if( Ctrl.IsDirInput( CONTROLLER_R, INPUT_REPEAT ) )	Step =  m_pSliderParam->Step;
	}
	else if( m_pSliderParam->SelectDir == MENU_SELECT_V )
	{
		//	縦方向
		if( Ctrl.IsDirInput( CONTROLLER_D, INPUT_REPEAT ) )			Step = -m_pSliderParam->Step;
		else if( Ctrl.IsDirInput( CONTROLLER_U, INPUT_REPEAT ) )	Step =  m_pSliderParam->Step;
	}

	//	移動
	if( Step != 0.0f )
	{
		float Prev = m_Value;
		m_Value = Clamp( m_Value + Step, m_MinValue, m_MaxValue );

		if( Prev != m_Value )
		{
			//	通知
			GetMenu()->OnChangeValue( this );

			//	サウンド再生
			GetMenu()->PlaySound( m_pSliderParam->MoveSound, m_pSliderParam->MoveSoundVol );
		}
	}
}