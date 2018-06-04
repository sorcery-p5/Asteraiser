#include "stdafx.h"
#include "TimeKey.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�^�C���L�[
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	�����񂩂��Ԏ�ʂ֕ϊ�
//------------------------------------------------------------------------------
TIME_KEY_INTP StrToTimeKeyIntpType( pstr pStr )
{
	const LabelPair<TIME_KEY_INTP>	INTP_TABLE[] =
	{
		{ "Fix",	TIME_KEY_INTP_FIX,		},
		{ "Lerp",	TIME_KEY_INTP_LERP,		},
	};

	return LabelToEnum<TIME_KEY_INTP>( pStr, INTP_TABLE, ArrayNum(INTP_TABLE), TIME_KEY_INTP_LERP );
}
