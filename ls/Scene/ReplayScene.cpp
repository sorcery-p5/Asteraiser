#include "stdafx.h"
#include "ReplayScene.h"
#include "ReplaySelectScene.h"
#include "TitleScene.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"
#include "SaveData/SaveDataManager.h"

#include "World/World.h"
#include "World/WorldData.h"
#include "Score/ScoreManager.h"
#include "Replay/ReplayManager.h"

namespace
{
	const Label	TITLE_SCENE				= "Title";
	const Label	REPLAY_SELECT_SCENE		= "ReplaySelect";

	pstr		REPLAY_SAVE_DIR			= "Replay/Rewrite";

	pstr		SCENE_DEMO1				= "Demo1";
	pstr		SCENE_DEMO2				= "Demo2";
	pstr		SCENE_DEMO3				= "Demo3";
}


///////////////////////////////////////////////////////////////////////////////
//
//	���v���C�V�[��
//
///////////////////////////////////////////////////////////////////////////////
ReplayScene::ReplayScene( Label Name, pstr pWorldPath, pstr pReplayPath, int Stage )
:Scene( Name )
{
	m_pWorld	= NULL;
	m_pWorldRes	= NULL;

	m_WorldPath		= pWorldPath;
	m_ReplayPath	= pReplayPath;
	m_InitStage		= Stage;
}
ReplayScene::~ReplayScene()
{
	SafeDelete( m_pWorld );
	SafeRelease( m_pWorldRes );
}

//*****************************************************************************
//	������
//*****************************************************************************
void ReplayScene::OnInit( void )
{
	//	���[���h���\�[�X
	m_pWorldRes = GetApp()->GetResMng()->LoadResource<WorldResource>( m_WorldPath.c_str() );
	_ASSERT( m_pWorldRes );

	//	���[���h�쐬
	m_pWorld = new World;
	MEM_CHECK( m_pWorld );

	World::INIT_PARAM InitParam;
	InitParam.Mode			= World::MODE_REPLAY;
	InitParam.Stage			= m_InitStage;
	InitParam.ReplayPath	= m_ReplayPath;
	InitParam.bPauseExit	= GetName() == Label(SCENE_DEMO1) || GetName() == Label(SCENE_DEMO2) || GetName() == Label(SCENE_DEMO3);
	InitParam.bDemoMenu		= GetName() == Label(SCENE_DEMO1) || GetName() == Label(SCENE_DEMO2) || GetName() == Label(SCENE_DEMO3);

	m_pWorld->Init( m_pWorldRes->GetData(), InitParam );
	
	//	�w�i�F�ύX
	P5::GetGraphix()->GetDevice()->SetScreenColor( Color(0,0,0) );
}

//*****************************************************************************
//	�X�V
//*****************************************************************************
void ReplayScene::Update( void )
{
	//	�X�V
	m_pWorld->Update();

	//	�I��
	if( m_pWorld->IsExit() )
	{
		if( !m_pWorld->GetReplayManager()->IsReplayEnable() )
		{
			//	�X�R�A�̎擾
			SAVEDATA_SCORE ScoreData;
			m_pWorld->GetScoreManager()->EndStage();
			m_pWorld->GetScoreManager()->GetScoreData( ScoreData );

			ScoreData.PlayID	= m_pWorld->GetReplayManager()->GetPlayID();

			//	���v���C�̋L�^
			m_pWorld->GetReplayManager()->Save( ScoreData, REPLAY_SAVE_DIR, 0 );
		}

		//	�f���̏ꍇ�F�^�C�g���ɖ߂�
		if( GetName() == Label(SCENE_DEMO1) || GetName() == Label(SCENE_DEMO2) || GetName() == Label(SCENE_DEMO3) )
		{
			ChangeScene( new TitleScene( TITLE_SCENE, GetName() ) );
		}
		//	�I����ʂɖ߂�
		else
		{
			ChangeScene( new ReplaySelectScene( REPLAY_SELECT_SCENE, m_WorldPath.c_str(), m_ReplayPath.c_str() ) );
		}

		return;
	}

	//	�`��
	m_pWorld->Draw();
}
