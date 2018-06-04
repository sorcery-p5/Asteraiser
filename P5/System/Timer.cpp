#include "P5.h"
#include "Timer.h"


namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	�^�C�}�[
//
////////////////////////////////////////////////////////////////////////////////
Timer::Timer( void )
{
	//	���x�̏�����
	TIMECAPS Caps;
	timeGetDevCaps( &Caps, sizeof(Caps) );
	m_MinPeriod = Caps.wPeriodMin;

	timeBeginPeriod( m_MinPeriod );

	m_PrevTime = timeGetTime();
}
Timer::~Timer()
{
	//	���x��߂�
	timeEndPeriod(m_MinPeriod);
}

//******************************************************************************
//	�t���[���̑҂�����
//******************************************************************************
void Timer::Wait( void )
{
	dword Cur		= timeGetTime();
	dword Elapse	= Cur - m_PrevTime;

	//	�C���^�[�o�����o��
	if( Elapse >= INTERVAL )
	{
		m_PrevTime = Cur;
		return;
	}

	if( (INTERVAL - Elapse) > WAIT_MIN )
	{
	    ::MsgWaitForMultipleObjects( 0, NULL, FALSE, (INTERVAL - Elapse) - WAIT_MIN, QS_ALLEVENTS );
	}

	//	�҂����Ԃ̂������Awhile�ő҂�
	while( (timeGetTime() - m_PrevTime) < INTERVAL );

	m_PrevTime += INTERVAL;
}

}