#pragma once

#include "SaveDataTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	セーブデータ管理
//
////////////////////////////////////////////////////////////////////////////////
class SaveDataManager
{
public:
	SaveDataManager( void );
	~SaveDataManager();

	bool					Load( void );
	bool					Save( void );
	void					Reset( void );

	uint					RecordScore( SAVEDATA_SCORE Socre, bool bClear );

	const SAVEDATA&			GetSaveData( void )								const	{ return m_SaveData; }
	int						GetScoreNum( void )								const	{ return (int)m_vScoreRecord.size(); }
	const SAVEDATA_SCORE&	GetScore( int Index )							const	{ return m_vScoreRecord[Index]; }

	int						FindRecord( uint PlayID )						const;

private:
	typedef std::vector<SAVEDATA_SCORE>		VecScore;

private:
	void					_LoadSaveData( const XML::Node* pNode );
	void					_LoadScore( const XML::Node* pNode );
	void					_LoadScore( SAVEDATA_SCORE& Score, const XML::Node* pNode );
	
	void					_SaveSaveData( XML::Node* pNode );
	void					_SaveScore( XML::Node* pNode );
	void					_SaveScore( XML::Node* pNode, const SAVEDATA_SCORE& Score );

	void					_UpdateTotalData( const SAVEDATA_SCORE& Socre, bool bClear );
	void					_InsertRecord( const SAVEDATA_SCORE& Socre );

private:
	SAVEDATA				m_SaveData;
	VecScore				m_vScoreRecord;
};