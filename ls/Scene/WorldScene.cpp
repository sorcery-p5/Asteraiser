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
//	���[���h�V�[��
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
//	������
//*****************************************************************************
void WorldScene::OnInit( void )
{
	//	���[���h���\�[�X
	m_pWorldRes = GetApp()->GetResMng()->LoadResource<WorldResource>( m_Path.c_str() );
	_ASSERT( m_pWorldRes );

	//	���[���h�쐬
	m_pWorld = new World;
	MEM_CHECK( m_pWorld );

	m_pWorld->Init( m_pWorldRes->GetData(), World::INIT_PARAM() );
	
	//	�w�i�F�ύX
	P5::GetGraphix()->GetDevice()->SetScreenColor( Color(0,0,0) );
}

//*****************************************************************************
//	�X�V
//*****************************************************************************
void WorldScene::Update( void )
{
	//	�X�V
	m_pWorld->Update();

	//	�I��
	if( m_pWorld->IsExit() )
	{
		if( m_pWorld->GetExitCode() == World::EXITCODE_DEAD ||
			m_pWorld->GetExitCode() == World::EXITCODE_CLEAR )
		{
			//	�X�R�A�̎擾
			SAVEDATA_SCORE ScoreData;
			m_pWorld->GetScoreManager()->EndStage();
			m_pWorld->GetScoreManager()->GetScoreData( ScoreData );

			//	�X�R�A�̕ۑ�
			ScoreData.PlayID = GetApp()->GetSaveDataMng()->RecordScore( ScoreData, m_pWorld->GetExitCode() == World::EXITCODE_CLEAR );
			GetApp()->GetSaveDataMng()->Save();

			//	���v���C�̋L�^
			m_pWorld->GetReplayManager()->Save( ScoreData, REPLAY_SAVE_DIR, REPLAY_FILE_MAX );

			//	�G���f�B���O
			if( m_pWorld->GetExitCode() == World::EXITCODE_CLEAR )
			{
				ChangeScene( new EndingScene( ENDING_SCENE, ScoreData, m_pWorld->GetScoreManager()->GetStageRecord() ) );
			}
			//	�Q�[���I�[�o�[
			else
			{
				ChangeScene( new GameoverScene( GAMEOVER_SCENE, ScoreData, m_pWorld->GetScoreManager()->GetStageRecord() ) );
			}
			return;
		}

		//	�^�C�g����
		else
		{
			ChangeScene( new TitleScene( TITLE_SCENE, GetName() ) );
			return;
		}
	}

	//	�`��
	m_pWorld->Draw();
}

//******************************************************************************
//	��O����
//******************************************************************************
void WorldScene::OnException( pstr pDirPath )
{
	if( m_pWorld && m_pWorld->GetScoreManager() )
	{
		//	�X�R�A�̎擾
		SAVEDATA_SCORE ScoreData;
		m_pWorld->GetScoreManager()->EndStage();
		m_pWorld->GetScoreManager()->GetScoreData( ScoreData );

		//	���v���C��ۑ�����
		m_pWorld->GetReplayManager()->Save( ScoreData, pDirPath, 0 );
	}
}