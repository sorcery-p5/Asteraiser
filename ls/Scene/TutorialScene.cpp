#include "stdafx.h"
#include "TutorialScene.h"
#include "TitleScene.h"
#include "WorldScene.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"

#include "World/World.h"
#include "World/WorldData.h"

namespace
{
	pstr		WORLD_PATH				= "Data/World/Tutorial.xml";
	pstr		WORLD_PATH_DEMO			= "Data/World/DemoTuto.xml";

	const Label	TITLE_SCENE				= "Title";
	const Label	WORLD_SCENE				= "World";
	const Label	DEMO_TUTORIAL_SCENE		= "DemoTuto";
}


///////////////////////////////////////////////////////////////////////////////
//
//	�`���[�g���A���V�[��
//
///////////////////////////////////////////////////////////////////////////////
TutorialScene::TutorialScene( Label Name )
:Scene( Name )
{
	m_pWorld	= NULL;
	m_pWorldRes	= NULL;
}
TutorialScene::~TutorialScene()
{
	SafeDelete( m_pWorld );
	SafeRelease( m_pWorldRes );
}

//*****************************************************************************
//	������
//*****************************************************************************
void TutorialScene::OnInit( void )
{
	pstr pWorldPath = GetName() == DEMO_TUTORIAL_SCENE? WORLD_PATH_DEMO : WORLD_PATH;

	//	���[���h���\�[�X
	m_pWorldRes = GetApp()->GetResMng()->LoadResource<WorldResource>( pWorldPath );
	_ASSERT( m_pWorldRes );

	//	���[���h�쐬
	m_pWorld = new World;
	MEM_CHECK( m_pWorld );

	World::INIT_PARAM InitParam;
	InitParam.Mode		= World::MODE_TUTORIAL;
	InitParam.bDemoMenu	= GetName() == DEMO_TUTORIAL_SCENE;

	m_pWorld->Init( m_pWorldRes->GetData(), InitParam );
	
	//	�w�i�F�ύX
	P5::GetGraphix()->GetDevice()->SetScreenColor( Color(0,0,0) );
}

//*****************************************************************************
//	�X�V
//*****************************************************************************
void TutorialScene::Update( void )
{
	//	�X�V
	m_pWorld->Update();

	//	�I��
	if( m_pWorld->IsExit() )
	{
		//	�^�C�g����
		ChangeScene( new TitleScene( TITLE_SCENE, GetName() ) );
		return;
	}

	//	�`��
	m_pWorld->Draw();
}