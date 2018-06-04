#include "stdafx.h"
#include "TimeCounter.h"

namespace
{
	__int64 s_aTimeReg[10] = {0};
}

////////////////////////////////////////////////////////////////////////////////
//
//	時間計測
//
////////////////////////////////////////////////////////////////////////////////
TimeCounter::TimeCounter( pstr pText )
{
	m_Text = pText;
	::QueryPerformanceCounter( &m_StartTime );
}
TimeCounter::~TimeCounter()
{
	LARGE_INTEGER EndTime;
	::QueryPerformanceCounter( &EndTime );

	__int64 Erapse  = EndTime.QuadPart - m_StartTime.QuadPart;
	
	FILE* fp = fopen( "TimeLog.txt", "a" );
	if( fp )
	{
		fprintf( fp, "%-32s : %I64d\n", m_Text.c_str(), Erapse );
		fclose( fp );
	}
}

////////////////////////////////////////////////////////////////////////////////
//
//	時間計測（登録型）
//
////////////////////////////////////////////////////////////////////////////////
TimeCounter_Reg::TimeCounter_Reg( int ID )
{
	m_ID = ID;
	::QueryPerformanceCounter( &m_StartTime );
}
TimeCounter_Reg::~TimeCounter_Reg()
{
	LARGE_INTEGER EndTime;
	::QueryPerformanceCounter( &EndTime );

	s_aTimeReg[m_ID] += EndTime.QuadPart - m_StartTime.QuadPart;
}

//------------------------------------------------------------------------------
//	登録された計測時間を書き出しリセット
//------------------------------------------------------------------------------
void FlushTimeReg( int ID, pstr pText )
{
	__int64 Erapse  = s_aTimeReg[ID];

	FILE* fp = fopen( "TimeLog.txt", "a" );
	if( fp )
	{
		fprintf( fp, "%-32s : %I64d\n", pText, Erapse );
		fclose( fp );
	}

	s_aTimeReg[ID] = 0;
}
