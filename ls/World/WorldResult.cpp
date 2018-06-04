#include "stdafx.h"
#include "WorldResult.h"

#include "App/App.h"
#include "Effect/EffectManager.h"
#include "BGM/BGMManager.h"
#include "SpriteFrame/SpriteFrame.h"

#include "World/World.h"
#include "Score/ScoreManager.h"
#include "Score/ScoreData.h"

#include "Menu/Menu.h"
#include "Menu/MenuLayer.h"

namespace
{
	const Label		LAYER_NAME				= "Main";

	const Label		NOTIFY_KEY_STATE		= "State";
	const Label		NOTIFY_KEY_VOLUME		= "Volume";

	const Label		NOTIFY_SKIP				= "Skip";
	const Label		NOTIFY_ON				= "On";
	const Label		NOTIFY_OFF				= "Off";
	
	const Label		STATE_NAME_NONE			= "None";
	const Label		STATE_NAME_OPEN			= "Open";
	const Label		STATE_NAME_SCORE		= "Score";
	const Label		STATE_NAME_ITEM			= "Item";
	const Label		STATE_NAME_DESTROY		= "Destroy";
	const Label		STATE_NAME_TIME			= "Time";
	const Label		STATE_NAME_RATE			= "Rate";
	const Label		STATE_NAME_HEAT			= "Heat";
	const Label		STATE_NAME_GRAPH		= "Graph";
	const Label		STATE_NAME_DAMAGE		= "Damage";
	const Label		STATE_NAME_TOTAL		= "Total";
	const Label		STATE_NAME_COMPLETE		= "Complete";

	const Label		BONE_NAME_GRAPH_RATE	= "GraphRate";
	const Label		BONE_NAME_GRAPH_HEAT	= "GraphHeat";

	const float		DEFAULT_RATE			= 0.0f;
	const float		DEFAULT_HEAT			= 0.5f;

	const float		COUNT_COEF				= 60.0f;
	const float		GRAPH_COEF				= 0.05f;

	const Color		COLOR_RATE_MIN			= Color( 255,255,255 );
	const Color		COLOR_RATE_MAX			= Color( 255,240,64 );
	const Color		COLOR_HEAT_MIN			= Color( 128,128,255 );
	const Color		COLOR_HEAT_MAX			= Color( 255,128,128 );
	const Color		COLOR_NO_CONT			= Color( 192,220,255 );
	const Color		COLOR_NO_BREAK			= Color( 128,255,192 );
	const Color		COLOR_NO_DAMAGE			= Color( 255,240,64 );
	
	const int		BGM_ON_FRAME			= 10;
	const int		BGM_OFF_FRAME			= 30;
	const float		BGM_FADE_VOLUME			= 0.05f;

	const int		SOUND_INTERVAL			= 3;
	
	pstr			STR_NO_CONT				= "No Continue";
	pstr			STR_NO_BREAK			= "No Break";
	pstr			STR_NO_DAMAGE			= "No Damage";
}


////////////////////////////////////////////////////////////////////////////////
//
//	ワールド：ステージリザルト
//
////////////////////////////////////////////////////////////////////////////////
WorldResult::WorldResult( World* pWorld )
{
	_ASSERT( pWorld );

	m_pWorld		= pWorld;
	m_pMenu			= NULL;

	m_pScoreSound	= NULL;
	m_SoundInterval	= 0;

	m_bActive		= false;
	m_State			= STATE_NONE;
	m_TotalScore	= 0;
}

WorldResult::~WorldResult()
{
	//	エフェクトの更新再開
	GetApp()->GetEffectMng()->SetUpdateEnable( true );

	SafeDelete( m_pMenu );
}

//******************************************************************************
//	初期化
//******************************************************************************
void WorldResult::Init( const MenuData* pMenuData, const SoundFile* pScoreSound )
{
	if( pMenuData )
	{
		m_pMenu = new Menu( this );
		MEM_CHECK( m_pMenu );

		m_pMenu->Init( pMenuData, Matrix3::Trans( m_pWorld->GetFieldRect().Center() ) );
	}

	m_pScoreSound = pScoreSound;

	m_bActive	= false;
}

//******************************************************************************
//	開く
//******************************************************************************
void WorldResult::Open( void )
{
	m_bActive		= true;
	m_State			= STATE_NONE;
	m_SoundInterval = 0;

	//	メニュー開く
	SafePtr(m_pMenu)->OpenLayer( LAYER_NAME );

	//	エフェクトの更新停止
	GetApp()->GetEffectMng()->SetUpdateEnable( false );

	//	スコア初期化
	m_TotalScore = 0;

	m_Score.Reset();
	m_CurScore.Reset();
	m_pWorld->GetScoreManager()->GetStageResult( m_Score );

	int Size = (int)m_pWorld->GetScoreManager()->GetRecord().size();
	m_vRate.clear();
	m_vHeat.clear();
	m_vRate.resize( Size, DEFAULT_RATE );
	m_vHeat.resize( Size, DEFAULT_HEAT );
}

//******************************************************************************
//	更新
//******************************************************************************
void WorldResult::Update( void )
{
	SafePtr(m_pMenu)->Update();

	if( m_bActive )
	{
		MenuLayer* pLayer = m_pMenu->GetCurLayer();
		if( pLayer && pLayer->GetName() == LAYER_NAME )	
		{
			//	メニュー表示更新
			_UpdateIndicate( pLayer );

			//	BGM更新
			_UpdateBGM( pLayer );
		}

		//	終了
		if( m_pMenu && m_pMenu->IsEmpty() )
		{
			m_bActive = false;
			
			//	エフェクトの更新
			GetApp()->GetEffectMng()->SetUpdateEnable( true );
		}
	}
}

//******************************************************************************
//	描画
//******************************************************************************
void WorldResult::Draw( void )
{
	if( m_pMenu ) m_pMenu->Draw( m_pWorld->GetDrawOffset() );
}

//******************************************************************************
//	通知
//******************************************************************************
void WorldResult::OnLayerNotify( MenuLayer* pLayer, Label Name, float Param )
{
	if( Name == NOTIFY_SKIP )
	{
		if( m_State >= STATE_COMPLETE )
		{
			//	閉じる
			m_pMenu->CloseCurLayer();
		}
		else if( m_State >= STATE_OPEN )
		{
			//	スキップ
			m_CurScore = m_Score;
			m_TotalScore = m_pWorld->GetScoreManager()->GetScore();

			m_State = STATE_COMPLETE;
		}
	}
}

//------------------------------------------------------------------------------
//	メニュー表示更新
//------------------------------------------------------------------------------
void WorldResult::_UpdateIndicate( MenuLayer* pLayer )
{
	const LabelPair<STATE>	STATE_TABLE[] =
	{
		{ STATE_NAME_NONE,		STATE_NONE,		},
		{ STATE_NAME_OPEN,		STATE_OPEN,		},
		{ STATE_NAME_SCORE,		STATE_SCORE,	},
		{ STATE_NAME_ITEM,		STATE_ITEM,		},
		{ STATE_NAME_DESTROY,	STATE_DESTROY,	},
		{ STATE_NAME_TIME,		STATE_TIME,		},
		{ STATE_NAME_RATE,		STATE_RATE,		},
		{ STATE_NAME_HEAT,		STATE_HEAT,		},
		{ STATE_NAME_GRAPH,		STATE_GRAPH,	},
		{ STATE_NAME_DAMAGE,	STATE_DAMAGE,	},
		{ STATE_NAME_TOTAL,		STATE_TOTAL,	},
		{ STATE_NAME_COMPLETE,	STATE_COMPLETE,	},
	};

	const SpriteFrame* pSprite = pLayer->GetSprite();

	const SCORE_PARAM& ScoreParam = m_pWorld->GetScoreManager()->GetData()->GetParam();
	uint Total = m_pWorld->GetScoreManager()->GetScore();

	//	通知を受け取る
	Label Notify = pSprite->GetAnimeNotify( NOTIFY_KEY_STATE );
	STATE State = LabelToEnum<STATE>( Notify, STATE_TABLE, ArrayNum(STATE_TABLE), STATE_NONE );

	//	状態更新
	if( m_State < State ) m_State = State;

	//	スコア更新
	if( m_State >= STATE_SCORE )
	{
		m_CurScore.Score = NearTo( m_CurScore.Score, m_Score.Score, Abs( (int)ceilf( (float)m_Score.Score / COUNT_COEF) ) );
		pLayer->SetText( STATE_NAME_SCORE, "%d", m_CurScore.Score );
	}
	if( m_State >= STATE_ITEM )
	{
		Increase( m_CurScore.ItemCount, m_Score.ItemCount, (uint)ceilf( (float)m_Score.ItemCount / COUNT_COEF) );
		pLayer->SetText( STATE_NAME_ITEM, "%d", m_CurScore.ItemCount );
	}
	if( m_State >= STATE_DESTROY )
	{
		Increase( m_CurScore.DestroyCount, m_Score.DestroyCount, (uint)ceilf( (float)m_Score.DestroyCount / COUNT_COEF) );
		pLayer->SetText( STATE_NAME_DESTROY, "%d", m_CurScore.DestroyCount );
	}
	if( m_State >= STATE_TIME )
	{
		Increase( m_CurScore.FrameCount, m_Score.FrameCount, (uint)ceilf( (float)m_Score.FrameCount / COUNT_COEF) );

		int Second = (m_CurScore.FrameCount / 60) % 60;
		int Minute = (m_CurScore.FrameCount / 60) / 60;

		pLayer->SetText( STATE_NAME_TIME, "%d:%02d", Minute, Second );
	}
	if( m_State >= STATE_RATE )
	{
		Increase( m_CurScore.Rate, m_Score.Rate, (float)m_Score.Rate / COUNT_COEF );
		pLayer->SetText( STATE_NAME_RATE, "%1.2f", m_CurScore.Rate );

		//	色の設定
		float t = Coef( m_CurScore.Rate, ScoreParam.Rate.RateMin, ScoreParam.Rate.RateMax );
		pLayer->SetColor( STATE_NAME_RATE, Color::Lerp( COLOR_RATE_MIN, COLOR_RATE_MAX, t ) );
	}
	if( m_State >= STATE_HEAT )
	{
		Increase( m_CurScore.Heat, m_Score.Heat, (float)m_Score.Heat / COUNT_COEF );
		pLayer->SetText( STATE_NAME_HEAT, "%.0f%%", m_CurScore.Heat * 100.0f );
		
		//	色の設定
		float t = Coef( m_CurScore.Heat, ScoreParam.Heat.HeatMin, ScoreParam.Heat.HeatMax );
		pLayer->SetColor( STATE_NAME_HEAT, Color::Lerp( COLOR_HEAT_MIN, COLOR_HEAT_MAX, t ) );
	}
	if( m_State >= STATE_GRAPH )
	{
		//	グラフ
		const SCORE_RECORD_VEC&	vRecord = m_pWorld->GetScoreManager()->GetRecord();
		const IntVec&			vArrival = m_pWorld->GetScoreManager()->GetArrivalIndex();
		_ASSERT( vRecord.size() >= m_vRate.size() && vRecord.size() >= m_vHeat.size() );

		for( int i = 0; i < (int)m_vRate.size(); i++ )
		{
			m_vRate[i] = Lerp( m_vRate[i], vRecord[i].Rate, GRAPH_COEF );
			m_vHeat[i] = Lerp( m_vHeat[i], vRecord[i].Heat, GRAPH_COEF );
		}

		pLayer->SetLineGraph( BONE_NAME_GRAPH_RATE, m_vRate, &vArrival );
		pLayer->SetLineGraph( BONE_NAME_GRAPH_HEAT, m_vHeat );
	}
	if( m_State >= STATE_DAMAGE )
	{
		//	ダメージ系
		if( m_Score.DamageCount == 0 )
		{
			pLayer->SetColor( STATE_NAME_DAMAGE,	COLOR_NO_DAMAGE );
			pLayer->SetText( STATE_NAME_DAMAGE,		STR_NO_DAMAGE );
		}
		else if( m_Score.BreakCount == 0 )
		{
			pLayer->SetColor( STATE_NAME_DAMAGE,	COLOR_NO_BREAK );
			pLayer->SetText( STATE_NAME_DAMAGE,		STR_NO_BREAK );
		}
		else if( m_Score.ContinueCount == 0 )
		{
			pLayer->SetColor( STATE_NAME_DAMAGE,	COLOR_NO_CONT );
			pLayer->SetText( STATE_NAME_DAMAGE,		STR_NO_CONT );
		}
		else
		{
			pLayer->SetText( STATE_NAME_DAMAGE,	"" );
		}
	}
	if( m_State >= STATE_TOTAL )
	{
		Increase( m_TotalScore, Total, (uint)ceilf( (float)Total / COUNT_COEF) );
		pLayer->SetText( STATE_NAME_TOTAL, "%d", m_TotalScore );

		//	終了チェック
		if( m_TotalScore >= Total )
		{
			m_State = STATE_COMPLETE;
		}
	}

	//	サウンド更新
	DecreaseZero( m_SoundInterval, 1 );
	if( m_State >= STATE_SCORE &&
		m_SoundInterval == 0 && m_pScoreSound )
	{
		if( m_Score != m_CurScore ||
			Total != m_TotalScore )
		{
			GetTone()->PlaySound( m_pScoreSound, SOUND_CHANNEL_SE, false );

			m_SoundInterval = SOUND_INTERVAL;
		}
	}
}

//------------------------------------------------------------------------------
//	BGM音量更新
//------------------------------------------------------------------------------
void WorldResult::_UpdateBGM( MenuLayer* pLayer )
{
	const SpriteFrame* pSprite = pLayer->GetSprite();

	Label Notify = pSprite->GetAnimeNotify( NOTIFY_KEY_VOLUME );

	//	BGMをON
	if( Notify == NOTIFY_ON )
	{
		GetApp()->GetBGMMng()->FadeVolume( BGM_ON_FRAME, 1.0f );
	}
	//	BGMをOFF
	else if( Notify == NOTIFY_OFF )
	{
		GetApp()->GetBGMMng()->FadeVolume( BGM_OFF_FRAME, BGM_FADE_VOLUME );
	}
}