#include "stdafx.h"
#include "MenuList.h"
#include "../Menu.h"
#include "../MenuLayer.h"
#include "../MenuData.h"

#include "Controller/SystemController.h"
#include "SpriteFrame/SpriteFrame.h"


////////////////////////////////////////////////////////////////////////////////
//
//	���j���[���X�g
//
////////////////////////////////////////////////////////////////////////////////
MenuList::MenuList( MenuLayer* pParent ) : MenuObject( pParent )
{
	m_pListParam	= NULL;
	m_SelectIndex	= 0;
	m_bChangeSelect	= false;
}

MenuList::~MenuList()
{
	DeleteVec( m_vSprite );
}

//******************************************************************************
//	������
//******************************************************************************
void MenuList::OnInit( void )
{
	m_pListParam = &_GetParam()->List;
	_ASSERT( m_pListParam );

	m_SelectIndex = 0;

	m_CursorPos = _GetSelectPos();

	//	�X�v���C�g����
	int Index = 0;
	for( int y = 0; y < m_pListParam->NumV; y++ )
	{
		for( int x = 0; x < m_pListParam->NumH; x++ )
		{
			const SpriteFrameFile* pSpriteFile = GetMenu()->GetData()->GetSpriteFrameFile( m_pListParam->CellSprite );
			_ASSERT( pSpriteFile );
		
			SpriteFrame* pSprite = new SpriteFrame;
			MEM_CHECK( pSprite );
			pSprite->Init( pSpriteFile, _GetSpriteMat( Point( x, y ) ) );

			m_vSprite.push_back( pSprite );
			Index++;
		}
	}
}

//******************************************************************************
//	�X�V
//******************************************************************************
void MenuList::OnUpdate( void )
{
	//	�J�[�\���ʒu�̈ړ�
	m_CursorPos += (_GetSelectPos() - m_CursorPos) * m_pListParam->MoveSpeed;
	_GetSprite()->SetBonePos( m_pListParam->CursorBone, m_CursorPos );

	//	�J�[�\���̔�\��
	_GetSprite()->SetBoneVisible( m_pListParam->CursorBone, GetParent()->IsCurrent() && IsSelect() );

	//	�X�v���C�g�̍X�V
	_UpdateSprite();
}

//******************************************************************************
//	�I�𒆂̍X�V
//******************************************************************************
void MenuList::OnUpdateSelect( void )
{
	if( m_bChangeSelect )
	{
		m_bChangeSelect = false;
		return;
	}

	SystemController Ctrl;

	int DirH = 0, DirV = 0;

	//	������
	if( m_pListParam->LineNum > 1 )
	{
		if( Ctrl.IsDirInput( CONTROLLER_L, INPUT_REPEAT ) )			DirH = -1;
		else if( Ctrl.IsDirInput( CONTROLLER_R, INPUT_REPEAT ) )	DirH =  1;
	}
	else
	{
		if( Ctrl.IsDirInput( CONTROLLER_L, INPUT_REPEAT ) )			DirV = -m_pListParam->NumV;
		else if( Ctrl.IsDirInput( CONTROLLER_R, INPUT_REPEAT ) )	DirV =  m_pListParam->NumV;
	}
	//	�c����
	if( m_pListParam->NumV > 1 )
	{
		if( Ctrl.IsDirInput( CONTROLLER_U, INPUT_REPEAT ) )			DirV = -1;
		else if( Ctrl.IsDirInput( CONTROLLER_D, INPUT_REPEAT ) )	DirV =  1;
	}
	else
	{
		if( Ctrl.IsDirInput( CONTROLLER_U, INPUT_REPEAT ) )			DirH = -m_pListParam->NumH;
		else if( Ctrl.IsDirInput( CONTROLLER_D, INPUT_REPEAT ) )	DirH =  m_pListParam->NumH;
	}

	if( DirH != 0 || DirV != 0 )
	{
		Point SelIndex	= _GetSelectIndex();
		Point CellMax	= _GetCellMax();

		//	�I�����ړ�
		if( SelIndex.x <= 0 && DirH < 0 )						SelIndex.x = CellMax.x - 1;
		else if( SelIndex.x >= CellMax.x - 1 && DirH > 0 )		SelIndex.x = 0;
		else if( m_SelectIndex >= GetCellNum() && DirH > 0 )	SelIndex.x = 0;
		else													SelIndex.x = Clamp( (int)SelIndex.x + DirH, 0, (int)CellMax.x - 1 );
		
		if( SelIndex.y <= 0 && DirV < 0 )						SelIndex.y = CellMax.y - 1;
		else if( SelIndex.y >= CellMax.y - 1 && DirV > 0 )		SelIndex.y = 0;
		else													SelIndex.y = Clamp( (int)SelIndex.y + DirV, 0, (int)CellMax.y - 1 );

		//	�ʒu�𔽉f
		_SetSelectIndex( SelIndex );

		//	�ʒm
		GetMenu()->OnChangeValue( this );

		//	�T�E���h�Đ�
		GetMenu()->PlaySound( m_pListParam->MoveSound, m_pListParam->MoveSoundVol );
	}
}

//******************************************************************************
//	�`��
//******************************************************************************
void MenuList::Draw( const Matrix3& OffsetMat )
{
	//	�X�v���C�g�̕`��
	for( int i = 0; i < (int)m_vSprite.size(); i++ )
	{
		m_vSprite[i]->Draw( OffsetMat );
	}
}

//******************************************************************************
//	�I�u�W�F�N�g���I�����ꂽ
//******************************************************************************
void MenuList::OnChangeSelect( MENU_DIR Dir )
{
	Point SelIndex	= _GetSelectIndex();
	Point CellMax	= _GetCellMax();

	switch( Dir )
	{
	case MENU_DIR_L:	SelIndex.x = CellMax.x - 1;	break;
	case MENU_DIR_U:	SelIndex.y = CellMax.y - 1;	break;
	case MENU_DIR_R:	SelIndex.x = 0;				break;
	case MENU_DIR_D:	SelIndex.y = 0;				break;
	}

	_SetSelectIndex( SelIndex );

	m_bChangeSelect = true;
}

//******************************************************************************
//	�ړ��\���H
//******************************************************************************
bool MenuList::IsMoveEnable( MENU_DIR Dir )
{
	Point SelIndex	= _GetSelectIndex();
	Point CellMax	= _GetCellMax();

	switch( Dir )
	{
	case MENU_DIR_L:	return SelIndex.x <= 0;
	case MENU_DIR_U:	return SelIndex.y <= 0;
	case MENU_DIR_R:	return SelIndex.x >= CellMax.x - 1;
	case MENU_DIR_D:	return SelIndex.y >= CellMax.y - 1;
	}
	return false;
}

//******************************************************************************
//	�v�f�̌���ݒ�
//******************************************************************************
void MenuList::SetCellNum( int Num )
{
	m_vCell.resize( Num );

	m_SelectIndex = Clamp( m_SelectIndex, 0, Max( Num - 1, 0 ) );
}

//******************************************************************************
//	�v�f�̗�I���C���f�N�X���擾
//******************************************************************************
int MenuList::GetCellTopIndex( MENU_SELECT_DIR Dir ) const
{
	Point TopIndex	= _GetCellTopIndex();
	return Dir == MENU_SELECT_H? (int)TopIndex.x : (int)TopIndex.y;
}

//******************************************************************************
//	�v�f�̗�����擾
//******************************************************************************
int MenuList::GetCellNum( MENU_SELECT_DIR Dir ) const
{
	Point CellNum = _GetCellMax();
	return Dir == MENU_SELECT_H? (int)CellNum.x : (int)CellNum.y;
}

//******************************************************************************
//	�v�f�̗�\�������擾
//******************************************************************************
int MenuList::GetListNum( MENU_SELECT_DIR Dir ) const
{
	return Dir == MENU_SELECT_H? m_pListParam->NumH : m_pListParam->NumV;
}

//******************************************************************************
//	�v�f�̕������o�^
//******************************************************************************
void MenuList::SetCellText( int Index, Label Bone, pstr pText, ... )
{
	if( Index < GetCellNum() )
	{
		String256 Str;
		Str.FormatV( pText, (char*)(&pText + 1) );

		m_vCell[Index].mapText[Bone] = Str.c_str();
	}
}

//******************************************************************************
//	�v�f�̐F��o�^
//******************************************************************************
void MenuList::SetCellColor( int Index, Label Bone, Color Col )
{
	if( Index < GetCellNum() )
	{
		m_vCell[Index].mapColor[Bone] = Col;
	}
}

//------------------------------------------------------------------------------
//	�X�v���C�g�̍X�V
//------------------------------------------------------------------------------
void MenuList::_UpdateSprite( void )
{
	Point CellTop	= _GetCellTopIndex();
	int SpriteIndex = 0;
	Color BoneCol = GetParent()->GetSprite()->GetBoneColor( _GetParam()->BoneName );
	for( int y = 0; y < m_pListParam->NumV; y++ )
	{
		for( int x = 0; x < m_pListParam->NumH; x++ )
		{
			Point			Index		= Point( x, y );
			int				CellIndex	= _ToIndex( CellTop + Index );
			SpriteFrame*	pSprite		= m_vSprite[SpriteIndex];

			if( CellIndex < GetCellNum() )
			{
				//	������K�p
				_ApplySpriteCell( pSprite, m_vCell[CellIndex] );

				//	�F�ݒ�
				pSprite->SetColor( _GetSpriteColor(Index) * Color::Gray( 255, BoneCol.a ) );

				pSprite->SetVisible( true );
			}
			else
			{
				pSprite->SetVisible( false );
			}


			pSprite->Update( &_GetSpriteMat( Index ) );

			SpriteIndex++;
		}
	}

	//	�I�t�Z�b�g�X�V
	m_SpriteOffset = Point::EpsilonCut( m_SpriteOffset - m_SpriteOffset * m_pListParam->MoveSpeed );
}

//------------------------------------------------------------------------------
//	�X�v���C�g�ɁA�Z���̓��e�𔽉f������
//------------------------------------------------------------------------------
void MenuList::_ApplySpriteCell( SpriteFrame* pSprite, const CELL& Cell )
{
	for( std::map<Label, std::string >::const_iterator it = Cell.mapText.begin(); it != Cell.mapText.end(); ++it )
	{
		pSprite->SetBoneText( it->first, it->second.c_str() );
	}

	for( std::map<Label, Color >::const_iterator it = Cell.mapColor.begin(); it != Cell.mapColor.end(); ++it )
	{
		pSprite->SetBoneColor( it->first, it->second );
	}
}

//------------------------------------------------------------------------------
//	2�����C���f�N�X�őI����ݒ�
//------------------------------------------------------------------------------
void MenuList::_SetSelectIndex( Point Index )
{
	Point PrevTop = _GetCellTopIndex();

	//	�ړ�
	m_SelectIndex = Clamp( _ToIndex(Index), 0, Max( GetCellNum() - 1, 0 ) );
	
	Point NextTop = _GetCellTopIndex();

	if( PrevTop.x > NextTop.x )			m_SpriteOffset.x = -1.0f;
	else if( PrevTop.x < NextTop.x )	m_SpriteOffset.x =  1.0f;
	if( PrevTop.y > NextTop.y )			m_SpriteOffset.y = -1.0f;
	else if( PrevTop.y < NextTop.y )	m_SpriteOffset.y =  1.0f;
}

//------------------------------------------------------------------------------
//	�I�𒆂�2�����C���f�N�X�𓾂�
//------------------------------------------------------------------------------
Point MenuList::_GetSelectIndex( void ) const
{
	return _To2DIndex( m_SelectIndex );
}

//------------------------------------------------------------------------------
//	�v�f��2�����ő吔�𓾂�
//------------------------------------------------------------------------------
Point MenuList::_GetCellMax( void ) const
{
	return Point( (float)m_pListParam->LineNum, ceilf( (float)GetCellNum() / (float)m_pListParam->LineNum ) );
}

//------------------------------------------------------------------------------
//	�\���v�f�̐擪2�����C���f�N�X�𓾂�
//------------------------------------------------------------------------------
Point MenuList::_GetCellTopIndex( void ) const
{
	Point Out;

	Point SelIndex	= _GetSelectIndex();
	Point CellMax	= _GetCellMax();
	Point HalfNum	= Point( m_pListParam->NumH / 2, m_pListParam->NumV / 2 );

	Out.x = Clamp( SelIndex.x - HalfNum.x, 0.0f, Max( CellMax.x - m_pListParam->NumH, 0.0f ) );
	Out.y = Clamp( SelIndex.y - HalfNum.y, 0.0f, Max( CellMax.y - m_pListParam->NumV, 0.0f ) );

	return Out;
}

//------------------------------------------------------------------------------
//	�I���ʒu���擾
//------------------------------------------------------------------------------
Point MenuList::_GetSelectPos( void ) const
{
	Point Index = (_GetSelectIndex() - _GetCellTopIndex());

	return GetParent()->GetSprite()->GetBoneLocalMatrix( _GetParam()->BoneName )->Transform( _GetIndexPos(Index) );
}

//------------------------------------------------------------------------------
//	�X�v���C�g�̕`��ʒu�𓾂�
//------------------------------------------------------------------------------
Matrix3 MenuList::_GetSpriteMat( Point Index ) const
{
	Matrix3 Mat = Matrix3::Trans( _GetIndexPos(Index) + _GetSpriteStride() * m_SpriteOffset );

	//	�e�̈ʒu�ɍ��킹��
	const Matrix3* pMat = GetParent()->GetSprite()->GetBoneMatrix( _GetParam()->BoneName );
	if( pMat ) Mat *= *pMat;

	return Mat;
}

//------------------------------------------------------------------------------
//	�X�v���C�g�̕��𓾂�
//------------------------------------------------------------------------------
Point MenuList::_GetSpriteStride( void ) const
{
	Rect	Rc = GetParent()->GetSprite()->GetBoneRect( _GetParam()->BoneName );

	return Rc.Size() * Point( 1.0f/m_pListParam->NumH, 1.0f/m_pListParam->NumV );
}

//------------------------------------------------------------------------------
//	�C���f�N�X����A���S���W�𓾂�
//------------------------------------------------------------------------------
Point MenuList::_GetIndexPos( Point Index ) const
{
	Rect	Rc = GetParent()->GetSprite()->GetBoneRect( _GetParam()->BoneName );

	return Rc.LT() + _GetSpriteStride() * (Index + Point(0.5f,0.5f) );
}

//------------------------------------------------------------------------------
//	�X�v���C�g�̕\���L�����擾
//------------------------------------------------------------------------------
Color MenuList::_GetSpriteColor( Point Index ) const
{
	Point SpriteMax( m_pListParam->NumH - 1, m_pListParam->NumV - 1 );

	if( m_SpriteOffset.x < 0.0f && Index.x <= 0 ||
		m_SpriteOffset.x > 0.0f && Index.x >= SpriteMax.x )
	{
		return Color::Gray( 255, Lerp( 255, 0, Abs(m_SpriteOffset.x) ) );
	}
	if( m_SpriteOffset.y < 0.0f && Index.y <= 0 ||
		m_SpriteOffset.y > 0.0f && Index.y >= SpriteMax.y )
	{
		return Color::Gray( 255, Lerp( 255, 0, Abs(m_SpriteOffset.y) ) );
	}

	return Color::White();
}

//------------------------------------------------------------------------------
//	�z��C���f�N�X��2�����C���f�N�X��
//------------------------------------------------------------------------------
Point MenuList::_To2DIndex( int Index ) const
{
	return Point( Index % m_pListParam->LineNum, Index / m_pListParam->LineNum );
}

//------------------------------------------------------------------------------
//	2�����C���f�N�X��z��C���f�N�X��
//------------------------------------------------------------------------------
int MenuList::_ToIndex( Point Index ) const
{
	return (int)( Index.y * m_pListParam->LineNum + Index.x );
}