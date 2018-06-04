#include "stdafx.h"
#include "CollideShape.h"

#include "CollideShapePoint.h"
#include "CollideShapeRect.h"
#include "CollideShapeLine.h"

#include "../CollideCaster.h"


///////////////////////////////////////////////////////////////////////////////
//
//	������`��(���)
//
///////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	�`��L���X�g
//******************************************************************************
bool CollideShape::ShapeCast( COLLIDE_RESULT& Result, const CollideCaster* pCaster, Point* pSolvePos )
{
	//	AABB�̃`�F�b�N
	if( !GetAabb().IsIntersect( pCaster->GetShape()->GetAabb() ) )
	{
		return false;
	}

	//	�`�󂲂Ƃ̏���
	switch( pCaster->GetShape()->GetType() )
	{
	case COLLIDE_SHAPE_POINT:	return _ShapeCast( Result, (const CollideShapePoint*)pCaster->GetShape(), pCaster->GetCalcType(), pSolvePos );
	case COLLIDE_SHAPE_LINE:	return _ShapeCast( Result, (const CollideShapeLine*)pCaster->GetShape(), pCaster->GetCalcType() );
	}

	return false;
}