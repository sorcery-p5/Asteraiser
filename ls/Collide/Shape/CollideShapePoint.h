#pragma once

#include "CollideShape.h"


///////////////////////////////////////////////////////////////////////////////
//
//	“–‚½‚èŒ`óF“_
//
///////////////////////////////////////////////////////////////////////////////
class CollideShapePoint : public CollideShape
{
public:
	CollideShapePoint( void );
	CollideShapePoint( float Radius );
	virtual ~CollideShapePoint();
	
	virtual void		UpdateMatrix( const Matrix3* pMat = NULL )					{ if(pMat){ m_Pos = pMat->GetPos(); _CalcAabb(); } }
	
	void				SetPos( const Point& Pos )									{ m_Pos = Pos;			_CalcAabb(); }
	virtual void		SetRadius( float Radius )									{ m_Radius = Radius;	_CalcAabb(); }

	virtual Point		GetPos( void )										const	{ return m_Pos; }
	float				GetRadius( void )									const	{ return m_Radius; }

	virtual void		DrawShape( Color Col )								const;

protected:
	virtual bool		_ShapeCast( COLLIDE_RESULT& Result, const CollideShapePoint* pShape, dword CalcType, Point* pSolvePos );
	virtual bool		_ShapeCast( COLLIDE_RESULT& Result, const CollideShapeLine* pShape, dword CalcType );

private:
	void				_CalcAabb( void )														{ _SetAabb( Rect(m_Pos.x - m_Radius, m_Pos.y - m_Radius, m_Pos.x + m_Radius, m_Pos.y + m_Radius) ); }

private:
	Point				m_Pos;
	float				m_Radius;
	
	ShapeCircle*		m_pDrawShape;
};