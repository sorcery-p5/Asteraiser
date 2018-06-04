#include "stdafx.h"
#include "CollideShape.h"

#include "CollideShapePoint.h"
#include "CollideShapeRect.h"
#include "CollideShapeLine.h"

#include "../CollideCaster.h"


///////////////////////////////////////////////////////////////////////////////
//
//	当たり形状(基底)
//
///////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	形状キャスト
//******************************************************************************
bool CollideShape::ShapeCast( COLLIDE_RESULT& Result, const CollideCaster* pCaster, Point* pSolvePos )
{
	//	AABBのチェック
	if( !GetAabb().IsIntersect( pCaster->GetShape()->GetAabb() ) )
	{
		return false;
	}

	//	形状ごとの処理
	switch( pCaster->GetShape()->GetType() )
	{
	case COLLIDE_SHAPE_POINT:	return _ShapeCast( Result, (const CollideShapePoint*)pCaster->GetShape(), pCaster->GetCalcType(), pSolvePos );
	case COLLIDE_SHAPE_LINE:	return _ShapeCast( Result, (const CollideShapeLine*)pCaster->GetShape(), pCaster->GetCalcType() );
	}

	return false;
}