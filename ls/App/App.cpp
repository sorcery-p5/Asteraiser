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
//	�A�v���P�[�V����
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
	//	���Z�b�g�ʒm�̓o�^����
	GetGraphix()->RemoveResetListener( m_pResourceManager );

	//	�T�E���h�S��~
	GetTone()->StopAllSound();

	SafeDelete( m_pSceneManager );
	SafeDelete( m_pEffectManager );
	SafeDelete( m_pBGMManager );
	SafeDelete( m_pSaveDataManager );

	ExitCollideSystem();

	SafeDelete( m_pResourceManager );
}

//*****************************************************************************
//	������
//*****************************************************************************
void App::Init( void )
{
	//	���\�[�X�Ǘ�
	m_pResourceManager	= new ResourceManager;
	MEM_CHECK( m_pResourceManager );

	GetGraphix()->AddResetListener( m_pResourceManager );

	//	�����蔻��
	InitCollideSystem();

	//	�G�t�F�N�g
	m_pEffectManager = new EffectManager;
	MEM_CHECK( m_pEffectManager );

	//	BGM
	m_pBGMManager = new BGMManager;
	MEM_CHECK( m_pBGMManager );

	//	�V�[��
	m_pSceneManager = new SceneManager;
	MEM_CHECK( m_pSceneManager );

	//	�Z�[�u�f�[�^
	m_pSaveDataManager = new SaveDataManager();
	MEM_CHECK( m_pSaveDataManager );

	m_pSaveDataManager->Load();

	m_pSceneManager->ChangeScene( new TitleScene( "Title" ) );
//	m_pSceneManager->ChangeScene( new TestScene( "Test" ) );
//	m_pSceneManager->ChangeScene( new HitTestScene( "HitTest" ) );
}

//*****************************************************************************
//	�X�V
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
//	�I���������ǂ���
//******************************************************************************
bool App::IsEnd( void ) const
{
	return m_pSceneManager->IsExit();
}

//******************************************************************************
//	��O�̏���
//******************************************************************************
void App::OnException( pstr pDirPath )
{
	if( m_pSceneManager ) m_pSceneManager->OnException( pDirPath );
}

//-----------------------------------------------------------------------------
//	�C���X�^���X�Ǘ�
//-----------------------------------------------------------------------------
namespace
{
	App* s_pApp	= NULL;
}

//*****************************************************************************
//	�C���X�^���X������
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
//	�C���X�^���X�I��
//*****************************************************************************
void ExitApp( void )
{
	SafeDelete( s_pApp );
}

//*****************************************************************************
//	�C���X�^���X�擾
//*****************************************************************************
App* GetApp( void )
{
	return s_pApp;
}