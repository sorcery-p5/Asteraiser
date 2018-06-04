#include "stdafx.h"
#include "MenuCursor.h"
#include "MenuObject.h"
#include "../Menu.h"
#include "../MenuLayer.h"
#include "../MenuData.h"

#include "Controller/SystemController.h"
#include "SpriteFrame/SpriteFrame.h"


////////////////////////////////////////////////////////////////////////////////
//
//	メニューカーソル
//
////////////////////////////////////////////////////////////////////////////////
MenuCursor::MenuCursor( MenuLayer* pParent )
{
	_ASSERT( pParent );

	m_pParent	= pParent;
	m_pParam	= NULL;
}
MenuCursor::~MenuCursor()
{
}

//******************************************************************************
//	初期化
//******************************************************************************
void MenuCursor::Init( const MENU_CURSOR* pParam, Label Root )
{
	_ASSERT( pParam && m_pParent->GetSprite() );
	_ASSERT( m_pParent->GetSprite()->FindBone(pParam->BoneName) >= 0 );

	m_pParam		= pParam;
	m_SelectObject	= Root;

	m_Pos			= _GetSelectPos();

	//	通知
	m_pParent->GetParent()->OnSelectObject( m_pParent->GetObject( m_SelectObject ), NULL );
}

//******************************************************************************
//	更新
//******************************************************************************
void MenuCursor::Update( void )
{
	//	位置の補間
	m_Pos += (_GetSelectPos() - m_Pos) * m_pParam->MoveSpeed;
	m_pParent->GetSprite()->SetBonePos( m_pParam->BoneName, m_Pos );

	bool bVisible = true;
	MenuObject* pObject = m_pParent->GetSelectObject();
	if( pObject && !pObject->IsCursorVisible() ) bVisible = false;

	//	非表示の切り替え
	m_pParent->GetSprite()->SetBoneVisible( m_pParam->BoneName, m_pParent->IsCurrent() && bVisible );
}

//******************************************************************************
//	現在レイヤーの更新
//******************************************************************************
void MenuCursor::UpdateCurrent( void )
{
	SystemController Ctrl;

	MenuObject* pObject = m_pParent->GetSelectObject();

	if( pObject )
	{
		//	カーソルの移動
		for( int i = 0; i < MENU_DIR_NUM; i++ )
		{
			if( !pObject->IsMoveEnable( MENU_DIR(i) ) ) continue;

			Label DirName = pObject->GetDirObjectName( MENU_DIR(i) );
			if( DirName.IsEmpty() ) continue;
					
			MenuObject* pDirObject = m_pParent->GetObject( DirName );
			while( pDirObject && !pDirObject->IsEnable() )
			{
				DirName = pDirObject->GetDirObjectName( MENU_DIR(i) );
				if( DirName.IsEmpty() ) break;
				
				pDirObject = m_pParent->GetObject( DirName );
			}
			_ASSERT( pDirObject );

			if( Ctrl.IsDirInput( CONTROLLER_INPUT( i + CONTROLLER_DIR_BEGIN ), INPUT_REPEAT ) )
			{
				SelectObject( DirName, true );
				pDirObject->OnChangeSelect( MENU_DIR(i) );

				break;
			}
		}
	}
}

//******************************************************************************
//	オブジェクトを選択する
//******************************************************************************
void MenuCursor::SelectObject( Label Name, bool bSound )
{
	MenuObject* pObject = m_pParent->GetObject( Name );
	if( !pObject ) return;

	if( Name != m_SelectObject )
	{	
		MenuObject* pPrevObject = m_pParent->GetObject( m_SelectObject );

		m_SelectObject = Name;

		//	サウンド再生
		bool bVisible = m_pParent->GetSprite()->GetBoneColor( m_pParam->BoneName ).a > 0 &&
						m_pParent->GetSprite()->GetBoneColor( Name ).a > 0;
		if( bSound && bVisible )
		{
			m_pParent->GetParent()->PlaySound( m_pParam->MoveSound, m_pParam->MoveSoundVol );
		}

		//	ヘルプテキスト更新
		m_pParent->UpdateHelpText();

		//	通知
		m_pParent->GetParent()->OnSelectObject( pObject, pPrevObject );
	}
}

//------------------------------------------------------------------------------
//	選択位置を取得
//------------------------------------------------------------------------------
Point MenuCursor::_GetSelectPos( void ) const
{
	SpriteFrame* pSprite = m_pParent->GetSprite();
	_ASSERT( pSprite );

	return pSprite->GetBoneLocalMatrix( m_SelectObject )->GetPos();
}
