#include "P5.h"

namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	ディレクトリ
//
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	開く
//******************************************************************************
bool Directory::Open( pstr pPath, FILTER_TYPE FilterType )
{
	//	一旦閉じる
	Close();

	if( !pPath || strlen(pPath) <= 0 ) return false;

	m_Path = GetFileDirectory<256>( pPath );

	//	検索開始
	WIN32_FIND_DATA FileData;
	HANDLE Handle = FindFirstFile( pPath, &FileData );
	if( Handle == INVALID_HANDLE_VALUE ) return false;

	do
	{
		CHILD_INFO ChildInfo;

		ChildInfo.bDirectory	= (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
		String256 FileName		= FileData.cFileName;

		//	ディレクトリのフィルタリング
		if( ((FilterType & FILTER_DIR) != 0) != ChildInfo.bDirectory )
		{
			continue;
		}
		
		//	不要なものを削除
		if( FileName == "." || FileName == "..")
		{
			continue;
		}

		//	パスの結合
		ChildInfo.Path.Format( "%s/%s", m_Path.c_str(), FileName.c_str() );

		m_vChild.push_back( ChildInfo );
	}
	while( FindNextFile( Handle, &FileData ) );

	FindClose( Handle );

	return true;
}

//******************************************************************************
//	閉じる
//******************************************************************************
void Directory::Close( void )
{
	m_Path.clear();
	m_vChild.clear();
}


};