#include "stdafx.h"
#include "MenuCursor.h"
#include "MenuObject.h"
#include "../Menu.h"
#include "../MenuLayer.h"
#include "../MenuData.h"

#include "Controller/SystemController.h"
#include "SpriteFrame/SpriteFrame.h"


////////////////////////////////////////////////////////////////////////////////
//
//	���j���[�J�[�\��
//
////////////////////////////////////////////////////////////////////////////////
MenuCursor::MenuCursor( MenuLayer* pParent )
{
	_ASSERT( pParent );

	m_pParent	= pParent;
	m_pParam	= NULL;
}
MenuCursor::~MenuCursor()
{
}

//******************************************************************************
//	������
//******************************************************************************
void MenuCursor::Init( const MENU_CURSOR* pParam, Label Root )
{
	_ASSERT( pParam && m_pParent->GetSprite() );
	_ASSERT( m_pParent->GetSprite()->FindBone(pParam->BoneName) >= 0 );

	m_pParam		= pParam;
	m_SelectObject	= Root;

	m_Pos			= _GetSelectPos();

	//	�ʒm
	m_pParent->GetParent()->OnSelectObject( m_pParent->GetObject( m_SelectObject ), NULL );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void MenuCursor::Update( void )
{
	//	�ʒu�̕��
	m_Pos += (_GetSelectPos() - m_Pos) * m_pParam->MoveSpeed;
	m_pParent->GetSprite()->SetBonePos( m_pParam->BoneName, m_Pos );

	bool bVisible = true;
	MenuObject* pObject = m_pParent->GetSelectObject();
	if( pObject && !pObject->IsCursorVisible() ) bVisible = false;

	//	��\���̐؂�ւ�
	m_pParent->GetSprite()->SetBoneVisible( m_pParam->BoneName, m_pParent->IsCurrent() && bVisible );
}

//******************************************************************************
//	���݃��C���[�̍X�V
//******************************************************************************
void MenuCursor::UpdateCurrent( void )
{
	SystemController Ctrl;

	MenuObject* pObject = m_pParent->GetSelectObject();

	if( pObject )
	{
		//	�J�[�\���̈ړ�
		for( int i = 0; i < MENU_DIR_NUM; i++ )
		{
			if( !pObject->IsMoveEnable( MENU_DIR(i) ) ) continue;

			Label DirName = pObject->GetDirObjectName( MENU_DIR(i) );
			if( DirName.IsEmpty() ) continue;
					
			MenuObject* pDirObject = m_pParent->GetObject( DirName );
			while( pDirObject && !pDirObject->IsEnable() )
			{
				DirName = pDirObject->GetDirObjectName( MENU_DIR(i) );
				if( DirName.IsEmpty() ) break;
				
				pDirObject = m_pParent->GetObject( DirName );
			}
			_ASSERT( pDirObject );

			if( Ctrl.IsDirInput( CONTROLLER_INPUT( i + CONTROLLER_DIR_BEGIN ), INPUT_REPEAT ) )
			{
				SelectObject( DirName, true );
				pDirObject->OnChangeSelect( MENU_DIR(i) );

				break;
			}
		}
	}
}

//******************************************************************************
//	�I�u�W�F�N�g��I������
//******************************************************************************
void MenuCursor::SelectObject( Label Name, bool bSound )
{
	MenuObject* pObject = m_pParent->GetObject( Name );
	if( !pObject ) return;

	if( Name != m_SelectObject )
	{	
		MenuObject* pPrevObject = m_pParent->GetObject( m_SelectObject );

		m_SelectObject = Name;

		//	�T�E���h�Đ�
		bool bVisible = m_pParent->GetSprite()->GetBoneColor( m_pParam->BoneName ).a > 0 &&
						m_pParent->GetSprite()->GetBoneColor( Name ).a > 0;
		if( bSound && bVisible )
		{
			m_pParent->GetParent()->PlaySound( m_pParam->MoveSound, m_pParam->MoveSoundVol );
		}

		//	�w���v�e�L�X�g�X�V
		m_pParent->UpdateHelpText();

		//	�ʒm
		m_pParent->GetParent()->OnSelectObject( pObject, pPrevObject );
	}
}

//------------------------------------------------------------------------------
//	�I���ʒu���擾
//------------------------------------------------------------------------------
Point MenuCursor::_GetSelectPos( void ) const
{
	SpriteFrame* pSprite = m_pParent->GetSprite();
	_ASSERT( pSprite );

	return pSprite->GetBoneLocalMatrix( m_SelectObject )->GetPos();
}
