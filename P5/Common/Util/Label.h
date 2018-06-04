#pragma once


namespace P5
{

typedef char	LabelStr[8];

///////////////////////////////////////////////////////////////////////////////
//
//	ƒ‰ƒxƒ‹
//
///////////////////////////////////////////////////////////////////////////////
class Label
{
public:
	Label( void )									{ value = 0; }
	Label( qword number )							{ value = number; }
	Label( pstr str )								{ SetStr(str); }
	Label( const Label& label )						{ value = label.value; }
	
	Label& operator = ( const Label& id	)			{ value = id.value;	return *this; }
	Label& operator = ( qword num )		 			{ value = num;		return *this; }
	Label& operator = ( pstr str )					{ SetStr(str);		return *this; }

	bool operator == ( const Label& comp )	const	{ return value == comp.value; }
	bool operator != ( const Label& comp )	const	{ return value != comp.value; }
	bool operator <= ( const Label& comp )	const	{ return value <= comp.value; }
	bool operator >= ( const Label& comp )	const	{ return value >= comp.value; }
	bool operator >  ( const Label& comp )	const	{ return value >  comp.value; }
	bool operator <  ( const Label& comp )	const	{ return value <  comp.value; }

	operator qword		( void ) 					{ return value; }
	operator pstr		( void ) 					{ return name; }

	void		Clear( void )						{ value = 0; }
	bool		IsEmpty( void )				const	{ return (value == 0); }
	qword		GetInt( void )				const	{ return value; }
	pstr		GetStr( void )				const	{ return name; }
	void		GetStr( char s[] )			const	{ if( s ) { strncpy( s, name, 7 ); s[7] = '\0'; } }
	void		SetStr( pstr s )					
	{
		value = 0;
		if( s )
		{
			for(int i = 0; i < 7 && s[i]; i++ )
			{
				name[i] = s[i];
			}
			name[7] = '\0';
		}
	}

private:
	union
	{
		qword		value;
		LabelStr	name;
	};
};

}