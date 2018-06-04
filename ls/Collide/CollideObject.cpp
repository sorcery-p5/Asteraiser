#include "stdafx.h"
#include "CollideObject.h"
#include "CollideSystem.h"

#include "Shape/CollideShape.h"


////////////////////////////////////////////////////////////////////////////////
//
//	当たりオブジェクト
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
	//	解除
	GetCollideSystem()->RemoveObject( this );

	SafeDelete( m_pShape );
}

//******************************************************************************
//	初期化
//******************************************************************************
void CollideObject::Init( CollideShape* pShape, uint Layer )
{
	MEM_CHECK( pShape );

	m_pShape	= pShape;
	m_Layer		= Layer;

	//	登録
	GetCollideSystem()->AddObject( this );	
}

//******************************************************************************
//	更新
//******************************************************************************
void CollideObject::Update( void )
{
	//	形状描画
	if( GetCollideSystem()->IsDrawShape() )
	{
		m_pShape->DrawShape( GetCollideSystem()->GetDrawShapeColor( false, m_bCastHit, m_Layer ) );
	}

	m_bCastHit = false;
}

//******************************************************************************
//	マトリクスの更新
//******************************************************************************
void CollideObject::UpdateMatrix( const Matrix3* pMatrix )
{
	m_pShape->UpdateMatrix( pMatrix );
}