#pragma once


////////////////////////////////////////////////////////////////////////////////
//
//	���Ԍv��
//
////////////////////////////////////////////////////////////////////////////////
class TimeCounter
{
public:
	TimeCounter( pstr pText );
	~TimeCounter();

private:
	String32		m_Text;
	LARGE_INTEGER	m_StartTime;
};

////////////////////////////////////////////////////////////////////////////////
//
//	���Ԍv���i�o�^�^�j
//
////////////////////////////////////////////////////////////////////////////////
class TimeCounter_Reg
{
public:
	TimeCounter_Reg( int ID );
	~TimeCounter_Reg();

private:
	int				m_ID;
	LARGE_INTEGER	m_StartTime;
};


//------------------------------------------------------------------------------
//	�o�^���ꂽ�v�����Ԃ������o�����Z�b�g
//------------------------------------------------------------------------------
void FlushTimeReg( int ID, pstr pText );


//------------------------------------------------------------------------------
//	�v���p�}�N��
//------------------------------------------------------------------------------
#define TIMECOUNT( p )		TimeCounter TimeCounter##__LINE__( p )
#define TIMECOUNT_REG( n )	TimeCounter_Reg TimeCounter_Reg##__LINE__( n )
