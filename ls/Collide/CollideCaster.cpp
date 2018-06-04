#include "stdafx.h"
#include "CollideCaster.h"
#include "CollideSystem.h"

#include "Shape/CollideShape.h"


///////////////////////////////////////////////////////////////////////////////
//
//	当たりキャスト
//
///////////////////////////////////////////////////////////////////////////////
CollideCaster::CollideCaster( void )
{
	m_pShape		= NULL;
	m_Filter		= COLLIDE_FILTER_ALL;
	m_CalcType		= 0;
	m_bHitEnable	= true;
	m_pListener		= NULL;
}
CollideCaster::~CollideCaster()
{
	SafeDelete( m_pShape );
}

//******************************************************************************
//	初期化
//******************************************************************************
void CollideCaster::Init( CollideShape* pShape )
{
	MEM_CHECK( pShape );
	_ASSERT( pShape->GetType() != COLLIDE_SHAPE_RECT &&
			 pShape->GetType() != COLLIDE_SHAPE_FRAME );

	m_pShape		= pShape;
	m_Filter		= COLLIDE_FILTER_ALL;
	m_CalcType		= 0;
}

//******************************************************************************
//	キャストを実行する
//******************************************************************************
bool CollideCaster::Cast( COLLIDE_CAST_RESULT& Result ) const
{
	if( !m_bHitEnable ) return false;

	return GetCollideSystem()->ShapeCast( Result, *this );
}

//******************************************************************************
//	行列の更新
//******************************************************************************
void CollideCaster::UpdateMatrix( const Matrix3* pMat )
{
	m_pShape->UpdateMatrix( pMat );
}