#pragma once


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	ディレクトリ
//
///////////////////////////////////////////////////////////////////////////////
class Directory
{
public:
	enum FILTER_TYPE
	{
		FILTER_FILE		= (1 << 0),
		FILTER_DIR		= (1 << 1),
		FILTER_ALL		= FILTER_FILE | FILTER_DIR,
	};

public:
	Directory( void )																		{}
	Directory( pstr pPath )																	{ Open( pPath ); }
	Directory( pstr pPath, FILTER_TYPE FilterType )											{ Open( pPath, FilterType ); }
	~Directory()																			{ Close(); }

	bool				Open( pstr pPath )													{ return Open( pPath, FILTER_ALL ); }
	bool				Open( pstr pPath, FILTER_TYPE FilterType );
	void				Close( void );

	pstr				GetPath( void )												const	{ return m_Path.c_str(); }
	bool				IsEmpty( void )												const	{ return m_vChild.empty(); }
	int					GetChildNum( void )											const	{ return (int)m_vChild.size(); }
	pstr				GetChildPath( int Index )									const	{ return m_vChild[Index].Path.c_str(); }
	bool				IsChildDirectory( int Index )								const	{ return m_vChild[Index].bDirectory; }

	String256			GetChildName( int Index )									const	{ return GetFileName<256>( GetChildPath(Index) ); }
	String256			GetChildTitle( int Index )									const	{ return GetFileTitle<256>( GetChildPath(Index) ); }
	String256			GetChildExt( int Index )									const	{ return GetFileExt<256>( GetChildPath(Index) ); }

private:
	struct CHILD_INFO
	{
		String256		Path;
		bool			bDirectory;
	};
	typedef std::vector< CHILD_INFO >		ChildVec;

private:
	std::string			m_Path;
	ChildVec			m_vChild;
};

};