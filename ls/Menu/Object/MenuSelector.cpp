#include "stdafx.h"
#include "MenuSelector.h"
#include "../Menu.h"
#include "../MenuLayer.h"
#include "../MenuData.h"

#include "Controller/SystemController.h"
#include "SpriteFrame/SpriteFrame.h"


////////////////////////////////////////////////////////////////////////////////
//
//	���j���[�Z���N�^�[
//
////////////////////////////////////////////////////////////////////////////////
MenuSelector::MenuSelector( MenuLayer* pParent ) : MenuObject( pParent )
{
	m_pSelectorParam	= NULL;
	m_SelectIndex		= 0;
}

MenuSelector::~MenuSelector()
{

}

//******************************************************************************
//	������
//******************************************************************************
void MenuSelector::OnInit( void )
{
	m_pSelectorParam = &_GetParam()->Selector;
	_ASSERT( m_pSelectorParam );
	_ASSERT( !m_pSelectorParam->vSelect.empty() );

	m_SelectIndex = 0;

	m_CursorPos = _GetSelectPos();
}

//******************************************************************************
//	�X�V
//******************************************************************************
void MenuSelector::OnUpdate( void )
{
	//	�J�[�\���ʒu�̈ړ�
	m_CursorPos += (_GetSelectPos() - m_CursorPos) * m_pSelectorParam->MoveSpeed;
	_GetSprite()->SetBonePos( m_pSelectorParam->CursorBone, m_CursorPos );

	//	�J�[�\���̔�\��
	_GetSprite()->SetBoneVisible( m_pSelectorParam->CursorBone, GetParent()->IsCurrent() );
}

//******************************************************************************
//	�I�𒆂̍X�V
//******************************************************************************
void MenuSelector::OnUpdateSelect( void )
{
	SystemController Ctrl;

	int Dir = 0;
	if( m_pSelectorParam->SelectDir == MENU_SELECT_H )
	{
		//	������
		if( Ctrl.IsDirInput( CONTROLLER_L, INPUT_REPEAT ) )			Dir = -1;
		else if( Ctrl.IsDirInput( CONTROLLER_R, INPUT_REPEAT ) )	Dir =  1;
	}
	else if( m_pSelectorParam->SelectDir == MENU_SELECT_V )
	{
		//	�c����
		if( Ctrl.IsDirInput( CONTROLLER_D, INPUT_REPEAT ) )			Dir = -1;
		else if( Ctrl.IsDirInput( CONTROLLER_U, INPUT_REPEAT ) )	Dir =  1;
	}

	//	�ړ�
	if( Dir != 0 )
	{
		m_SelectIndex = Loop( m_SelectIndex + Dir, 0, (int)m_pSelectorParam->vSelect.size() );

		//	�ʒm
		GetMenu()->OnChangeValue( this );

		//	�T�E���h�Đ�
		GetMenu()->PlaySound( m_pSelectorParam->MoveSound, m_pSelectorParam->MoveSoundVol );
	}
}

//******************************************************************************
//	���݂̒l���擾
//******************************************************************************
float MenuSelector::GetValue( void ) const
{
	_ASSERT( m_SelectIndex < (int)m_pSelectorParam->vSelect.size() );
	
	return m_pSelectorParam->vSelect[m_SelectIndex].Value;
}

//******************************************************************************
//	�l�̐ݒ�
//******************************************************************************
void MenuSelector::SetValue( float Value )
{
	for( int i = 0; i < (int)m_pSelectorParam->vSelect.size(); i++ )
	{
		if( m_pSelectorParam->vSelect[i].Value == Value )
		{
			_ASSERT( _GetSprite()->FindBone( m_pSelectorParam->vSelect[i].Name ) >= 0 );

			m_SelectIndex = i;
			
			//	�J�[�\���ʒu���ړ�
			m_CursorPos = _GetSelectPos();
			_GetSprite()->SetBonePos( m_pSelectorParam->CursorBone, m_CursorPos );
			break;
		}
	}
}

//******************************************************************************
//	�I���I�u�W�F�N�g�̎擾
//******************************************************************************
Label MenuSelector::GetSelectName( void ) const
{
	_ASSERT( m_SelectIndex < (int)m_pSelectorParam->vSelect.size() );
	
	return m_pSelectorParam->vSelect[m_SelectIndex].Name;
}

//******************************************************************************
//	�I���I�u�W�F�N�g�̐ݒ�
//******************************************************************************
void MenuSelector::SetSelectName( Label Name )
{
	for( int i = 0; i < (int)m_pSelectorParam->vSelect.size(); i++ )
	{
		if( m_pSelectorParam->vSelect[i].Name == Name )
		{
			_ASSERT( _GetSprite()->FindBone( m_pSelectorParam->vSelect[i].Name ) >= 0 );

			m_SelectIndex = i;
			
			//	�J�[�\���ʒu���ړ�
			m_CursorPos = _GetSelectPos();
			_GetSprite()->SetBonePos( m_pSelectorParam->CursorBone, m_CursorPos );
			break;
		}
	}
}

//------------------------------------------------------------------------------
//	�I���ʒu���擾
//------------------------------------------------------------------------------
Point MenuSelector::_GetSelectPos( void ) const
{
	_ASSERT( m_SelectIndex < (int)m_pSelectorParam->vSelect.size() );

	return _GetSprite()->GetBoneLocalMatrix( m_pSelectorParam->vSelect[m_SelectIndex].Name )->GetPos();
}
