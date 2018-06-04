#include "stdafx.h"
#include "App.h"

#include "Resource/ResourceManager.h"
#include "Collide/CollideSystem.h"
#include "Effect/EffectManager.h"
#include "BGM/BGMManager.h"
#include "SaveData/SaveDataManager.h"

#include "Scene/SceneManager.h"
#include "Scene/TitleScene.h"
#include "Scene/WorldScene.h"
#include "Scene/TestScene.h"
#include "Scene/HitTestScene.h"


///////////////////////////////////////////////////////////////////////////////
//
//	アプリケーション
//
///////////////////////////////////////////////////////////////////////////////
App::App( void )
{
	m_pSceneManager		= NULL;
	m_pResourceManager	= NULL;
	m_pEffectManager	= NULL;
	m_pBGMManager		= NULL;
	m_pSaveDataManager	= NULL;
}
App::~App()
{
	//	リセット通知の登録解除
	GetGraphix()->RemoveResetListener( m_pResourceManager );

	//	サウンド全停止
	GetTone()->StopAllSound();

	SafeDelete( m_pSceneManager );
	SafeDelete( m_pEffectManager );
	SafeDelete( m_pBGMManager );
	SafeDelete( m_pSaveDataManager );

	ExitCollideSystem();

	SafeDelete( m_pResourceManager );
}

//*****************************************************************************
//	初期化
//*****************************************************************************
void App::Init( void )
{
	//	リソース管理
	m_pResourceManager	= new ResourceManager;
	MEM_CHECK( m_pResourceManager );

	GetGraphix()->AddResetListener( m_pResourceManager );

	//	当たり判定
	InitCollideSystem();

	//	エフェクト
	m_pEffectManager = new EffectManager;
	MEM_CHECK( m_pEffectManager );

	//	BGM
	m_pBGMManager = new BGMManager;
	MEM_CHECK( m_pBGMManager );

	//	シーン
	m_pSceneManager = new SceneManager;
	MEM_CHECK( m_pSceneManager );

	//	セーブデータ
	m_pSaveDataManager = new SaveDataManager();
	MEM_CHECK( m_pSaveDataManager );

	m_pSaveDataManager->Load();

	m_pSceneManager->ChangeScene( new TitleScene( "Title" ) );
//	m_pSceneManager->ChangeScene( new TestScene( "Test" ) );
//	m_pSceneManager->ChangeScene( new HitTestScene( "HitTest" ) );
}

//*****************************************************************************
//	更新
//*****************************************************************************
void App::Update( void )
{
	m_pResourceManager->Update();

	m_pEffectManager->Update();
	m_pBGMManager->Update();
	m_pSceneManager->Update();

	m_pEffectManager->Draw();

	GetCollideSystem()->Update();
}

//******************************************************************************
//	終了したかどうか
//******************************************************************************
bool App::IsEnd( void ) const
{
	return m_pSceneManager->IsExit();
}

//******************************************************************************
//	例外の処理
//******************************************************************************
void App::OnException( pstr pDirPath )
{
	if( m_pSceneManager ) m_pSceneManager->OnException( pDirPath );
}

//-----------------------------------------------------------------------------
//	インスタンス管理
//-----------------------------------------------------------------------------
namespace
{
	App* s_pApp	= NULL;
}

//*****************************************************************************
//	インスタンス初期化
//*****************************************************************************
void InitApp( void )
{
	if( !s_pApp )
	{
		s_pApp = new App;
		MEM_CHECK( s_pApp );

		s_pApp->Init();
	}
}

//*****************************************************************************
//	インスタンス終了
//*****************************************************************************
void ExitApp( void )
{
	SafeDelete( s_pApp );
}

//*****************************************************************************
//	インスタンス取得
//*****************************************************************************
App* GetApp( void )
{
	return s_pApp;
}