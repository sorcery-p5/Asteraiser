#pragma once

#include "ReplayTypes.h"
#include "SaveData/SaveDataTypes.h"

class ReplayData;
class World;


////////////////////////////////////////////////////////////////////////////////
//
//	ÉäÉvÉåÉCä«óù
//
////////////////////////////////////////////////////////////////////////////////
class ReplayManager
{
public:
	ReplayManager( World* pWorld );
	~ReplayManager();

	void					InitRecord( void );
	bool					InitLoad( pstr pPath, int Stage );
	bool					Save( const SAVEDATA_SCORE& Score, pstr pDirName, int SaveMax );
	void					ChangeManual( uint FrameCount );

	void					RecordData( byte Data );
	void					RecordChangeStage( uint FrameCount, int Stage );

	bool					IsReplayEnable( void )				const	{ return m_pLoadData != NULL; }
	bool					IsReplayOver( uint FrameCount )		const;

	byte					GetReplayData( uint FrameCount )	const;
	int						GetContinueCount( void )			const;
	uint					GetPlayID( void )					const	{ return m_PlayID; }

private:
	ReplayData*				m_pLoadData;
	ReplayData*				m_pRecordData;

	uint					m_FrameOffset;
	uint					m_PlayID;
};