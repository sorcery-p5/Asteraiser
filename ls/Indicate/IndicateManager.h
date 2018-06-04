#pragma once

#include "IndicateTypes.h"

class Indicate;
class IndicateData;
class World;


////////////////////////////////////////////////////////////////////////////////
//
//	インジケート管理
//
////////////////////////////////////////////////////////////////////////////////
class IndicateManager
{
public:
	IndicateManager( World* pWorld );
	~IndicateManager();

	void				Update( void );
	void				Draw( void );

	void				AddIndicate( Label Name, const IndicateData* pData );
	void				DeleteIndicate( Label Name );

	Indicate*			GetIndicate( Label Name )		const;

	World*				GetWorld( void )						{ return m_pWorld; }

private:
	typedef std::map< Label, Indicate* >		MapIndicate;

private:
	World*				m_pWorld;
	MapIndicate			m_mapIndicate;
};