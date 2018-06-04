#include "stdafx.h"
#include "StageSelectScene.h"
#include "TitleScene.h"
#include "PracticeScene.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"
#include "SaveData/SaveDataManager.h"

#include "SpriteFrame/SpriteFrame.h"
#include "Menu/Menu.h"
#include "Menu/MenuData.h"
#include "Menu/MenuLayer.h"
#include "Menu/Object/MenuObject.h"


namespace
{
	pstr		MENU_PATH		= "Data/Menu/AreaSelect.xml";

	pstr		SCENE_PRACTICE	= "Practice";
	pstr		SCENE_TITLE		= "Title";

	const Label	NOTIFY_EXIT		= "Exit";
	const Label	NOTIFY_CHANGE	= "Change";

	const Label	ANIME_WAIT		= "Wait";
	const Label	ANIME_WAIT_FADE	= "WaitFade";

	const Label	BONE_NAME		= "Name";
	const Label	BONE_CAPTION	= "Caption";
	const Label	BONE_PICT		= "Pict";

	struct STAGE_STATE
	{
		int		Arrival;
		Label	ObjName;
		int		Stage;
		Label	State;
		int		PieceIndex;
		pstr	pCaption;
		pstr	pName;
	};

	const STAGE_STATE	STAGE_STATE_TABLE[] =
	{
		{ 10, "10", 0, "Boot",		0,	"[Ancient]",								"Obsidian Ruins." },
		{ 11, "11", 0, "Area11",	1,	"Imperial Guard.",							"Knight of Jet." },
		{ 12, "12", 0, "Area12",	2,	"[Ancient]",								"Quiescency Street." },
		{ 13, "13", 0, "Area13",	3,	"The Last Empress.",						"AUTHA" },

		{ 20, "20", 1, "Init",		4,	"[Industrial]",								"Endless Arsenal." },
		{ 21, "21", 1, "Area21",	5,	"Defense Core.",							"Gigas Wall." },
		{ 22, "22", 1, "Area22",	6,	"[Industrial]",								"Blockade Corridor." },
		{ 23, "23", 1, "Area23",	7,	"Arsenal Transaction\nIntegrate System.",	"A.T.I.S." },

		{ 30, "30", 2, "Init",		8,	"[Eschata]",								"Dusty Ocean." },
		{ 31, "31", 2, "Area31",	9,	"Exodus Ark.",								"Esperanza" },

		{ 40, "40", 3, "Init",		10,	"[Relict]",									"Starlight Wreckage." },
		{ 41, "41", 3, "Area41",	11,	"Divine Servant.",							"Sacred Keeper" },
		{ 42, "42", 3, "Area42",	12,	"[Relict]",									"Eternal Mausoleum." },
		{ 43, "43", 3, "Area43",	13,	"Eternity Sword Incarnation.",				"ERASE" },

		{ 99, "50", 4, "Init",		14,	"[Origin]",									"Resteraiser." },
		{ 99, "51", 4, "Init",		14,	"[Origin]",									"Resteraiser." },
	};

	//------------------------------------------------------------------------------
	//	メニューオブジェクト名からステージ状態情報を得る
	//------------------------------------------------------------------------------
	const STAGE_STATE* _ObjNameToStageState( Label ObjName )
	{
		for( int i = 0; i < ArrayNum( STAGE_STATE_TABLE ); i++ )
		{
			const STAGE_STATE& Info = STAGE_STATE_TABLE[i];
			if( Info.ObjName == ObjName )
			{
				return &Info;
			}
		}
		return NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	ステージ選択シーン
//
///////////////////////////////////////////////////////////////////////////////
StageSelectScene::StageSelectScene( Label Name, pstr pWorldPath, int PrevStage, Label PrevState )
:Scene( Name )
{
	m_pMenuRes		= NULL;
	m_pMenu			= NULL;

	m_WorldPath		= pWorldPath;
	m_PrevStage		= PrevStage;
	m_PrevState		= PrevState;
}
StageSelectScene::~StageSelectScene()
{
	SafeDelete( m_pMenu );
	SafeRelease( m_pMenuRes );
}

//*****************************************************************************
//	初期化
//*****************************************************************************
void StageSelectScene::OnInit( void )
{
	//	メニュー
	m_pMenuRes = GetApp()->GetResMng()->LoadResource<MenuResource>( MENU_PATH );
	if( m_pMenuRes && m_pMenuRes->GetData() )
	{
		m_pMenu = new Menu( this );
		MEM_CHECK( m_pMenu );

		m_pMenu->Init( m_pMenuRes->GetData(), Matrix3::Trans( 0, 0 ) );
	}
	_ASSERT( m_pMenu );

	m_SelectNotify.Clear();

	//	背景色変更
	P5::GetGraphix()->GetDevice()->SetScreenColor( Color(0,0,0) );

	//	前の設定に応じて、メニューの選択を変える
	MenuLayer* pLayer = m_pMenu->GetCurLayer();
	if( pLayer )
	{
		pLayer->SelectObject( _StageStateToObjName( m_PrevStage, m_PrevState ) );
	}
}

//*****************************************************************************
//	更新
//*****************************************************************************
void StageSelectScene::Update( void )
{
	if( !m_pMenu ) return;

	m_pMenu->Update();

	//	メニュー項目の更新
	_UpdateMenu();

	if( m_pMenu->IsEmpty() )
	{
		//	ステージ決定
		const STAGE_STATE* pInfo = _ObjNameToStageState( m_SelectNotify );
		if( pInfo )
		{
			ChangeScene( new PracticeScene( SCENE_PRACTICE, m_WorldPath.c_str(), pInfo->Stage, pInfo->State ) );
			return;
		}

		//	タイトルへ戻る
		else
		{
			ChangeScene( new TitleScene( SCENE_TITLE, GetName() ) );
			return;
		}
	}

	m_pMenu->Draw( Matrix3::Identity() );
}

//******************************************************************************
//	メニューからの通知：レイヤー開く
//******************************************************************************
void StageSelectScene::OnOpenLayer( MenuLayer* pLayer )
{
	//	名前と写真非表示
	pLayer->SetText( BONE_NAME, "" );
	pLayer->SetBoneVisible( BONE_PICT, false );
}

//******************************************************************************
//	メニューからの通知：レイヤー通知
//******************************************************************************
void StageSelectScene::OnLayerNotify( MenuLayer* pLayer, Label Name, float Param )	
{
	m_SelectNotify = Name;
}

//******************************************************************************
//	メニューからの通知：オブジェクト通知
//******************************************************************************
void StageSelectScene::OnObjectNotify( MenuObject* pObject, Label Name, float Param )
{
	m_SelectNotify = Name;
}

//******************************************************************************
//	メニューからの通知：オブジェクト選択
//******************************************************************************
void StageSelectScene::OnObjectSelect( MenuObject* pObject, MenuObject* pPrevObject )
{
	MenuLayer* pLayer = m_pMenu->GetCurLayer();
	if( !pLayer ) return;
	
	//	アニメ
	if( pLayer->GetAnimeName() == ANIME_WAIT ||
		pLayer->GetAnimeName() == ANIME_WAIT_FADE )
	{		
		if( !pLayer->GetSprite()->IsAnimeLoopFrame() )
		{
			const STAGE_STATE* pInfo = _ObjNameToStageState( pObject->GetName() );
			if( pInfo )
			{
				//	メニューの説明文と写真更新
				pLayer->SetText( BONE_NAME, pInfo->pName );
				pLayer->SetText( BONE_CAPTION, pInfo->pCaption );
				pLayer->SetBoneVisible( BONE_PICT, true );
				pLayer->SetPieceIndex( BONE_PICT, pInfo->PieceIndex );
			}
			else
			{
				//	対象が見つからなかった
				pLayer->SetText( BONE_NAME, "" );
				pLayer->SetText( BONE_CAPTION, "" );
				pLayer->SetBoneVisible( BONE_PICT, false );
			}
			pLayer->ChangeAnime( ANIME_WAIT_FADE, 0 );
		}
		else
		{
			pLayer->ChangeAnime( ANIME_WAIT, 0 );
		}
	}
}

//------------------------------------------------------------------------------
//	メニュー項目のON/OFFを更新
//------------------------------------------------------------------------------
void StageSelectScene::_UpdateMenu( void )
{
	MenuLayer* pLayer = m_pMenu->GetCurLayer();
	if( !pLayer ) return;

	//	有効無効
	for( int i = 0; i < ArrayNum( STAGE_STATE_TABLE ); i++ )
	{
		const STAGE_STATE& Info = STAGE_STATE_TABLE[i];

		bool bEnable = Info.Arrival <= GetApp()->GetSaveDataMng()->GetSaveData().Arrival;

		pLayer->SetObjectEnable( Info.ObjName, bEnable );
		pLayer->SetBoneVisible( Info.ObjName, bEnable );
	}

	//	名前と写真
	if( pLayer->GetSprite()->GetAnimeNotify( NOTIFY_CHANGE ) )
	{
		MenuObject* pObject = pLayer->GetSelectObject();
		if( pObject )
		{
			const STAGE_STATE* pInfo = _ObjNameToStageState( pObject->GetName() );
			if( pInfo )
			{
				//	メニューの説明文と写真更新
				pLayer->SetText( BONE_NAME, pInfo->pName );
				pLayer->SetText( BONE_CAPTION, pInfo->pCaption );
				pLayer->SetBoneVisible( BONE_PICT, true );
				pLayer->SetPieceIndex( BONE_PICT, pInfo->PieceIndex );
			}
			else
			{
				//	対象が見つからなかった
				pLayer->SetText( BONE_NAME, "" );
				pLayer->SetText( BONE_CAPTION, "" );
				pLayer->SetBoneVisible( BONE_PICT, false );
			}
		}
	}
}
	
//------------------------------------------------------------------------------
//	ステージとステートから、メニューOBJ名を得る
//------------------------------------------------------------------------------
Label StageSelectScene::_StageStateToObjName( int Stage, Label State ) const
{
	if( State.IsEmpty() ) return Label();

	for( int i = 0; i < ArrayNum( STAGE_STATE_TABLE ); i++ )
	{
		const STAGE_STATE& Info = STAGE_STATE_TABLE[i];
		if( Info.Stage == Stage && Info.State == State )
		{
			return Info.ObjName;
		}
	}
	return Label();
}
