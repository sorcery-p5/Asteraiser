#pragma once

#include "CollideTypes.h"

class CollideShape;
class CollideObjectListener;


///////////////////////////////////////////////////////////////////////////////
//
//	当たりオブジェクト
//
///////////////////////////////////////////////////////////////////////////////
class CollideObject
{
public:
	CollideObject( void );
	virtual	~CollideObject();

	void					Init( CollideShape* pShape, uint Layer );
	void					Update( void );
	
	void					UpdateMatrix( const Matrix3* pMat = NULL );

	void					SetOwner( COLLIDE_OWNER_TYPE Type, void* pData )				{ m_Owner.Type = Type; m_Owner.pData = pData; }
	void					SetHitEnable( bool bEnable )									{ m_bHitEnable = bEnable; }
	void					SetSolveEnable( bool bEnable )									{ m_bSolveEnable = bEnable; }
	void					SetListener( CollideObjectListener* pListener )					{ m_pListener = pListener; }

	void					SetCastHit( bool bHit )											{ m_bCastHit = bHit; }

	CollideShape*			GetShape( void )												{ return m_pShape; }
	const CollideShape*		GetShape( void )										const	{ return m_pShape; }
	const COLLIDE_OWNER&	GetOwner( void )										const	{ return m_Owner; }
	bool					IsHitEnable( void )										const	{ return m_bHitEnable; }
	bool					IsSolveEnable( void )									const	{ return m_bSolveEnable; }
	uint					GetHitLayer( void )										const	{ return m_Layer; }
	CollideObjectListener*	GetListener( void )										const	{ return m_pListener; }

private:
	bool					m_bHitEnable;
	bool					m_bSolveEnable;
	uint					m_Layer;
	bool					m_bCastHit;
	CollideShape*			m_pShape;
	COLLIDE_OWNER			m_Owner;
	CollideObjectListener*	m_pListener;
};
