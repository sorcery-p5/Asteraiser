#include "stdafx.h"
#include "WorldContinue.h"

#include "App/App.h"
#include "Effect/EffectManager.h"

#include "World/World.h"
#include "Replay/ReplayManager.h"

#include "Menu/Menu.h"
#include "Menu/MenuLayer.h"

namespace
{
	const Label		LAYER_NAME		= "Main";
	const Label		BONE_CREDIT		= "Credit";

	const Label		NOTIFY_DECIDE	= "Decide";
	const Label		NOTIFY_EXIT		= "Exit";

	pstr			STRING_CREDIT	= "Rest:%02d";
}


////////////////////////////////////////////////////////////////////////////////
//
//	ワールド：コンティニュー管理
//
////////////////////////////////////////////////////////////////////////////////
WorldContinue::WorldContinue( World* pWorld )
{
	_ASSERT( pWorld );

	m_pWorld		= pWorld;
	m_pMenu			= NULL;

	m_Credit		= 0;
	m_InitCredit	= 0;
	m_bActive		= false;
}

WorldContinue::~WorldContinue()
{
	//	エフェクトの更新再開
	GetApp()->GetEffectMng()->SetUpdateEnable( true );

	SafeDelete( m_pMenu );
}

//******************************************************************************
//	初期化
//******************************************************************************
void WorldContinue::Init( const MenuData* pMenuData, int Credit )
{
	if( pMenuData )
	{
		m_pMenu = new Menu( this );
		MEM_CHECK( m_pMenu );

		m_pMenu->Init( pMenuData, Matrix3::Trans( m_pWorld->GetFieldRect().Center() ) );
	}

	m_bActive		= false;
	m_Credit		= Credit;
	m_InitCredit	= Credit;
}

//******************************************************************************
//	開く
//******************************************************************************
void WorldContinue::Open( void )
{
	_ASSERT( m_Credit > 0 );

	m_bActive = true;

	//	メニュー開く
	SafePtr(m_pMenu)->OpenLayer( LAYER_NAME );

	//	エフェクトの更新停止
	GetApp()->GetEffectMng()->SetUpdateEnable( false );
}

//******************************************************************************
//	自動コンティニュー
//******************************************************************************
void WorldContinue::AutoDecide( void )
{
	_ASSERT( m_Credit > 0 );

	m_bActive = true;

	//	クレジット減らす
	DecreaseZero( m_Credit, 1 );

	m_pWorld->OnContinue();

	//	エフェクトの更新停止
	GetApp()->GetEffectMng()->SetUpdateEnable( false );
}

//******************************************************************************
//	更新
//******************************************************************************
void WorldContinue::Update( void )
{
	SafePtr(m_pMenu)->Update();

	if( m_bActive )
	{
		//	テキスト設定
		MenuLayer* pLayer = m_pMenu->GetCurLayer();
		if( pLayer && pLayer->GetName() == LAYER_NAME )	
		{
			pLayer->SetText( BONE_CREDIT, STRING_CREDIT, m_Credit );
		}

		//	解除
		if( !m_pMenu || m_pMenu->IsEmpty() )
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
void WorldContinue::Draw( void )
{
	if( m_pMenu ) m_pMenu->Draw( m_pWorld->GetDrawOffset() );
}

//******************************************************************************
//	レイヤーの通知
//******************************************************************************
void WorldContinue::OnLayerNotify( MenuLayer* pLayer, Label Name, float Param )
{
	//	終了
	if( Name == NOTIFY_EXIT )
	{
		m_pWorld->Exit( World::EXITCODE_DEAD );
	}
}

//******************************************************************************
//	オブジェクトの通知
//******************************************************************************
void WorldContinue::OnObjectNotify( MenuObject* pObject, Label Name, float Param )
{
	//	継続
	if( Name == NOTIFY_DECIDE )
	{
		//	クレジット減らす
		DecreaseZero( m_Credit, 1 );

		m_pWorld->OnContinue();
	}
	//	終了
	else if( Name == NOTIFY_EXIT )
	{
		m_pWorld->Exit( World::EXITCODE_DEAD );
	}
}