#pragma once


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	�񋓌^���[�e�B���e�B
//
///////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	������ƌ^�̃y�A
//------------------------------------------------------------------------------
template<typename T> struct StrPair
{
	pstr	pStr;
	T		Value;
};

//------------------------------------------------------------------------------
//	���x���ƌ^�̃y�A
//------------------------------------------------------------------------------
template<typename T> struct LabelPair
{
	Label	Name;
	T		Value;
};

//	�����񂩂�C���f�N�X�ւ̕ϊ�
int StrToInt( pstr pStr, pstr aTable[], int Size, int Default = -1 );

//	�����񂩂�C���f�N�X�ւ̕ϊ�
int LabelToInt( Label Name, const Label aTable[], int Size, int Default = -1 );


//******************************************************************************
//	�����񂩂����̌^�ւ̕ϊ�
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
//	�����񂩂����̌^�ւ̕ϊ�
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
//	����̌^���當����ւ̕ϊ�
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