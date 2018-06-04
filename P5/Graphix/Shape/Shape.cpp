#include "P5.h"
#include "Shape.h"

#include "Graphix/DrawDevice.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	�V�F�C�v���
//
///////////////////////////////////////////////////////////////////////////////
Shape::Shape( int Num ) : DrawItem()
{
	m_aVertex	= new SHAPE_VERTEX[ Num ];
	MEM_CHECK( m_aVertex );
	m_Num		= Num;
	
	ZeroStructArray( m_aVertex, m_Num );
	
	for( int i = 0; i < m_Num; i++ )
	{
		m_aVertex[i].rhw	= 1.0f;
		m_aVertex[i].color	= Color(255,255,255);
	}
}
Shape::~Shape()
{
	SafeDeleteA( m_aVertex );

}

//------------------------------------------------------------------------------
//	�F�ݒ�
//------------------------------------------------------------------------------
void Shape::_SetColor( Color Col )
{
	for( int i = 0; i < m_Num; i++ )
	{
		m_aVertex[i].color = Col;
	}
}

//------------------------------------------------------------------------------
//	�`��̎��s
//------------------------------------------------------------------------------
void Shape::_OnRender( void ) const
{
	DrawDevice* pDevice = GetGraphix()->GetDevice();

	//	�e�N�X�`���o�^
	pDevice->SetTexture( NULL );

	//	�`��
	pDevice->GetDevice()->DrawPrimitiveUP( D3DPT_LINESTRIP, m_Num-1, m_aVertex, sizeof(SHAPE_VERTEX) );
}

}