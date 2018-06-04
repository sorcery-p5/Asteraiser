#pragma once


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	列挙型ユーティリティ
//
///////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	文字列と型のペア
//------------------------------------------------------------------------------
template<typename T> struct StrPair
{
	pstr	pStr;
	T		Value;
};

//------------------------------------------------------------------------------
//	ラベルと型のペア
//------------------------------------------------------------------------------
template<typename T> struct LabelPair
{
	Label	Name;
	T		Value;
};

//	文字列からインデクスへの変換
int StrToInt( pstr pStr, pstr aTable[], int Size, int Default = -1 );

//	文字列からインデクスへの変換
int LabelToInt( Label Name, const Label aTable[], int Size, int Default = -1 );


//******************************************************************************
//	文字列から特定の型への変換
//******************************************************************************
template<typename T> T StrToEnum( pstr pStr, const StrPair<T> aTable[], int Size, T Default )
{
	for( int i = 0; i < Size; i++ )
	{
		if( stricmp(aTable[i].pStr, pStr) == 0 )
		{
			return aTable[i].Value;
		}
	}
	return Default;
}

//******************************************************************************
//	文字列から特定の型への変換
//******************************************************************************
template<typename T> T LabelToEnum( Label Name, const LabelPair<T> aTable[], int Size, T Default )
{
	for( int i = 0; i < Size; i++ )
	{
		if( aTable[i].Name == Name )
		{
			return aTable[i].Value;
		}
	}
	return Default;
}

//******************************************************************************
//	特定の型から文字列への変換
//******************************************************************************
template<typename T> pstr EnumToStr( T Value, const StrPair<T> aTable[], int Size, pstr pDefault = "" )
{
	for( int i = 0; i < Size; i++ )
	{
		if( aTable[i].Value == Value )
		{
			return aTable[i].pStr;
		}
	}
	return pDefault;
}

}