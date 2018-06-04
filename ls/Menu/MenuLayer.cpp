#include "stdafx.h"
#include "MenuLayer.h"
#include "Menu.h"
#include "MenuData.h"
#include "Object/MenuCursor.h"
#include "Object/MenuObject.h"
#include "Object/MenuSelector.h"
#include "Object/MenuSlider.h"
#include "Object/MenuList.h"
#include "Object/MenuScroll.h"

#include "App/App.h"
#include "Config/Config.h"
#include "Controller/SystemController.h"
#include "SpriteFrame/SpriteFrame.h"
#include "BGM/BGMManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	メニューレイヤー
//
////////////////////////////////////////////////////////////////////////////////
MenuLayer::MenuLayer( Menu* pParent ) : IndicateBase()
{
	_ASSERT( pParent );

	m_pParent			= pParent;
	m_pParam			= NULL;

	m_pSprite			= NULL;
	m_pCursor			= NULL;

	m_bDelete			= false;
	m_bVisible			= true;
	m_State				= STATE_OPEN;
	m_FrameCount		= 0;

	m_HelpTextLength	= 0;
}

MenuLayer::~MenuLayer()
{
	SafeDelete( m_pCursor );
	DeleteMap( m_mapObject );
	SafeDelete( m_pSprite );
}

//******************************************************************************
//	初期化
//******************************************************************************
void MenuLayer::Init( const MENU_LAYER* pParam )
{
	_ASSERT( pParam );
	m_pParam		= pParam;
	m_State			= STATE_OPEN;
	m_FrameCount	= 0;
	
	//	スプライトフレーム初期化
	const SpriteFrameFile* pSpriteFile = m_pParent->GetData()->GetSpriteFrameFile( m_pParam->SpriteName );
	if( pSpriteFile )
	{
		m_pSprite = new SpriteFrame;
		MEM_CHECK( m_pSprite );
		m_pSprite->Init( pSpriteFile, Matrix3::Trans( m_pParam->Pos ) * m_pParent->GetMatrix() );

		//	開くアニメ
		if( !m_pParam->OpenAnime.IsEmpty() )
		{
			m_pSprite->ChangeAnime( m_pParam->OpenAnime );
		}
			
		//	オブジェクト
		for( int i = 0; i < (int)m_pParam->vObject.size(); i++ )
		{
			MenuObject* pObject = _CreateObject( m_pParam->vObject[i] );

			m_mapObject.insert( MenuObjectMap::value_type( pObject->GetName(), pObject ) );
		}

		//	カーソル
		if( !m_pParam->Cursor.BoneName.IsEmpty() )
		{
			m_pCursor = new MenuCursor( this );
			MEM_CHECK( m_pCursor );
			m_pCursor->Init( &m_pParam->Cursor, m_pParam->RootObject );
		}

		//	ヘルプテキスト
		UpdateHelpText();
	}

	//	表示状態
	UpdateVisible();
	
	//	初期化アクション
	_ExecActionList( m_pParam->vInitAction );
}

//******************************************************************************
//	更新
//******************************************************************************
void MenuLayer::Update( void )
{
	//	時間イベント
	_UpdateTimeEvent();

	switch( m_State )
	{
	case STATE_OPEN:
		{
			//	アニメの終了
			if( m_pSprite->IsAnimeLast() || !m_pSprite->IsAnimePlaying() )
			{
				//	待機状態へ
				if( !m_pParam->WaitAnime.IsEmpty() )
				{
					m_pSprite->ChangeAnime( m_pParam->WaitAnime );
				}

				m_State = STATE_WAIT;
			}
		}
		break;
	case STATE_WAIT:
		{
			//	アニメ終了で終了
			if( m_pParam->bAnimeWaitEnd )
			{
				if( m_pSprite && m_pSprite->IsAnimePlaying() && m_pSprite->IsAnimeLast() )
				{
					Close();
				}
			}
		}
		break;
	case STATE_CLOSE:
		{
			//	アニメの終了
			if( m_pSprite->IsAnimeLast() || !m_pSprite->IsAnimePlaying() )
			{
				Delete();
			}
		}
		break;
	};

	//	スプライト
	if( m_pSprite ) m_pSprite->Update( NULL );

	//	カーソル
	if( m_pCursor ) m_pCursor->Update();
		
	//	オブジェクト
	for( MenuObjectMap::iterator it = m_mapObject.begin(); it != m_mapObject.end(); ++it )
	{
		(it->second)->Update();
	}

	//	ヘルプテキスト
	_UpdateHelpTextLength();

	//	インジケート
	IndicateBase::Update();

	//	カウントアップ
	m_FrameCount++;
}

//******************************************************************************
//	現在レイヤーの更新
//******************************************************************************
void MenuLayer::UpdateCurrent( void )
{
	SystemController Ctrl;

	if( m_State == STATE_OPEN )
	{
		//	ボタンによる開きアニメスキップ
		if( m_pParam->bSkipOpen &&
			Ctrl.IsAnyInput( INPUT_TRIG ) )
		{
			//	待機状態へ
			if( !m_pParam->WaitAnime.IsEmpty() )
			{
				m_pSprite->ChangeAnime( m_pParam->WaitAnime );
			}

			m_State = STATE_WAIT;
		}
	}
	else if( m_State == STATE_WAIT )
	{
		//	カーソル
		if( m_pCursor ) m_pCursor->UpdateCurrent();

		//	選択中オブジェクトの操作
		MenuObject* pSelectObj = GetSelectObject();
		if( pSelectObj )
		{
			pSelectObj->UpdateSelect();
		}

		//	キーイベント
		for( MENU_KEY_EVENT_VEC::const_iterator it = m_pParam->vKeyEvent.begin(); it != m_pParam->vKeyEvent.end(); ++it )
		{
			bool bInput	= it->Type == MENU_KEY_EVENT_ANY ?
							Ctrl.IsAnyInput( INPUT_TRIG ) :
							Ctrl.IsInput( CONTROLLER_SYSTEM_KEY(it->Type), INPUT_TRIG );

			if( bInput )
			{
				_ExecActionList( it->vAction );
			}
		}
	}
}

//******************************************************************************
//	描画
//******************************************************************************
void MenuLayer::Draw( const Matrix3& OffsetMat )
{
	if( m_pSprite ) m_pSprite->Draw( OffsetMat );
	
	//	基底
	IndicateBase::Draw( OffsetMat );

	//	オブジェクト
	for( MenuObjectMap::iterator it = m_mapObject.begin(); it != m_mapObject.end(); ++it )
	{
		(it->second)->Draw( OffsetMat );
	}
}

//******************************************************************************
//	閉じる
//******************************************************************************
void MenuLayer::Close( void )
{
	if( m_State != STATE_CLOSE )
	{
		if( m_pSprite )
		{
			//	閉じるアニメ
			if( !m_pParam->CloseAnime.IsEmpty() )
			{
				m_pSprite->ChangeAnime( m_pParam->CloseAnime, m_pParam->CloseIntp );
			}
			else
			{
				Delete();
			}
		}

		//	終了アクション
		_ExecActionList( m_pParam->vCloseAction );

		m_State = STATE_CLOSE;
	}
}

//******************************************************************************
//	現在レイヤーに移動
//******************************************************************************
void MenuLayer::OnMoveCurrent( void )
{
	//	色を元に戻す
	if( m_pSprite ) m_pSprite->SetColor( Color::White() );
}

//******************************************************************************
//	背面レイヤーに移動
//******************************************************************************
void MenuLayer::OnMoveBack( void )
{
	//	色をバックカラーに
	if( m_pSprite ) m_pSprite->SetColor( m_pParam->BackColor );
}

//******************************************************************************
//	現在レイヤーかどうか
//******************************************************************************
bool MenuLayer::IsCurrent( void ) const
{
	return m_pParent->GetCurLayer() == this;
}

//******************************************************************************
//	表示有無の更新する
//******************************************************************************
void MenuLayer::UpdateVisible( void )
{
	if( m_pSprite ) m_pSprite->SetVisible( m_pParent->IsVisible() && m_bVisible );
}

//******************************************************************************
//	オブジェクトの取得
//******************************************************************************
MenuObject* MenuLayer::GetObject( Label Name ) const
{
	MenuObjectMap::const_iterator it = m_mapObject.find( Name );
	if( it != m_mapObject.end() )
	{
		return (it->second);
	}
	return NULL;
}
	
//******************************************************************************
//	選択オブジェクトの取得
//******************************************************************************
MenuObject* MenuLayer::GetSelectObject( void ) const
{
	if( m_pCursor )
	{
		return GetObject( m_pCursor->GetSelectObject() );
	}
	return NULL;
}

//******************************************************************************
//	選択オブジェクトの設定
//******************************************************************************
void MenuLayer::SelectObject( Label Name )
{
	if( m_pCursor )
	{
		m_pCursor->SelectObject( Name, false );
	}
}

//******************************************************************************
//	オブジェクトの選択対象を設定
//******************************************************************************
void MenuLayer::SetObjectSelect( Label Name, Label Value )
{
	MenuObject* pObject = GetObject( Name );
	if( pObject ) pObject->SetSelectName( Value );
}

//******************************************************************************
//	オブジェクトの値を設定
//******************************************************************************
void MenuLayer::SetObjectValue( Label Name, float Value )
{
	MenuObject* pObject = GetObject( Name );
	if( pObject ) pObject->SetValue( Value );
}

//******************************************************************************
//	オブジェクトの値を取得
//******************************************************************************
float MenuLayer::GetObjectValue( Label Name ) const
{
	MenuObject* pObject = GetObject( Name );
	if( pObject ) return pObject->GetValue();

	return 0.0f;
}

//******************************************************************************
//	オブジェクトの有効/無効を設定
//******************************************************************************
void MenuLayer::SetObjectEnable( Label Name, bool bEnable )
{
	MenuObject* pObject = GetObject( Name );
	if( pObject ) pObject->SetEnable( bEnable );
}

//******************************************************************************
//	ヘルプテキストの更新
//******************************************************************************
void MenuLayer::UpdateHelpText( void )
{
	if( m_pParam->HelpBoneName.IsEmpty() ) return;

	String256 Str;

	MenuObject* pObject = GetSelectObject();
	if( !m_ReplaceHelpText.empty() )
	{
		Str = m_ReplaceHelpText.c_str();
	}
	else if( pObject )
	{
		Str = pObject->GetHelpText();
	}
	else
	{
		Str = m_pParam->HelpText.c_str();
	}

	//	キー名文字列の置換
	GetConfig()->ReplaceKeyName( Str );

	//	登録
	m_HelpText = Str.c_str();
	m_HelpTextLength = 0;
}

//******************************************************************************
//	ヘルプテキストの差し替え
//******************************************************************************
void MenuLayer::ReplaceHelpText( pstr pText )
{
	if( m_pParam->HelpBoneName.IsEmpty() ) return;

	m_ReplaceHelpText = pText;

	//	更新
	UpdateHelpText();
}

//------------------------------------------------------------------------------
//	ピースの取得
//------------------------------------------------------------------------------
const Piece* MenuLayer::_GetPiece( Label Name ) const
{
	return m_pParent->GetData()->GetPiece( Name );
}

//------------------------------------------------------------------------------
//	テクスチャの取得
//------------------------------------------------------------------------------
const Texture* MenuLayer::_GetTex( Label Name ) const
{
	return m_pParent->GetData()->GetTex( Name );
}

//------------------------------------------------------------------------------
//	インジケート・マーカーパラメータの取得
//------------------------------------------------------------------------------
const INDICATE_MARKER* MenuLayer::_GetMarker( Label BoneName ) const
{
	std::map< Label, INDICATE_MARKER >::const_iterator it = m_pParam->mapMarker.find( BoneName );
	if( it != m_pParam->mapMarker.end() )
	{
		return &(it->second);
	}
	return NULL;
}

//------------------------------------------------------------------------------
//	インジケート・マーカーパラメータの取得
//------------------------------------------------------------------------------
const INDICATE_SPIN* MenuLayer::_GetSpin( Label BoneName ) const
{
	std::map< Label, INDICATE_SPIN >::const_iterator it = m_pParam->mapSpin.find( BoneName );
	if( it != m_pParam->mapSpin.end() )
	{
		return &(it->second);
	}
	return NULL;
}

//------------------------------------------------------------------------------
//	インジケート・線グラフパラメータの取得
//------------------------------------------------------------------------------
const INDICATE_LINE_GRAPH* MenuLayer::_GetLineGraph( Label BoneName ) const
{
	std::map< Label, INDICATE_LINE_GRAPH >::const_iterator it = m_pParam->mapLineGraph.find( BoneName );
	if( it != m_pParam->mapLineGraph.end() )
	{
		return &(it->second);
	}
	return NULL;
}


//------------------------------------------------------------------------------
//	時間イベント更新
//------------------------------------------------------------------------------
void MenuLayer::_UpdateTimeEvent( void )
{
	for( int i = 0; i < (int)m_pParam->vTimeEvent.size(); i++ )
	{
		const MENU_TIME_EVENT& Event = m_pParam->vTimeEvent[i];

		if( Event.Frame == m_FrameCount )
		{
			_ExecAction( Event.Action );
		}
	}
}

//------------------------------------------------------------------------------
//	アクションリスト実行
//------------------------------------------------------------------------------
void MenuLayer::_ExecActionList( const MENU_ACTION_VEC& vAction )
{
	for( int i = 0; i < (int)vAction.size(); i++ )
	{
		_ExecAction( vAction[i] );
	}
}

//------------------------------------------------------------------------------
//	アクション実行
//------------------------------------------------------------------------------
void MenuLayer::_ExecAction( const MENU_ACTION& Action )
{
	switch( Action.Type )
	{
	case MENU_ACTION_OPEN:		m_pParent->OpenLayer( Action.Name );									break;
	case MENU_ACTION_CLOSE:		Action.Name.IsEmpty()? Close() : m_pParent->CloseLayer( Action.Name );	break;

	case MENU_ACTION_ANIME:		m_pSprite->ChangeAnime( Action.Name, Action.Param );					break;
	case MENU_ACTION_NOTIFY:	m_pParent->OnNotify( this, Action.Name, Action.Param );					break;
	case MENU_ACTION_SELECT:	SelectObject( Action.Name );											break;

	case MENU_ACTION_EFFECT:	m_pParent->CreateEffect( Action.Name, m_pSprite->GetBaseMatrix() );		break;
	case MENU_ACTION_SOUND:		m_pParent->PlaySound( Action.Name, Action.Param );						break;

	case MENU_ACTION_BGM_PLAY:	GetApp()->GetBGMMng()->Play( Action.Text.c_str(), Action.Param );		break;
	case MENU_ACTION_BGM_STOP:	GetApp()->GetBGMMng()->Stop();											break;
	case MENU_ACTION_BGM_FADE:	GetApp()->GetBGMMng()->Fade( Action.Param );							break;
	}
}

//------------------------------------------------------------------------------
//	オブジェクトの生成
//------------------------------------------------------------------------------
MenuObject* MenuLayer::_CreateObject( const MENU_OBJECT& Param )
{
	MenuObject* pObject = NULL;

	switch( Param.Type )
	{
	case MENU_OBJECT_SELECTOR:	pObject	= new MenuSelector( this );	break;
	case MENU_OBJECT_SLIDER:	pObject	= new MenuSlider( this );	break;
	case MENU_OBJECT_LIST:		pObject	= new MenuList( this );		break;
	case MENU_OBJECT_SCROLL:	pObject	= new MenuScroll( this );	break;
	default:					pObject = new MenuObject( this );	break;
	}
	
	MEM_CHECK( pObject );

	pObject->Init( &Param );
	return pObject;
}

//------------------------------------------------------------------------------
//	ヘルプテキストの表示長さを更新
//------------------------------------------------------------------------------
void MenuLayer::_UpdateHelpTextLength( void )
{
	if( m_pParam->HelpBoneName.IsEmpty() ) return;

	if( m_HelpTextLength < (int)m_HelpText.length() )
	{
		char Ch = m_HelpText[m_HelpTextLength];

		if( IsEmQuad( Ch ) )
		{
			m_HelpTextLength += 2;
		}
		else
		{
			m_HelpTextLength += 1;
		}

		String256 Str = m_HelpText.c_str();

		if( m_HelpTextLength < (int)m_HelpText.length() )
		{
			Str.c_str()[m_HelpTextLength] = '\0';
		}

		m_pSprite->SetBoneText( m_pParam->HelpBoneName, Str.c_str() );
		m_pSprite->SetBoneColor( m_pParam->HelpBoneName,
							Color::Gray(255, ((float)m_HelpTextLength / (float)m_HelpText.length()) * 255 ) );
	}
}