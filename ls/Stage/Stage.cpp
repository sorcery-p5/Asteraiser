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
//	�X�e�[�W
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
//	������
//******************************************************************************
void Stage::Init( const StageData* pData, Label State )
{
	_ASSERT( pData );
	m_pData			= pData;

	m_FrameCount	= 0;

	//	�X�e�[�g
	m_pState		= new StageState( this );
	MEM_CHECK( m_pState );
	m_pState->Init( m_pData, State );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void Stage::Update( void )
{
	//	�X�e�[�g
	m_pState->Update();

	//	�A�N�V����
	for( ActionList::iterator it = m_ActionList.begin(); it != m_ActionList.end(); )
	{
		(*it)->Update();

		//	�폜
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

	//	�e���b�v
	for( TelopList::iterator it = m_TelopList.begin(); it != m_TelopList.end(); )
	{
		(*it)->Update();

		//	�폜
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

	//	�J�E���g
	m_FrameCount++;

	if( m_pData->GetParam().Time &&
		m_FrameCount >= m_pData->GetParam().Time )
	{
		//	�I��
		End();
	}
}

//******************************************************************************
//	�`��
//******************************************************************************
void Stage::Draw( void )
{
	//	�e���b�v
	for( TelopList::const_iterator it = m_TelopList.begin(); it != m_TelopList.end(); ++it )
	{
		(*it)->Draw();
	}
}

//******************************************************************************
//	�A�N�V�����ǉ�
//******************************************************************************
void Stage::AddAction( const STAGE_ACTION* pActionParam )
{
	StageAction* pAction = _CreateAction( pActionParam );
	if( !pAction ) return;

	pAction->Init( pActionParam );

	m_ActionList.push_back( pAction );
}

//******************************************************************************
//	�e���b�v�ǉ�
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
//	�ʒm��ǉ�����
//******************************************************************************
void Stage::AddNotify( Label Notify )
{
	m_pState->AddNotify( Notify );
}

//******************************************************************************
//	�e���b�v���I�����Ă��邩�ǂ���
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
//	�e���b�v�̎擾
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
//	���[���h�擾
//******************************************************************************
World* Stage::GetWorld( void )
{
	return m_pManager->GetWorld();
}

//------------------------------------------------------------------------------
//	�A�N�V�������쐬
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