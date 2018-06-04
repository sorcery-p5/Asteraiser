#pragma once

#include "../PlayerTypes.h"
#include "Collide/CollideTypes.h"

class Player;
class Effect;
class CollideObject;
class CollideCaster;


////////////////////////////////////////////////////////////////////////////////
//
//	オーヴァドライヴ・スピア
//
////////////////////////////////////////////////////////////////////////////////
class Spear
{
public:
	Spear( const Player* pParent );
	~Spear();

	void					Init( const SPEAR_PARAM* pParam, const Matrix3& Mat, float AddEnergy );
	void					Update( void );

	bool					OnHitAttack( float Damage, Point HitPos );
	void					Crash( void );
	
	void					Delete( void )											{ m_bDelete = true; }

	bool					IsDelete( void )								const	{ return m_bDelete; }
	float					GetAttackPower( void )							const	{ return m_Attack; }
	const Matrix3&			GetMatrix( void )								const	{ return m_Matrix; }

private:
	bool					_OnCastHit( const COLLIDE_RESULT& Result );
	void					_Exhaust( float Damage, Point HitPos, bool bHitEffect );
	void					_UpdateMatrix( void );
	Matrix3					_GetHitMatrix( Point HitPos );

private:
	const Player*			m_pParent;
	const SPEAR_PARAM*		m_pParam;

	CollideObject*			m_pCollide;
	CollideCaster*			m_pCaster;
	RefPtr<Effect>			m_pEffect;

	bool					m_bDelete;
	Matrix3					m_Matrix;
	int						m_FrameCount;
	float					m_Energy;
	float					m_Attack;
};