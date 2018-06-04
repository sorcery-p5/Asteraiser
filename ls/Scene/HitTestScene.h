#pragma once

#include "Scene.h"

class CollideObject;
class CollideCaster;


///////////////////////////////////////////////////////////////////////////////
//
//	当たりテスト用シーン
//
///////////////////////////////////////////////////////////////////////////////
class HitTestScene : public Scene
{
public:
	HitTestScene( Label Name );
	virtual ~HitTestScene();

	virtual void			OnInit( void );
	virtual void			Update( void );

private:
	CollideObject*			m_pCollidePoint;
	CollideObject*			m_pCollideLine;
	CollideObject*			m_pCollideRect;
	CollideObject*			m_pCollideMove;

	CollideCaster*			m_pCasterPoint;
	CollideCaster*			m_pCasterLine;
	ShapeCircle*			m_pShapeHit;
	ShapeCircle*			m_pShapeSolve;
	ShapeLine*				m_pShapeNormal;

	Point					m_DragPos;
	Angle					m_MoveRectAngle;
};