#include "stdafx.h"
#include "MenuList.h"
#include "../Menu.h"
#include "../MenuLayer.h"
#include "../MenuData.h"

#include "Controller/SystemController.h"
#include "SpriteFrame/SpriteFrame.h"


////////////////////////////////////////////////////////////////////////////////
//
//	メニューリスト
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
//	初期化
//******************************************************************************
void MenuList::OnInit( void )
{
	m_pListParam = &_GetParam()->List;
	_ASSERT( m_pListParam );

	m_SelectIndex = 0;

	m_CursorPos = _GetSelectPos();

	//	スプライト生成
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
//	更新
//******************************************************************************
void MenuList::OnUpdate( void )
{
	//	カーソル位置の移動
	m_CursorPos += (_GetSelectPos() - m_CursorPos) * m_pListParam->MoveSpeed;
	_GetSprite()->SetBonePos( m_pListParam->CursorBone, m_CursorPos );

	//	カーソルの非表示
	_GetSprite()->SetBoneVisible( m_pListParam->CursorBone, GetParent()->IsCurrent() && IsSelect() );

	//	スプライトの更新
	_UpdateSprite();
}

//******************************************************************************
//	選択中の更新
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

	//	横方向
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
	//	縦方向
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

		//	選択を移動
		if( SelIndex.x <= 0 && DirH < 0 )						SelIndex.x = CellMax.x - 1;
		else if( SelIndex.x >= CellMax.x - 1 && DirH > 0 )		SelIndex.x = 0;
		else if( m_SelectIndex >= GetCellNum() && DirH > 0 )	SelIndex.x = 0;
		else													SelIndex.x = Clamp( (int)SelIndex.x + DirH, 0, (int)CellMax.x - 1 );
		
		if( SelIndex.y <= 0 && DirV < 0 )						SelIndex.y = CellMax.y - 1;
		else if( SelIndex.y >= CellMax.y - 1 && DirV > 0 )		SelIndex.y = 0;
		else													SelIndex.y = Clamp( (int)SelIndex.y + DirV, 0, (int)CellMax.y - 1 );

		//	位置を反映
		_SetSelectIndex( SelIndex );

		//	通知
		GetMenu()->OnChangeValue( this );

		//	サウンド再生
		GetMenu()->PlaySound( m_pListParam->MoveSound, m_pListParam->MoveSoundVol );
	}
}

//******************************************************************************
//	描画
//******************************************************************************
void MenuList::Draw( const Matrix3& OffsetMat )
{
	//	スプライトの描画
	for( int i = 0; i < (int)m_vSprite.size(); i++ )
	{
		m_vSprite[i]->Draw( OffsetMat );
	}
}

//******************************************************************************
//	オブジェクトが選択された
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
//	移動可能か？
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
//	要素の個数を設定
//******************************************************************************
void MenuList::SetCellNum( int Num )
{
	m_vCell.resize( Num );

	m_SelectIndex = Clamp( m_SelectIndex, 0, Max( Num - 1, 0 ) );
}

//******************************************************************************
//	要素の列選択インデクスを取得
//******************************************************************************
int MenuList::GetCellTopIndex( MENU_SELECT_DIR Dir ) const
{
	Point TopIndex	= _GetCellTopIndex();
	return Dir == MENU_SELECT_H? (int)TopIndex.x : (int)TopIndex.y;
}

//******************************************************************************
//	要素の列個数を取得
//******************************************************************************
int MenuList::GetCellNum( MENU_SELECT_DIR Dir ) const
{
	Point CellNum = _GetCellMax();
	return Dir == MENU_SELECT_H? (int)CellNum.x : (int)CellNum.y;
}

//******************************************************************************
//	要素の列表示数を取得
//******************************************************************************
int MenuList::GetListNum( MENU_SELECT_DIR Dir ) const
{
	return Dir == MENU_SELECT_H? m_pListParam->NumH : m_pListParam->NumV;
}

//******************************************************************************
//	要素の文字列を登録
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
//	要素の色を登録
//******************************************************************************
void MenuList::SetCellColor( int Index, Label Bone, Color Col )
{
	if( Index < GetCellNum() )
	{
		m_vCell[Index].mapColor[Bone] = Col;
	}
}

//------------------------------------------------------------------------------
//	スプライトの更新
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
				//	文字列適用
				_ApplySpriteCell( pSprite, m_vCell[CellIndex] );

				//	色設定
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

	//	オフセット更新
	m_SpriteOffset = Point::EpsilonCut( m_SpriteOffset - m_SpriteOffset * m_pListParam->MoveSpeed );
}

//------------------------------------------------------------------------------
//	スプライトに、セルの内容を反映させる
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
//	2次元インデクスで選択を設定
//------------------------------------------------------------------------------
void MenuList::_SetSelectIndex( Point Index )
{
	Point PrevTop = _GetCellTopIndex();

	//	移動
	m_SelectIndex = Clamp( _ToIndex(Index), 0, Max( GetCellNum() - 1, 0 ) );
	
	Point NextTop = _GetCellTopIndex();

	if( PrevTop.x > NextTop.x )			m_SpriteOffset.x = -1.0f;
	else if( PrevTop.x < NextTop.x )	m_SpriteOffset.x =  1.0f;
	if( PrevTop.y > NextTop.y )			m_SpriteOffset.y = -1.0f;
	else if( PrevTop.y < NextTop.y )	m_SpriteOffset.y =  1.0f;
}

//------------------------------------------------------------------------------
//	選択中の2次元インデクスを得る
//------------------------------------------------------------------------------
Point MenuList::_GetSelectIndex( void ) const
{
	return _To2DIndex( m_SelectIndex );
}

//------------------------------------------------------------------------------
//	要素の2次元最大数を得る
//------------------------------------------------------------------------------
Point MenuList::_GetCellMax( void ) const
{
	return Point( (float)m_pListParam->LineNum, ceilf( (float)GetCellNum() / (float)m_pListParam->LineNum ) );
}

//------------------------------------------------------------------------------
//	表示要素の先頭2次元インデクスを得る
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
//	選択位置を取得
//------------------------------------------------------------------------------
Point MenuList::_GetSelectPos( void ) const
{
	Point Index = (_GetSelectIndex() - _GetCellTopIndex());

	return GetParent()->GetSprite()->GetBoneLocalMatrix( _GetParam()->BoneName )->Transform( _GetIndexPos(Index) );
}

//------------------------------------------------------------------------------
//	スプライトの描画位置を得る
//------------------------------------------------------------------------------
Matrix3 MenuList::_GetSpriteMat( Point Index ) const
{
	Matrix3 Mat = Matrix3::Trans( _GetIndexPos(Index) + _GetSpriteStride() * m_SpriteOffset );

	//	親の位置に合わせる
	const Matrix3* pMat = GetParent()->GetSprite()->GetBoneMatrix( _GetParam()->BoneName );
	if( pMat ) Mat *= *pMat;

	return Mat;
}

//------------------------------------------------------------------------------
//	スプライトの幅を得る
//------------------------------------------------------------------------------
Point MenuList::_GetSpriteStride( void ) const
{
	Rect	Rc = GetParent()->GetSprite()->GetBoneRect( _GetParam()->BoneName );

	return Rc.Size() * Point( 1.0f/m_pListParam->NumH, 1.0f/m_pListParam->NumV );
}

//------------------------------------------------------------------------------
//	インデクスから、中心座標を得る
//------------------------------------------------------------------------------
Point MenuList::_GetIndexPos( Point Index ) const
{
	Rect	Rc = GetParent()->GetSprite()->GetBoneRect( _GetParam()->BoneName );

	return Rc.LT() + _GetSpriteStride() * (Index + Point(0.5f,0.5f) );
}

//------------------------------------------------------------------------------
//	スプライトの表示有無を取得
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
//	配列インデクスを2次元インデクスへ
//------------------------------------------------------------------------------
Point MenuList::_To2DIndex( int Index ) const
{
	return Point( Index % m_pListParam->LineNum, Index / m_pListParam->LineNum );
}

//------------------------------------------------------------------------------
//	2次元インデクスを配列インデクスへ
//------------------------------------------------------------------------------
int MenuList::_ToIndex( Point Index ) const
{
	return (int)( Index.y * m_pListParam->LineNum + Index.x );
}