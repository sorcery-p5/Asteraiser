#pragma once

#include "ScoreTypes.h"
#include "Resource/Resource.h"


////////////////////////////////////////////////////////////////////////////////
//
//	スコアデータ
//
////////////////////////////////////////////////////////////////////////////////
class ScoreData
{
public:
	ScoreData( void );
	~ScoreData();

	bool					Load( pstr pPath );

	const SCORE_PARAM&		GetParam( void )							const	{ return m_Param; }
	const SCORE_EVENT*		GetEvent( SCORE_EVENT_TYPE Type )			const;
	const SCORE_TIME_VALUE*	GetTimeValue( SCORE_TIME_VALUE_TYPE Type )	const;

private:
	typedef std::map< SCORE_EVENT_TYPE,			SCORE_EVENT >		MapEvent;
	typedef std::map< SCORE_TIME_VALUE_TYPE,	SCORE_TIME_VALUE >	MapTimeValue;

private:
	void					_LoadParam( const XML::Node* pNode );
	void					_LoadEvent( const XML::Node* pNode );
	void					_LoadTimeValue( const XML::Node* pNode );

private:
	SCORE_PARAM				m_Param;
	MapEvent				m_mapEvent;
	MapTimeValue			m_mapTimeValue;
};

//------------------------------------------------------------------------------
//	スコアリソース
//------------------------------------------------------------------------------
class ScoreResource : public ResourceTemplate< ScoreData, RES_SCORE >
{
};