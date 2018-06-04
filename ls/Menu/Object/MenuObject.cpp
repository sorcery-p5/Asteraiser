#include "stdafx.h"
#include "MenuObject.h"
#include "../Menu.h"
#include "../MenuLayer.h"
#include "../MenuData.h"

#include "App/App.h"
#include "Controller/SystemController.h"
#include "SpriteFrame/SpriteFrame.h"
#include "BGM/BGMManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	メニューオブジェクト
//
////////////////////////////////////////////////////////////////////////////////
MenuObject::MenuObject( MenuLayer* pParent )
{
	_ASSERT( pParent );

	m_pParent		= pParent;
	m_pParam		= NULL;
	m_bEnable		= true;
	m_FrameCount	= 0;
}

MenuObject::~MenuObject()
{
}

//******************************************************************************
//	初期化
//******************************************************************************
void MenuObject::Init( const MENU_OBJECT* pParam )
{
	_ASSERT( pParam && m_pParent->GetSprite() );
	_ASSERT( m_pParent->GetSprite()->FindBone(pParam->BoneName) >= 0 );

	m_pParam		= pParam;
	m_FrameCount	= 0;

	//	派生先
	OnInit();
}

//******************************************************************************
//	更新
//******************************************************************************
void MenuObject::Update( void )
{
	//	派生先
	OnUpdate();

	m_FrameCount++;
}

//******************************************************************************
//	選択中の更新
//******************************************************************************
void MenuObject::UpdateSelect( void )
{
	if( IsEnable() )
	{
		SystemController Ctrl;

		//	キーイベント
		for( MENU_KEY_EVENT_VEC::const_iterator it = m_pParam->vKeyEvent.begin(); it != m_pParam->vKeyEvent.end(); ++it )
		{
			bool bInput	= it->Type == MENU_KEY_EVENT_ANY ?
							Ctrl.IsAnyInput( INPUT_TRIG ) :
							Ctrl.IsInput( CONTROLLER_SYSTEM_KEY(it->Type), INPUT_TRIG );

			if( bInput )
			{
				for( int i = 0; i < (int)it->vAction.size(); i++ )
				{
					_ExecAction( it->vAction[i] );
				}
			}
		}
		
		//	派生先
		OnUpdateSelect();
	}
}

//******************************************************************************
//	選択中かどうか
//******************************************************************************
bool MenuObject::IsSelect( void ) const
{
	return m_pParent->GetSelectObject() == this;
}

//******************************************************************************
//	所属するメニューを取得
//******************************************************************************
Menu* MenuObject::GetMenu( void ) const
{
	return m_pParent->GetParent();
}

//------------------------------------------------------------------------------
//	スプライトフレーム取得
//------------------------------------------------------------------------------
SpriteFrame* MenuObject::_GetSprite( void ) const
{
	return m_pParent->GetSprite();
}

//------------------------------------------------------------------------------
//	アクション実行
//------------------------------------------------------------------------------
void MenuObject::_ExecAction( const MENU_ACTION& Action )
{
	switch( Action.Type )
	{
	case MENU_ACTION_OPEN:		m_pParent->GetParent()->OpenLayer( Action.Name );														break;
	case MENU_ACTION_CLOSE:		Action.Name.IsEmpty()? m_pParent->Close() : m_pParent->GetParent()->CloseLayer( Action.Name );			break;

	case MENU_ACTION_ANIME:		_GetSprite()->ChangeAnime( Action.Name, Action.Param );													break;
	case MENU_ACTION_NOTIFY:	m_pParent->GetParent()->OnNotify( this, Action.Name, Action.Param );									break;
	case MENU_ACTION_SELECT:	m_pParent->SelectObject( Action.Name );																	break;

	case MENU_ACTION_EFFECT:	m_pParent->GetParent()->CreateEffect( Action.Name, *_GetSprite()->GetBoneMatrix(m_pParam->BoneName) );	break;
	case MENU_ACTION_SOUND:		m_pParent->GetParent()->PlaySound( Action.Name, Action.Param );											break;

	case MENU_ACTION_BGM_PLAY:	GetApp()->GetBGMMng()->Play( Action.Text.c_str(), Action.Param );										break;
	case MENU_ACTION_BGM_STOP:	GetApp()->GetBGMMng()->Stop();																			break;
	case MENU_ACTION_BGM_FADE:	GetApp()->GetBGMMng()->Fade( Action.Param );															break;
	}
}