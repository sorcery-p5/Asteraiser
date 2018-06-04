#include "P5.h"
#include "RectSprite.h"

#include "Graphix/Texture/Texture.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	��`�X�v���C�g
//
///////////////////////////////////////////////////////////////////////////////
RectSprite::RectSprite( void ) : Sprite()
{
	m_Index		= 0;
	m_pPiece	= NULL;

	ZeroArray( m_aVertex );
	
	for( int i = 0; i < VERTEX_NUM; i++ )
	{
		m_aVertex[i].rhw	= 1.0f;
		m_aVertex[i].color	= Color(255,255,255);
	}
}

//*****************************************************************************
//	�s�[�X�ݒ�
//*****************************************************************************
void RectSprite::SetPiece( const Piece* pPiece, const Texture* pTex )
{
	//	�T�C�Y���o
	if( pPiece ) m_Size = pPiece->GetRect(m_Index).Size();

	//	�e�N�X�`���ݒ�
	m_pPiece = pPiece;
	SetTex( pTex );
	
	//	UV�K�p
	_ApplyVertexUv();
}

//*****************************************************************************
//	�����w��`��
//*****************************************************************************
void RectSprite::Draw( const Point& Center )
{
	const Point Half	= (m_Size * 0.5f).GetFloor();
	const Point LT		= Center - Half;
	const Point RB		= LT + m_Size;

	const Point aPt[] = 
	{
		Point( LT.x, LT.y ),	Point( RB.x, LT.y ),
		Point( LT.x, RB.y ),	Point( RB.x, RB.y ),
	};
	Draw( aPt );
}

//*****************************************************************************
//	�}�g���N�X�w��`��
//*****************************************************************************
void RectSprite::Draw( const Matrix3& Mat )
{
	Point aPt[] = 
	{
		Point( 0.0f, 0.0f ),		Point( m_Size.x, 0.0f ),
		Point( 0.0f, m_Size.y ),	m_Size,
	};

	//	�}�g���N�X�ϊ�
	for( int i = 0; i < VERTEX_NUM; i++ )
	{
		aPt[i] = Mat.Transform( aPt[i] );
	}

	Draw( aPt );
}

//*****************************************************************************
//	��`�w��`��
//*****************************************************************************
void RectSprite::Draw( const Rect& Rc )
{
	Point aPt[] = { Rc.LT(), Rc.RT(), Rc.LB(), Rc.RB() };

	Draw( aPt );
}

//*****************************************************************************
//	�l�_�w��`��
//*****************************************************************************
void RectSprite::Draw( const Point aPt[] )
{
	//	���_�̐ݒ�
	for( int i = 0; i < VERTEX_NUM; i++ )
	{
		m_aVertex[i].x = Round( aPt[i].x ) + 0.1f;
		m_aVertex[i].y = Round( aPt[i].y ) + 0.1f;
	}
	
	//	�o�^
	DrawItem::Draw();
}

//*****************************************************************************
//	�F�̐ݒ�
//*****************************************************************************
void RectSprite::SetColor( Color Col )
{
	for( int i = 0; i < VERTEX_NUM; i++ )
	{
		m_aVertex[i].color = Col;
	}
}

//*****************************************************************************
//	�F�̐ݒ�i�S�_�j
//*****************************************************************************
void RectSprite::SetColor( const Color aCol[] )
{
	for( int i = 0; i < VERTEX_NUM; i++ )
	{
		m_aVertex[i].color = aCol[i];
	}
}

//******************************************************************************
//	�C���f�N�X�̐ݒ�
//******************************************************************************
void RectSprite::SetIndex( int Index )
{
	if( m_Index != Index )
	{
		m_Index = Index;

		//	�T�C�Y�ύX
		if( m_pPiece ) m_Size = m_pPiece->GetRect(m_Index).Size();

		//	���_�̔��f
		_ApplyVertexUv();
	}
}

//------------------------------------------------------------------------------
//	�`����s
//------------------------------------------------------------------------------
void RectSprite::_OnRender( void ) const
{
	_RenderVertex( m_aVertex, VERTEX_NUM );
}

//-----------------------------------------------------------------------------
//	���_��UV�l�𔽉f
//-----------------------------------------------------------------------------
void RectSprite::_ApplyVertexUv( void )
{
	if( !m_pPiece ) return;

	Sprite::_ApplyVertexUv( m_aVertex, m_pPiece->GetRect( m_Index ) );
}

}