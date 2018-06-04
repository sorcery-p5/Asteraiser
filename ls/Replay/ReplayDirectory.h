#pragma once

#include "ReplayTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	リプレイディレクトリ
//
////////////////////////////////////////////////////////////////////////////////
class ReplayDirectory
{
public:
	ReplayDirectory( void );
	~ReplayDirectory();

	void					Open( void );
	void					Close( void );

	int						GetDirNum( void )						const		{ return (int)m_vDirectory.size(); }
	pstr					GetDirName( int DirID )					const		{ return m_vDirectory[DirID].Name.c_str(); }
	int						GetFileNum( int DirID )					const		{ return (int)m_vDirectory[DirID].vFile.size(); }
	pstr					GetFilePath( int DirID, int FileID )	const		{ return m_vDirectory[DirID].vFile[FileID].Path.c_str(); }
	const REPLAY_HEADER&	GetFileHeader( int DirID, int FileID )	const		{ return m_vDirectory[DirID].vFile[FileID].Header; }

	int						FindDir( pstr pName )					const;
	int						FindFile( int DirID, pstr pPath )		const;

private:
	struct FILE_INFO
	{
		std::string			Path;
		REPLAY_HEADER		Header;
	};
	typedef std::vector< FILE_INFO >		VecFile;

	struct DIR_INFO
	{
		std::string			Name;
		VecFile				vFile;
	};
	typedef std::vector< DIR_INFO >			VecDirectory;

private:
	void					_LoadDir( DIR_INFO& Out, const Directory& Dir );

private:
	VecDirectory			m_vDirectory;
};