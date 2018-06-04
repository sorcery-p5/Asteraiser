#include "stdafx.h"
#include "SceneManager.h"
#include "Scene.h"

#include "App/App.h"
#include "Effect/EffectManager.h"


///////////////////////////////////////////////////////////////////////////////
//
//	シーン管理
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
//	更新
//*****************************************************************************
void SceneManager::Update( void )
{
	//	シーン変更
	if( m_pNextScene || m_bExit )
	{
		//	サウンドの全停止
		GetTone()->StopChannelSound( SOUND_CHANNEL_SE );

		//	エフェクトの全削除
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
//	現在のシーンを変更
//*****************************************************************************
void SceneManager::ChangeScene( Scene* pScene )
{
	_ASSERT( pScene && !m_pNextScene );

	m_pNextScene = pScene;

	//	サウンドの全停止
	GetTone()->StopChannelSound( SOUND_CHANNEL_SE );

	//	エフェクトの全削除
	GetApp()->GetEffectMng()->DeleteAll();
}

//*****************************************************************************
//	現在のシーンを終了
//*****************************************************************************
void SceneManager::Exit( void )
{
	m_bExit = true;
}

//******************************************************************************
//	例外処理
//******************************************************************************
void SceneManager::OnException( pstr pDirPath )
{
	if( m_pCurScene ) m_pCurScene->OnException( pDirPath );
}