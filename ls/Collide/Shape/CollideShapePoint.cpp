#include "stdafx.h"
#include "CollideShapePoint.h"
#include "CollideShapeLine.h"

#include "../CollideFunction.h"


///////////////////////////////////////////////////////////////////////////////
//
//	当たり形状：点
//
///////////////////////////////////////////////////////////////////////////////
CollideShapePoint::CollideShapePoint( void ) : CollideShape( COLLIDE_SHAPE_POINT )
{
	m_Radius = 0.0f;
	
	m_pDrawShape	= new ShapeCircle;
}
CollideShapePoint::CollideShapePoint( float Radius ) : CollideShape( COLLIDE_SHAPE_POINT )
{
	m_Radius = Radius;
	
	m_pDrawShape	= new ShapeCircle;
}
CollideShapePoint::~CollideShapePoint()
{
	SafeDelete( m_pDrawShape );
}

//******************************************************************************
//	形状を描画する
//******************************************************************************
void CollideShapePoint::DrawShape( Color Col ) const
{
	if( m_pDrawShape )
	{
		m_pDrawShape->SetPhase( 10000 );
		m_pDrawShape->SetColor( Col );
		m_pDrawShape->Draw( m_Pos, m_Radius );
	}
}

//------------------------------------------------------------------------------
//	形状キャスト：点
//------------------------------------------------------------------------------
bool CollideShapePoint::_ShapeCast( COLLIDE_RESULT& Result, const CollideShapePoint* pShape, dword CalcType, Point* pSolvePos )
{
	Point CastPoint = (CalcType & COLLIDE_CALC_SOLVE && pSolvePos)? *pSolvePos : pShape->GetPos();

	return CollideCalcPointToPoint( Result, CastPoint, pShape->GetRadius(), m_Pos, m_Radius, CalcType, pSolvePos );
}

//------------------------------------------------------------------------------
//	形状キャスト：線
//------------------------------------------------------------------------------
bool CollideShapePoint::_ShapeCast( COLLIDE_RESULT& Result, const CollideShapeLine* pShape, dword CalcType )
{
	return CollideCalcLineToPoint( Result, pShape, m_Pos, m_Radius, CalcType );
}