#pragma once

#include "AttackTypes.h"

class World;
class Attack;
class AttackData;

//	ÉxÉNÉ^
typedef std::vector<Attack*>		VecAttack;


////////////////////////////////////////////////////////////////////////////////
//
//	çUåÇä«óù
//
////////////////////////////////////////////////////////////////////////////////
class AttackManager
{
public:
	AttackManager( World* pWorld );
	~AttackManager();

	void				Update( void );
	VecAttack			AddAttack( const AttackData* pData, Matrix3 Mat );

	World*				GetWorld( void )						{ return m_pWorld; }
	Rand&				GetRand( void )							{ return m_Rand; }

private:
	typedef std::list< Attack* >		AttackList;

private:
	Attack*				_CreateAttack( const AttackData* pData, const Matrix3& Mat );
	Matrix3				_GetConcurrentMatrix( const ATTACK_CONCURRENT& Param, int Index, const Matrix3& Mat );

private:
	World*				m_pWorld;
	AttackList			m_AttackList;
	XORRand				m_Rand;
};