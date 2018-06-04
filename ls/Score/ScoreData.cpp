#include "stdafx.h"
#include "ScoreData.h"

namespace
{
	pstr		NODE_SCORE			= "Score";
	pstr		NODE_EVENT			= "Event";
	pstr		NODE_TIME_VALUE		= "TimeValue";
	pstr		NODE_RATE			= "Rate";
	pstr		NODE_HEAT			= "Heat";

	pstr		ATTR_MIN			= "Min";
	pstr		ATTR_MAX			= "Max";
	pstr		ATTR_LOW			= "Low";
	pstr		ATTR_HIGH			= "High";
	pstr		ATTR_RATE_PLUS_MIN	= "RatePlusMin";
	pstr		ATTR_RATE_PLUS_MAX	= "RatePlusMax";
	pstr		ATTR_RATE_MINUS_MIN	= "RateMinusMin";
	pstr		ATTR_RATE_MINUS_MAX	= "RateMinusMax";
	pstr		ATTR_BASE			= "Base";

	pstr		ATTR_TIME			= "Time";
	pstr		ATTR_RATE			= "Rate";
	pstr		ATTR_HEAT			= "Heat";


	const StrPair<SCORE_EVENT_TYPE>	SCORE_EVENT_TABLE[] =
	{
		{ "Hit",			SCORE_EVENT_HIT,				},
		{ "Combo",			SCORE_EVENT_COMBO,				},
		{ "Miss",			SCORE_EVENT_MISS,				},
		{ "Crash",			SCORE_EVENT_CRASH,				},
		{ "Cancel",			SCORE_EVENT_CANCEL,				},
		{ "BulletErase",	SCORE_EVENT_BULLET_ERASE,		},
		{ "BladeErase",		SCORE_EVENT_BLADE_ERASE,		},
		{ "Damage",			SCORE_EVENT_DAMAGE,				},
		{ "SerialDamage",	SCORE_EVENT_SERIAL_DAMAGE,		},
		{ "Break",			SCORE_EVENT_BREAK,				},
		{ "EnemyDestroy",	SCORE_EVENT_ENEMY_DESTROY,		},
		{ "EnemyEscape",	SCORE_EVENT_ENEMY_ESCAPE,		},
		{ "GetItem",		SCORE_EVENT_GET_ITEM,			},
		{ "LostItem",		SCORE_EVENT_LOST_ITEM,			},
		{ "Overdrive",		SCORE_EVENT_OVERDRIVE,			},
		{ "Spear",			SCORE_EVENT_SPEAR,				},
		{ "Ruler",			SCORE_EVENT_RULER,				},
	};

	const StrPair<SCORE_TIME_VALUE_TYPE>	SCORE_TIME_VALUE_TABLE[] =
	{
		{ "DecayRate",		SCORE_TIME_VALUE_DECAY_RATE,	},
		{ "NoDamage",		SCORE_TIME_VALUE_NO_DAMAGE,		},
		{ "EnemyExist",		SCORE_TIME_VALUE_ENEMY_EXIST,	},
		{ "AttackExist",	SCORE_TIME_VALUE_ATTACK_EXIST,	},
	};
}


////////////////////////////////////////////////////////////////////////////////
//
//	スコアデータ
//
////////////////////////////////////////////////////////////////////////////////
ScoreData::ScoreData( void )
{
}

ScoreData::~ScoreData()
{

}

//******************************************************************************
//	読み込み
//******************************************************************************
bool ScoreData::Load( pstr pPath )
{
	//	ファイル読み込む
	XML Xml;
	if( !Xml.Load( pPath ) )
	{
		return false;
	}

	//	パラメータ
	const XML::Node* pParamNode = Xml.GetRoot().FindChild( NODE_SCORE );
	if( pParamNode ) _LoadParam( pParamNode );

	//	イベント
	const XML::Node* pEventNode = Xml.GetRoot().FindChild( NODE_EVENT );
	if( pEventNode ) _LoadEvent( pEventNode );

	//	時間変化
	const XML::Node* pTimeValueNode = Xml.GetRoot().FindChild( NODE_TIME_VALUE );
	if( pTimeValueNode ) _LoadTimeValue( pTimeValueNode );

	return true;
}

//******************************************************************************
//	イベントパラメータ取得
//******************************************************************************
const SCORE_EVENT* ScoreData::GetEvent( SCORE_EVENT_TYPE Type )	const
{
	MapEvent::const_iterator it = m_mapEvent.find( Type );
	if( it != m_mapEvent.end() )
	{
		return &(it->second);
	}

	return NULL;
}

//******************************************************************************
//	時間変化パラメータ取得
//******************************************************************************
const SCORE_TIME_VALUE* ScoreData::GetTimeValue( SCORE_TIME_VALUE_TYPE Type )	const
{
	MapTimeValue::const_iterator it = m_mapTimeValue.find( Type );
	if( it != m_mapTimeValue.end() )
	{
		return &(it->second);
	}

	return NULL;
}

//------------------------------------------------------------------------------
//	パラメータの読み込み
//------------------------------------------------------------------------------
void ScoreData::_LoadParam( const XML::Node* pNode )
{
	m_Param.ScoreBase	= pNode->GetAttr( ATTR_BASE ).GetInt();
	m_Param.ScoreMax	= pNode->GetAttr( ATTR_MAX ).GetInt();

	//	レート
	const XML::Node* pRateNode = pNode->FindChild( NODE_RATE );
	if( pRateNode )
	{
		m_Param.Rate.RateMin	= pRateNode->GetAttr( ATTR_MIN ).GetFloat();
		m_Param.Rate.RateMax	= pRateNode->GetAttr( ATTR_MAX ).GetFloat();
		m_Param.Rate.RateLow	= pRateNode->GetAttr( ATTR_LOW ).GetFloat();
		m_Param.Rate.RateHigh	= pRateNode->GetAttr( ATTR_HIGH ).GetFloat();
	}
	
	//	ヒート
	const XML::Node* pHeatNode = pNode->FindChild( NODE_HEAT );
	if( pHeatNode )
	{
		m_Param.Heat.HeatMin		= pHeatNode->GetAttr( ATTR_MIN ).GetFloat();
		m_Param.Heat.HeatMax		= pHeatNode->GetAttr( ATTR_MAX ).GetFloat();
		m_Param.Heat.RatePlusMin	= pHeatNode->GetAttr( ATTR_RATE_PLUS_MIN ).GetFloat();
		m_Param.Heat.RatePlusMax	= pHeatNode->GetAttr( ATTR_RATE_PLUS_MAX ).GetFloat();
		m_Param.Heat.RateMinusMin	= pHeatNode->GetAttr( ATTR_RATE_MINUS_MIN ).GetFloat();
		m_Param.Heat.RateMinusMax	= pHeatNode->GetAttr( ATTR_RATE_MINUS_MAX ).GetFloat();
	}
}

//------------------------------------------------------------------------------
//	イベントパラメータの読み込み
//------------------------------------------------------------------------------
void ScoreData::_LoadEvent( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pChildNode = pNode->GetChild( i );

		SCORE_EVENT_TYPE Type = StrToEnum<SCORE_EVENT_TYPE>( pChildNode->GetName(), SCORE_EVENT_TABLE, ArrayNum(SCORE_EVENT_TABLE), SCORE_EVENT_NULL );
		if( Type == SCORE_EVENT_NULL ) continue;

		SCORE_EVENT Event;
		Event.Rate	= pChildNode->GetAttr( ATTR_RATE ).GetFloat();
		Event.Heat	= pChildNode->GetAttr( ATTR_HEAT ).GetFloat();

		m_mapEvent.insert( MapEvent::value_type(Type, Event) );
	}
}

//------------------------------------------------------------------------------
//	時間変化パラメータの読み込み
//------------------------------------------------------------------------------
void ScoreData::_LoadTimeValue( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pChildNode = pNode->GetChild( i );

		SCORE_TIME_VALUE_TYPE Type = StrToEnum<SCORE_TIME_VALUE_TYPE>( pChildNode->GetName(), SCORE_TIME_VALUE_TABLE, ArrayNum(SCORE_TIME_VALUE_TABLE), SCORE_TIME_VALUE_NULL );
		if( Type == SCORE_TIME_VALUE_NULL ) continue;

		SCORE_TIME_VALUE TimeValue;
		TimeValue.Time	= pChildNode->GetAttr( ATTR_TIME ).GetInt();
		TimeValue.Rate	= pChildNode->GetAttr( ATTR_RATE ).GetFloat();
		TimeValue.Heat	= pChildNode->GetAttr( ATTR_HEAT ).GetFloat();

		m_mapTimeValue.insert( MapTimeValue::value_type(Type, TimeValue) );
	}
}