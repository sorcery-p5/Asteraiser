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
//	���[���h�F�R���e�B�j���[�Ǘ�
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
	//	�G�t�F�N�g�̍X�V�ĊJ
	GetApp()->GetEffectMng()->SetUpdateEnable( true );

	SafeDelete( m_pMenu );
}

//******************************************************************************
//	������
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
//	�J��
//******************************************************************************
void WorldContinue::Open( void )
{
	_ASSERT( m_Credit > 0 );

	m_bActive = true;

	//	���j���[�J��
	SafePtr(m_pMenu)->OpenLayer( LAYER_NAME );

	//	�G�t�F�N�g�̍X�V��~
	GetApp()->GetEffectMng()->SetUpdateEnable( false );
}

//******************************************************************************
//	�����R���e�B�j���[
//******************************************************************************
void WorldContinue::AutoDecide( void )
{
	_ASSERT( m_Credit > 0 );

	m_bActive = true;

	//	�N���W�b�g���炷
	DecreaseZero( m_Credit, 1 );

	m_pWorld->OnContinue();

	//	�G�t�F�N�g�̍X�V��~
	GetApp()->GetEffectMng()->SetUpdateEnable( false );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void WorldContinue::Update( void )
{
	SafePtr(m_pMenu)->Update();

	if( m_bActive )
	{
		//	�e�L�X�g�ݒ�
		MenuLayer* pLayer = m_pMenu->GetCurLayer();
		if( pLayer && pLayer->GetName() == LAYER_NAME )	
		{
			pLayer->SetText( BONE_CREDIT, STRING_CREDIT, m_Credit );
		}

		//	����
		if( !m_pMenu || m_pMenu->IsEmpty() )
		{
			m_bActive = false;
			
			//	�G�t�F�N�g�̍X�V
			GetApp()->GetEffectMng()->SetUpdateEnable( true );
		}
	}
}

//******************************************************************************
//	�`��
//******************************************************************************
void WorldContinue::Draw( void )
{
	if( m_pMenu ) m_pMenu->Draw( m_pWorld->GetDrawOffset() );
}

//******************************************************************************
//	���C���[�̒ʒm
//******************************************************************************
void WorldContinue::OnLayerNotify( MenuLayer* pLayer, Label Name, float Param )
{
	//	�I��
	if( Name == NOTIFY_EXIT )
	{
		m_pWorld->Exit( World::EXITCODE_DEAD );
	}
}

//******************************************************************************
//	�I�u�W�F�N�g�̒ʒm
//******************************************************************************
void WorldContinue::OnObjectNotify( MenuObject* pObject, Label Name, float Param )
{
	//	�p��
	if( Name == NOTIFY_DECIDE )
	{
		//	�N���W�b�g���炷
		DecreaseZero( m_Credit, 1 );

		m_pWorld->OnContinue();
	}
	//	�I��
	else if( Name == NOTIFY_EXIT )
	{
		m_pWorld->Exit( World::EXITCODE_DEAD );
	}
}