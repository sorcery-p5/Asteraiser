#include "stdafx.h"
#include "Menu.h"

#include "MenuData.h"
#include "MenuLayer.h"
#include "MenuListener.h"

#include "App/App.h"

#include "SpriteFrame/SpriteFrame.h"
#include "Effect/EffectManager.h"
#include "Controller/SystemController.h"
#include "BGM/BGMManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	���j���[
//
////////////////////////////////////////////////////////////////////////////////
Menu::Menu( MenuListener* pListener )
{
	m_pData			= NULL;
	m_pListener		= pListener;
	m_bVisible		= true;
	m_FrameCount	= 0;
}

Menu::~Menu()
{
	//	�I���A�N�V����
	_ExecActionList( m_pData->GetParam().vCloseAction );

	DeleteVec( m_vLayer );
	DeleteVec( m_vInactiveLayer );
}

//******************************************************************************
//	������
//******************************************************************************
void Menu::Init( const MenuData* pData, Matrix3 Mat )
{
	_ASSERT( pData );

	m_pData			= pData;
	m_Matrix		= Mat;
	m_FrameCount	= 0;

	//	���[�g���C���[�o�^
	if( !m_pData->GetParam().RootLayer.IsEmpty() )
	{
		_PushLayer( m_pData->GetParam().RootLayer );
	}

	//	�������A�N�V����
	_ExecActionList( m_pData->GetParam().vInitAction );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void Menu::Update( void )
{
	SystemController Ctrl;

	//	���ԃC�x���g
	_UpdateTimeEvent();

	//	�V���C���[�̒ǉ�
	if( !m_NextLayer.IsEmpty() )
	{
		_PushLayer( m_NextLayer );
		m_NextLayer.Clear();
	}

	//	�񓮍샌�C���[�X�V
	for( MenuLayerVec::iterator it = m_vInactiveLayer.begin(); it != m_vInactiveLayer.end(); )
	{
		(*it)->Update();

		if( (*it)->IsDelete() )
		{
			//	���X�i�[�ɒʒm����
			if( m_pListener ) m_pListener->OnCloseLayer( *it );

			//	�폜
			SafeDelete( *it );
			it = m_vInactiveLayer.erase( it );
		}
		else
		{
			++it;
		}
	}

	//	���C���[�X�V
	for( MenuLayerVec::iterator it = m_vLayer.begin(); it != m_vLayer.end(); ++it )
	{
		(*it)->Update();
	}

	//	���݃��C���[�̍X�V
	MenuLayer* pLayer = GetCurLayer();
	if( pLayer )
	{
		pLayer->UpdateCurrent();

		//	�폜
		if( pLayer->IsDelete() )
		{
			//	���X�i�[�ɒʒm����
			if( m_pListener ) m_pListener->OnCloseLayer( pLayer );

			SafeDelete( pLayer );
			m_vLayer.pop_back();

			//	�V�������݃��C���[�Ɉڍs
			MenuLayer* pNewLayer = GetCurLayer();
			if( pNewLayer )
			{
				pNewLayer->OnMoveCurrent();
			}
		}
	}

	//	�L�[�C�x���g
	for( MENU_KEY_EVENT_VEC::const_iterator it = m_pData->GetParam().vKeyEvent.begin(); it != m_pData->GetParam().vKeyEvent.end(); ++it )
	{
		bool bInput	= it->Type == MENU_KEY_EVENT_ANY ?
						Ctrl.IsAnyInput( INPUT_TRIG ) :
						Ctrl.IsInput( CONTROLLER_SYSTEM_KEY(it->Type), INPUT_TRIG );

		if( bInput )
		{
			_ExecActionList( it->vAction );
		}
	}

	//	�J�E���g�A�b�v
	m_FrameCount++;
}

//******************************************************************************
//	�`��
//******************************************************************************
void Menu::Draw( const Matrix3& OffsetMat )
{
	for( MenuLayerVec::iterator it = m_vInactiveLayer.begin(); it != m_vInactiveLayer.end(); ++it )
	{
		(*it)->Draw( OffsetMat );
	}
	for( MenuLayerVec::iterator it = m_vLayer.begin(); it != m_vLayer.end(); ++it )
	{
		(*it)->Draw( OffsetMat );
	}
}

//******************************************************************************
//	���C���[�擾
//******************************************************************************
MenuLayer* Menu::GetLayer( Label Name ) const
{
	for( int i = 0; i < (int)m_vLayer.size(); i++ )
	{
		MenuLayer* pLayer = m_vLayer[i];
		if( pLayer->GetName() == Name )
		{
			return pLayer;
		}
	}

	for( int i = 0; i < (int)m_vInactiveLayer.size(); i++ )
	{
		MenuLayer* pLayer = m_vInactiveLayer[i];
		if( pLayer->GetName() == Name )
		{
			return pLayer;
		}
	}

	return NULL;
}

//******************************************************************************
//	���݃��C���[�擾
//******************************************************************************
MenuLayer* Menu::GetCurLayer( void ) const
{
	if( m_vLayer.empty() ) return NULL;

	return m_vLayer.back();
}

//******************************************************************************
//	���C���[���J��
//******************************************************************************
void Menu::OpenLayer( Label Name )
{
	const MENU_LAYER* pParam = m_pData->GetLayer( Name );
	if( !pParam ) return;
	
	//	�񓮍�
	if( pParam->bInactive )
	{
		_OpenInactiveLayer( Name );
	}
	//	����
	else
	{
		//	���ڒǉ�
		if( IsEmpty() )
		{
			_PushLayer( Name );
		}
		//	�\��
		else
		{
			_ASSERT( m_NextLayer.IsEmpty() );

			m_NextLayer = Name;
		}
	}
}

//******************************************************************************
//	�w�背�C���[�����
//******************************************************************************
void Menu::CloseLayer( Label Name )
{
	MenuLayer* pLayer = GetLayer( Name );
	if( pLayer ) pLayer->Close();
}

//******************************************************************************
//	���݃��C���[�����
//******************************************************************************
void Menu::CloseCurLayer( void )
{
	MenuLayer* pLayer = GetCurLayer();
	if( pLayer ) pLayer->Close();
}

//******************************************************************************
//	�\���̗L����ݒ�
//******************************************************************************
void Menu::SetVisible( bool bVisible )
{
	m_bVisible = bVisible;

	for( int i = 0; i < (int)m_vInactiveLayer.size(); i++ )
	{
		m_vInactiveLayer[i]->UpdateVisible();
	}

	for( int i = 0; i < (int)m_vLayer.size(); i++ )
	{
		m_vLayer[i]->UpdateVisible();
	}
}

//******************************************************************************
//	���X�i�[�֒ʒm�i���j���[�j
//******************************************************************************
void Menu::OnNotify( Label Name, float Param )
{
	if( m_pListener ) m_pListener->OnMenuNotify( this, Name, Param );
}

//******************************************************************************
//	���X�i�[�֒ʒm�i���C���[�j
//******************************************************************************
void Menu::OnNotify( MenuLayer* pLayer, Label Name, float Param )
{
	if( m_pListener ) m_pListener->OnLayerNotify( pLayer, Name, Param );
}

//******************************************************************************
//	���X�i�[�֒ʒm�i�I�u�W�F�N�g�j
//******************************************************************************
void Menu::OnNotify( MenuObject* pObject, Label Name, float Param )
{
	if( m_pListener ) m_pListener->OnObjectNotify( pObject, Name, Param );
}

//******************************************************************************
//	���X�i�[�֒ʒm�i�I�u�W�F�N�g�l�ύX�j
//******************************************************************************
void Menu::OnChangeValue( MenuObject* pObject )
{
	if( m_pListener ) m_pListener->OnObjectChangeValue( pObject );
}

//******************************************************************************
//	���X�i�[�֒ʒm�i�I�u�W�F�N�g�I���j
//******************************************************************************
void Menu::OnSelectObject( MenuObject* pObject, MenuObject* pPrevObject )
{
	if( m_pListener ) m_pListener->OnObjectSelect( pObject, pPrevObject );
}

//******************************************************************************
//	�T�E���h�̍Đ�
//******************************************************************************
Sound* Menu::PlaySound( Label Name, float Vol )
{
	const SoundFile* pFile = m_pData->GetSoundFile( Name );
	if( !pFile ) return NULL;

	Sound* pSound = GetTone()->PlaySound( pFile, SOUND_CHANNEL_SE, false );
	if( !pSound ) return NULL;

	pSound->SetVolume( Vol );

	return pSound;
}

//******************************************************************************
//	�G�t�F�N�g�̐���
//******************************************************************************
Effect* Menu::CreateEffect( Label Name, const Matrix3& Mat )
{
	const EffectFile* pFile = m_pData->GetEffectFile( Name );
	if( !pFile ) return NULL;

	return GetApp()->GetEffectMng()->CreateEffect( pFile, Mat );
}

//------------------------------------------------------------------------------
//	���C���[�ǉ�
//------------------------------------------------------------------------------
void Menu::_PushLayer( Label Name )
{
	if( Name.IsEmpty() ) return;

	//	���łɊJ���Ă���
	{
		MenuLayer* pLayer = GetLayer( Name );
		if( pLayer ) return;
	}

	const MENU_LAYER* pParam = m_pData->GetLayer( Name );
	if( !pParam ) return;
	
	//	�ȑO�̌��݃��C���[��w�ʂ�
	MenuLayer* pOldLayer = GetCurLayer();
	if( pOldLayer )
	{
		pOldLayer->OnMoveBack();
	}

	//	�V�K�쐬
	MenuLayer* pLayer = new MenuLayer( this );
	MEM_CHECK( pLayer );

	m_vLayer.push_back( pLayer );
	pLayer->Init( pParam );

	//	���X�i�[�ɒʒm����
	if( m_pListener ) m_pListener->OnOpenLayer( pLayer );
}

//------------------------------------------------------------------------------
//	�񓮍샌�C���[���J��
//------------------------------------------------------------------------------
void Menu::_OpenInactiveLayer( Label Name )
{
	if( Name.IsEmpty() ) return;

	//	���łɊJ���Ă���
	{
		MenuLayer* pLayer = GetLayer( Name );
		if( pLayer ) return;
	}

	const MENU_LAYER* pParam = m_pData->GetLayer( Name );
	if( !pParam ) return;
	
	//	�V�K�쐬
	MenuLayer* pLayer = new MenuLayer( this );
	MEM_CHECK( pLayer );

	pLayer->Init( pParam );
	m_vInactiveLayer.push_back( pLayer );

	//	���X�i�[�ɒʒm����
	if( m_pListener ) m_pListener->OnOpenLayer( pLayer );
}

//------------------------------------------------------------------------------
//	���ԃC�x���g�X�V
//------------------------------------------------------------------------------
void Menu::_UpdateTimeEvent( void )
{
	for( int i = 0; i < (int)m_pData->GetParam().vTimeEvent.size(); i++ )
	{
		const MENU_TIME_EVENT& Event = m_pData->GetParam().vTimeEvent[i];

		if( Event.Frame == m_FrameCount )
		{
			_ExecAction( Event.Action );
		}
	}
}

//------------------------------------------------------------------------------
//	�A�N�V�������X�g���s
//------------------------------------------------------------------------------
void Menu::_ExecActionList( const MENU_ACTION_VEC& vAction )
{
	for( int i = 0; i < (int)vAction.size(); i++ )
	{
		_ExecAction( vAction[i] );
	}
}

//------------------------------------------------------------------------------
//	�A�N�V�������s
//------------------------------------------------------------------------------
void Menu::_ExecAction( const MENU_ACTION& Action )
{
	MenuLayer* pLayer = GetCurLayer();

	switch( Action.Type )
	{
	case MENU_ACTION_OPEN:		OpenLayer( Action.Name );														break;
	case MENU_ACTION_CLOSE:		Action.Name.IsEmpty()? CloseCurLayer() : CloseLayer( Action.Name );				break;

	case MENU_ACTION_ANIME:		if( pLayer ) pLayer->GetSprite()->ChangeAnime( Action.Name, Action.Param );		break;
	case MENU_ACTION_NOTIFY:	OnNotify( Action.Name, Action.Param );											break;
	case MENU_ACTION_SELECT:	if( pLayer ) pLayer->SelectObject( Action.Name );								break;

	case MENU_ACTION_EFFECT:	CreateEffect( Action.Name, m_Matrix );											break;
	case MENU_ACTION_SOUND:		PlaySound( Action.Name, Action.Param );											break;
	case MENU_ACTION_BGM_PLAY:	GetApp()->GetBGMMng()->Play( Action.Text.c_str(), Action.Param );				break;
	case MENU_ACTION_BGM_STOP:	GetApp()->GetBGMMng()->Stop();													break;
	case MENU_ACTION_BGM_FADE:	GetApp()->GetBGMMng()->Fade( Action.Param );									break;
	}
}