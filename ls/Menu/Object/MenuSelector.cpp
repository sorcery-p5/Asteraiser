#include "stdafx.h"
#include "MenuSelector.h"
#include "../Menu.h"
#include "../MenuLayer.h"
#include "../MenuData.h"

#include "Controller/SystemController.h"
#include "SpriteFrame/SpriteFrame.h"


////////////////////////////////////////////////////////////////////////////////
//
//	メニューセレクター
//
////////////////////////////////////////////////////////////////////////////////
MenuSelector::MenuSelector( MenuLayer* pParent ) : MenuObject( pParent )
{
	m_pSelectorParam	= NULL;
	m_SelectIndex		= 0;
}

MenuSelector::~MenuSelector()
{

}

//******************************************************************************
//	初期化
//******************************************************************************
void MenuSelector::OnInit( void )
{
	m_pSelectorParam = &_GetParam()->Selector;
	_ASSERT( m_pSelectorParam );
	_ASSERT( !m_pSelectorParam->vSelect.empty() );

	m_SelectIndex = 0;

	m_CursorPos = _GetSelectPos();
}

//******************************************************************************
//	更新
//******************************************************************************
void MenuSelector::OnUpdate( void )
{
	//	カーソル位置の移動
	m_CursorPos += (_GetSelectPos() - m_CursorPos) * m_pSelectorParam->MoveSpeed;
	_GetSprite()->SetBonePos( m_pSelectorParam->CursorBone, m_CursorPos );

	//	カーソルの非表示
	_GetSprite()->SetBoneVisible( m_pSelectorParam->CursorBone, GetParent()->IsCurrent() );
}

//******************************************************************************
//	選択中の更新
//******************************************************************************
void MenuSelector::OnUpdateSelect( void )
{
	SystemController Ctrl;

	int Dir = 0;
	if( m_pSelectorParam->SelectDir == MENU_SELECT_H )
	{
		//	横方向
		if( Ctrl.IsDirInput( CONTROLLER_L, INPUT_REPEAT ) )			Dir = -1;
		else if( Ctrl.IsDirInput( CONTROLLER_R, INPUT_REPEAT ) )	Dir =  1;
	}
	else if( m_pSelectorParam->SelectDir == MENU_SELECT_V )
	{
		//	縦方向
		if( Ctrl.IsDirInput( CONTROLLER_D, INPUT_REPEAT ) )			Dir = -1;
		else if( Ctrl.IsDirInput( CONTROLLER_U, INPUT_REPEAT ) )	Dir =  1;
	}

	//	移動
	if( Dir != 0 )
	{
		m_SelectIndex = Loop( m_SelectIndex + Dir, 0, (int)m_pSelectorParam->vSelect.size() );

		//	通知
		GetMenu()->OnChangeValue( this );

		//	サウンド再生
		GetMenu()->PlaySound( m_pSelectorParam->MoveSound, m_pSelectorParam->MoveSoundVol );
	}
}

//******************************************************************************
//	現在の値を取得
//******************************************************************************
float MenuSelector::GetValue( void ) const
{
	_ASSERT( m_SelectIndex < (int)m_pSelectorParam->vSelect.size() );
	
	return m_pSelectorParam->vSelect[m_SelectIndex].Value;
}

//******************************************************************************
//	値の設定
//******************************************************************************
void MenuSelector::SetValue( float Value )
{
	for( int i = 0; i < (int)m_pSelectorParam->vSelect.size(); i++ )
	{
		if( m_pSelectorParam->vSelect[i].Value == Value )
		{
			_ASSERT( _GetSprite()->FindBone( m_pSelectorParam->vSelect[i].Name ) >= 0 );

			m_SelectIndex = i;
			
			//	カーソル位置を移動
			m_CursorPos = _GetSelectPos();
			_GetSprite()->SetBonePos( m_pSelectorParam->CursorBone, m_CursorPos );
			break;
		}
	}
}

//******************************************************************************
//	選択オブジェクトの取得
//******************************************************************************
Label MenuSelector::GetSelectName( void ) const
{
	_ASSERT( m_SelectIndex < (int)m_pSelectorParam->vSelect.size() );
	
	return m_pSelectorParam->vSelect[m_SelectIndex].Name;
}

//******************************************************************************
//	選択オブジェクトの設定
//******************************************************************************
void MenuSelector::SetSelectName( Label Name )
{
	for( int i = 0; i < (int)m_pSelectorParam->vSelect.size(); i++ )
	{
		if( m_pSelectorParam->vSelect[i].Name == Name )
		{
			_ASSERT( _GetSprite()->FindBone( m_pSelectorParam->vSelect[i].Name ) >= 0 );

			m_SelectIndex = i;
			
			//	カーソル位置を移動
			m_CursorPos = _GetSelectPos();
			_GetSprite()->SetBonePos( m_pSelectorParam->CursorBone, m_CursorPos );
			break;
		}
	}
}

//------------------------------------------------------------------------------
//	選択位置を取得
//------------------------------------------------------------------------------
Point MenuSelector::_GetSelectPos( void ) const
{
	_ASSERT( m_SelectIndex < (int)m_pSelectorParam->vSelect.size() );

	return _GetSprite()->GetBoneLocalMatrix( m_pSelectorParam->vSelect[m_SelectIndex].Name )->GetPos();
}
