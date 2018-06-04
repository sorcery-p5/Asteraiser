#pragma once

#include "GroundTypes.h"

class World;
class Ground;
class GroundData;


////////////////////////////////////////////////////////////////////////////////
//
//	ínå`ä«óù
//
////////////////////////////////////////////////////////////////////////////////
class GroundManager
{
public:
	GroundManager( World* pWorld );
	~GroundManager();

	void			Update( void );
	void			Draw( void );
	
	void			AddGround( const GroundData* pData, const Matrix3& Mat, Label Name, bool bNoStart );
	Ground*			GetGround( Label Name )			const;

	void			EndGround( Label Name );
	void			EraseGround( Label Name );
	void			EraseAllGround( void );

	World*			GetWorld( void )						{ return m_pWorld; }

private:
	typedef std::list< Ground* >	GroundList;

private:
	World*			m_pWorld;
	GroundList		m_GroundList;
};