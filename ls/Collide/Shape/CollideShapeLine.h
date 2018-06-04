#pragma once

#include "CollideShape.h"


///////////////////////////////////////////////////////////////////////////////
//
//	ìñÇΩÇËå`èÛÅFíºê¸
//
///////////////////////////////////////////////////////////////////////////////
class CollideShapeLine : public CollideShape
{
public:
	CollideShapeLine( void );
	CollideShapeLine( float Len, float Rad );
	CollideShapeLine( float MinLen, float MaxLen, float Rad );
	virtual ~CollideShapeLine();
	
	virtual void		UpdateMatrix( const Matrix3* pMat = NULL )					{ if(pMat){ m_Matrix = *pMat; _CalcPos(); } }

	void				SetParam( float Len, float Rad )							{ m_MinLength = 0.0f; m_MaxLength = Len; m_Radius = Rad; _CalcPos(); }
	void				SetParam( float MinLen, float MaxLen, float Rad )			{ m_MinLength = MinLen; m_MaxLength = MaxLen; m_Radius = Rad; _CalcPos(); }

	virtual void		SetLength( float Len )										{ m_MinLength = 0.0f; m_MaxLength = Len; _CalcPos(); }
	virtual void		SetLength( float MinLen, float MaxLen )						{ m_MinLength = MinLen; m_MaxLength = MaxLen; _CalcPos(); }
	virtual void		SetRadius( float Rad )										{ m_Radius = Rad;	_CalcAabb(); }

	virtual Point		GetPos( void )										const	{ return m_Matrix.GetPos(); }
	const Point&		GetStart( void )									const	{ return m_Start; }
	const Point&		GetEnd( void )										const	{ return m_End; }
	float				GetLength( void )									const	{ return m_MaxLength - m_MinLength; }
	float				GetRadius( void )									const	{ return m_Radius; }
	Point				GetAxis( void )										const	{ return m_Matrix.GetAxisY(); }
	Point				GetNormal( void )									const	{ return m_Matrix.GetAxisY().RotMinus90(); }

	virtual void		DrawShape( Color Col )								const;

protected:
	virtual bool		_ShapeCast( COLLIDE_RESULT& Result, const CollideShapePoint* pShape, dword CalcType, Point* pSolvePos );
	virtual bool		_ShapeCast( COLLIDE_RESULT& Result, const CollideShapeLine* pShape, dword CalcType );

private:
	void				_CalcPos( void );
	void				_CalcAabb( void );

private:
	float				m_MinLength;
	float				m_MaxLength;
	float				m_Radius;

	Matrix3				m_Matrix;
	
	Point				m_Start;
	Point				m_End;

	ShapeSolidLine*		m_pDrawShape;
};