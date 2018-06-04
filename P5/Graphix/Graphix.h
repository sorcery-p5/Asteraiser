#pragma once

#include <d3d9.h>
#include <d3dx9.h>

namespace P5
{

class DrawDevice;
class DrawItemManager;
class Timer;

class Texture;

const int			FPS			= 60;

//-----------------------------------------------------------------------------
//	�A���t�@�u�����h���[�h
//-----------------------------------------------------------------------------
enum BLEND_MODE
{
	BLEND_NONE,		//	�s����
	BLEND_ALPHA,	//	�A���t�@
	BLEND_ADD,		//	���Z
	BLEND_SUB,		//	���Z
	BLEND_MUL,		//	��Z
	BLEND_REV,		//	���]

	BLEND_MODE_NUM,
};


//------------------------------------------------------------------------------
//	�f�o�C�X�̃��Z�b�g�ʒm���󂯎��C���^�t�F�[�X
//------------------------------------------------------------------------------
class DeviceResetListener
{
public:
	virtual void	OnDrawDeviceReset( void )		= 0;
	virtual void	OnDrawDeviceRestore( void )		= 0;
};

///////////////////////////////////////////////////////////////////////////////
//
//	�O���t�B�N�X
//
///////////////////////////////////////////////////////////////////////////////
class Graphix
{
public:
	Graphix( void );
	~Graphix();

	bool					Init( Window* pWindow );
	bool					Update( void );
	
	void					Reset( void );
	void					AddResetListener( DeviceResetListener* pListener );
	void					RemoveResetListener( DeviceResetListener* pListener );

	void					SetFrameSkip( int Frame )					{ m_FrameSkip = Frame; }

	DrawDevice*				GetDevice( void )							{ return m_pDevice; }
	DrawItemManager*		GetDrawItemMng( void )						{ return m_pDrawItemMng; }

	static BLEND_MODE		StrToBlendMode( pstr pStr );

private:	
	typedef std::vector<DeviceResetListener*>	VecResetListener;

private:
	DrawDevice*				m_pDevice;
	DrawItemManager*		m_pDrawItemMng;
	Timer*					m_pTimer;

	VecResetListener		m_vResetListener;

	int						m_FrameSkip;
	int						m_FrameSkipCount;
};

}

#include "DrawDevice.h"
#include "Texture/Texture.h"
#include "Piece/Piece.h"
#include "Piece/RectPiece.h"
#include "Piece/PagePiece.h"
#include "Piece/ArrayPiece.h"
#include "Font/Font.h"
#include "Sprite/RectSprite.h"
#include "Sprite/LineSprite.h"
#include "Sprite/RingSprite.h"
#include "Sprite/FontSprite.h"
#include "Shape/ShapeLine.h"
#include "Shape/ShapeSolidLine.h"
#include "Shape/ShapeArrowLine.h"
#include "Shape/ShapeCircle.h"
#include "Shape/ShapePolygon.h"
#include "DrawItem/DrawItemManager.h"