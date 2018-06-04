#include "stdafx.h"
#include "CollideShapeRect.h"
#include "CollideShapePoint.h"
#include "CollideShapeLine.h"

#include "../CollideFunction.h"


///////////////////////////////////////////////////////////////////////////////
//
//	������`��F��`
//
///////////////////////////////////////////////////////////////////////////////
CollideShapeRect::CollideShapeRect( void ) : CollideShape( COLLIDE_SHAPE_RECT )
{
	m_pDrawShape	= new ShapePolygon( 4 );
}
CollideShapeRect::CollideShapeRect( const Rect& Rect ) : CollideShape( COLLIDE_SHAPE_RECT )
{
	m_Rect = Rect;
	
	m_pDrawShape	= new ShapePolygon( 4 );
}
CollideShapeRect::CollideShapeRect( float w, float h ) : CollideShape( COLLIDE_SHAPE_RECT )
{
	m_Rect.Set( -w/2, -h/2, w/2, h/2 );
	
	m_pDrawShape	= new ShapePolygon( 4 );
}
CollideShapeRect::~CollideShapeRect()
{
	SafeDelete( m_pDrawShape );
}

//******************************************************************************
//	�`���`�悷��
//******************************************************************************
void CollideShapeRect::DrawShape( Color Col ) const
{
	if( m_pDrawShape )
	{
		m_pDrawShape->SetPhase( 10000 );
		m_pDrawShape->SetColor( Col );
		m_pDrawShape->Draw( m_aPoint );
	}
}

//------------------------------------------------------------------------------
//	�`��L���X�g�F�_
//------------------------------------------------------------------------------
bool CollideShapeRect::_ShapeCast( COLLIDE_RESULT& Result, const CollideShapePoint* pShape, dword CalcType, Point* pSolvePos )
{
	Point CastPoint = (CalcType & COLLIDE_CALC_SOLVE && pSolvePos)? *pSolvePos : pShape->GetPos();

	Point DirX = m_aPoint[RT] - m_aPoint[LT];
	Point DirY = m_aPoint[LB] - m_aPoint[LT];

	float DotX0	= DirX.Dot(		CastPoint - m_aPoint[LT] );
	float DotX1	= (-DirX).Dot(	CastPoint - m_aPoint[RT] );
	float DotY0	= DirY.Dot(		CastPoint - m_aPoint[LT] );
	float DotY1	= (-DirY).Dot(	CastPoint - m_aPoint[LB] );

	if( DotY0 < -FLT_EPSILON )
	{
		//	����
		if( DotX0 < -FLT_EPSILON )
		{
			return CollideCalcPointToPoint( Result, CastPoint, pShape->GetRadius(), m_aPoint[LT], 0.0f, CalcType, pSolvePos );
		}
		//	�E��
		else if( DotX1 < -FLT_EPSILON )
		{
			return CollideCalcPointToPoint( Result, CastPoint, pShape->GetRadius(), m_aPoint[RT], 0.0f, CalcType, pSolvePos );
		}
		//	��
		else
		{
			return CollideCalcPointToLine( Result, CastPoint, pShape->GetRadius(), 
											m_aPoint[LT], m_aPoint[RT], m_Rect.Width(), 0.0f, DirX.RotMinus90() / m_Rect.Width(), CalcType, pSolvePos );
		}
	}
	else if( DotY1 < -FLT_EPSILON )
	{
		//	����
		if( DotX0 < -FLT_EPSILON )
		{
			return CollideCalcPointToPoint( Result, CastPoint, pShape->GetRadius(), m_aPoint[LB], 0.0f, CalcType, pSolvePos );
		}
		//	�E��
		else if( DotX1 < -FLT_EPSILON )
		{
			return CollideCalcPointToPoint( Result, CastPoint, pShape->GetRadius(), m_aPoint[RB], 0.0f, CalcType, pSolvePos );
		}
		//	��
		else
		{
			return CollideCalcPointToLine( Result, CastPoint, pShape->GetRadius(), 
											m_aPoint[RB], m_aPoint[LB], m_Rect.Width(), 0.0f, DirX.Rot90() / m_Rect.Width(), CalcType, pSolvePos );
		}
	}
	else
	{
		//	����
		if( DotX0 < -FLT_EPSILON )
		{
			return CollideCalcPointToLine( Result, CastPoint, pShape->GetRadius(), 
											m_aPoint[LB], m_aPoint[LT], m_Rect.Height(), 0.0f, DirY.Rot90() / m_Rect.Height(), CalcType, pSolvePos );
		}
		//	���E
		else if( DotX1 < -FLT_EPSILON )
		{
			return CollideCalcPointToLine( Result, CastPoint, pShape->GetRadius(), 
											m_aPoint[RT], m_aPoint[RB], m_Rect.Height(), 0.0f, DirY.RotMinus90() / m_Rect.Height(), CalcType, pSolvePos );
		}
		//	��
		else
		{
			Result.Length = 0.0f;
			Result.HitPos = CastPoint;

			//	��
			if( DotX0 < DotX1 && DotX0 < DotY0 && DotX0 < DotY1 )
			{
				Result.Normal = -DirX / m_Rect.Width();
				if( pSolvePos ) *pSolvePos = CastPoint + Result.Normal * (DotX0/m_Rect.Width() + pShape->GetRadius());
			}
			//	�E
			else if( DotX1 < DotY0 && DotX1 < DotY1 )
			{
				Result.Normal = DirX / m_Rect.Width();
				if( pSolvePos ) *pSolvePos = CastPoint + Result.Normal * (DotX1/m_Rect.Width() + pShape->GetRadius());
			}
			//	��
			else if( DotY0 < DotY1 )
			{
				Result.Normal = -DirY / m_Rect.Height();
				if( pSolvePos ) *pSolvePos = CastPoint + Result.Normal * (DotY0/m_Rect.Height() + pShape->GetRadius());
			}
			//	��
			else
			{
				Result.Normal = DirY / m_Rect.Height();
				if( pSolvePos ) *pSolvePos = CastPoint + Result.Normal * (DotY1/m_Rect.Height() + pShape->GetRadius());
			}

			return true;
		}
	}
}

//------------------------------------------------------------------------------
//	�`��L���X�g�F��
//------------------------------------------------------------------------------
bool CollideShapeRect::_ShapeCast( COLLIDE_RESULT& Result, const CollideShapeLine* pShape, dword CalcType )
{
	Point DirX = m_aPoint[RT] - m_aPoint[LT];
	Point DirY = m_aPoint[LB] - m_aPoint[LT];

	float DotX0	= DirX.Dot(		pShape->GetStart() - m_aPoint[LT] );
	float DotX1	= (-DirX).Dot(	pShape->GetStart() - m_aPoint[RT] );
	float DotY0	= DirY.Dot(		pShape->GetStart() - m_aPoint[LT] );
	float DotY1	= (-DirY).Dot(	pShape->GetStart() - m_aPoint[LB] );

	Point* pSt[2][2]	= { NULL };
	float Len[2]		= { 0.0f };
	Point Normal[2];

	if( DotY0 < FLT_EPSILON )
	{
		//	����
		if( DotX0 < FLT_EPSILON )
		{
			pSt[0][0] = &m_aPoint[LT];		pSt[0][1] = &m_aPoint[RT];	Len[0] = m_Rect.Width();	Normal[0] = DirX.RotMinus90() / m_Rect.Width();
			pSt[1][0] = &m_aPoint[LB];		pSt[1][1] = &m_aPoint[LT];	Len[1] = m_Rect.Height();	Normal[1] = DirY.Rot90() / m_Rect.Height();
		}
		//	�E��
		else if( DotX1 < FLT_EPSILON )
		{
			pSt[0][0] = &m_aPoint[LT];		pSt[0][1] = &m_aPoint[RT];	Len[0] = m_Rect.Width();	Normal[0] = DirX.RotMinus90() / m_Rect.Width();
			pSt[1][0] = &m_aPoint[RT];		pSt[1][1] = &m_aPoint[RB];	Len[1] = m_Rect.Height();	Normal[1] = DirY.RotMinus90() / m_Rect.Height();
		}
		//	��
		else
		{
			pSt[0][0] = &m_aPoint[LT];		pSt[0][1] = &m_aPoint[RT];	Len[0] = m_Rect.Width();	Normal[0] = DirX.RotMinus90() / m_Rect.Width();
		}
	}
	else if( DotY1 < FLT_EPSILON )
	{
		//	����
		if( DotX0 < FLT_EPSILON )
		{
			pSt[0][0] = &m_aPoint[RB];		pSt[0][1] = &m_aPoint[LB];	Len[0] = m_Rect.Width();	Normal[0] = DirX.Rot90() / m_Rect.Width();
			pSt[1][0] = &m_aPoint[LB];		pSt[1][1] = &m_aPoint[LT];	Len[1] = m_Rect.Height();	Normal[1] = DirY.Rot90() / m_Rect.Height();
		}
		//	�E��
		else if( DotX1 < FLT_EPSILON )
		{
			pSt[0][0] = &m_aPoint[RB];		pSt[0][1] = &m_aPoint[LB];	Len[0] = m_Rect.Width();	Normal[0] = DirX.Rot90() / m_Rect.Width();
			pSt[1][0] = &m_aPoint[RT];		pSt[1][1] = &m_aPoint[RB];	Len[1] = m_Rect.Height();	Normal[1] = DirY.RotMinus90() / m_Rect.Height();
		}
		//	��
		else
		{
			pSt[0][0] = &m_aPoint[RB];		pSt[0][1] = &m_aPoint[LB];	Len[0] = m_Rect.Width();	Normal[0] = DirX.Rot90() / m_Rect.Width();
		}
	}
	else
	{
		//	����
		if( DotX0 < FLT_EPSILON )
		{
			pSt[0][0] = &m_aPoint[LB];		pSt[0][1] = &m_aPoint[LT];	Len[0] = m_Rect.Height();	Normal[0] = DirY.Rot90() / m_Rect.Height();
		}
		//	���E
		else if( DotX1 < FLT_EPSILON )
		{
			pSt[0][0] = &m_aPoint[RT];		pSt[0][1] = &m_aPoint[RB];	Len[0] = m_Rect.Height();	Normal[0] = DirY.RotMinus90() / m_Rect.Height();
		}
		//	��
		else
		{
			Result.Length = 0.0f;
			Result.HitPos = pShape->GetStart();
			Result.Normal = -pShape->GetAxis();

			return true;
		}
	}

	//	����
	{
		if( CollideCalcLineToLine( Result, pShape, *pSt[0][0], *pSt[0][1], Len[0], Normal[0], CalcType, true ) )
		{
			return true;
		}
	}
	
	if( pSt[1][0] )
	{
		if( CollideCalcLineToLine( Result, pShape, *pSt[1][0], *pSt[1][1], Len[1], Normal[1], CalcType, true ) )
		{
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------
//	�ʒu�̍Čv�Z
//------------------------------------------------------------------------------
void CollideShapeRect::_CalcPoint( void )
{
	m_aPoint[LT] = m_Matrix.Transform( m_Rect.LT() );
	m_aPoint[RT] = m_Matrix.Transform( m_Rect.RT() );
	m_aPoint[LB] = m_Matrix.Transform( m_Rect.LB() );
	m_aPoint[RB] = m_Matrix.Transform( m_Rect.RB() );

	_CalcAabb();
}

//------------------------------------------------------------------------------
//	AABB�̍Čv�Z
//------------------------------------------------------------------------------
void CollideShapeRect::_CalcAabb( void )
{
	Point MinPt = Point::Min( Point::Min( m_aPoint[LT], m_aPoint[RT] ), Point::Min( m_aPoint[LB], m_aPoint[RB] ) );
	Point MaxPt = Point::Max( Point::Max( m_aPoint[LT], m_aPoint[RT] ), Point::Max( m_aPoint[LB], m_aPoint[RB] ) );
		
	_SetAabb( Rect( MinPt, MaxPt ) );
}