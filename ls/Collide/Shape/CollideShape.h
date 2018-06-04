#pragma once

#include "../CollideTypes.h"

class CollideCaster;
class CollideShapePoint;
class CollideShapeLine;


///////////////////////////////////////////////////////////////////////////////
//
//	ìñÇΩÇËå`èÛ(äÓíÍ)
//
///////////////////////////////////////////////////////////////////////////////
class CollideShape
{
public:
	CollideShape( COLLIDE_SHAPE_TYPE Type )														{ m_Type = Type; }
	virtual ~CollideShape()																		{}
	
	virtual void		UpdateMatrix( const Matrix3* pMat = NULL )								= 0;

	virtual Point		GetPos( void )													const	{ return Point(); }
	const Rect&			GetAabb( void )													const	{ return m_Aabb; }
	COLLIDE_SHAPE_TYPE	GetType( void )													const	{ return m_Type; }

	virtual void		SetRadius( float Rad )													{}
	virtual void		SetLength( float Len )													{}
	virtual void		SetLength( float MinLen, float MaxLen )									{}
	virtual void		SetRect( const Rect& Rc )												{}

	virtual bool		ShapeCast( COLLIDE_RESULT& Result, const CollideCaster* pCaster, Point* pSolvePos );

	virtual void		DrawShape( Color Col )											const	= 0;

protected:
	void				_SetAabb( const Rect& Aabb )											{ m_Aabb = Aabb; }
	
	virtual bool		_ShapeCast( COLLIDE_RESULT& Result, const CollideShapePoint* pShape, dword CalcType, Point* pSolvePos )		{ return true; }
	virtual bool		_ShapeCast( COLLIDE_RESULT& Result, const CollideShapeLine* pShape, dword CalcType )						{ return true; }

private:
	COLLIDE_SHAPE_TYPE	m_Type;
	Rect				m_Aabb;
};