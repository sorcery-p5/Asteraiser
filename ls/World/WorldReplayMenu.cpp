#include "stdafx.h"
#include "WorldReplayMenu.h"

#include "App/App.h"
#include "Effect/EffectManager.h"

#include "World/World.h"
#include "Replay/ReplayManager.h"
#include "Controller/SystemController.h"

#include "Menu/Menu.h"
#include "Menu/MenuLayer.h"

namespace
{
	const Label		LAYER_MAIN		= "Main";
	const Label		LAYER_SKIP		= "Skip";

	const Label		BONE_CANCEL		= "Cancel";
	const Label		BONE_MULTI		= "Multi";

	const Label		NOTIFY_CANCEL	= "Cancel";
	const Label		NOTIFY_MANUAL	= "Manual";
	const Label		NOTIFY_EXIT		= "Exit";

	const int		FRAME_SKIP_MAX	= 256;
}


////////////////////////////////////////////////////////////////////////////////
//
//	���[���h�F���v���C���j���[�Ǘ�
//
////////////////////////////////////////////////////////////////////////////////
WorldReplayMenu::WorldReplayMenu( World* pWorld )
{
	_ASSERT( pWorld );

	m_pWorld		= pWorld;
	m_pMenu			= NULL;

	m_bActive		= false;
	m_FrameSkip		= 1;
}

WorldReplayMenu::~WorldReplayMenu()
{
	//	�G�t�F�N�g�̍X�V�ĊJ
	GetApp()->GetEffectMng()->SetUpdateEnable( true );

	SafeDelete( m_pMenu );

	GetGraphix()->SetFrameSkip( 0 );
}

//******************************************************************************
//	������
//******************************************************************************
void WorldReplayMenu::Init( const MenuData* pMenuData )
{
	if( pMenuData )
	{
		m_pMenu = new Menu( this );
		MEM_CHECK( m_pMenu );

		m_pMenu->Init( pMenuData, Matrix3::Trans( m_pWorld->GetFieldRect().Center() ) );
	}

	m_bActive		= false;
}

//******************************************************************************
//	�J��
//******************************************************************************
void WorldReplayMenu::Open( bool bCancelEnable )
{
#if DEMO_VER == DEMO_PLAY_VER
	//	�f�����[�h�E�����ɏI��
	m_pWorld->Exit( World::EXITCODE_DEAD );
	return;
#endif

	if( m_bActive ) return;

	m_bActive = true;
	m_bCancelEnable	= bCancelEnable;

	//	���j���[�J��
	if( m_pMenu )
	{
		m_pMenu->OpenLayer( LAYER_MAIN );

		//	�{�[���ݒ�
		MenuLayer* pLayer = m_pMenu->GetCurLayer();
		if( pLayer && pLayer->GetName() == LAYER_MAIN )	
		{
			pLayer->SetObjectEnable( BONE_CANCEL, m_bCancelEnable );
			pLayer->SetColor( BONE_CANCEL, m_bCancelEnable? Color::White() : Color::Gray(128) );

			if( !m_bCancelEnable )
			{
				pLayer->SelectObject( NOTIFY_EXIT );
			}
		}
	}

	//	�G�t�F�N�g�̍X�V��~
	GetApp()->GetEffectMng()->SetUpdateEnable( false );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void WorldReplayMenu::Update( void )
{
	SafePtr(m_pMenu)->Update();

	if( m_bActive )
	{
		//	����
		if( !m_pMenu || m_pMenu->IsEmpty() )
		{
			m_bActive = false;
			
			//	�G�t�F�N�g�̍X�V
			GetApp()->GetEffectMng()->SetUpdateEnable( true );
		}
	}

	//	�t���[���X�L�b�v
#if DEMO_VER != DEMO_PLAY_VER
	if( m_pWorld->GetReplayManager()->IsReplayEnable() && m_pWorld->IsReplayMode() )
	{
		SystemController Ctrl;

		if( Ctrl.IsInput( CONTROLLER_SYSTEM_CANCEL, INPUT_TRIG ) )
		{
			m_FrameSkip *= 2;
			if( m_FrameSkip > FRAME_SKIP_MAX ) m_FrameSkip = 1;

			if( m_pMenu )
			{
				m_pMenu->OpenLayer( LAYER_SKIP );

				MenuLayer* pLayer = m_pMenu->GetLayer( LAYER_SKIP );
				if( pLayer ) pLayer->SetText( BONE_MULTI, "x %d", m_FrameSkip );
			}
		}
	}
#endif

	//	�t���[���X�L�b�v
	if( m_pWorld->GetReplayManager()->IsReplayEnable() && !m_bActive && m_pWorld->IsReplayMode() )
	{
		GetGraphix()->SetFrameSkip( m_FrameSkip );
	}
	else
	{
		GetGraphix()->SetFrameSkip( 0 );
	}
}

//******************************************************************************
//	�`��
//******************************************************************************
void WorldReplayMenu::Draw( void )
{
	if( m_pMenu ) m_pMenu->Draw( m_pWorld->GetDrawOffset() );
}

//******************************************************************************
//	���C���[�̒ʒm
//******************************************************************************
void WorldReplayMenu::OnLayerNotify( MenuLayer* pLayer, Label Name, float Param )
{
	//	�L�����Z��
	if( Name == NOTIFY_CANCEL )
	{
		if( m_bCancelEnable )
		{
			m_pMenu->CloseCurLayer();
		}
	}

	//	�I��
	if( Name == NOTIFY_EXIT )
	{
		m_pWorld->Exit( World::EXITCODE_DEAD );
	}
}

//******************************************************************************
//	�I�u�W�F�N�g�̒ʒm
//******************************************************************************
void WorldReplayMenu::OnObjectNotify( MenuObject* pObject, Label Name, float Param )
{
	//	�L�����Z��
	if( Name == NOTIFY_CANCEL )
	{
		if( m_bCancelEnable )
		{
			m_pMenu->CloseCurLayer();
		}
	}
	//	�蓮����
	if( Name == NOTIFY_MANUAL )
	{
		m_pWorld->ChangeReplayManual();
		m_pMenu->CloseCurLayer();
	}
	//	�I��
	else if( Name == NOTIFY_EXIT )
	{
		m_pWorld->Exit( World::EXITCODE_DEAD );
	}
}