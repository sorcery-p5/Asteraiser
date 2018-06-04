#include "stdafx.h"
#include "Controller.h"

namespace
{
	//	方向取得ビットマスク
	enum
	{
		DIR_MASK_L		= (1 << CONTROLLER_L),
		DIR_MASK_R		= (1 << CONTROLLER_R),
		DIR_MASK_U		= (1 << CONTROLLER_U),
		DIR_MASK_D		= (1 << CONTROLLER_D),
	
		DIR_MASK_LU		= DIR_MASK_L | DIR_MASK_U,
		DIR_MASK_LD		= DIR_MASK_L | DIR_MASK_D,
		DIR_MASK_RU		= DIR_MASK_R | DIR_MASK_U,
		DIR_MASK_RD		= DIR_MASK_R | DIR_MASK_D,

		DIR_MASK_H		= DIR_MASK_L | DIR_MASK_R,
		DIR_MASK_V		= DIR_MASK_U | DIR_MASK_D,
		DIR_MASK_ALL	= DIR_MASK_H | DIR_MASK_V,
	};
};

////////////////////////////////////////////////////////////////////////////////
//
//	コントローラ基底
//
////////////////////////////////////////////////////////////////////////////////
Controller::Controller( void )
{
	m_CurInfo	= 0;
	m_PreInfo	= 0;
}
Controller::~Controller()
{

}

//******************************************************************************
//	状態をコピーする
//******************************************************************************
void Controller::CopyInfo( const Controller& Ctrl )
{
	m_CurInfo	= Ctrl.m_CurInfo;
	m_PreInfo	= Ctrl.m_PreInfo;
}

//******************************************************************************
//	指定の入力があるかどうか
//******************************************************************************
bool Controller::IsInput( CONTROLLER_INPUT Input, INPUT_TYPE Type ) const
{
	byte Mask = (1 << Input);

	switch( Type )
	{
	case INPUT_PRESS:	return (m_CurInfo & Mask) != 0;
	case INPUT_TRIG:	return ((m_CurInfo & ~m_PreInfo) & Mask) != 0;
	case INPUT_OFF:		return ((~m_CurInfo & m_PreInfo) & Mask) != 0;
	}
	return false;
}

//******************************************************************************
//	方向キーの方向を得る
//******************************************************************************
Point Controller::GetDir( void ) const
{
	return InfoToDir( m_CurInfo );
}

//******************************************************************************
//	方向キーが中立かどうか
//******************************************************************************
bool Controller::IsNeutral( void ) const
{
	return (m_CurInfo & DIR_MASK_ALL) == 0;
}

//******************************************************************************
//	方向キーが水平方向に中立かどうか
//******************************************************************************
bool Controller::IsHNeutral( void ) const
{
	return (m_CurInfo & DIR_MASK_H) == 0;
}

//******************************************************************************
//	方向キーが垂直方向に中立かどうか
//******************************************************************************
bool Controller::IsVNeutral( void ) const
{
	return (m_CurInfo & DIR_MASK_V) == 0;
}

//------------------------------------------------------------------------------
//	現フレームの入力情報を設定する
//------------------------------------------------------------------------------
void Controller::_UpdateCurInfo( byte Info )
{
	m_PreInfo = m_CurInfo;
	m_CurInfo = Info;
}

//------------------------------------------------------------------------------
//	キー情報を方向に変換
//------------------------------------------------------------------------------
Point Controller::InfoToDir( byte Info )
{
	byte Dir = (Info & DIR_MASK_ALL);

	switch( Dir )
	{
	case DIR_MASK_U:	return Point( 0.0f, -1.0f );
	case DIR_MASK_RU:	return Point( SQ2,  -SQ2  );
	case DIR_MASK_R:	return Point( 1.0f,  0.0f );
	case DIR_MASK_RD:	return Point( SQ2,   SQ2  );
	case DIR_MASK_D:	return Point( 0.0f,  1.0f );
	case DIR_MASK_LD:	return Point(-SQ2,   SQ2  );
	case DIR_MASK_L:	return Point(-1.0f,	 0.0f );
	case DIR_MASK_LU:	return Point(-SQ2,  -SQ2  );
	}

	return Point();
}