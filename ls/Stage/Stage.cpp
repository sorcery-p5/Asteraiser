#include "stdafx.h"
#include "Stage.h"
#include "StageManager.h"
#include "StageData.h"
#include "StageState.h"
#include "StageTelop.h"

#include "Action/StageAction.h"
#include "Action/StagePlayerMoveAction.h"


////////////////////////////////////////////////////////////////////////////////
//
//	ステージ
//
////////////////////////////////////////////////////////////////////////////////
Stage::Stage( StageManager* pManager )
{
	_ASSERT( pManager );

	m_pManager		= pManager;
	m_pData			= NULL;

	m_FrameCount	= 0;
	m_bEnd			= false;
	m_pState		= NULL;
}

Stage::~Stage()
{
	DeleteVec( m_ActionList );
	DeleteVec( m_TelopList );
	SafeDelete( m_pState );
}

//******************************************************************************
//	初期化
//******************************************************************************
void Stage::Init( const StageData* pData, Label State )
{
	_ASSERT( pData );
	m_pData			= pData;

	m_FrameCount	= 0;

	//	ステート
	m_pState		= new StageState( this );
	MEM_CHECK( m_pState );
	m_pState->Init( m_pData, State );
}

//******************************************************************************
//	更新
//******************************************************************************
void Stage::Update( void )
{
	//	ステート
	m_pState->Update();

	//	アクション
	for( ActionList::iterator it = m_ActionList.begin(); it != m_ActionList.end(); )
	{
		(*it)->Update();

		//	削除
		if( (*it)->IsDelete() )
		{
			SafeDelete( *it );
			it = m_ActionList.erase( it );
		}
		else
		{
			++it;
		}
	}

	//	テロップ
	for( TelopList::iterator it = m_TelopList.begin(); it != m_TelopList.end(); )
	{
		(*it)->Update();

		//	削除
		if( (*it)->IsDelete() )
		{
			SafeDelete( *it );
			it = m_TelopList.erase( it );
		}
		else
		{
			++it;
		}
	}

	//	カウント
	m_FrameCount++;

	if( m_pData->GetParam().Time &&
		m_FrameCount >= m_pData->GetParam().Time )
	{
		//	終了
		End();
	}
}

//******************************************************************************
//	描画
//******************************************************************************
void Stage::Draw( void )
{
	//	テロップ
	for( TelopList::const_iterator it = m_TelopList.begin(); it != m_TelopList.end(); ++it )
	{
		(*it)->Draw();
	}
}

//******************************************************************************
//	アクション追加
//******************************************************************************
void Stage::AddAction( const STAGE_ACTION* pActionParam )
{
	StageAction* pAction = _CreateAction( pActionParam );
	if( !pAction ) return;

	pAction->Init( pActionParam );

	m_ActionList.push_back( pAction );
}

//******************************************************************************
//	テロップ追加
//******************************************************************************
void Stage::AddTelop( Label Name, const Matrix3& Mat )
{
	const STAGE_TELOP* pParam = m_pData->GetTelop( Name );
	if( !pParam ) return;

	const SpriteFrameFile* pFile = m_pData->GetSpriteFrameFile( pParam->SpriteName );
	if( !pFile ) return;

	StageTelop* pTelop = new StageTelop( this );
	MEM_CHECK( pTelop );

	pTelop->Init( pFile, pParam, Name, Mat );

	m_TelopList.push_back( pTelop );
}

//******************************************************************************
//	通知を追加する
//******************************************************************************
void Stage::AddNotify( Label Notify )
{
	m_pState->AddNotify( Notify );
}

//******************************************************************************
//	テロップが終了しているかどうか
//******************************************************************************
bool Stage::IsTelopEnd( Label Name ) const
{
	for( TelopList::const_iterator it = m_TelopList.begin(); it != m_TelopList.end(); ++it )
	{
		if( (*it)->GetName() == Name )
		{
			return false;
		}
	}
	return true;
}

//******************************************************************************
//	テロップの取得
//******************************************************************************
StageTelop* Stage::GetTelop( Label Name ) const
{
	for( TelopList::const_iterator it = m_TelopList.begin(); it != m_TelopList.end(); ++it )
	{
		if( (*it)->GetName() == Name )
		{
			return (*it);
		}
	}
	return NULL;
}

//******************************************************************************
//	ワールド取得
//******************************************************************************
World* Stage::GetWorld( void )
{
	return m_pManager->GetWorld();
}

//------------------------------------------------------------------------------
//	アクションを作成
//------------------------------------------------------------------------------
StageAction* Stage::_CreateAction( const STAGE_ACTION* pActionParam )
{
	_ASSERT( pActionParam );

	switch( pActionParam->Type )
	{
	case STAGE_ACTION_PLAYER_POS:		return new StagePlayerMoveAction( this );
	}

	return NULL;
}