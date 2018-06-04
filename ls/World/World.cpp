#include "stdafx.h"
#include "World.h"
#include "WorldData.h"
#include "WorldPause.h"
#include "WorldContinue.h"
#include "WorldResult.h"
#include "WorldReplayMenu.h"

#include "App/App.h"
#include "Config/Config.h"

#include "Controller/ControllerManager.h"
#include "Replay/ReplayManager.h"
#include "Stage/StageManager.h"
#include "Player/PlayerManager.h"
#include "Enemy/EnemyManager.h"
#include "Attack/AttackManager.h"
#include "Ground/GroundManager.h"
#include "Indicate/IndicateManager.h"
#include "Item/ItemManager.h"
#include "Score/ScoreManager.h"

#include "SpriteFrame/SpriteFrameUtil.h"
#include "Collide/CollideSystem.h"
#include "BGM/BGMManager.h"
#include "Menu/Menu.h"
#include "Fade/FadeScreen.h"

namespace
{
	const int	FADE_TIME			= 25;	
	const int	FADE_TIME_LONG		= 60;	
	const uint	FADE_PHASE			= 10000;

	const Label INDICATE_TABLE[]	= 
	{
		"Wall",
		"Score",
		"Rate",
		"Heat",
		"Blade",
		"Overdrive",
		"Shield",
	};
}

const Label	World::EXITCODE_EXIT	= "Exit";
const Label World::EXITCODE_CLEAR	= "Clear";
const Label World::EXITCODE_DEAD	= "Dead";


///////////////////////////////////////////////////////////////////////////////
//
//	ワールド
//
///////////////////////////////////////////////////////////////////////////////
World::World( void )
{
	m_pData				= NULL;
	m_pControllerMng	= NULL;
	m_pReplayManager	= NULL;
	m_pStageManager		= NULL;
	m_pPlayerManager	= NULL;
	m_pEnemyManager		= NULL;
	m_pAttackManager	= NULL;
	m_pGroundManager	= NULL;
	m_pItemManager		= NULL;
	m_pIndicateManager	= NULL;
	m_pScoreManager		= NULL;

	m_pPause			= NULL;
	m_pContinue			= NULL;
	m_pResult			= NULL;
	m_pReplayMenu		= NULL;
	m_pDemoMenu			= NULL;
	m_pFade				= NULL;

	m_Mode				= MODE_ALL;
	m_FrameCount		= 0;
	m_bExit				= false;

	m_bUpdateEnable		= true;
	m_bDrawEnable		= true;
}
World::~World()
{
	//	BGM停止
	GetApp()->GetBGMMng()->Stop();

	SafeDelete( m_pFade );
	SafeDelete( m_pDemoMenu );
	SafeDelete( m_pPause );
	SafeDelete( m_pContinue );
	SafeDelete( m_pReplayMenu );
	SafeDelete( m_pResult );

	SafeDelete( m_pScoreManager );
	SafeDelete( m_pItemManager );
	SafeDelete( m_pGroundManager );
	SafeDelete( m_pAttackManager );
	SafeDelete( m_pEnemyManager );
	SafeDelete( m_pPlayerManager );
	SafeDelete( m_pStageManager );
	SafeDelete( m_pIndicateManager );
	SafeDelete( m_pReplayManager );
	SafeDelete( m_pControllerMng );
}

//*****************************************************************************
//	初期化
//*****************************************************************************
void World::Init( const WorldData* pData, const INIT_PARAM& InitParam )
{
	m_pData = pData;

	//	パラメータ
	m_Mode			= InitParam.Mode;
	m_FrameCount	= 0;
	m_FieldRect		= m_pData->GetParam().FieldRect;

	//	リプレイ管理
	m_pReplayManager = new ReplayManager( this );
	MEM_CHECK(m_pReplayManager);

	if( InitParam.Mode == MODE_REPLAY )
	{
		m_pReplayManager->InitLoad( InitParam.ReplayPath.c_str(), InitParam.Stage );
	}
	else
	{
		m_pReplayManager->InitRecord();
	}

	//	コントローラ
	m_pControllerMng = new ControllerManager( this );
	MEM_CHECK(m_pControllerMng);
	
	m_pControllerMng->Init( IsReplayMode() );

	//	表示管理
	m_pIndicateManager = new IndicateManager( this );
	MEM_CHECK(m_pIndicateManager);

	//	プレイヤー管理
	m_pPlayerManager = new PlayerManager( this );
	MEM_CHECK(m_pPlayerManager);

	m_pPlayerManager->AddPlayer( m_pData->GetPlayerData( m_pData->GetParam().PlayerName ), m_pData->GetParam().InitPos + GetFieldOffset() );

	//	ステージ管理
	m_pStageManager = new StageManager( this );
	MEM_CHECK(m_pStageManager);

	//	敵管理
	m_pEnemyManager	= new EnemyManager( this );
	MEM_CHECK(m_pEnemyManager);

	//	攻撃管理
	m_pAttackManager = new AttackManager( this );
	MEM_CHECK(m_pAttackManager);

	//	地形管理
	m_pGroundManager = new GroundManager( this );
	MEM_CHECK(m_pGroundManager);

	//	アイテム管理
	m_pItemManager = new ItemManager( this, m_pData->GetItemData() );
	MEM_CHECK(m_pItemManager);

	//	スコア管理
	m_pScoreManager = new ScoreManager( this, m_pData->GetScoreData() );
	MEM_CHECK(m_pScoreManager);

	//	ポーズ管理
	m_pPause		= new WorldPause( this );
	MEM_CHECK(m_pPause);
	m_pPause->Init( m_pData->GetMenuData( m_pData->GetParam().PauseMenuName ), m_pData->GetParam().bPauseExit || InitParam.bPauseExit );

	//	コンティニュー管理
	m_pContinue		= new WorldContinue( this );
	MEM_CHECK(m_pContinue);
	m_pContinue->Init( m_pData->GetMenuData( m_pData->GetParam().ContinueMenuName ), m_pData->GetParam().bContinueEnable? 3 : 0 );

	//	リプレイメニュー管理
	m_pReplayMenu		= new WorldReplayMenu( this );
	MEM_CHECK(m_pReplayMenu);
	m_pReplayMenu->Init( m_pData->GetMenuData( m_pData->GetParam().ReplayMenuName ) );

	//	ステージリザルト管理
	m_pResult		= new WorldResult( this );
	MEM_CHECK(m_pResult);
	m_pResult->Init( m_pData->GetMenuData( m_pData->GetParam().ResultMenuName ), 
						m_pData->GetSoundFile( m_pData->GetParam().ResultScoreSoundName ) );

	//	デモメニュー
	if( InitParam.bDemoMenu )
	{
		m_pDemoMenu		= new Menu( NULL );
		m_pDemoMenu->Init( m_pData->GetMenuData( m_pData->GetParam().DemoMenuName ), Matrix3::Trans( GetFieldRect().Center() ) );
	}

	//	フェードスクリーン
	m_pFade			= new FadeScreen( FADE_PHASE );
	MEM_CHECK(m_pFade);
	m_pFade->FadeIn( FADE_TIME, Color::Black() );

	//	ステージ登録
	for( int i = 0; i < m_pData->GetStageNum(); i++ )
	{
		m_pStageManager->PushStageData( m_pData->GetStageData( i ) );
	}
	m_pStageManager->Start( InitParam.Stage, InitParam.State );
	
	//	インジケート
	for( int i = 0; i < ArrayNum(INDICATE_TABLE); i++ )
	{
		m_pIndicateManager->AddIndicate( INDICATE_TABLE[i], m_pData->GetIndicateData( INDICATE_TABLE[i] ) );
	}

	//	コンフィグの反映
	SpriteFrameUtil::SetDrawDummyBone( GetConfig()->GetDebugInfo().bDrawDummyBone );
	GetCollideSystem()->SetDrawShapeEnable( GetConfig()->GetDebugInfo().bDrawCollideShape );
}

//*****************************************************************************
//	更新
//*****************************************************************************
void World::Update( void )
{
	//	フェード更新
	m_pFade->Update();

	//	デモメニュー更新
	if( m_pDemoMenu ) m_pDemoMenu->Update();

	if( m_bExit ) return;

	//	リプレイ終了チェック
	if( m_pReplayManager->IsReplayEnable() )
	{
		if( m_pReplayManager->IsReplayOver( m_FrameCount ) )
		{
			OpenReplayMenu( false );
		}
	}

	//	コンティニュー更新
	m_pContinue->Update();
	if( m_pContinue->IsActive() ) return;

	//	ステージリザルト更新
	m_pResult->Update();
	if( m_pResult->IsActive() ) return;

	//	リプレイメニュー更新
	m_pReplayMenu->Update();
	if( m_pReplayMenu->IsActive() ) return;

	//	ポーズ更新
	m_pPause->Update();
	if( m_pPause->IsPause() ) return;

	//	コントローラ
	m_pControllerMng->Update( m_FrameCount );

	//	ステージ
	m_pStageManager->Update();

	//	地形
	m_pGroundManager->Update();

	//	敵
	m_pEnemyManager->Update();

	//	攻撃
	m_pAttackManager->Update();

	//	アイテム
	m_pItemManager->Update();

	//	プレイヤー
	m_pPlayerManager->Update();

	//	スコア
	m_pScoreManager->Update();

	//	インジケート
	m_pIndicateManager->Update();

	m_FrameCount++;

	//	全ステージ終了
	if( m_pStageManager->IsEndAllStage() )
	{
		Exit( EXITCODE_CLEAR );
	}
}

//******************************************************************************
//	描画
//******************************************************************************
void World::Draw( void )
{
	//	地形
	m_pGroundManager->Draw();

	//	敵
	m_pEnemyManager->Draw();

	//	プレイヤー
	m_pPlayerManager->Draw();

	//	ステージ
	m_pStageManager->Draw();

	//	インジケート
	m_pIndicateManager->Draw();

	//	ポーズ
	m_pPause->Draw();

	//	コンティニュー
	m_pContinue->Draw();

	//	リプレイメニュー
	m_pReplayMenu->Draw();

	//	結果
	m_pResult->Draw();

	//	デモメニュー
	SafePtr(m_pDemoMenu)->Draw( GetDrawOffset() );

	//	フェード
	m_pFade->Draw();
}

//******************************************************************************
//	ワールドの終了
//******************************************************************************
void World::Exit( Label ExitCode )
{
	m_bExit		= true;
	m_ExitCode	= ExitCode;

	Color	FadeCol		= Color::Black();
	int		FadeTime	= FADE_TIME;

	//	オールクリア
	if( m_Mode == MODE_ALL && ExitCode == EXITCODE_CLEAR )
	{
		FadeCol		= Color::White();
		FadeTime	= FADE_TIME_LONG;
		P5::GetGraphix()->GetDevice()->SetScreenColor( Color::White() );
	}

	m_pFade->FadeOut( FadeTime, FadeCol );
}

//******************************************************************************
//	終了状態かどうか
//******************************************************************************
bool World::IsExit( void ) const
{
	return m_bExit && !m_pFade->IsActive();
}

//******************************************************************************
//	自機の死亡通知
//******************************************************************************
void World::OnPlayerDead( void )
{
	if( m_pContinue->IsCountinueEnable() )
	{
		//	リプレイ中
		if( m_pReplayManager->IsReplayEnable() )
		{
			//	リプレイ内のコンティニュー回数以内
			if( m_pContinue->GetContinueCount() < m_pReplayManager->GetContinueCount() )
			{
				//	自動コンティニュー
				m_pContinue->AutoDecide();
			}
			else
			{
				//	リプレイメニュー開く
				OpenReplayMenu( false );
			}
		}
		else
		{
			//	コンティニュー開く
			m_pContinue->Open();
		}
	}
	else
	{
		//	ゲームオーバー
		Exit( EXITCODE_DEAD );
	}
}

//******************************************************************************
//	コンティニュー通知
//******************************************************************************
void World::OnContinue( void )
{
	if( m_bExit ) return;

	//	スコア通知
	m_pScoreManager->OnContinue();

	//	自機を復帰
	m_pPlayerManager->OnContinue();
}

//******************************************************************************
//	ステージ変更通知
//******************************************************************************
void World::OnChangeStage( int StageIndex )
{
	if( m_bExit ) return;

	//	スコア通知
	m_pScoreManager->EndStage();

	//	リプレイ通知
	m_pReplayManager->RecordChangeStage( m_FrameCount, StageIndex );

	//	乱数初期化
	_ResetRand();
}

//******************************************************************************
//	ステージリザルトを開く
//******************************************************************************
void World::OpenResult( void )
{
	if( m_bExit ) return;

	m_pResult->Open();
}

//******************************************************************************
//	リプレイメニューを開く
//******************************************************************************
void World::OpenReplayMenu( bool bCancelEnable )
{
	if( m_bExit ) return;

	m_pReplayMenu->Open( bCancelEnable );
}

//******************************************************************************
//	リプレイをマニュアル操作に切り替える
//******************************************************************************
void World::ChangeReplayManual( void )
{
	//	リプレイ管理差し替え
	m_pReplayManager->ChangeManual( m_FrameCount );

	//	コントローラ差し替え
	m_pControllerMng->ChangeManual();
}

//******************************************************************************
//	コントローラを取得
//******************************************************************************
const Controller* World::GetController( void ) const
{
	return m_pControllerMng->GetController();
}

//******************************************************************************
//	フェード中かどうか
//******************************************************************************
bool World::IsFade( void ) const
{
	return m_pFade->IsActive();
}

//------------------------------------------------------------------------------
//	乱数をリセットする
//------------------------------------------------------------------------------
void World::_ResetRand( void )
{
	m_pPlayerManager->GetRand().Reset();
	m_pEnemyManager->GetRand().Reset();
	m_pAttackManager->GetRand().Reset();
	m_pItemManager->GetRand().Reset();
}