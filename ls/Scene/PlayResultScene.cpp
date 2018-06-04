#include "stdafx.h"
#include "PlayResultScene.h"
#include "TitleScene.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"
#include "SaveData/SaveDataManager.h"

#include "Score/ScoreData.h"

#include "Menu/Menu.h"
#include "Menu/MenuData.h"
#include "Menu/MenuLayer.h"
#include "Menu/Object/MenuObject.h"

namespace
{
	pstr		MENU_PATH		= "Data/Menu/PlayResult.xml";
	pstr		SCORE_PATH		= "Data/Score/Score.xml";

	pstr		SCENE_TITLE		= "Title";

	const Label	LAYER_MAIN		= "Main";
	const Label	LAYER_AREA		= "Area";

	const Label	NOTIFY_EXIT		= "Exit";
	const Label	NOTIFY_DECIDE	= "Decide";
	const Label	NOTIFY_START	= "Start";

	const Label BONE_LIST		= "List";
	const Label BONE_ARRIVAL	= "Arrival";
	const Label BONE_TIME		= "Time";
	const Label BONE_SCORE		= "Score";
	const Label BONE_ITEM		= "Item";
	const Label BONE_DESTROY	= "Destroy";
	const Label BONE_DAMAGE		= "Damage";
	const Label BONE_PLAY_ID	= "PlayID";
	const Label BONE_RANK		= "Rank";
	const Label BONE_CAP_RANK	= "CapRank";
	const Label BONE_AREA		= "Area";
	const Label BONE_RATE		= "Rate";
	const Label BONE_HEAT		= "Heat";

	const StrPair<int> ARRIVAL_NAME_TABLE[] =
	{
		{ "Nothing",	0, },
		{ "Area 1-1",	10, },
		{ "Area 1-1B",	11, },
		{ "Area 1-2",	12, },
		{ "Area 1-2B",	13, },
					
		{ "Area 2-1",	20, },
		{ "Area 2-1B",	21, },
		{ "Area 2-2",	22, },
		{ "Area 2-2B",	23, },
					
		{ "Area 3",		30, },
		{ "Area 3 B",	31, },
					
		{ "Area 4-1",	40, },
		{ "Area 4-1B",	41, },
		{ "Area 4-2",	42, },
		{ "Area 4-2B",	43, },
					
		{ "All Clear",	50, },
	};

	pstr AREA_NAME_TABLE[] =
	{
		"Area1", "Area2", "Area3", "Area4",
	};

	const Color		COLOR_NO_CONT		= Color( 192,220,255 );
	const Color		COLOR_NO_BREAK		= Color( 128,255,192 );
	const Color		COLOR_NO_DAMAGE		= Color( 255,240,64 );
	const Color		COLOR_ALL_CLEAR		= Color( 255,220,220 );
	const Color		COLOR_RANK_1		= Color( 255,255,192 );
	const Color		COLOR_RANK_2		= Color( 230,230,255 );
	const Color		COLOR_RANK_3		= Color( 255,230,180 );

	const Color		COLOR_RATE_MIN		= Color( 255,255,255 );
	const Color		COLOR_RATE_MAX		= Color( 255,240,64 );
	const Color		COLOR_HEAT_MIN		= Color( 128,128,255 );
	const Color		COLOR_HEAT_MAX		= Color( 255,128,128 );

	pstr			STR_NO_CONT			= "No Continue";
	pstr			STR_NO_BREAK		= "No Break";
	pstr			STR_NO_DAMAGE		= "No Damage";

	pstr			STR_NO_CONT_S		= "NC";
	pstr			STR_NO_BREAK_S		= "NB";
	pstr			STR_NO_DAMAGE_S		= "ND";

	const int		ARRIVAL_ALL			= 50;
}


///////////////////////////////////////////////////////////////////////////////
//
//	プレイ結果シーン
//
///////////////////////////////////////////////////////////////////////////////
PlayResultScene::PlayResultScene( Label Name, const SAVEDATA_SCORE& Score, const SCORE_STAGE_RECORD_VEC& vRecord )
:Scene( Name )
{
	m_pMenuRes		= NULL;
	m_pMenu			= NULL;
	m_pScoreRes		= NULL;

	m_Score			= Score;
	m_vRecord		= vRecord;
}
PlayResultScene::~PlayResultScene()
{
	SafeDelete( m_pMenu );
	SafeRelease( m_pMenuRes );
	SafeRelease( m_pScoreRes );
}

//*****************************************************************************
//	初期化
//*****************************************************************************
void PlayResultScene::OnInit( void )
{
	//	スコアデータ
	m_pScoreRes = GetApp()->GetResMng()->LoadResource<ScoreResource>( SCORE_PATH );
	_ASSERT( m_pScoreRes );

	//	メニュー
	m_pMenuRes = GetApp()->GetResMng()->LoadResource<MenuResource>( MENU_PATH );
	if( m_pMenuRes && m_pMenuRes->GetData() )
	{
		m_pMenu = new Menu( this );
		MEM_CHECK( m_pMenu );

		m_pMenu->Init( m_pMenuRes->GetData(), Matrix3::Trans( 0, 0 ) );
	}
	_ASSERT( m_pMenu );

	//	メニュー更新
	_UpdateMenu();

	//	背景色変更
	P5::GetGraphix()->GetDevice()->SetScreenColor( Color(0,0,0) );
}

//*****************************************************************************
//	更新
//*****************************************************************************
void PlayResultScene::Update( void )
{
	if( !m_pMenu ) return;

	m_pMenu->Update();

	if( m_pMenu->IsEmpty() )
	{
		ChangeScene( new TitleScene( SCENE_TITLE, GetName() ) );
		return;
	}

	m_pMenu->Draw( Matrix3::Identity() );
}

//------------------------------------------------------------------------------
//	メニューの更新
//------------------------------------------------------------------------------
void PlayResultScene::_UpdateMenu( void )
{
	MenuLayer* pLayer = m_pMenu->GetCurLayer();
	if( !pLayer ) return;

	const SCORE_PARAM& ScoreParam = m_pScoreRes->GetData()->GetParam();

	int Rank	= GetApp()->GetSaveDataMng()->FindRecord( m_Score.PlayID );

	//	基本パラメータ
	pLayer->SetText( BONE_ARRIVAL,		"%s",			EnumToStr<int>( m_Score.Arrival, ARRIVAL_NAME_TABLE, ArrayNum(ARRIVAL_NAME_TABLE), "" ) );
	pLayer->SetText( BONE_PLAY_ID,		"%d",			m_Score.PlayID );
	pLayer->SetText( BONE_TIME,			"%d:%02d",		m_Score.FrameCount / 3600, m_Score.FrameCount / 60 % 60 );
	pLayer->SetText( BONE_SCORE,		"%d",			m_Score.Score );
	pLayer->SetText( BONE_ITEM,			"%d",			m_Score.ItemCount );
	pLayer->SetText( BONE_DESTROY,		"%d",			m_Score.DestroyCount );
	pLayer->SetText( BONE_RATE,			"%1.2f",		m_Score.RateAverage );
	pLayer->SetText( BONE_HEAT,			"%.0f%%",		m_Score.HeatAverage * 100.0f );

	pLayer->SetColor( BONE_ARRIVAL,		m_Score.Arrival >= ARRIVAL_ALL?		COLOR_ALL_CLEAR : Color::White() );

	//	色の設定
	float Rate = Coef( m_Score.RateAverage, ScoreParam.Rate.RateMin, ScoreParam.Rate.RateMax );
	float Heat = Coef( m_Score.HeatAverage, ScoreParam.Heat.HeatMin, ScoreParam.Heat.HeatMax );
	pLayer->SetColor( BONE_RATE,	Color::Lerp( COLOR_RATE_MIN, COLOR_RATE_MAX, Rate ) );
	pLayer->SetColor( BONE_HEAT,	Color::Lerp( COLOR_HEAT_MIN, COLOR_HEAT_MAX, Heat ) );

	//	ランク
	if( Rank >= 0 )
	{
		pLayer->SetText( BONE_RANK,	"%d",	Rank + 1 );
		pLayer->SetColor( BONE_RANK,		Rank == 0?	COLOR_RANK_1 :
											Rank == 1?	COLOR_RANK_2 : 
											Rank == 2?	COLOR_RANK_3 : Color::White() );
	}
	else
	{
		pLayer->SetBoneVisible( BONE_CAP_RANK, false );
		pLayer->SetBoneVisible( BONE_RANK, false );
	}

	//	ダメージ系
	if( m_Score.DamageCount == 0 && m_Score.Arrival >= ARRIVAL_ALL )
	{
		pLayer->SetColor( BONE_DAMAGE,	COLOR_NO_DAMAGE );
		pLayer->SetText( BONE_DAMAGE,	STR_NO_DAMAGE );
	}
	else if( m_Score.BreakCount == 0 && m_Score.Arrival >= ARRIVAL_ALL )
	{
		pLayer->SetColor( BONE_DAMAGE,	COLOR_NO_BREAK );
		pLayer->SetText( BONE_DAMAGE,	STR_NO_BREAK );
	}
	else if( m_Score.ContinueCount == 0 && m_Score.Arrival >= ARRIVAL_ALL )
	{
		pLayer->SetColor( BONE_DAMAGE,	COLOR_NO_CONT );
		pLayer->SetText( BONE_DAMAGE,	STR_NO_CONT );
	}
	else
	{
		pLayer->SetText( BONE_DAMAGE,	"" );
	}

	//	リスト変更
	MenuObject* pList = pLayer->GetObject( BONE_LIST );
	if( !pList ) return;

	pList->SetCellNum( m_vRecord.size() );

	SCORE_STAGE_RECORD PrevScore;
	for( int i = 0; i < (int)m_vRecord.size(); i++ )
	{
		const SCORE_STAGE_RECORD& Score = m_vRecord[i];
		int FrameCount = Score.FrameCount - PrevScore.FrameCount;

		pList->SetCellText( i, BONE_AREA,		"%s",		AREA_NAME_TABLE[i] );
		pList->SetCellText( i, BONE_SCORE,		"%d",		Score.Score - PrevScore.Score );

		pList->SetCellText( i, BONE_ITEM,		"%d",		Score.ItemCount - PrevScore.ItemCount );
		pList->SetCellText( i, BONE_DESTROY,	"%d",		Score.DestroyCount - PrevScore.DestroyCount );
		pList->SetCellText( i, BONE_RATE,		"%1.2f",	Score.Rate );
		pList->SetCellText( i, BONE_HEAT,		"%.0f%%",	Score.Heat * 100.0f );
		pList->SetCellText( i, BONE_TIME,		"%d:%02d",	FrameCount / 3600, FrameCount / 60 % 60 );

		//	色の設定
		float Rate = Coef( Score.Rate, ScoreParam.Rate.RateMin, ScoreParam.Rate.RateMax );
		float Heat = Coef( Score.Heat, ScoreParam.Heat.HeatMin, ScoreParam.Heat.HeatMax );
		pList->SetCellColor( i, BONE_RATE,	Color::Lerp( COLOR_RATE_MIN, COLOR_RATE_MAX, Rate ) );
		pList->SetCellColor( i, BONE_HEAT,	Color::Lerp( COLOR_HEAT_MIN, COLOR_HEAT_MAX, Heat ) );

		//	ダメージ系
		if( m_Score.Arrival >= ARRIVAL_ALL || i < (int)m_vRecord.size() - 1 )
		{
			if( Score.DamageCount - PrevScore.DamageCount == 0 )
			{
				pList->SetCellColor( i, BONE_DAMAGE,	COLOR_NO_DAMAGE );
				pList->SetCellText( i, BONE_DAMAGE,	STR_NO_DAMAGE_S );
			}
			else if( Score.BreakCount - PrevScore.BreakCount == 0 )
			{
				pList->SetCellColor( i, BONE_DAMAGE,	COLOR_NO_BREAK );
				pList->SetCellText( i, BONE_DAMAGE,	STR_NO_BREAK_S );
			}
			else if( Score.ContinueCount - PrevScore.ContinueCount == 0 )
			{
				pList->SetCellColor( i, BONE_DAMAGE,	COLOR_NO_CONT );
				pList->SetCellText( i, BONE_DAMAGE,	STR_NO_CONT_S );
			}
			else
			{
				pList->SetCellText( i, BONE_DAMAGE,	"" );
			}
		}
		else
		{
			pList->SetCellText( i, BONE_DAMAGE,	"" );
		}

		PrevScore = Score;
	}
}