#include "stdafx.h"
#include "WorldPause.h"

#include "App/App.h"
#include "Effect/EffectManager.h"

#include "Menu/Menu.h"

#include "World/World.h"
#include "Controller/SystemController.h"
#include "Replay/ReplayManager.h"


namespace
{
	const Label		LAYER_NAME		= "Main";
	const Label		NOTIFY_EXIT		= "Exit";
}

////////////////////////////////////////////////////////////////////////////////
//
//	ワールド：ポーズ管理
//
////////////////////////////////////////////////////////////////////////////////
WorldPause::WorldPause( World* pWorld )
{
	_ASSERT( pWorld );

	m_pWorld		= pWorld;
	m_pMenu			= NULL;
	m_bPause		= false;
	m_bHide			= false;
}

WorldPause::~WorldPause()
{
	//	エフェクトの更新再開
	GetApp()->GetEffectMng()->SetUpdateEnable( true );

	SafeDelete( m_pMenu );
}

//******************************************************************************
//	初期化
//******************************************************************************
void WorldPause::Init( const MenuData* pMenuData, bool bPauseExit )
{
	if( pMenuData )
	{
		m_pMenu = new Menu( this );
		MEM_CHECK( m_pMenu );

		m_pMenu->Init( pMenuData, Matrix3::Trans( m_pWorld->GetFieldRect().Center() ) );
	}
	m_bPauseExit = bPauseExit;

	m_bPause = false;
}

//******************************************************************************
//	更新
//******************************************************************************
void WorldPause::Update( void )
{
	if( !m_bHide )
	{
		SafePtr(m_pMenu)->Update();
	}

	SystemController Ctrl;

	if( !m_pWorld->IsFade() )
	{
		if( m_bPauseExit )
		{
			//	ポーズキーで終了
			if( Ctrl.IsInput( CONTROLLER_SYSTEM_PAUSE, INPUT_TRIG ) )
			{
				m_pWorld->Exit( World::EXITCODE_EXIT );
				return;
			}
		}
		else
		{
			if( !m_bPause )
			{
				//	ポーズキー押下
				if( Ctrl.IsInput( CONTROLLER_SYSTEM_PAUSE, INPUT_TRIG ) )
				{
					//	リプレイ中
					if( m_pWorld->GetReplayManager()->IsReplayEnable() )
					{
						m_pWorld->OpenReplayMenu( true );
						return;
					}

					m_bPause = true;

					SafePtr(m_pMenu)->OpenLayer( LAYER_NAME );

					//	エフェクトの更新停止
					GetApp()->GetEffectMng()->SetUpdateEnable( false );
				}
			}
			else
			{
				m_bHide = Ctrl.IsInput( CONTROLLER_SYSTEM_CANCEL, INPUT_PRESS );

				if( !m_bHide )
				{
					//	解除
					if( m_pMenu && m_pMenu->IsEmpty() )
					{
						m_bPause = false;
						
						//	エフェクトの更新再開
						GetApp()->GetEffectMng()->SetUpdateEnable( true );
					}
				}
			}
		}
	}
}

//******************************************************************************
//	描画
//******************************************************************************
void WorldPause::Draw( void )
{
	if( !m_bHide )
	{
		if( m_pMenu ) m_pMenu->Draw( m_pWorld->GetDrawOffset() );
	}
}

//******************************************************************************
//	通知
//******************************************************************************
void WorldPause::OnLayerNotify( MenuLayer* pLayer, Label Name, float Param )
{
	//	終了
	if( Name == NOTIFY_EXIT )
	{
		m_pWorld->Exit( World::EXITCODE_EXIT );
	}
}

//******************************************************************************
//	通知
//******************************************************************************
void WorldPause::OnObjectNotify( MenuObject* pObject, Label Name, float Param )
{

}