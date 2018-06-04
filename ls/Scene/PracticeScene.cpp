#include "stdafx.h"
#include "PracticeScene.h"
#include "StageSelectScene.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"
#include "SaveData/SaveDataManager.h"

#include "World/World.h"
#include "World/WorldData.h"
#include "Score/ScoreManager.h"

namespace
{
	const Label	STAGE_SELECT_SCENE		= "StageSelect";
}


///////////////////////////////////////////////////////////////////////////////
//
//	プラクティスシーン
//
///////////////////////////////////////////////////////////////////////////////
PracticeScene::PracticeScene( Label Name, pstr pPath, int Stage, Label State )
:Scene( Name )
{
	m_pWorld	= NULL;
	m_pWorldRes	= NULL;

	m_Path		= pPath;
	m_InitStage	= Stage;
	m_InitState	= State;
}
PracticeScene::~PracticeScene()
{
	SafeDelete( m_pWorld );
	SafeRelease( m_pWorldRes );
}

//*****************************************************************************
//	初期化
//*****************************************************************************
void PracticeScene::OnInit( void )
{
	//	ワールドリソース
	m_pWorldRes = GetApp()->GetResMng()->LoadResource<WorldResource>( m_Path.c_str() );
	_ASSERT( m_pWorldRes );

	//	ワールド作成
	m_pWorld = new World;
	MEM_CHECK( m_pWorld );

	World::INIT_PARAM InitParam;
	InitParam.Mode	= World::MODE_STAGE_SELECT;
	InitParam.Stage	= m_InitStage;
	InitParam.State	= m_InitState;

	m_pWorld->Init( m_pWorldRes->GetData(), InitParam );
	
	//	背景色変更
	P5::GetGraphix()->GetDevice()->SetScreenColor( Color(0,0,0) );
}

//*****************************************************************************
//	更新
//*****************************************************************************
void PracticeScene::Update( void )
{
	//	更新
	m_pWorld->Update();

	//	終了
	if( m_pWorld->IsExit() )
	{
		ChangeScene( new StageSelectScene( STAGE_SELECT_SCENE, m_Path.c_str(), m_InitStage, m_InitState ) );
		return;
	}

	//	描画
	m_pWorld->Draw();
}