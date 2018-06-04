#pragma once

#include "MenuObject.h"


////////////////////////////////////////////////////////////////////////////////
//
//	メニューリスト
//
////////////////////////////////////////////////////////////////////////////////
class MenuList : public MenuObject
{
public:
	MenuList( MenuLayer* pParent );
	virtual ~MenuList();

	virtual void			OnInit( void );
	virtual void			OnUpdate( void );
	virtual void			OnUpdateSelect( void );
	virtual void			OnChangeSelect( MENU_DIR Dir );
	virtual void			Draw( const Matrix3& OffsetMat );
	virtual bool			IsMoveEnable( MENU_DIR Dir );
	virtual bool			IsCursorVisible( void )											{ return false; }

	virtual int				GetSelectIndex( void )									const	{ return m_SelectIndex; }
	virtual void			SetSelectIndex( int Index )										{ m_SelectIndex = Index; }
	virtual int				GetCellNum( void )										const	{ return (int)m_vCell.size(); }
	virtual void			SetCellNum( int Num );
	virtual int				GetCellTopIndex( MENU_SELECT_DIR Dir )					const;
	virtual int				GetCellNum( MENU_SELECT_DIR Dir )						const;
	virtual int				GetListNum( MENU_SELECT_DIR Dir )						const;
	virtual void			SetCellText( int Index, Label Bone, pstr pText, ... );
	virtual void			SetCellColor( int Index, Label Bone, Color Col );

private:
	struct CELL
	{
		std::map<Label, std::string >		mapText;
		std::map<Label, Color >				mapColor;
	};
	typedef std::vector< CELL >				VecCell;
	typedef std::vector< SpriteFrame* >		VecSprite;

private:
	void					_UpdateSprite( void );
	void					_ApplySpriteCell( SpriteFrame* pSprite, const CELL& Cell );
	void					_SetSelectIndex( Point Index );
	Point					_GetSelectIndex( void )								const;
	Point					_GetCellMax( void )									const;
	Point					_GetCellTopIndex( void )							const;
	Point					_GetSelectPos( void )								const;
	Matrix3					_GetSpriteMat( Point Index )						const;
	Point					_GetSpriteStride( void )							const;
	Point					_GetIndexPos( Point Index )							const;
	Color					_GetSpriteColor( Point Index )						const;
	Point					_To2DIndex( int Index )								const;
	int						_ToIndex( Point Index )								const;

private:
	const MENU_LIST*		m_pListParam;
	Point					m_CursorPos;
	int						m_SelectIndex;
	Point					m_SpriteOffset;
	bool					m_bChangeSelect;

	VecSprite				m_vSprite;
	VecCell					m_vCell;
};