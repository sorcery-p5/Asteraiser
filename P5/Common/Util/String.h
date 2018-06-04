#pragma once


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	������
//
///////////////////////////////////////////////////////////////////////////////
template <uint SIZE> 
class String
{
public:
	String( void )										{ strcpy( m_Str, "" ); }
	String( const String& s )							{ strncpy(m_Str, s.m_Str, SIZE); }
	String( pstr p )									{ strcpy( m_Str, p ); }

	String  operator +( pstr pStr ) 			const	{ String result( m_Str ); result += pStr; return result; }
	String& operator =( pstr pStr )						{ strncpy(m_Str, pStr, SIZE); return *this; }	
	String& operator +=( pstr pStr )					{ strncat(m_Str, pStr, SIZE); return *this; }

	bool operator ==( pstr pStr ) 				const	{ return strcmp(m_Str, pStr) == 0;	}
	bool operator !=( pstr pStr ) 				const	{ return strcmp(m_Str, pStr) != 0;	}
	bool operator < ( pstr pStr ) 				const	{ return strcmp(m_Str, pStr) < 0;	}

	operator pstr ( void )						const	{ return m_Str; }

	void	Format( pstr p, ... )						{ vsprintf( m_Str, p, (char*)(&p + 1) ); }
	void	FormatV( pstr p, char* v )					{ vsprintf( m_Str, p, v ); }
	void	Replace( pstr pTarget, pstr pNew );
	void	Insert( int Index, pstr pInsStr );
	void	Delete( int Index, int Num );

	int		ToInt( void )						const	{ return atoi( m_Str ); }
	float	ToFloat( void )						const	{ return atof( m_Str ); }

	uint	Length( void )						const	{ return strlen( m_Str ); }
	uint	Size( void )						const	{ return SIZE; }
	char*	Str( void )									{ return m_Str; }
	pstr	Str( void )							const	{ return m_Str; }
	char*	c_str( void )								{ return m_Str; }
	pstr	c_str( void )						const	{ return m_Str; }
	bool	Comp( pstr pStr ) 					const	{ return strcmp(m_Str, pStr) == 0;	}
	bool	IComp( pstr pStr ) 					const	{ return stricmp(m_Str, pStr) == 0;	}
	char*	Find( pstr s )								{ return strstr( m_Str, s ); }
	char*	FindChar( char c )							{ return strchr( m_Str, c ); }
	char*	FindCharR( char c )							{ return strrchr( m_Str, c ); }

private:
	char	m_Str[SIZE];
};

typedef String<1024>	String1024;
typedef String<256>		String256;
typedef String<32>		String32;


//------------------------------------------------------------------------------
//	�������S�p���ǂ������ׂ�
//------------------------------------------------------------------------------
inline bool IsEmQuad( byte Char )
{
	return ( Char >= 0x81 && Char <= 0x9F || Char >= 0xE0 && Char <= 0xFF );
}

//------------------------------------------------------------------------------
//	�񕶎����ǂ������ׂ�
//------------------------------------------------------------------------------
inline bool IsNotLetter( char Char )
{
	return Char == '\0' || Char == '\n' || Char == '\r' || Char == '\t';
}

//-----------------------------------------------------------------------------
//	�t�@�C���p�X����f�B���N�g���𓾂�
//-----------------------------------------------------------------------------
template <uint SIZE>
String<SIZE> GetFileDirectory( pstr Path )
{
	String<SIZE> Result( Path );

	char* p;

	//	�t�H���_�p�X�����邩���ׂ�
	p = Result.FindCharR('\\');
	if( p )	 *p = '\0';

	p = Result.FindCharR('/');
	if( p )	*p = '\0';

	return Result;
}

//-----------------------------------------------------------------------------
//	�t�@�C�����𓾂�
//-----------------------------------------------------------------------------
template <uint SIZE>
String<SIZE> GetFileName( pstr Path )
{
	String<SIZE> Result( Path );

	char* p;

	//	�t�H���_�p�X�����邩���ׂ�
	p = Result.FindCharR('\\');
	if( p )	Result = (p + 1);

	p = Result.FindCharR('/');
	if( p )	Result = (p + 1);

	return Result;
}

//-----------------------------------------------------------------------------
//	�t�@�C���̃f�B���N�g�����𓾂�
//-----------------------------------------------------------------------------
template <uint SIZE>
String<SIZE> GetFileDirName( pstr Path )
{
	return GetFileName<SIZE>( GetFileDirectory<SIZE>(Path).c_str() );
}

//-----------------------------------------------------------------------------
//	�t�@�C�����𓾂�
//-----------------------------------------------------------------------------
template <uint SIZE>
String<SIZE> GetFileTitle( pstr Path )
{
	String<SIZE> Result( Path );

	char* p;

	//	�t�H���_�p�X�����邩���ׂ�
	p = Result.FindCharR('\\');
	if( p )	Result = (p + 1);

	p = Result.FindCharR('/');
	if( p )	Result = (p + 1);

	//	�s���I�h�����邩���ׂ�
	p = Result.FindCharR( '.' );
	if( p )	 *p = '\0';

	return Result;
}

//-----------------------------------------------------------------------------
//	�t�@�C���g���q�𓾂�
//-----------------------------------------------------------------------------
template <uint SIZE>
String<SIZE> GetFileExt( pstr Path )
{
	String<SIZE> Result( Path );

	//	�s���I�h�����邩���ׂ�
	char* p = Result.FindCharR( '.' );
	if( p )
	{
		return p + 1;
	}

	return "";
}

//******************************************************************************
//	������̍����ւ�
//******************************************************************************
template <uint SIZE> 
void String<SIZE>::Replace( pstr pTarget, pstr pNew )
{
	if( !pTarget || !pNew || *pTarget == '\0' ) return;

	size_t	TargetLen	= strlen( pTarget );
	size_t	StrLen		= strlen( pNew );
	pstr	pDst	= m_Str;
	pstr	pFind	= NULL;
	
	do
	{
		pFind = strstr( pDst, pTarget );
		if( pFind == 0 ) break;
	
		int Index = pFind - m_Str;
		
		Delete( Index, TargetLen );
		Insert( Index, pNew );
		pDst = m_Str + Index + StrLen;

		if ( pDst - m_Str >= SIZE ) break;
	}
	while( *pDst != '\0' );
}

//******************************************************************************
//	������̑}��
//******************************************************************************
template <uint SIZE> 
void String<SIZE>::Insert( int Index, pstr pInsStr )
{
	if( Index < 0 )	return;
	
	char*	pBuffEnd	= m_Str + SIZE-1;
	int		OrgLen		= Length();
	int		InsLen		= ( pInsStr == 0 ) ? 1 : (int)strlen( pInsStr );
	
	if( Index >= OrgLen ) 
	{
		(*this) += pInsStr;
		return;
	}
	
	if( Index + InsLen >= SIZE - 1 ) InsLen = SIZE - Index-1;
	
	{
		pstr	pSrcTop		= m_Str + Index;
		char*	pSrcTail	= m_Str + OrgLen;	
		char*	pDst		= m_Str + OrgLen + InsLen;
		pstr	pSrc;
		
		if( pDst > pBuffEnd )
		{
			int	CutLen	= pDst-pBuffEnd;
			
			pDst		-= CutLen;
			pSrcTail	-= CutLen;
			*pSrcTail	= '\0';
		}
		_ASSERT( *pSrcTail == '\0' );
		
		for( pSrc = pSrcTail; pSrc >= pSrcTop; --pSrc,--pDst)
		{
			*pDst = *pSrc;
		}
	}

	{
		char*	pDst	= m_Str + Index;
		pstr	pSrc	= pInsStr;
		int			i;
		if( pInsStr )
		{
		    for( i = 0; i < InsLen; ++i )
		    {
			    *pDst++ = *pSrc++;
		    }
		}
		else
		{
		    *pDst++ = '\0';
		}
	}
}

//******************************************************************************
//	������̍폜
//******************************************************************************
template <uint SIZE> 
void String<SIZE>::Delete( int Index, int Num)
{
	int		Len			= Length();
	char*	pDst		= m_Str + Clamp( 0, Index, Len);
	pstr	pSrc		= m_Str + Clamp( 0, Index + Num, Len);
	pstr	pSrcEnd		= m_Str + Len;
	
	while( pSrc <= pSrcEnd )
	{
		*pDst++ = *pSrc++;
	}
}

}