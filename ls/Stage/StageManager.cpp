#include "stdafx.h"
#include "StageManager.h"
#include "Stage.h"

#include "World/World.h"
#include "Score/ScoreManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�X�e�[�W�Ǘ�
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
//	�X�V
//******************************************************************************
void StageManager::Update( void )
{
	if( m_pCurStage )
	{
		m_pCurStage->Update();
			
		//	�I��
		if( m_pCurStage->IsEnd() )
		{
			_ChangeNextStage();
		}
	}
}

//******************************************************************************
//	�`��
//******************************************************************************
void StageManager::Draw( void )
{
	if( m_pCurStage )
	{
		m_pCurStage->Draw();
	}
}

//******************************************************************************
//	�J�n
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
//	�X�e�[�W�f�[�^�ǉ�
//******************************************************************************
void StageManager::PushStageData( const StageData* pData )
{
	_ASSERT( pData );

	m_vStageData.push_back( pData );
}

//------------------------------------------------------------------------------
//	���̃X�e�[�W�ֈڍs
//------------------------------------------------------------------------------
void StageManager::_ChangeNextStage( void )
{
	//	�ȑO�̃X�e�[�W�I��
	SafeDelete( m_pCurStage );

	m_StageIndex++;

	//	���̃X�e�[�W��
	if( m_StageIndex < (int)m_vStageData.size() &&
		!m_pWorld->IsOneStageMode() )
	{
		//	���[���h�ʒm
		m_pWorld->OnChangeStage( m_StageIndex );

		m_pCurStage = new Stage( this );
		MEM_CHECK( m_pCurStage );

		m_pCurStage->Init( m_vStageData[m_StageIndex], Label() );
	}
	//	�I��
	else
	{
		m_bEndAll = true;
	}
}