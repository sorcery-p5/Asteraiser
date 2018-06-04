#include "stdafx.h"
#include "StageManager.h"
#include "Stage.h"

#include "World/World.h"
#include "Score/ScoreManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	ステージ管理
//
////////////////////////////////////////////////////////////////////////////////
StageManager::StageManager( World* pWorld )
{
	_ASSERT( pWorld );

	m_pWorld		= pWorld;
	m_StageIndex	= 0;

	m_pCurStage		= NULL;
	m_bEndAll		= false;
}
StageManager::~StageManager()
{
	SafeDelete( m_pCurStage );
}

//******************************************************************************
//	更新
//******************************************************************************
void StageManager::Update( void )
{
	if( m_pCurStage )
	{
		m_pCurStage->Update();
			
		//	終了
		if( m_pCurStage->IsEnd() )
		{
			_ChangeNextStage();
		}
	}
}

//******************************************************************************
//	描画
//******************************************************************************
void StageManager::Draw( void )
{
	if( m_pCurStage )
	{
		m_pCurStage->Draw();
	}
}

//******************************************************************************
//	開始
//******************************************************************************
void StageManager::Start( int InitStage, Label InitState )
{
	m_StageIndex = InitStage;

	if( !m_vStageData.empty() )
	{
		m_pCurStage = new Stage( this );
		MEM_CHECK( m_pCurStage );

		m_pCurStage->Init( m_vStageData[m_StageIndex], InitState );
	}
}

//******************************************************************************
//	ステージデータ追加
//******************************************************************************
void StageManager::PushStageData( const StageData* pData )
{
	_ASSERT( pData );

	m_vStageData.push_back( pData );
}

//------------------------------------------------------------------------------
//	次のステージへ移行
//------------------------------------------------------------------------------
void StageManager::_ChangeNextStage( void )
{
	//	以前のステージ終了
	SafeDelete( m_pCurStage );

	m_StageIndex++;

	//	次のステージへ
	if( m_StageIndex < (int)m_vStageData.size() &&
		!m_pWorld->IsOneStageMode() )
	{
		//	ワールド通知
		m_pWorld->OnChangeStage( m_StageIndex );

		m_pCurStage = new Stage( this );
		MEM_CHECK( m_pCurStage );

		m_pCurStage->Init( m_vStageData[m_StageIndex], Label() );
	}
	//	終了
	else
	{
		m_bEndAll = true;
	}
}