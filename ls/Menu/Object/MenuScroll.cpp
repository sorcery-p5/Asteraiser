#include "stdafx.h"
#include "MenuScroll.h"
#include "../Menu.h"
#include "../MenuLayer.h"
#include "../MenuData.h"

#include "Controller/SystemController.h"

#include "SpriteFrame/SpriteFrame.h"


////////////////////////////////////////////////////////////////////////////////
//
//	���j���[�X�N���[��
//
////////////////////////////////////////////////////////////////////////////////
MenuScroll::MenuScroll( MenuLayer* pParent ) : MenuObject( pParent )
{
	m_pScrollParam	= NULL;
	
	m_CurScale.One();
	m_TargetScale.One();
}

MenuScroll::~MenuScroll()
{

}

//******************************************************************************
//	������
//******************************************************************************
void MenuScroll::OnInit( void )
{
	m_pScrollParam = &_GetParam()->Scroll;
	_ASSERT( m_pScrollParam );
	
	//	�X�N���[���̍X�V
	_UpdateScroll();
	m_CurPos = m_TargetPos;
	m_CurScale = m_TargetScale;
	
	//	�I�u�W�F�N�g�̍X�V
	_UpdateObject();
}

//******************************************************************************
//	�X�V
//******************************************************************************
void MenuScroll::OnUpdate( void )
{
	//	�X�N���[���̍X�V
	_UpdateScroll();
	
	//	�ʒu�̕��
	m_CurPos += (m_TargetPos - m_CurPos) * m_pScrollParam->MoveSpeed;
	m_CurScale += (m_TargetScale - m_CurScale) * m_pScrollParam->MoveSpeed;

	//	�I�u�W�F�N�g�̍X�V
	_UpdateObject();
}

//******************************************************************************
//	�I�𒆂̍X�V
//******************************************************************************
void MenuScroll::OnUpdateSelect( void )
{

}

//------------------------------------------------------------------------------
//	�X�N���[���ʒu�̍X�V
//------------------------------------------------------------------------------
void MenuScroll::_UpdateScroll( void )
{
	m_TargetScale.One();

	Rect BoneRect = GetParent()->GetSprite()->GetBoneRect( _GetParam()->BoneName );
	Rect BaseRect = GetParent()->GetSprite()->GetBoneRect( m_pScrollParam->BaseBone );
	if( BaseRect.IsEmpty() || BoneRect.IsEmpty() ) return;

	Point BaseScale = BoneRect.Size() / BaseRect.Size();

	MenuObject* pList = GetParent()->GetObject( m_pScrollParam->TargetBone );
	if( !pList ) return;

	//	����
	if( m_pScrollParam->SelectDir == MENU_SELECT_H )
	{
		int ListNum = pList->GetListNum( MENU_SELECT_H );
		int CellNum = pList->GetCellNum( MENU_SELECT_H );
		int TopIndex = pList->GetCellTopIndex( MENU_SELECT_H );
		m_TargetScale.x = BaseScale.x * (ListNum < CellNum)? (float)ListNum / (float)CellNum : 1.0f;

		float PosCoef = (TopIndex < CellNum)? (float)TopIndex / (float)CellNum : 0.0f;

		m_TargetPos.x = BaseRect.l + (m_TargetScale.x * BoneRect.Size().x * 0.5f) + BaseRect.Size().x * PosCoef;
		m_TargetPos.y = BaseRect.CenterY();
	}
	//	����
	else if( m_pScrollParam->SelectDir == MENU_SELECT_V )
	{
		int ListNum = pList->GetListNum( MENU_SELECT_V );
		int CellNum = pList->GetCellNum( MENU_SELECT_V );
		int TopIndex = pList->GetCellTopIndex( MENU_SELECT_V );

		m_TargetScale.y = BaseScale.y * (ListNum < CellNum)? (float)ListNum / (float)CellNum : 1.0f;
		float PosCoef = (TopIndex < CellNum)? (float)TopIndex / (float)CellNum : 0.0f;

		m_TargetPos.y = BaseRect.t + (m_TargetScale.y * BoneRect.Size().y * 0.5f) + BaseRect.Size().y * PosCoef;
		m_TargetPos.x = BaseRect.CenterX();
	}

	//	�֐߈ʒu�Ɉړ�
	const Matrix3* pMat = GetParent()->GetSprite()->GetBoneLocalMatrix( m_pScrollParam->BaseBone );
	if( pMat ) m_TargetPos = pMat->Transform( m_TargetPos );
}

//------------------------------------------------------------------------------
//	�I�u�W�F�N�g�̍X�V
//------------------------------------------------------------------------------
void MenuScroll::_UpdateObject( void )
{
	GetParent()->SetPos( _GetParam()->BoneName, m_CurPos );
	GetParent()->SetGauge( _GetParam()->BoneName, m_CurScale.x, m_CurScale.y );

}