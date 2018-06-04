#pragma once


///////////////////////////////////////////////////////////////////////////////
//
//	参照ポインタ
//
///////////////////////////////////////////////////////////////////////////////
template< typename T >
class RefPtr
{
public:
	RefPtr( void )							{ m_pPtr = NULL; }
	RefPtr( T* p )							{ m_pPtr = NULL; Set( p ); }
	RefPtr( const RefPtr<T>& p )			{ m_pPtr = NULL; Set( p.m_pPtr ); }
	~RefPtr()								{ Delete(); }

	void Set( T* p )						{ Delete(); if( p ){ p->GET_REF_PTR_VEC().Add( this ); m_pPtr = p; } }
	void Delete( void )						{ if( m_pPtr ){ m_pPtr->GET_REF_PTR_VEC().Erase( this ); m_pPtr = NULL; } }
	void Release( void )					{ m_pPtr = NULL; }

	T* operator =	( T* p )				{ Set( p ); return m_pPtr; }
	T* operator =	( const RefPtr<T>& p )	{ Set( p.m_pPtr ); return m_pPtr; }

	T**	operator &		( void )			{ return &m_pPtr; }
	const T& operator *	( void )	const	{ return *m_pPtr; }
	T&	operator *		( void )			{ return *m_pPtr; }
	const T* operator ->( void )	const	{ return m_pPtr; }
	T* operator	->		( void )			{ return m_pPtr; }
	operator const T*	( void )	const	{ return m_pPtr; }
	operator T*			( void )			{ return m_pPtr; }

	bool operator !		( void )	const	{ return m_pPtr == NULL; }
	operator bool		( void )	const	{ return m_pPtr != NULL; }

private:
	T*		m_pPtr;
};


///////////////////////////////////////////////////////////////////////////////
//
//	参照ポインタの参照元格納
//
///////////////////////////////////////////////////////////////////////////////
template< typename T >
class RefPtrVec
{
public:
	~RefPtrVec()						{ ReleaseVec(m_PtrList); }

	void Add( RefPtr<T>* p )			{ m_PtrList.push_back(p); }
	void Erase( RefPtr<T>* p )
	{
		std::list< RefPtr<T>*	>::iterator it = StdFind( m_PtrList, p );
		if( it != m_PtrList.end() )	m_PtrList.erase( it );
	}
	bool	IsEmpty( void )		const	{ return m_PtrList.empty(); }
	int		Size( void )		const	{ return (int)m_PtrList.size(); }
	
private:
	std::list< RefPtr<T>* >				m_PtrList;
};


///////////////////////////////////////////////////////////////////////////////
//
//	被参照クラスのインプリメント
//
///////////////////////////////////////////////////////////////////////////////
#define REFERENCE_IMPL( THIS )	\
	public:	\
		RefPtrVec<THIS>&	GET_REF_PTR_VEC()			{ return VEC_REF_PTR; }\
		bool				IS_REFERENCE()		const	{ return !VEC_REF_PTR.IsEmpty(); }\
		int					GET_REFERENCE_NUM()	const	{ return VEC_REF_PTR.Size(); }\
	private: \
		RefPtrVec<THIS>		VEC_REF_PTR;