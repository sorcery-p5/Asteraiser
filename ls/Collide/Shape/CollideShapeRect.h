#pragma once

#include "CollideShape.h"


///////////////////////////////////////////////////////////////////////////////
//
//	ìñÇΩÇËå`èÛÅFãÈå`
//
///////////////////////////////////////////////////////////////////////////////
class CollideShapeRect : public CollideShape
{
public:
	CollideShapeRect( void );
	CollideShapeRect( const Rect& Rect );
	CollideShapeRect( float w, float h );
	virtual ~CollideShapeRect();

	virtual void		UpdateMatrix( const Matrix3* pMat = NULL )						{ if( pMat ){ m_Matrix = *pMat; _CalcPoint(); } }

	virtual void		SetRect( const Rect& Rect )										{ m_Rect = Rect;	_CalcPoint(); }

	virtual Point		GetPos( void )											const	{ return m_Matrix.GetPos(); }
	const Rect&			GetRect( void )											const	{ return m_Rect; }

	virtual void		DrawShape( Color Col )									const;

protected:
	virtual bool		_ShapeCast( COLLIDE_RESULT& Result, const CollideShapePoint* pShape, dword CalcType, Point* pSolvePos );
	virtual bool		_ShapeCast( COLLIDE_RESULT& Result, const CollideShapeLine* pShape, dword CalcType );

private:
	enum
	{
		LT,
		RT,
		RB,
		LB,

		PT_NUM,
	};

private:
	void				_CalcPoint( void );
	void				_CalcAabb( void );

private:
	Rect				m_Rect;
	Matrix3				m_Matrix;
	Point				m_aPoint[PT_NUM];
	
	ShapePolygon*		m_pDrawShape;
};