#include "stdafx.h"
#include "MenuLayer.h"
#include "Menu.h"
#include "MenuData.h"
#include "Object/MenuCursor.h"
#include "Object/MenuObject.h"
#include "Object/MenuSelector.h"
#include "Object/MenuSlider.h"
#include "Object/MenuList.h"
#include "Object/MenuScroll.h"

#include "App/App.h"
#include "Config/Config.h"
#include "Controller/SystemController.h"
#include "SpriteFrame/SpriteFrame.h"
#include "BGM/BGMManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	���j���[���C���[
//
////////////////////////////////////////////////////////////////////////////////
MenuLayer::MenuLayer( Menu* pParent ) : IndicateBase()
{
	_ASSERT( pParent );

	m_pParent			= pParent;
	m_pParam			= NULL;

	m_pSprite			= NULL;
	m_pCursor			= NULL;

	m_bDelete			= false;
	m_bVisible			= true;
	m_State				= STATE_OPEN;
	m_FrameCount		= 0;

	m_HelpTextLength	= 0;
}

MenuLayer::~MenuLayer()
{
	SafeDelete( m_pCursor );
	DeleteMap( m_mapObject );
	SafeDelete( m_pSprite );
}

//******************************************************************************
//	������
//******************************************************************************
void MenuLayer::Init( const MENU_LAYER* pParam )
{
	_ASSERT( pParam );
	m_pParam		= pParam;
	m_State			= STATE_OPEN;
	m_FrameCount	= 0;
	
	//	�X�v���C�g�t���[��������
	const SpriteFrameFile* pSpriteFile = m_pParent->GetData()->GetSpriteFrameFile( m_pParam->SpriteName );
	if( pSpriteFile )
	{
		m_pSprite = new SpriteFrame;
		MEM_CHECK( m_pSprite );
		m_pSprite->Init( pSpriteFile, Matrix3::Trans( m_pParam->Pos ) * m_pParent->GetMatrix() );

		//	�J���A�j��
		if( !m_pParam->OpenAnime.IsEmpty() )
		{
			m_pSprite->ChangeAnime( m_pParam->OpenAnime );
		}
			
		//	�I�u�W�F�N�g
		for( int i = 0; i < (int)m_pParam->vObject.size(); i++ )
		{
			MenuObject* pObject = _CreateObject( m_pParam->vObject[i] );

			m_mapObject.insert( MenuObjectMap::value_type( pObject->GetName(), pObject ) );
		}

		//	�J�[�\��
		if( !m_pParam->Cursor.BoneName.IsEmpty() )
		{
			m_pCursor = new MenuCursor( this );
			MEM_CHECK( m_pCursor );
			m_pCursor->Init( &m_pParam->Cursor, m_pParam->RootObject );
		}

		//	�w���v�e�L�X�g
		UpdateHelpText();
	}

	//	�\�����
	UpdateVisible();
	
	//	�������A�N�V����
	_ExecActionList( m_pParam->vInitAction );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void MenuLayer::Update( void )
{
	//	���ԃC�x���g
	_UpdateTimeEvent();

	switch( m_State )
	{
	case STATE_OPEN:
		{
			//	�A�j���̏I��
			if( m_pSprite->IsAnimeLast() || !m_pSprite->IsAnimePlaying() )
			{
				//	�ҋ@��Ԃ�
				if( !m_pParam->WaitAnime.IsEmpty() )
				{
					m_pSprite->ChangeAnime( m_pParam->WaitAnime );
				}

				m_State = STATE_WAIT;
			}
		}
		break;
	case STATE_WAIT:
		{
			//	�A�j���I���ŏI��
			if( m_pParam->bAnimeWaitEnd )
			{
				if( m_pSprite && m_pSprite->IsAnimePlaying() && m_pSprite->IsAnimeLast() )
				{
					Close();
				}
			}
		}
		break;
	case STATE_CLOSE:
		{
			//	�A�j���̏I��
			if( m_pSprite->IsAnimeLast() || !m_pSprite->IsAnimePlaying() )
			{
				Delete();
			}
		}
		break;
	};

	//	�X�v���C�g
	if( m_pSprite ) m_pSprite->Update( NULL );

	//	�J�[�\��
	if( m_pCursor ) m_pCursor->Update();
		
	//	�I�u�W�F�N�g
	for( MenuObjectMap::iterator it = m_mapObject.begin(); it != m_mapObject.end(); ++it )
	{
		(it->second)->Update();
	}

	//	�w���v�e�L�X�g
	_UpdateHelpTextLength();

	//	�C���W�P�[�g
	IndicateBase::Update();

	//	�J�E���g�A�b�v
	m_FrameCount++;
}

//******************************************************************************
//	���݃��C���[�̍X�V
//******************************************************************************
void MenuLayer::UpdateCurrent( void )
{
	SystemController Ctrl;

	if( m_State == STATE_OPEN )
	{
		//	�{�^���ɂ��J���A�j���X�L�b�v
		if( m_pParam->bSkipOpen &&
			Ctrl.IsAnyInput( INPUT_TRIG ) )
		{
			//	�ҋ@��Ԃ�
			if( !m_pParam->WaitAnime.IsEmpty() )
			{
				m_pSprite->ChangeAnime( m_pParam->WaitAnime );
			}

			m_State = STATE_WAIT;
		}
	}
	else if( m_State == STATE_WAIT )
	{
		//	�J�[�\��
		if( m_pCursor ) m_pCursor->UpdateCurrent();

		//	�I�𒆃I�u�W�F�N�g�̑���
		MenuObject* pSelectObj = GetSelectObject();
		if( pSelectObj )
		{
			pSelectObj->UpdateSelect();
		}

		//	�L�[�C�x���g
		for( MENU_KEY_EVENT_VEC::const_iterator it = m_pParam->vKeyEvent.begin(); it != m_pParam->vKeyEvent.end(); ++it )
		{
			bool bInput	= it->Type == MENU_KEY_EVENT_ANY ?
							Ctrl.IsAnyInput( INPUT_TRIG ) :
							Ctrl.IsInput( CONTROLLER_SYSTEM_KEY(it->Type), INPUT_TRIG );

			if( bInput )
			{
				_ExecActionList( it->vAction );
			}
		}
	}
}

//******************************************************************************
//	�`��
//******************************************************************************
void MenuLayer::Draw( const Matrix3& OffsetMat )
{
	if( m_pSprite ) m_pSprite->Draw( OffsetMat );
	
	//	���
	IndicateBase::Draw( OffsetMat );

	//	�I�u�W�F�N�g
	for( MenuObjectMap::iterator it = m_mapObject.begin(); it != m_mapObject.end(); ++it )
	{
		(it->second)->Draw( OffsetMat );
	}
}

//******************************************************************************
//	����
//******************************************************************************
void MenuLayer::Close( void )
{
	if( m_State != STATE_CLOSE )
	{
		if( m_pSprite )
		{
			//	����A�j��
			if( !m_pParam->CloseAnime.IsEmpty() )
			{
				m_pSprite->ChangeAnime( m_pParam->CloseAnime, m_pParam->CloseIntp );
			}
			else
			{
				Delete();
			}
		}

		//	�I���A�N�V����
		_ExecActionList( m_pParam->vCloseAction );

		m_State = STATE_CLOSE;
	}
}

//******************************************************************************
//	���݃��C���[�Ɉړ�
//******************************************************************************
void MenuLayer::OnMoveCurrent( void )
{
	//	�F�����ɖ߂�
	if( m_pSprite ) m_pSprite->SetColor( Color::White() );
}

//******************************************************************************
//	�w�ʃ��C���[�Ɉړ�
//******************************************************************************
void MenuLayer::OnMoveBack( void )
{
	//	�F���o�b�N�J���[��
	if( m_pSprite ) m_pSprite->SetColor( m_pParam->BackColor );
}

//******************************************************************************
//	���݃��C���[���ǂ���
//******************************************************************************
bool MenuLayer::IsCurrent( void ) const
{
	return m_pParent->GetCurLayer() == this;
}

//******************************************************************************
//	�\���L���̍X�V����
//******************************************************************************
void MenuLayer::UpdateVisible( void )
{
	if( m_pSprite ) m_pSprite->SetVisible( m_pParent->IsVisible() && m_bVisible );
}

//******************************************************************************
//	�I�u�W�F�N�g�̎擾
//******************************************************************************
MenuObject* MenuLayer::GetObject( Label Name ) const
{
	MenuObjectMap::const_iterator it = m_mapObject.find( Name );
	if( it != m_mapObject.end() )
	{
		return (it->second);
	}
	return NULL;
}
	
//******************************************************************************
//	�I���I�u�W�F�N�g�̎擾
//******************************************************************************
MenuObject* MenuLayer::GetSelectObject( void ) const
{
	if( m_pCursor )
	{
		return GetObject( m_pCursor->GetSelectObject() );
	}
	return NULL;
}

//******************************************************************************
//	�I���I�u�W�F�N�g�̐ݒ�
//******************************************************************************
void MenuLayer::SelectObject( Label Name )
{
	if( m_pCursor )
	{
		m_pCursor->SelectObject( Name, false );
	}
}

//******************************************************************************
//	�I�u�W�F�N�g�̑I��Ώۂ�ݒ�
//******************************************************************************
void MenuLayer::SetObjectSelect( Label Name, Label Value )
{
	MenuObject* pObject = GetObject( Name );
	if( pObject ) pObject->SetSelectName( Value );
}

//******************************************************************************
//	�I�u�W�F�N�g�̒l��ݒ�
//******************************************************************************
void MenuLayer::SetObjectValue( Label Name, float Value )
{
	MenuObject* pObject = GetObject( Name );
	if( pObject ) pObject->SetValue( Value );
}

//******************************************************************************
//	�I�u�W�F�N�g�̒l���擾
//******************************************************************************
float MenuLayer::GetObjectValue( Label Name ) const
{
	MenuObject* pObject = GetObject( Name );
	if( pObject ) return pObject->GetValue();

	return 0.0f;
}

//******************************************************************************
//	�I�u�W�F�N�g�̗L��/������ݒ�
//******************************************************************************
void MenuLayer::SetObjectEnable( Label Name, bool bEnable )
{
	MenuObject* pObject = GetObject( Name );
	if( pObject ) pObject->SetEnable( bEnable );
}

//******************************************************************************
//	�w���v�e�L�X�g�̍X�V
//******************************************************************************
void MenuLayer::UpdateHelpText( void )
{
	if( m_pParam->HelpBoneName.IsEmpty() ) return;

	String256 Str;

	MenuObject* pObject = GetSelectObject();
	if( !m_ReplaceHelpText.empty() )
	{
		Str = m_ReplaceHelpText.c_str();
	}
	else if( pObject )
	{
		Str = pObject->GetHelpText();
	}
	else
	{
		Str = m_pParam->HelpText.c_str();
	}

	//	�L�[��������̒u��
	GetConfig()->ReplaceKeyName( Str );

	//	�o�^
	m_HelpText = Str.c_str();
	m_HelpTextLength = 0;
}

//******************************************************************************
//	�w���v�e�L�X�g�̍����ւ�
//******************************************************************************
void MenuLayer::ReplaceHelpText( pstr pText )
{
	if( m_pParam->HelpBoneName.IsEmpty() ) return;

	m_ReplaceHelpText = pText;

	//	�X�V
	UpdateHelpText();
}

//------------------------------------------------------------------------------
//	�s�[�X�̎擾
//------------------------------------------------------------------------------
const Piece* MenuLayer::_GetPiece( Label Name ) const
{
	return m_pParent->GetData()->GetPiece( Name );
}

//------------------------------------------------------------------------------
//	�e�N�X�`���̎擾
//------------------------------------------------------------------------------
const Texture* MenuLayer::_GetTex( Label Name ) const
{
	return m_pParent->GetData()->GetTex( Name );
}

//------------------------------------------------------------------------------
//	�C���W�P�[�g�E�}�[�J�[�p�����[�^�̎擾
//------------------------------------------------------------------------------
const INDICATE_MARKER* MenuLayer::_GetMarker( Label BoneName ) const
{
	std::map< Label, INDICATE_MARKER >::const_iterator it = m_pParam->mapMarker.find( BoneName );
	if( it != m_pParam->mapMarker.end() )
	{
		return &(it->second);
	}
	return NULL;
}

//------------------------------------------------------------------------------
//	�C���W�P�[�g�E�}�[�J�[�p�����[�^�̎擾
//------------------------------------------------------------------------------
const INDICATE_SPIN* MenuLayer::_GetSpin( Label BoneName ) const
{
	std::map< Label, INDICATE_SPIN >::const_iterator it = m_pParam->mapSpin.find( BoneName );
	if( it != m_pParam->mapSpin.end() )
	{
		return &(it->second);
	}
	return NULL;
}

//------------------------------------------------------------------------------
//	�C���W�P�[�g�E���O���t�p�����[�^�̎擾
//------------------------------------------------------------------------------
const INDICATE_LINE_GRAPH* MenuLayer::_GetLineGraph( Label BoneName ) const
{
	std::map< Label, INDICATE_LINE_GRAPH >::const_iterator it = m_pParam->mapLineGraph.find( BoneName );
	if( it != m_pParam->mapLineGraph.end() )
	{
		return &(it->second);
	}
	return NULL;
}


//------------------------------------------------------------------------------
//	���ԃC�x���g�X�V
//------------------------------------------------------------------------------
void MenuLayer::_UpdateTimeEvent( void )
{
	for( int i = 0; i < (int)m_pParam->vTimeEvent.size(); i++ )
	{
		const MENU_TIME_EVENT& Event = m_pParam->vTimeEvent[i];

		if( Event.Frame == m_FrameCount )
		{
			_ExecAction( Event.Action );
		}
	}
}

//------------------------------------------------------------------------------
//	�A�N�V�������X�g���s
//------------------------------------------------------------------------------
void MenuLayer::_ExecActionList( const MENU_ACTION_VEC& vAction )
{
	for( int i = 0; i < (int)vAction.size(); i++ )
	{
		_ExecAction( vAction[i] );
	}
}

//------------------------------------------------------------------------------
//	�A�N�V�������s
//------------------------------------------------------------------------------
void MenuLayer::_ExecAction( const MENU_ACTION& Action )
{
	switch( Action.Type )
	{
	case MENU_ACTION_OPEN:		m_pParent->OpenLayer( Action.Name );									break;
	case MENU_ACTION_CLOSE:		Action.Name.IsEmpty()? Close() : m_pParent->CloseLayer( Action.Name );	break;

	case MENU_ACTION_ANIME:		m_pSprite->ChangeAnime( Action.Name, Action.Param );					break;
	case MENU_ACTION_NOTIFY:	m_pParent->OnNotify( this, Action.Name, Action.Param );					break;
	case MENU_ACTION_SELECT:	SelectObject( Action.Name );											break;

	case MENU_ACTION_EFFECT:	m_pParent->CreateEffect( Action.Name, m_pSprite->GetBaseMatrix() );		break;
	case MENU_ACTION_SOUND:		m_pParent->PlaySound( Action.Name, Action.Param );						break;

	case MENU_ACTION_BGM_PLAY:	GetApp()->GetBGMMng()->Play( Action.Text.c_str(), Action.Param );		break;
	case MENU_ACTION_BGM_STOP:	GetApp()->GetBGMMng()->Stop();											break;
	case MENU_ACTION_BGM_FADE:	GetApp()->GetBGMMng()->Fade( Action.Param );							break;
	}
}

//------------------------------------------------------------------------------
//	�I�u�W�F�N�g�̐���
//------------------------------------------------------------------------------
MenuObject* MenuLayer::_CreateObject( const MENU_OBJECT& Param )
{
	MenuObject* pObject = NULL;

	switch( Param.Type )
	{
	case MENU_OBJECT_SELECTOR:	pObject	= new MenuSelector( this );	break;
	case MENU_OBJECT_SLIDER:	pObject	= new MenuSlider( this );	break;
	case MENU_OBJECT_LIST:		pObject	= new MenuList( this );		break;
	case MENU_OBJECT_SCROLL:	pObject	= new MenuScroll( this );	break;
	default:					pObject = new MenuObject( this );	break;
	}
	
	MEM_CHECK( pObject );

	pObject->Init( &Param );
	return pObject;
}

//------------------------------------------------------------------------------
//	�w���v�e�L�X�g�̕\���������X�V
//------------------------------------------------------------------------------
void MenuLayer::_UpdateHelpTextLength( void )
{
	if( m_pParam->HelpBoneName.IsEmpty() ) return;

	if( m_HelpTextLength < (int)m_HelpText.length() )
	{
		char Ch = m_HelpText[m_HelpTextLength];

		if( IsEmQuad( Ch ) )
		{
			m_HelpTextLength += 2;
		}
		else
		{
			m_HelpTextLength += 1;
		}

		String256 Str = m_HelpText.c_str();

		if( m_HelpTextLength < (int)m_HelpText.length() )
		{
			Str.c_str()[m_HelpTextLength] = '\0';
		}

		m_pSprite->SetBoneText( m_pParam->HelpBoneName, Str.c_str() );
		m_pSprite->SetBoneColor( m_pParam->HelpBoneName,
							Color::Gray(255, ((float)m_HelpTextLength / (float)m_HelpText.length()) * 255 ) );
	}
}