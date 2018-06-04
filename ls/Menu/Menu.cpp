#include "stdafx.h"
#include "Menu.h"

#include "MenuData.h"
#include "MenuLayer.h"
#include "MenuListener.h"

#include "App/App.h"

#include "SpriteFrame/SpriteFrame.h"
#include "Effect/EffectManager.h"
#include "Controller/SystemController.h"
#include "BGM/BGMManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	メニュー
//
////////////////////////////////////////////////////////////////////////////////
Menu::Menu( MenuListener* pListener )
{
	m_pData			= NULL;
	m_pListener		= pListener;
	m_bVisible		= true;
	m_FrameCount	= 0;
}

Menu::~Menu()
{
	//	終了アクション
	_ExecActionList( m_pData->GetParam().vCloseAction );

	DeleteVec( m_vLayer );
	DeleteVec( m_vInactiveLayer );
}

//******************************************************************************
//	初期化
//******************************************************************************
void Menu::Init( const MenuData* pData, Matrix3 Mat )
{
	_ASSERT( pData );

	m_pData			= pData;
	m_Matrix		= Mat;
	m_FrameCount	= 0;

	//	ルートレイヤー登録
	if( !m_pData->GetParam().RootLayer.IsEmpty() )
	{
		_PushLayer( m_pData->GetParam().RootLayer );
	}

	//	初期化アクション
	_ExecActionList( m_pData->GetParam().vInitAction );
}

//******************************************************************************
//	更新
//******************************************************************************
void Menu::Update( void )
{
	SystemController Ctrl;

	//	時間イベント
	_UpdateTimeEvent();

	//	新レイヤーの追加
	if( !m_NextLayer.IsEmpty() )
	{
		_PushLayer( m_NextLayer );
		m_NextLayer.Clear();
	}

	//	非動作レイヤー更新
	for( MenuLayerVec::iterator it = m_vInactiveLayer.begin(); it != m_vInactiveLayer.end(); )
	{
		(*it)->Update();

		if( (*it)->IsDelete() )
		{
			//	リスナーに通知する
			if( m_pListener ) m_pListener->OnCloseLayer( *it );

			//	削除
			SafeDelete( *it );
			it = m_vInactiveLayer.erase( it );
		}
		else
		{
			++it;
		}
	}

	//	レイヤー更新
	for( MenuLayerVec::iterator it = m_vLayer.begin(); it != m_vLayer.end(); ++it )
	{
		(*it)->Update();
	}

	//	現在レイヤーの更新
	MenuLayer* pLayer = GetCurLayer();
	if( pLayer )
	{
		pLayer->UpdateCurrent();

		//	削除
		if( pLayer->IsDelete() )
		{
			//	リスナーに通知する
			if( m_pListener ) m_pListener->OnCloseLayer( pLayer );

			SafeDelete( pLayer );
			m_vLayer.pop_back();

			//	新しく現在レイヤーに移行
			MenuLayer* pNewLayer = GetCurLayer();
			if( pNewLayer )
			{
				pNewLayer->OnMoveCurrent();
			}
		}
	}

	//	キーイベント
	for( MENU_KEY_EVENT_VEC::const_iterator it = m_pData->GetParam().vKeyEvent.begin(); it != m_pData->GetParam().vKeyEvent.end(); ++it )
	{
		bool bInput	= it->Type == MENU_KEY_EVENT_ANY ?
						Ctrl.IsAnyInput( INPUT_TRIG ) :
						Ctrl.IsInput( CONTROLLER_SYSTEM_KEY(it->Type), INPUT_TRIG );

		if( bInput )
		{
			_ExecActionList( it->vAction );
		}
	}

	//	カウントアップ
	m_FrameCount++;
}

//******************************************************************************
//	描画
//******************************************************************************
void Menu::Draw( const Matrix3& OffsetMat )
{
	for( MenuLayerVec::iterator it = m_vInactiveLayer.begin(); it != m_vInactiveLayer.end(); ++it )
	{
		(*it)->Draw( OffsetMat );
	}
	for( MenuLayerVec::iterator it = m_vLayer.begin(); it != m_vLayer.end(); ++it )
	{
		(*it)->Draw( OffsetMat );
	}
}

//******************************************************************************
//	レイヤー取得
//******************************************************************************
MenuLayer* Menu::GetLayer( Label Name ) const
{
	for( int i = 0; i < (int)m_vLayer.size(); i++ )
	{
		MenuLayer* pLayer = m_vLayer[i];
		if( pLayer->GetName() == Name )
		{
			return pLayer;
		}
	}

	for( int i = 0; i < (int)m_vInactiveLayer.size(); i++ )
	{
		MenuLayer* pLayer = m_vInactiveLayer[i];
		if( pLayer->GetName() == Name )
		{
			return pLayer;
		}
	}

	return NULL;
}

//******************************************************************************
//	現在レイヤー取得
//******************************************************************************
MenuLayer* Menu::GetCurLayer( void ) const
{
	if( m_vLayer.empty() ) return NULL;

	return m_vLayer.back();
}

//******************************************************************************
//	レイヤーを開く
//******************************************************************************
void Menu::OpenLayer( Label Name )
{
	const MENU_LAYER* pParam = m_pData->GetLayer( Name );
	if( !pParam ) return;
	
	//	非動作
	if( pParam->bInactive )
	{
		_OpenInactiveLayer( Name );
	}
	//	動作
	else
	{
		//	直接追加
		if( IsEmpty() )
		{
			_PushLayer( Name );
		}
		//	予約
		else
		{
			_ASSERT( m_NextLayer.IsEmpty() );

			m_NextLayer = Name;
		}
	}
}

//******************************************************************************
//	指定レイヤーを閉じる
//******************************************************************************
void Menu::CloseLayer( Label Name )
{
	MenuLayer* pLayer = GetLayer( Name );
	if( pLayer ) pLayer->Close();
}

//******************************************************************************
//	現在レイヤーを閉じる
//******************************************************************************
void Menu::CloseCurLayer( void )
{
	MenuLayer* pLayer = GetCurLayer();
	if( pLayer ) pLayer->Close();
}

//******************************************************************************
//	表示の有無を設定
//******************************************************************************
void Menu::SetVisible( bool bVisible )
{
	m_bVisible = bVisible;

	for( int i = 0; i < (int)m_vInactiveLayer.size(); i++ )
	{
		m_vInactiveLayer[i]->UpdateVisible();
	}

	for( int i = 0; i < (int)m_vLayer.size(); i++ )
	{
		m_vLayer[i]->UpdateVisible();
	}
}

//******************************************************************************
//	リスナーへ通知（メニュー）
//******************************************************************************
void Menu::OnNotify( Label Name, float Param )
{
	if( m_pListener ) m_pListener->OnMenuNotify( this, Name, Param );
}

//******************************************************************************
//	リスナーへ通知（レイヤー）
//******************************************************************************
void Menu::OnNotify( MenuLayer* pLayer, Label Name, float Param )
{
	if( m_pListener ) m_pListener->OnLayerNotify( pLayer, Name, Param );
}

//******************************************************************************
//	リスナーへ通知（オブジェクト）
//******************************************************************************
void Menu::OnNotify( MenuObject* pObject, Label Name, float Param )
{
	if( m_pListener ) m_pListener->OnObjectNotify( pObject, Name, Param );
}

//******************************************************************************
//	リスナーへ通知（オブジェクト値変更）
//******************************************************************************
void Menu::OnChangeValue( MenuObject* pObject )
{
	if( m_pListener ) m_pListener->OnObjectChangeValue( pObject );
}

//******************************************************************************
//	リスナーへ通知（オブジェクト選択）
//******************************************************************************
void Menu::OnSelectObject( MenuObject* pObject, MenuObject* pPrevObject )
{
	if( m_pListener ) m_pListener->OnObjectSelect( pObject, pPrevObject );
}

//******************************************************************************
//	サウンドの再生
//******************************************************************************
Sound* Menu::PlaySound( Label Name, float Vol )
{
	const SoundFile* pFile = m_pData->GetSoundFile( Name );
	if( !pFile ) return NULL;

	Sound* pSound = GetTone()->PlaySound( pFile, SOUND_CHANNEL_SE, false );
	if( !pSound ) return NULL;

	pSound->SetVolume( Vol );

	return pSound;
}

//******************************************************************************
//	エフェクトの生成
//******************************************************************************
Effect* Menu::CreateEffect( Label Name, const Matrix3& Mat )
{
	const EffectFile* pFile = m_pData->GetEffectFile( Name );
	if( !pFile ) return NULL;

	return GetApp()->GetEffectMng()->CreateEffect( pFile, Mat );
}

//------------------------------------------------------------------------------
//	レイヤー追加
//------------------------------------------------------------------------------
void Menu::_PushLayer( Label Name )
{
	if( Name.IsEmpty() ) return;

	//	すでに開いている
	{
		MenuLayer* pLayer = GetLayer( Name );
		if( pLayer ) return;
	}

	const MENU_LAYER* pParam = m_pData->GetLayer( Name );
	if( !pParam ) return;
	
	//	以前の現在レイヤーを背面へ
	MenuLayer* pOldLayer = GetCurLayer();
	if( pOldLayer )
	{
		pOldLayer->OnMoveBack();
	}

	//	新規作成
	MenuLayer* pLayer = new MenuLayer( this );
	MEM_CHECK( pLayer );

	m_vLayer.push_back( pLayer );
	pLayer->Init( pParam );

	//	リスナーに通知する
	if( m_pListener ) m_pListener->OnOpenLayer( pLayer );
}

//------------------------------------------------------------------------------
//	非動作レイヤーを開く
//------------------------------------------------------------------------------
void Menu::_OpenInactiveLayer( Label Name )
{
	if( Name.IsEmpty() ) return;

	//	すでに開いている
	{
		MenuLayer* pLayer = GetLayer( Name );
		if( pLayer ) return;
	}

	const MENU_LAYER* pParam = m_pData->GetLayer( Name );
	if( !pParam ) return;
	
	//	新規作成
	MenuLayer* pLayer = new MenuLayer( this );
	MEM_CHECK( pLayer );

	pLayer->Init( pParam );
	m_vInactiveLayer.push_back( pLayer );

	//	リスナーに通知する
	if( m_pListener ) m_pListener->OnOpenLayer( pLayer );
}

//------------------------------------------------------------------------------
//	時間イベント更新
//------------------------------------------------------------------------------
void Menu::_UpdateTimeEvent( void )
{
	for( int i = 0; i < (int)m_pData->GetParam().vTimeEvent.size(); i++ )
	{
		const MENU_TIME_EVENT& Event = m_pData->GetParam().vTimeEvent[i];

		if( Event.Frame == m_FrameCount )
		{
			_ExecAction( Event.Action );
		}
	}
}

//------------------------------------------------------------------------------
//	アクションリスト実行
//------------------------------------------------------------------------------
void Menu::_ExecActionList( const MENU_ACTION_VEC& vAction )
{
	for( int i = 0; i < (int)vAction.size(); i++ )
	{
		_ExecAction( vAction[i] );
	}
}

//------------------------------------------------------------------------------
//	アクション実行
//------------------------------------------------------------------------------
void Menu::_ExecAction( const MENU_ACTION& Action )
{
	MenuLayer* pLayer = GetCurLayer();

	switch( Action.Type )
	{
	case MENU_ACTION_OPEN:		OpenLayer( Action.Name );														break;
	case MENU_ACTION_CLOSE:		Action.Name.IsEmpty()? CloseCurLayer() : CloseLayer( Action.Name );				break;

	case MENU_ACTION_ANIME:		if( pLayer ) pLayer->GetSprite()->ChangeAnime( Action.Name, Action.Param );		break;
	case MENU_ACTION_NOTIFY:	OnNotify( Action.Name, Action.Param );											break;
	case MENU_ACTION_SELECT:	if( pLayer ) pLayer->SelectObject( Action.Name );								break;

	case MENU_ACTION_EFFECT:	CreateEffect( Action.Name, m_Matrix );											break;
	case MENU_ACTION_SOUND:		PlaySound( Action.Name, Action.Param );											break;
	case MENU_ACTION_BGM_PLAY:	GetApp()->GetBGMMng()->Play( Action.Text.c_str(), Action.Param );				break;
	case MENU_ACTION_BGM_STOP:	GetApp()->GetBGMMng()->Stop();													break;
	case MENU_ACTION_BGM_FADE:	GetApp()->GetBGMMng()->Fade( Action.Param );									break;
	}
}