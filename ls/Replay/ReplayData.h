#pragma once

#include "ReplayTypes.h"
#include "SaveData/SaveDataTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	リプレイデータ
//
////////////////////////////////////////////////////////////////////////////////
class ReplayData
{
public:
	ReplayData( void );
	~ReplayData();

	bool					Load( pstr pPath );
	bool					LoadHeader( pstr pPath );
	bool					Save( pstr pPath, const SAVEDATA_SCORE& Score );
	void					Reset( void );

	void					AddEvent( const REPLAY_EVENT& Event );
	void					AddData( byte Data );
	void					ChangeManual( uint FrameCount );

	const REPLAY_HEADER&	GetHeader( void )							const	{ return m_Header; }
	const REPLAY_EVENT&		GetEvent( uint Index )						const	{ return m_vEvent[Index]; }
	byte					GetData( uint Frame )						const	{ return m_vData[Frame]; }

private:
	typedef std::vector<REPLAY_EVENT>	VecEvent;
	typedef std::vector<byte>			VecData;

private:
	REPLAY_HEADER			m_Header;
	VecEvent				m_vEvent;
	VecData					m_vData;
};