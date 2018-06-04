#include "stdafx.h"
#include "WorldScene.h"
#include "TitleScene.h"
#include "GameoverScene.h"
#include "EndingScene.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"
#include "SaveData/SaveDataManager.h"

#include "World/World.h"
#include "World/WorldData.h"
#include "Score/ScoreManager.h"
#include "Replay/ReplayManager.h"

namespace
{
	const Label	TITLE_SCENE			= "Title";
	const Label	GAMEOVER_SCENE		= "Gameover";
	const Label	ENDING_SCENE		= "Ending";
	
	pstr		REPLAY_SAVE_DIR		= "Replay/Save";
}


///////////////////////////////////////////////////////////////////////////////
//
//	ワールドシーン
//
///////////////////////////////////////////////////////////////////////////////
WorldScene::WorldScene( Label Name, pstr pPath )
:Scene( Name )
{
	m_pWorld	= NULL;
	m_pWorldRes	= NULL;

	m_Path		= pPath;
}
WorldScene::~WorldScene()
{
	SafeDelete( m_pWorld );
	SafeRelease( m_pWorldRes );
}

//*****************************************************************************
//	初期化
//*****************************************************************************
void WorldScene::OnInit( void )
{
	//	ワールドリソース
	m_pWorldRes = GetApp()->GetResMng()->LoadResource<WorldResource>( m_Path.c_str() );
	_ASSERT( m_pWorldRes );

	//	ワールド作成
	m_pWorld = new World;
	MEM_CHECK( m_pWorld );

	m_pWorld->Init( m_pWorldRes->GetData(), World::INIT_PARAM() );
	
	//	背景色変更
	P5::GetGraphix()->GetDevice()->SetScreenColor( Color(0,0,0) );
}

//*****************************************************************************
//	更新
//*****************************************************************************
void WorldScene::Update( void )
{
	//	更新
	m_pWorld->Update();

	//	終了
	if( m_pWorld->IsExit() )
	{
		if( m_pWorld->GetExitCode() == World::EXITCODE_DEAD ||
			m_pWorld->GetExitCode() == World::EXITCODE_CLEAR )
		{
			//	スコアの取得
			SAVEDATA_SCORE ScoreData;
			m_pWorld->GetScoreManager()->EndStage();
			m_pWorld->GetScoreManager()->GetScoreData( ScoreData );

			//	スコアの保存
			ScoreData.PlayID = GetApp()->GetSaveDataMng()->RecordScore( ScoreData, m_pWorld->GetExitCode() == World::EXITCODE_CLEAR );
			GetApp()->GetSaveDataMng()->Save();

			//	リプレイの記録
			m_pWorld->GetReplayManager()->Save( ScoreData, REPLAY_SAVE_DIR, REPLAY_FILE_MAX );

			//	エンディング
			if( m_pWorld->GetExitCode() == World::EXITCODE_CLEAR )
			{
				ChangeScene( new EndingScene( ENDING_SCENE, ScoreData, m_pWorld->GetScoreManager()->GetStageRecord() ) );
			}
			//	ゲームオーバー
			else
			{
				ChangeScene( new GameoverScene( GAMEOVER_SCENE, ScoreData, m_pWorld->GetScoreManager()->GetStageRecord() ) );
			}
			return;
		}

		//	タイトルへ
		else
		{
			ChangeScene( new TitleScene( TITLE_SCENE, GetName() ) );
			return;
		}
	}

	//	描画
	m_pWorld->Draw();
}

//******************************************************************************
//	例外処理
//******************************************************************************
void WorldScene::OnException( pstr pDirPath )
{
	if( m_pWorld && m_pWorld->GetScoreManager() )
	{
		//	スコアの取得
		SAVEDATA_SCORE ScoreData;
		m_pWorld->GetScoreManager()->EndStage();
		m_pWorld->GetScoreManager()->GetScoreData( ScoreData );

		//	リプレイを保存する
		m_pWorld->GetReplayManager()->Save( ScoreData, pDirPath, 0 );
	}
}