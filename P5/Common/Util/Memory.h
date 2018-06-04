#pragma once

namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	ÉÅÉÇÉä
//
////////////////////////////////////////////////////////////////////////////////
class Memory
{
public:
	Memory( void )													{ m_Size = 0; m_pData = NULL; }
	Memory( uint Size )												{ m_Size = 0; m_pData = NULL; Alloc( Size ); }
	Memory( void* pData, uint Size )								{ m_Size = 0; m_pData = NULL; Assign( pData, Size ); }
	~Memory()														{ Free(); }

	void			Alloc( uint Size )								{ Free(); m_Size = Size; m_pData = new byte[Size]; }
	void			Free( void )									{ m_Size = 0; SafeDeleteA( m_pData ); }
	void			Assign( void* pData, uint Size )				{ Alloc( Size ); CopyMemory( m_pData, pData, Size ); }

	void			Zero( void )									{ ZeroMemory( m_pData, m_Size ); }
	void			Write( void* pData, uint Size )					{ if(Size <= m_Size) CopyMemory( m_pData, pData, Size ); }
	void			Write( void* pData, uint Size, uint Offset )	{ if(Size + Offset <= m_Size) CopyMemory( m_pData + Offset, pData, Size ); }

	uint			GetSize( void ) 						const	{ return m_Size; }
	byte*			GetData( void )									{ return m_pData; }
	const byte*		GetData( void )							const	{ return m_pData; }
	
	template< typename TYPE >
	operator TYPE*( void )									const	{ return (TYPE*)m_pData; }
	
private:
	uint			m_Size;
	byte*			m_pData;
};


}