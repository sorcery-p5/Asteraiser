#pragma once

#include "CollideShape.h"

class SpriteFrame;


///////////////////////////////////////////////////////////////////////////////
//
//	当たり形状：スプライトフレーム
//
///////////////////////////////////////////////////////////////////////////////
class CollideShapeFrame : public CollideShape
{
public:
	CollideShapeFrame( void ) : CollideShape( COLLIDE_SHAPE_FRAME )							{ m_pSprite = NULL; }
	CollideShapeFrame( const SpriteFrame* pSprite ) : CollideShape( COLLIDE_SHAPE_FRAME )	{ Init( pSprite ); }
	virtual ~CollideShapeFrame()															{ DeleteMap(m_ShapeMap); }
	
	void				Init( const SpriteFrame* pSprite );
	virtual void		UpdateMatrix( const Matrix3* pMat = NULL );
	
	virtual bool		ShapeCast( COLLIDE_RESULT& Result, const CollideCaster* pCaster, Point* pSolvePos );
	virtual void		DrawShape( Color Col )																const;

private:
	typedef std::map<int, CollideShape*>		ShapeMap;

private:
	void				_CalcAabb( void );

private:
	ShapeMap			m_ShapeMap;
	const SpriteFrame*	m_pSprite;
};