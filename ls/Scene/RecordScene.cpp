#include "stdafx.h"
#include "RecordScene.h"
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
	pstr		MENU_PATH		= "Data/Menu/Record.xml";
	pstr		SCORE_PATH		= "Data/Score/Score.xml";

	pstr		SCENE_TITLE		= "Title";

	const Label	LAYER_MAIN		= "Main";
	const Label	LAYER_AREA		= "Area";

	const Label	NOTIFY_EXIT		= "Exit";
	const Label	NOTIFY_DECIDE	= "Decide";
	const Label	NOTIFY_START	= "Start";

	const Label BONE_LIST		= "List";
	const Label BONE_ARRIVAL	= "Arrival";
	const Label BONE_PLAY_NUM	= "PlayNum";
	const Label BONE_TIME		= "Time";
	const Label BONE_SCORE		= "Score";
	const Label BONE_ITEM		= "Item";
	const Label BONE_DESTROY	= "Destroy";
	const Label BONE_LAST_DATE	= "LastDate";
	const Label BONE_CAP_LAST	= "CapLastDate";
	const Label BONE_DAMAGE		= "Damage";
	const Label BONE_CLEAR_NUM	= "CrearNum";
	const Label BONE_PLAY_ID	= "PlayID";
	const Label BONE_RANK		= "Rank";
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
//	プレイ記録シーン
//
///////////////////////////////////////////////////////////////////////////////
RecordScene::RecordScene( Label Name )
:Scene( Name )
{
	m_pMenuRes		= NULL;
	m_pMenu			= NULL;
	m_pScoreRes		= NULL;
}
RecordScene::~RecordScene()
{
	SafeDelete( m_pMenu );
	SafeRelease( m_pMenuRes );
	SafeRelease( m_pScoreRes );
}

//*****************************************************************************
//	初期化
//*****************************************************************************
void RecordScene::OnInit( void )
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
void RecordScene::Update( void )
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
void RecordScene::_UpdateMenu( void )
{
	MenuLayer* pLayer = m_pMenu->GetCurLayer();
	if( !pLayer ) return;

	const SCORE_PARAM& ScoreParam = m_pScoreRes->GetData()->GetParam();

	//	基本パラメータ
	const SAVEDATA& SaveData = GetApp()->GetSaveDataMng()->GetSaveData();

	pLayer->SetText( BONE_ARRIVAL,		"%s",			EnumToStr<int>( SaveData.Arrival, ARRIVAL_NAME_TABLE, ArrayNum(ARRIVAL_NAME_TABLE), "" ) );
	pLayer->SetText( BONE_PLAY_NUM,		"%d",			SaveData.PlayNum );
	pLayer->SetText( BONE_TIME,			"%d:%02d:%02d",	SaveData.FrameCount	/ 216000, SaveData.FrameCount / 3600 % 60, SaveData.FrameCount / 60 % 60 );
	pLayer->SetText( BONE_SCORE,		"%d",			SaveData.TotalScore );
	pLayer->SetText( BONE_ITEM,			"%d",			SaveData.ItemCount );
	pLayer->SetText( BONE_DESTROY,		"%d",			SaveData.DestroyCount );
	
	pLayer->SetColor( BONE_ARRIVAL,		SaveData.Arrival >= ARRIVAL_ALL?	COLOR_ALL_CLEAR : Color::White() );

	//	最終更新
	if( SaveData.LastDateTime.empty() )
	{
		pLayer->SetBoneVisible( BONE_CAP_LAST, false );
		pLayer->SetBoneVisible( BONE_LAST_DATE, false );
	}
	else
	{
		pLayer->SetText( BONE_LAST_DATE,	"%s",		SaveData.LastDateTime.c_str() );
	}

	//	クリア回数
	if( SaveData.ClearNum <= 0 )
	{
		pLayer->SetBoneVisible( BONE_CLEAR_NUM, false );
	}
	else
	{
		pLayer->SetText( BONE_CLEAR_NUM, "Clear %3d", SaveData.ClearNum );
	}

	//	ダメージ系
	if( SaveData.bNoDamage && SaveData.Arrival >= ARRIVAL_ALL )
	{
		pLayer->SetColor( BONE_DAMAGE,	COLOR_NO_DAMAGE );
		pLayer->SetText( BONE_DAMAGE,	STR_NO_DAMAGE );
	}
	else if( SaveData.bNoBreak && SaveData.Arrival >= ARRIVAL_ALL )
	{
		pLayer->SetColor( BONE_DAMAGE,	COLOR_NO_BREAK );
		pLayer->SetText( BONE_DAMAGE,	STR_NO_BREAK );
	}
	else if( SaveData.bNoContinue && SaveData.Arrival >= ARRIVAL_ALL )
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

	pList->SetCellNum( GetApp()->GetSaveDataMng()->GetScoreNum() );

	for( int i = 0; i < GetApp()->GetSaveDataMng()->GetScoreNum(); i++ )
	{
		const SAVEDATA_SCORE& Score = GetApp()->GetSaveDataMng()->GetScore( i );

		pList->SetCellText( i, BONE_RANK,		"%d",		i + 1 );
		pList->SetCellText( i, BONE_SCORE,		"%d",		Score.Score );
		pList->SetCellText( i, BONE_ARRIVAL,	"%s",		EnumToStr<int>( Score.Arrival, ARRIVAL_NAME_TABLE, ArrayNum(ARRIVAL_NAME_TABLE), "" ) );

		pList->SetCellText( i, BONE_ITEM,		"%d",		Score.ItemCount );
		pList->SetCellText( i, BONE_DESTROY,	"%d",		Score.DestroyCount );
		pList->SetCellText( i, BONE_RATE,		"%1.2f",	Score.RateAverage );
		pList->SetCellText( i, BONE_HEAT,		"%.0f%%",	Score.HeatAverage * 100.0f );
		pList->SetCellText( i, BONE_PLAY_ID,	"%d",		Score.PlayID );

		pList->SetCellColor( i, BONE_ARRIVAL,	Score.Arrival >= ARRIVAL_ALL?	COLOR_ALL_CLEAR : Color::White() );

		pList->SetCellColor( i, BONE_RANK,		i == 0?	COLOR_RANK_1 :
												i == 1?	COLOR_RANK_2 : 
												i == 2?	COLOR_RANK_3 : Color::White() );
		//	色の設定
		float Rate = Coef( Score.RateAverage, ScoreParam.Rate.RateMin, ScoreParam.Rate.RateMax );
		float Heat = Coef( Score.HeatAverage, ScoreParam.Heat.HeatMin, ScoreParam.Heat.HeatMax );
		pList->SetCellColor( i, BONE_RATE,	Color::Lerp( COLOR_RATE_MIN, COLOR_RATE_MAX, Rate ) );
		pList->SetCellColor( i, BONE_HEAT,	Color::Lerp( COLOR_HEAT_MIN, COLOR_HEAT_MAX, Heat ) );

		//	ダメージ系
		if( Score.DamageCount == 0 && Score.Arrival >= ARRIVAL_ALL )
		{
			pList->SetCellColor( i, BONE_DAMAGE,	COLOR_NO_DAMAGE );
			pList->SetCellText( i, BONE_DAMAGE,	STR_NO_DAMAGE_S );
		}
		else if( Score.BreakCount == 0 && Score.Arrival >= ARRIVAL_ALL )
		{
			pList->SetCellColor( i, BONE_DAMAGE,	COLOR_NO_BREAK );
			pList->SetCellText( i, BONE_DAMAGE,	STR_NO_BREAK_S );
		}
		else if( Score.ContinueCount == 0 && Score.Arrival >= ARRIVAL_ALL )
		{
			pList->SetCellColor( i, BONE_DAMAGE,	COLOR_NO_CONT );
			pList->SetCellText( i, BONE_DAMAGE,	STR_NO_CONT_S );
		}
		else
		{
			pList->SetCellText( i, BONE_DAMAGE,	"" );
		}
	}
}