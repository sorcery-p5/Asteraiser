#include "P5.h"

namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	�f�B���N�g��
//
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	�J��
//******************************************************************************
bool Directory::Open( pstr pPath, FILTER_TYPE FilterType )
{
	//	��U����
	Close();

	if( !pPath || strlen(pPath) <= 0 ) return false;

	m_Path = GetFileDirectory<256>( pPath );

	//	�����J�n
	WIN32_FIND_DATA FileData;
	HANDLE Handle = FindFirstFile( pPath, &FileData );
	if( Handle == INVALID_HANDLE_VALUE ) return false;

	do
	{
		CHILD_INFO ChildInfo;

		ChildInfo.bDirectory	= (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
		String256 FileName		= FileData.cFileName;

		//	�f�B���N�g���̃t�B���^�����O
		if( ((FilterType & FILTER_DIR) != 0) != ChildInfo.bDirectory )
		{
			continue;
		}
		
		//	�s�v�Ȃ��̂��폜
		if( FileName == "." || FileName == "..")
		{
			continue;
		}

		//	�p�X�̌���
		ChildInfo.Path.Format( "%s/%s", m_Path.c_str(), FileName.c_str() );

		m_vChild.push_back( ChildInfo );
	}
	while( FindNextFile( Handle, &FileData ) );

	FindClose( Handle );

	return true;
}

//******************************************************************************
//	����
//******************************************************************************
void Directory::Close( void )
{
	m_Path.clear();
	m_vChild.clear();
}


};