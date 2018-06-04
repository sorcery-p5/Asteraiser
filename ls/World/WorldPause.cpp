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
//	���[���h�F�|�[�Y�Ǘ�
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
	//	�G�t�F�N�g�̍X�V�ĊJ
	GetApp()->GetEffectMng()->SetUpdateEnable( true );

	SafeDelete( m_pMenu );
}

//******************************************************************************
//	������
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
//	�X�V
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
			//	�|�[�Y�L�[�ŏI��
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
				//	�|�[�Y�L�[����
				if( Ctrl.IsInput( CONTROLLER_SYSTEM_PAUSE, INPUT_TRIG ) )
				{
					//	���v���C��
					if( m_pWorld->GetReplayManager()->IsReplayEnable() )
					{
						m_pWorld->OpenReplayMenu( true );
						return;
					}

					m_bPause = true;

					SafePtr(m_pMenu)->OpenLayer( LAYER_NAME );

					//	�G�t�F�N�g�̍X�V��~
					GetApp()->GetEffectMng()->SetUpdateEnable( false );
				}
			}
			else
			{
				m_bHide = Ctrl.IsInput( CONTROLLER_SYSTEM_CANCEL, INPUT_PRESS );

				if( !m_bHide )
				{
					//	����
					if( m_pMenu && m_pMenu->IsEmpty() )
					{
						m_bPause = false;
						
						//	�G�t�F�N�g�̍X�V�ĊJ
						GetApp()->GetEffectMng()->SetUpdateEnable( true );
					}
				}
			}
		}
	}
}

//******************************************************************************
//	�`��
//******************************************************************************
void WorldPause::Draw( void )
{
	if( !m_bHide )
	{
		if( m_pMenu ) m_pMenu->Draw( m_pWorld->GetDrawOffset() );
	}
}

//******************************************************************************
//	�ʒm
//******************************************************************************
void WorldPause::OnLayerNotify( MenuLayer* pLayer, Label Name, float Param )
{
	//	�I��
	if( Name == NOTIFY_EXIT )
	{
		m_pWorld->Exit( World::EXITCODE_EXIT );
	}
}

//******************************************************************************
//	�ʒm
//******************************************************************************
void WorldPause::OnObjectNotify( MenuObject* pObject, Label Name, float Param )
{

}