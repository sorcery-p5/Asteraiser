#pragma once

#include "CollideTypes.h"

class CollideObject;
class CollideShape;
class CollideCaster;


///////////////////////////////////////////////////////////////////////////////
//
//	当たりシステム
//
///////////////////////////////////////////////////////////////////////////////
class CollideSystem
{
public:
	CollideSystem( void );
	~CollideSystem();

	void				Update( void );

	void				AddObject( CollideObject* pObject );
	void				RemoveObject( CollideObject* pObject );

	bool				ShapeCast( COLLIDE_CAST_RESULT& CastResult, const CollideCaster& Caster );
	
	void				SetDrawShapeEnable( bool bEnable )								{ m_bDrawShape = bEnable; }
	bool				IsDrawShape( void )										const	{ return m_bDrawShape; }
	Color				GetDrawShapeColor( bool bCast, bool bHit, uint Layer )	const;

private:
	typedef std::list< CollideObject* >	ObjectList;

private:
	ObjectList			m_aObjectList[COLLIDE_LAYER_NUM];

	bool				m_bDrawShape;
};

//------------------------------------------------------------------------------
//	インスタンス
//------------------------------------------------------------------------------
void						InitCollideSystem( void );
void						ExitCollideSystem( void );
CollideSystem*				GetCollideSystem( void );


#include "CollideObject.h"
#include "CollideObjectListener.h"
#include "CollideCaster.h"
#include "Shape/CollideShapePoint.h"
#include "Shape/CollideShapeLine.h"
#include "Shape/CollideShapeRect.h"
#include "Shape/CollideShapeFrame.h"