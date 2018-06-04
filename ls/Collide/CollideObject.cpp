#include "stdafx.h"
#include "CollideObject.h"
#include "CollideSystem.h"

#include "Shape/CollideShape.h"


////////////////////////////////////////////////////////////////////////////////
//
//	������I�u�W�F�N�g
//
////////////////////////////////////////////////////////////////////////////////
CollideObject::CollideObject( void )
{
	m_pShape		= NULL;
	m_Layer			= 0;
	m_bHitEnable	= true;
	m_bCastHit		= false;
	m_bSolveEnable	= false;
	m_pListener		= NULL;
}

CollideObject::~CollideObject()
{
	//	����
	GetCollideSystem()->RemoveObject( this );

	SafeDelete( m_pShape );
}

//******************************************************************************
//	������
//******************************************************************************
void CollideObject::Init( CollideShape* pShape, uint Layer )
{
	MEM_CHECK( pShape );

	m_pShape	= pShape;
	m_Layer		= Layer;

	//	�o�^
	GetCollideSystem()->AddObject( this );	
}

//******************************************************************************
//	�X�V
//******************************************************************************
void CollideObject::Update( void )
{
	//	�`��`��
	if( GetCollideSystem()->IsDrawShape() )
	{
		m_pShape->DrawShape( GetCollideSystem()->GetDrawShapeColor( false, m_bCastHit, m_Layer ) );
	}

	m_bCastHit = false;
}

//******************************************************************************
//	�}�g���N�X�̍X�V
//******************************************************************************
void CollideObject::UpdateMatrix( const Matrix3* pMatrix )
{
	m_pShape->UpdateMatrix( pMatrix );
}