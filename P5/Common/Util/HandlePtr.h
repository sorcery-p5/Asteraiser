#pragma once


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	ファイルハンドル
//
///////////////////////////////////////////////////////////////////////////////
class FileHandle
{
public:
	FileHandle()			: fp(NULL)	{}
	FileHandle( FILE* p )	: fp(p)		{}
	FileHandle( pstr p, pstr f )		{ Open( p, f ); }
	~FileHandle()						{ Close(); }
	
	FILE* operator =( FILE* p )			{ Close(); fp = p; return fp; }
	operator FILE* ( void )		const	{ return fp; }
	operator bool  ( void )		const	{ return fp != NULL; }

	bool	Open( pstr p, pstr f )		{ fp = fopen( p, f ); return fp != NULL; }
	void	Close( void )				{ if(fp){ fclose(fp); } }

private:
	FILE*		fp;
};


///////////////////////////////////////////////////////////////////////////////
//
//	自動削除ポインタ
//
///////////////////////////////////////////////////////////////////////////////
template<typename T>
class DeletePtr
{
public:
	DeletePtr()			: p(NULL)		{}
	DeletePtr( T* r )	: p(r)			{}
	~DeletePtr()						{ Delete(); }
	
	T* operator =	( T* r )			{ Delete(); p = r; return p; }
	T**	operator &	( void )			{ return &p; }
	T* operator	->	( void )	const	{ return p; }
	operator T*		( void )	const	{ return p; }
	operator bool	( void )	const	{ return p != NULL; }

	void	Delete( void )				{ SafeDelete(p); }

private:
	T*			p;
};


///////////////////////////////////////////////////////////////////////////////
//
//	参照ポインタ
//
///////////////////////////////////////////////////////////////////////////////
template<typename T>
class ReleasePtr
{
public:
	ReleasePtr()		: p(NULL)		{}
	ReleasePtr( T* r )	: p(r)			{}
	~ReleasePtr()						{ Release(); }
	
	T* operator =	( T* r )			{ Release(); p = r; return p; }
	T**	operator &	( void )			{ return &p; }
	T* operator	->	( void )	const	{ return p; }
	operator T*		( void )	const	{ return p; }
	operator bool	( void )	const	{ return p != NULL; }

	void	Release( void )				{ SafeRelease(p); }

private:
	T*			p;
};

///////////////////////////////////////////////////////////////////////////////
//
//	MMIOハンドル
//
///////////////////////////////////////////////////////////////////////////////
class MmioHandle
{
public:
	MmioHandle()			: hMmio(NULL)	{}
	MmioHandle( HMMIO h )	: hMmio(h)		{}
	~MmioHandle()							{ if( hMmio ) mmioClose( hMmio, 0 ); }
	
	operator HMMIO ( void )			const	{ return hMmio; }
	operator bool  ( void )			const	{ return hMmio != NULL; }

private:
	HMMIO hMmio;
};


};