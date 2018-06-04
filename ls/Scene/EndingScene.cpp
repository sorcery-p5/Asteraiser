#include "stdafx.h"
#include "EndingScene.h"
#include "TitleScene.h"
#include "PlayResultScene.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"

#include "Menu/Menu.h"
#include "Menu/MenuData.h"
#include "Fade/FadeScreen.h"

namespace
{
	const int	FADE_TIME			= 40;	
	const uint	FADE_PHASE			= 10000;
	const Label	NOTIFY_EXIT			= "Exit";

	pstr		MENU_PATH			= "Data/Menu/Ending.xml";
	pstr		SCENE_PLAY_RESULT	= "PlayResult";
}


///////////////////////////////////////////////////////////////////////////////
//
//	エンディングシーン
//
///////////////////////////////////////////////////////////////////////////////
EndingScene::EndingScene( Label Name, const SAVEDATA_SCORE& Score, const SCORE_STAGE_RECORD_VEC& vRecord )
:Scene( Name )
{
	m_pMenuRes		= NULL;
	m_pMenu			= NULL;
	m_pFade			= NULL;

	m_Score			= Score;
	m_vRecord		= vRecord;
	m_bExit			= false;
}
EndingScene::~EndingScene()
{
	SafeDelete( m_pFade );
	SafeDelete( m_pMenu );
	SafeRelease( m_pMenuRes );
}

//*****************************************************************************
//	初期化
//*****************************************************************************
void EndingScene::OnInit( void )
{
	//	メニュー
	m_pMenuRes = GetApp()->GetResMng()->LoadResource<MenuResource>( MENU_PATH );
	if( m_pMenuRes && m_pMenuRes->GetData() )
	{
		m_pMenu = new Menu( NULL );
		MEM_CHECK( m_pMenu );

		m_pMenu->SetListener( this );
		m_pMenu->Init( m_pMenuRes->GetData(), Matrix3::Trans( 0.0f, 0.0f ) );
	}
	_ASSERT( m_pMenu );

	//	フェードスクリーン
	m_pFade			= new FadeScreen( FADE_PHASE );
	MEM_CHECK(m_pFade);

	//	背景色変更
	P5::GetGraphix()->GetDevice()->SetScreenColor( Color(0,0,0) );
}

//*****************************************************************************
//	更新
//*****************************************************************************
void EndingScene::Update( void )
{
	//	フェード更新
	m_pFade->Update();
	m_pFade->Draw();

	//	メニュー更新
	if( !m_pMenu ) return;

	m_pMenu->Update();

	//	終了
	if( m_pMenu->IsEmpty() || 
		m_bExit && !m_pFade->IsActive() )
	{
		//	結果表示へ
		ChangeScene( new PlayResultScene( SCENE_PLAY_RESULT, m_Score, m_vRecord ) );
		return;
	}
	
	m_pMenu->Draw( Matrix3::Identity() );

}

//******************************************************************************
//	通知
//******************************************************************************
void EndingScene::OnLayerNotify( MenuLayer* pLayer, Label Name, float Param )
{
	//	終了
	if( Name == NOTIFY_EXIT )
	{
		if( !m_bExit )
		{
			m_pFade->FadeOut( FADE_TIME, Color::Black() );
			m_bExit = true;
		}
	}
}