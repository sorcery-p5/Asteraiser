#include "stdafx.h"
#include "TitleScene.h"
#include "WorldScene.h"
#include "StageSelectScene.h"
#include "TutorialScene.h"
#include "ReplaySelectScene.h"
#include "ReplayScene.h"
#include "RecordScene.h"
#include "ConfigScene.h"
#include "TestScene.h"
#include "EndingScene.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"
#include "SaveData/SaveDataManager.h"
#include "Controller/SystemController.h"
#include "BGM/BGMManager.h"

#include "Menu/Menu.h"
#include "Menu/MenuData.h"
#include "Menu/MenuLayer.h"

namespace
{
	pstr		MENU_PATH				= "Data/Menu/Title.xml";
	pstr		WORLD_PATH				= "Data/World/World.xml";

	pstr		DEMO_REPLAY_PATH_1		= "Data/Replay/Demo1.rep";
	pstr		DEMO_REPLAY_PATH_2		= "Data/Replay/Demo2.rep";
	pstr		DEMO_REPLAY_PATH_3		= "Data/Replay/Demo3.rep";

	pstr		SCENE_WORLD				= "World";
	pstr		SCENE_STAGE_SELECT		= "StageSelect";
	pstr		SCENE_TUTORIAL			= "Tutorial";
	pstr		SCENE_REPLAY_SELECT		= "ReplaySelect";
	pstr		SCENE_RECORD			= "Record";
	pstr		SCENE_CONFIG			= "Config";
	pstr		SCENE_ENDING			= "Ending";
	pstr		SCENE_TEST				= "Test";
	pstr		SCENE_DEMO1				= "Demo1";
	pstr		SCENE_DEMO2				= "Demo2";
	pstr		SCENE_DEMO3				= "Demo3";
	pstr		SCENE_DEMO_TUTORIAL		= "DemoTuto";

	const Label	LAYER_OPENING			= "Opening";
	const Label	LAYER_TITLE				= "Title";

	const Label	NOTIFY_START			= "Start";
	const Label	NOTIFY_STAGE_SELECT		= "StageSelect";
	const Label	NOTIFY_TUTORIAL			= "Tutorial";
	const Label	NOTIFY_REPLAY_SELECT	= "ReplaySelect";
	const Label	NOTIFY_RECORD			= "Record";
	const Label	NOTIFY_CONFIG			= "Config";
	const Label	NOTIFY_EXIT				= "Exit";
	const Label	NOTIFY_DEMO1			= "Demo1";
	const Label	NOTIFY_DEMO2			= "Demo2";
	const Label	NOTIFY_DEMO3			= "Demo3";
	const Label	NOTIFY_DEMO_TUTORIAL	= "DemoTuto";

	const int	DEMO_TIME				= 60 * 30;
}


///////////////////////////////////////////////////////////////////////////////
//
//	タイトルシーン
//
///////////////////////////////////////////////////////////////////////////////
TitleScene::TitleScene( Label Name, Label PrevScene )
:Scene( Name )
{
	m_pMenuRes		= NULL;
	m_pMenu			= NULL;
	m_PrevScene		= PrevScene;
	m_DemoCount		= 0;
}
TitleScene::~TitleScene()
{
	SafeDelete( m_pMenu );
	SafeRelease( m_pMenuRes );
}

//*****************************************************************************
//	初期化
//*****************************************************************************
void TitleScene::OnInit( void )
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

	//	オープニングから開始
	if( m_PrevScene.IsEmpty() ||
		m_PrevScene == Label(SCENE_DEMO3) )
	{
		m_pMenu->OpenLayer( LAYER_OPENING );
	}
	//	タイトルから開始
	else
	{
		m_pMenu->OpenLayer( LAYER_TITLE );

		//	前のシーンに応じて、メニューの選択を変える
		MenuLayer* pLayer = m_pMenu->GetCurLayer();
		if( pLayer )
		{
			pLayer->SelectObject( m_PrevScene );
		}
	}
}

//*****************************************************************************
//	更新
//*****************************************************************************
void TitleScene::Update( void )
{
	if( !m_pMenu ) return;

	//	項目のON/OFF
	_UpdateMenuEnable();

	m_pMenu->Update();

	//	デモ用時間カウント
	_UpdateDemoCount();

	if( m_pMenu->IsEmpty() )
	{
		//	スタート
		if( m_SelectNotify == NOTIFY_START )
		{
			//	ステージ開始
			ChangeScene( new WorldScene( SCENE_WORLD, WORLD_PATH ) );
			return;
		}

		//	ステージセレクト
		else if( m_SelectNotify == NOTIFY_STAGE_SELECT )
		{
			ChangeScene( new StageSelectScene( SCENE_STAGE_SELECT, WORLD_PATH, 0, Label() ) );
			return;
		}

		//	チュートリアル
		else if( m_SelectNotify == NOTIFY_TUTORIAL )
		{
			ChangeScene( new TutorialScene( SCENE_TUTORIAL ) );
			return;
		}

		//	リプレイ
		else if( m_SelectNotify == NOTIFY_REPLAY_SELECT )
		{
			ChangeScene( new ReplaySelectScene( SCENE_REPLAY_SELECT, WORLD_PATH, "" ) );
			return;
		}

		//	プレイ記録
		else if( m_SelectNotify == NOTIFY_RECORD )
		{
			ChangeScene( new RecordScene( SCENE_RECORD ) );
			return;
		}

		//	コンフィグ
		else if( m_SelectNotify == NOTIFY_CONFIG )
		{
			ChangeScene( new ConfigScene( SCENE_CONFIG ) );
			return;
		}

		//	デモ１
		else if( m_SelectNotify == NOTIFY_DEMO1 )
		{
			ChangeScene( new ReplayScene( SCENE_DEMO1, WORLD_PATH, DEMO_REPLAY_PATH_1, 0 ) );
			return;
		}

		//	デモ２
		else if( m_SelectNotify == NOTIFY_DEMO2 )
		{
			ChangeScene( new ReplayScene( SCENE_DEMO2, WORLD_PATH, DEMO_REPLAY_PATH_2, 1 ) );
			return;
		}

		//	デモ３
		else if( m_SelectNotify == NOTIFY_DEMO3 )
		{
			ChangeScene( new ReplayScene( SCENE_DEMO3, WORLD_PATH, DEMO_REPLAY_PATH_3, 2 ) );
			return;
		}
		
		//	デモチュートリアル
		else if( m_SelectNotify == NOTIFY_DEMO_TUTORIAL )
		{
			ChangeScene( new TutorialScene( SCENE_DEMO_TUTORIAL ) );
			return;
		}

		//	終了
		else if( m_SelectNotify == NOTIFY_EXIT )
		{
			Exit();
			return;
		}
	}
	
#ifdef _DEBUG
	//	テスト用
	if( P5::GetSign()->GetKeyboard()->IsKeyInput( VK_ESCAPE, INPUT_TRIG ) )
	{
		ChangeScene( new TestScene( SCENE_TEST ) );
		return;
	}
	if( P5::GetSign()->GetKeyboard()->IsKeyInput( VK_PAUSE, INPUT_TRIG ) )
	{
		ChangeScene( new WorldScene( SCENE_WORLD, "Data/World/Test.xml" ) );
		return;
	}
	if( P5::GetSign()->GetKeyboard()->IsKeyInput( VK_HOME, INPUT_TRIG ) )
	{
		SAVEDATA_SCORE				Score;
		SCORE_STAGE_RECORD_VEC		Record;
		ChangeScene( new EndingScene( SCENE_ENDING, Score, Record ) );
		return;
	}
#endif

	m_pMenu->Draw( Matrix3::Identity() );
}

//******************************************************************************
//	メニューからの通知
//******************************************************************************
void TitleScene::OnObjectNotify( MenuObject* pObject, Label Name, float Param )
{
	m_SelectNotify = Name;
}

//------------------------------------------------------------------------------
//	項目のON/OFF切り替え
//------------------------------------------------------------------------------
void TitleScene::_UpdateMenuEnable( void )
{
	MenuLayer* pLayer = m_pMenu->GetCurLayer();
	if( !pLayer || pLayer->GetName() != LAYER_TITLE ) return;

	//	ステージ選択
	bool bStageSelect = GetApp()->GetSaveDataMng()->GetSaveData().Arrival > 0;
	pLayer->SetObjectEnable( NOTIFY_STAGE_SELECT, bStageSelect );

	pLayer->SetColor( NOTIFY_STAGE_SELECT, bStageSelect? Color::White() : Color::Gray(128) );

#if DEMO_VER == DEMO_PLAY_VER
	//	デモ用：リプレイ・コンフィグ封印
	pLayer->SetObjectEnable( NOTIFY_REPLAY_SELECT, false );
	pLayer->SetObjectEnable( NOTIFY_CONFIG, false );
	pLayer->SetColor( NOTIFY_REPLAY_SELECT, Color::Gray(128) );
	pLayer->SetColor( NOTIFY_CONFIG, Color::Gray(128) );

	//	バージョン表記
	pLayer->SetText( "Version", "Demo Version" );

#endif
}

//------------------------------------------------------------------------------
//	デモ時間のカウント
//------------------------------------------------------------------------------
void TitleScene::_UpdateDemoCount( void )
{
	MenuLayer* pLayer = m_pMenu->GetCurLayer();
	if( !pLayer ) return;

	SystemController Ctrl;
	if( pLayer->GetName() == LAYER_TITLE &&
		!Ctrl.IsAnyInput( INPUT_TRIG ) && Ctrl.GetDirBit( INPUT_TRIG ) == 0 )
	{
		m_DemoCount++;

		if( m_DemoCount > DEMO_TIME )
		{
			m_DemoCount = 0;
			_ChangeDemoScene();
		}
	}
	else
	{
		m_DemoCount	= 0;
	}
}

//------------------------------------------------------------------------------
//	デモ開始
//------------------------------------------------------------------------------
void TitleScene::_ChangeDemoScene( void )
{
	//	デモ１→デモチュートリアル
	if( m_PrevScene == Label(SCENE_DEMO1) )
	{
		m_SelectNotify = NOTIFY_DEMO_TUTORIAL;
	}
	//	デモチュートリアル→デモ２
	else if( m_PrevScene == Label(SCENE_DEMO_TUTORIAL) )
	{
		m_SelectNotify = NOTIFY_DEMO2;
	}
	//	デモ２→デモ３
	else if( m_PrevScene == Label(SCENE_DEMO2) )
	{
		m_SelectNotify = NOTIFY_DEMO3;
	}
	//	デモ３or最初→デモ１
	else
	{
		m_SelectNotify = NOTIFY_DEMO1;
	}

	//	メニュー閉じ
	m_pMenu->CloseCurLayer();

	//	BGM閉じ
	GetApp()->GetBGMMng()->Fade( 15 );
}