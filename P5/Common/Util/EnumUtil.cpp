#include "P5.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	列挙型ユーティリティ
//
///////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	文字列からインデクスへの変換
//******************************************************************************
int StrToInt( pstr pStr, pstr aTable[], int Size, int Default )
{
	for( int i = 0; i < Size; i++ )
	{
		if( stricmp(aTable[i], pStr) == 0 )
		{
			return i;
		}
	}
	return Default;
}

//******************************************************************************
//	文字列からインデクスへの変換
//******************************************************************************
int LabelToInt( Label Name, const Label aTable[], int Size, int Default )
{
	for( int i = 0; i < Size; i++ )
	{
		if( aTable[i] == Name )
		{
			return i;
		}
	}
	return Default;
}

}