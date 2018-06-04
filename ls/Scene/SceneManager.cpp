#include "stdafx.h"
#include "SceneManager.h"
#include "Scene.h"

#include "App/App.h"
#include "Effect/EffectManager.h"


///////////////////////////////////////////////////////////////////////////////
//
//	�V�[���Ǘ�
//
///////////////////////////////////////////////////////////////////////////////
SceneManager::SceneManager()
{
	m_pCurScene = NULL;
	m_pNextScene = NULL;

	m_bExit = false;
}
SceneManager::~SceneManager()
{
	SafeDelete( m_pCurScene );
	SafeDelete( m_pNextScene );
}

//*****************************************************************************
//	�X�V
//*****************************************************************************
void SceneManager::Update( void )
{
	//	�V�[���ύX
	if( m_pNextScene || m_bExit )
	{
		//	�T�E���h�̑S��~
		GetTone()->StopChannelSound( SOUND_CHANNEL_SE );

		//	�G�t�F�N�g�̑S�폜
		GetApp()->GetEffectMng()->DeleteAll();

		SafeDelete( m_pCurScene );
		m_pCurScene = m_pNextScene;

		if( m_pCurScene )
		{
			m_pCurScene->SetSceneManager( this );
			m_pCurScene->OnInit();
			m_pNextScene = NULL;
		}
	}

	if( m_pCurScene ) m_pCurScene->Update();
}

//*****************************************************************************
//	���݂̃V�[����ύX
//*****************************************************************************
void SceneManager::ChangeScene( Scene* pScene )
{
	_ASSERT( pScene && !m_pNextScene );

	m_pNextScene = pScene;

	//	�T�E���h�̑S��~
	GetTone()->StopChannelSound( SOUND_CHANNEL_SE );

	//	�G�t�F�N�g�̑S�폜
	GetApp()->GetEffectMng()->DeleteAll();
}

//*****************************************************************************
//	���݂̃V�[�����I��
//*****************************************************************************
void SceneManager::Exit( void )
{
	m_bExit = true;
}

//******************************************************************************
//	��O����
//******************************************************************************
void SceneManager::OnException( pstr pDirPath )
{
	if( m_pCurScene ) m_pCurScene->OnException( pDirPath );
}