#pragma once

#include "CollideTypes.h"

class CollideShape;
class CollideObjectListener;


///////////////////////////////////////////////////////////////////////////////
//
//	“–‚½‚èƒLƒƒƒXƒg
//
///////////////////////////////////////////////////////////////////////////////
class CollideCaster
{
public:
	CollideCaster( void );
	~CollideCaster();
	
	void						Init( CollideShape* pShape );
	bool						Cast( COLLIDE_CAST_RESULT& Resilt )						const;

	void						UpdateMatrix( const Matrix3* pMat );
	void						SetOwner( COLLIDE_OWNER_TYPE Type, void* pData )				{ m_Owner.Type = Type; m_Owner.pData = pData; }

	void						SetFilter( dword Filter )										{ m_Filter = Filter; }
	void						SetCalcType( dword Type )										{ m_CalcType = Type; }
	void						AddCalcType( COLLIDE_CALC_TYPE Type )							{ m_CalcType |= Type; }
	void						RemoveCalcType( COLLIDE_CALC_TYPE Type )						{ m_CalcType &= ~Type; }
	void						SetListener( CollideObjectListener* pListener )					{ m_pListener = pListener; }
	void						SetHitEnable( bool bEnable )									{ m_bHitEnable = bEnable; }

	CollideShape*				GetShape( void )												{ return m_pShape; }
	const CollideShape*			GetShape( void )										const	{ return m_pShape; }
	const COLLIDE_OWNER&		GetOwner( void )										const	{ return m_Owner; }
	dword						GetFilter( void )										const	{ return m_Filter; }
	dword						GetCalcType( void )										const	{ return m_CalcType; }
	CollideObjectListener*		GetListener( void )										const	{ return m_pListener; }
	bool						IsHitEnable( void )										const	{ return m_bHitEnable; }

private:
	CollideShape*				m_pShape;
	COLLIDE_OWNER				m_Owner;
	dword						m_Filter;
	dword						m_CalcType;
	bool						m_bHitEnable;
	CollideObjectListener*		m_pListener;
};