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
//	�v���N�e�B�X�V�[��
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
//	������
//*****************************************************************************
void PracticeScene::OnInit( void )
{
	//	���[���h���\�[�X
	m_pWorldRes = GetApp()->GetResMng()->LoadResource<WorldResource>( m_Path.c_str() );
	_ASSERT( m_pWorldRes );

	//	���[���h�쐬
	m_pWorld = new World;
	MEM_CHECK( m_pWorld );

	World::INIT_PARAM InitParam;
	InitParam.Mode	= World::MODE_STAGE_SELECT;
	InitParam.Stage	= m_InitStage;
	InitParam.State	= m_InitState;

	m_pWorld->Init( m_pWorldRes->GetData(), InitParam );
	
	//	�w�i�F�ύX
	P5::GetGraphix()->GetDevice()->SetScreenColor( Color(0,0,0) );
}

//*****************************************************************************
//	�X�V
//*****************************************************************************
void PracticeScene::Update( void )
{
	//	�X�V
	m_pWorld->Update();

	//	�I��
	if( m_pWorld->IsExit() )
	{
		ChangeScene( new StageSelectScene( STAGE_SELECT_SCENE, m_Path.c_str(), m_InitStage, m_InitState ) );
		return;
	}

	//	�`��
	m_pWorld->Draw();
}