#include "P5.h"
#include "Timer.h"


namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	タイマー
//
////////////////////////////////////////////////////////////////////////////////
Timer::Timer( void )
{
	//	精度の初期化
	TIMECAPS Caps;
	timeGetDevCaps( &Caps, sizeof(Caps) );
	m_MinPeriod = Caps.wPeriodMin;

	timeBeginPeriod( m_MinPeriod );

	m_PrevTime = timeGetTime();
}
Timer::~Timer()
{
	//	精度を戻す
	timeEndPeriod(m_MinPeriod);
}

//******************************************************************************
//	フレームの待ち処理
//******************************************************************************
void Timer::Wait( void )
{
	dword Cur		= timeGetTime();
	dword Elapse	= Cur - m_PrevTime;

	//	インターバルを経過
	if( Elapse >= INTERVAL )
	{
		m_PrevTime = Cur;
		return;
	}

	if( (INTERVAL - Elapse) > WAIT_MIN )
	{
	    ::MsgWaitForMultipleObjects( 0, NULL, FALSE, (INTERVAL - Elapse) - WAIT_MIN, QS_ALLEVENTS );
	}

	//	待ち時間のあいだ、whileで待つ
	while( (timeGetTime() - m_PrevTime) < INTERVAL );

	m_PrevTime += INTERVAL;
}

}