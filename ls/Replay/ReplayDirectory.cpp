#include "stdafx.h"
#include "ReplayDirectory.h"
#include "ReplayData.h"

namespace
{
	pstr			PATH_REPLAY_ROOT_PATH			= "Replay/*.rep";
	pstr			PATH_REPLAY_CHILD_DIR			= "Replay/*";
	pstr			PATH_REPLAY_CHILD_PATH			= "Replay/%s/*.rep";
}

////////////////////////////////////////////////////////////////////////////////
//
//	リプレイディレクトリ
//
////////////////////////////////////////////////////////////////////////////////
ReplayDirectory::ReplayDirectory( void )
{
	
}
ReplayDirectory::~ReplayDirectory()
{
	
}

//******************************************************************************
//	開く
//******************************************************************************
void ReplayDirectory::Open( void )
{
	Directory RootDir;

	//	ルートのファイル取得
	RootDir.Open( PATH_REPLAY_ROOT_PATH, Directory::FILTER_FILE );

	DIR_INFO DirInfo;
	_LoadDir( DirInfo, RootDir );

	m_vDirectory.push_back( DirInfo );

	//	子ディレクトリの探索
	RootDir.Open( PATH_REPLAY_CHILD_DIR, Directory::FILTER_DIR );
	for( int i = 0; i < RootDir.GetChildNum(); i++ )
	{
		Directory ChildDir;

		String256 Path;
		Path.Format( PATH_REPLAY_CHILD_PATH, RootDir.GetChildTitle(i).c_str() );

		ChildDir.Open( Path.c_str(), Directory::FILTER_FILE );

		DIR_INFO ChildDirInfo;
		_LoadDir( ChildDirInfo, ChildDir );
		
		m_vDirectory.push_back( ChildDirInfo );
	}
}

//******************************************************************************
//	閉じる
//******************************************************************************
void ReplayDirectory::Close( void )
{
	m_vDirectory.clear();
}

//******************************************************************************
//	文字列からディレクトリを検索する
//******************************************************************************
int ReplayDirectory::FindDir( pstr pName ) const
{
	for( int i = 0; i < GetDirNum(); i++ )
	{
		if( m_vDirectory[i].Name == pName )
		{
			return i;
		}
	}
	return -1;
}

//******************************************************************************
//	文字列からファイルを検索する
//******************************************************************************
int ReplayDirectory::FindFile( int DirID, pstr pPath ) const
{
	if( DirID >= GetDirNum() ) return -1;

	for( int i = 0; i < GetFileNum(DirID); i++ )
	{
		if( m_vDirectory[DirID].vFile[i].Path == pPath )
		{
			return i;
		}
	}
	return -1;
}

//------------------------------------------------------------------------------
//	ディレクトリ情報読み込み
//------------------------------------------------------------------------------
void ReplayDirectory::_LoadDir( DIR_INFO& Out, const Directory& Dir )
{
	Out.Name	= GetFileTitle<256>( Dir.GetPath() );

	for( int i = 0; i < Dir.GetChildNum(); i++ )
	{
		ReplayData RepData;
		if( !RepData.LoadHeader( Dir.GetChildPath(i) ) ) continue;

		FILE_INFO FileInfo;

		FileInfo.Header	= RepData.GetHeader();
		FileInfo.Path	= Dir.GetChildPath(i);

		Out.vFile.push_back( FileInfo );
	}
}
