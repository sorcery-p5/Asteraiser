#include "stdafx.h"
#include "GameoverScene.h"
#include "TitleScene.h"
#include "PlayResultScene.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"

#include "Menu/Menu.h"
#include "Menu/MenuData.h"

namespace
{
	pstr		MENU_PATH			= "Data/Menu/GameOver.xml";
	pstr		SCENE_PLAY_RESULT	= "PlayResult";
}


///////////////////////////////////////////////////////////////////////////////
//
//	�Q�[���I�[�o�[�V�[��
//
///////////////////////////////////////////////////////////////////////////////
GameoverScene::GameoverScene( Label Name, const SAVEDATA_SCORE& Score, const SCORE_STAGE_RECORD_VEC& vRecord )
:Scene( Name )
{
	m_pMenuRes		= NULL;
	m_pMenu			= NULL;

	m_Score			= Score;
	m_vRecord		= vRecord;
}
GameoverScene::~GameoverScene()
{
	SafeDelete( m_pMenu );
	SafeRelease( m_pMenuRes );
}

//*****************************************************************************
//	������
//*****************************************************************************
void GameoverScene::OnInit( void )
{
	//	���j���[
	m_pMenuRes = GetApp()->GetResMng()->LoadResource<MenuResource>( MENU_PATH );
	if( m_pMenuRes && m_pMenuRes->GetData() )
	{
		m_pMenu = new Menu( NULL );
		MEM_CHECK( m_pMenu );

		Point ScreenSize = P5::GetWindow()->GetSize();

		m_pMenu->Init( m_pMenuRes->GetData(), Matrix3::Trans( ScreenSize / 2.0f ) );
	}
	_ASSERT( m_pMenu );

	//	�w�i�F�ύX
	P5::GetGraphix()->GetDevice()->SetScreenColor( Color(0,0,0) );
}

//*****************************************************************************
//	�X�V
//*****************************************************************************
void GameoverScene::Update( void )
{
	if( !m_pMenu ) return;

	m_pMenu->Update();

	//	���j���[�I��
	if( m_pMenu->IsEmpty() )
	{
		//	���ʕ\����
		ChangeScene( new PlayResultScene( SCENE_PLAY_RESULT, m_Score, m_vRecord ) );
		return;
	}
				
	m_pMenu->Draw( Matrix3::Identity() );
}
