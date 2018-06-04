#pragma once

#include "ResourceTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	リソース(基底)
//
////////////////////////////////////////////////////////////////////////////////
class Resource
{
public:
	Resource( void )										{ m_RefCount = 0; m_Type = RES_NULL; }
	virtual ~Resource()										{}

	virtual bool			Load( pstr pPath )				= 0;
	virtual void			Update( void )					{}

	virtual void			Reset( void )					{}
	virtual void			Restore( void )					{}

	void					AddRef( void )					{ m_RefCount++; }
	void					Release( void );
	int						GetRefCount( void )		const	{ return m_RefCount; }

	void					SetName( Label Name )			{ m_Name = Name; }
	const Label&			GetName( void )			const	{ return m_Name; }

	void					SetType( RES_TYPE Type )		{ m_Type = Type; }
	RES_TYPE				GetType( void )			const	{ return m_Type; }

private:
	int						m_RefCount;
	Label					m_Name;
	RES_TYPE				m_Type;
};

////////////////////////////////////////////////////////////////////////////////
//
//	リソーステンプレート
//
////////////////////////////////////////////////////////////////////////////////
template< typename T, RES_TYPE Type >
class ResourceTemplate : public Resource
{
public:
	static const RES_TYPE	TYPE = Type;

public:
	ResourceTemplate( void )								{ m_pData = NULL; }
	virtual ~ResourceTemplate()								{ SafeDelete( m_pData ); }

	virtual bool			Load( pstr pPath )
	{
		m_pData = new T;
		MEM_CHECK( m_pData );
		
		return m_pData->Load( pPath );
	}

	const T*				GetData( void )			const	{ return m_pData; }

protected:
	T*						m_pData;
};