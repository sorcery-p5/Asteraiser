#include "stdafx.h"
#include "CollideShapeLine.h"
#include "CollideShapePoint.h"

#include "../CollideFunction.h"


///////////////////////////////////////////////////////////////////////////////
//
//	当たり形状：直線
//
///////////////////////////////////////////////////////////////////////////////
CollideShapeLine::CollideShapeLine( void ) : CollideShape( COLLIDE_SHAPE_LINE )
{
	m_MinLength		= 0.0f;
	m_MaxLength		= 0.0f;
	m_Radius		= 0.0f;

	m_pDrawShape	= new ShapeSolidLine;
}
CollideShapeLine::CollideShapeLine( float Len, float Rad ) : CollideShape( COLLIDE_SHAPE_LINE )
{
	m_MinLength		= 0.0f;
	m_MaxLength		= Len;
	m_Radius		= Rad;

	m_pDrawShape	= new ShapeSolidLine;
}
CollideShapeLine::CollideShapeLine( float MinLen, float MaxLen, float Rad ) : CollideShape( COLLIDE_SHAPE_LINE )
{
	m_MinLength		= MinLen;
	m_MaxLength		= MaxLen;
	m_Radius		= Rad;

	m_pDrawShape	= new ShapeSolidLine;
}
CollideShapeLine::~CollideShapeLine()
{
	SafeDelete( m_pDrawShape );
}

//******************************************************************************
//	形状を描画する
//******************************************************************************
void CollideShapeLine::DrawShape( Color Col ) const
{
	if( m_pDrawShape )
	{
		m_pDrawShape->SetPhase( 10000 );
		m_pDrawShape->SetColor( Col );
		m_pDrawShape->Draw( m_Start, m_End, m_Radius );
	}
}

//------------------------------------------------------------------------------
//	形状キャスト：点
//------------------------------------------------------------------------------
bool CollideShapeLine::_ShapeCast( COLLIDE_RESULT& Result, const CollideShapePoint* pShape, dword CalcType, Point* pSolvePos )
{
	Point CastPoint = (CalcType & COLLIDE_CALC_SOLVE && pSolvePos)? *pSolvePos : pShape->GetPos();

	return CollideCalcPointToLine( Result, CastPoint, pShape->GetRadius(), 
									m_Start, m_End, GetLength(), m_Radius, GetNormal(),
									CalcType, pSolvePos );
}

//------------------------------------------------------------------------------
//	形状キャスト：線
//------------------------------------------------------------------------------
bool CollideShapeLine::_ShapeCast( COLLIDE_RESULT& Result, const CollideShapeLine* pShape, dword CalcType )
{
	if( m_Radius == 0.0f )
	{
		//	太さ０
		return CollideCalcLineToLine( Result, pShape, m_Start, m_End, GetLength(), GetNormal(), CalcType );
	}
	else
	{
		Point Side	= GetNormal() * m_Radius;

		//	表側
		if( Side.Dot( pShape->GetStart() - (m_Start + Side) ) > FLT_EPSILON )
		{
			if( CollideCalcLineToLine( Result, pShape, m_Start + Side, m_End + Side, GetLength(), GetNormal(), CalcType ) )
			{
				return true;
			}
		}
		//	裏側
		else if( (-Side).Dot( pShape->GetStart() - (m_Start - Side) ) > FLT_EPSILON )
		{
			if( CollideCalcLineToLine( Result, pShape, m_Start - Side, m_End - Side, GetLength(), GetNormal(), CalcType ) )
			{
				return true;
			}
		}
		//	内側
		else if( GetAxis().Dot( pShape->GetStart() - m_Start ) > FLT_EPSILON &&
				(-GetAxis()).Dot( pShape->GetStart() - m_End ) > FLT_EPSILON )
		{
			Result.Length = 0.0f;
			Result.HitPos = pShape->GetStart();
			Result.Normal = -pShape->GetAxis();

			return true;
		}

		if( m_Radius > FLT_EPSILON )
		{
			//	相手の始点・終点に線キャスト
			if( GetAxis().Dot( pShape->GetStart() - m_Start ) < FLT_EPSILON )
			{
				//	始点の方が近い
				if( CollideCalcLineToPoint( Result, pShape, m_Start, m_Radius, CalcType ) )
				{
					return true;
				}
				if( CollideCalcLineToPoint( Result, pShape, m_End, m_Radius, CalcType ) )
				{
					return true;
				}
			}
			else
			{
				//	終点の方が近い
				if( CollideCalcLineToPoint( Result, pShape, m_End, m_Radius, CalcType ) )
				{
					return true;
				}
				if( CollideCalcLineToPoint( Result, pShape, m_Start, m_Radius, CalcType ) )
				{
					return true;
				}
			}
		}
	}

	return false;
}

//------------------------------------------------------------------------------
//	位置の再計算
//------------------------------------------------------------------------------
void CollideShapeLine::_CalcPos( void )
{
	//	軸を取り出す
	Point Axis = GetAxis();

	m_Start	= m_Matrix.GetPos() + Axis * m_MinLength;
	m_End	= m_Matrix.GetPos() + Axis * m_MaxLength;

	//	AABBの再計算
	_CalcAabb();
}

//------------------------------------------------------------------------------
//	AABBの再計算
//------------------------------------------------------------------------------
void CollideShapeLine::_CalcAabb( void )
{
	_SetAabb( Rect( Point::Min( m_Start, m_End ) - m_Radius, Point::Max( m_Start, m_End ) + m_Radius ) );
}
