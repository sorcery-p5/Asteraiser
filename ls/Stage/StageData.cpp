#include "stdafx.h"
#include "StageData.h"

#include "App/App.h"

#include "Resource/ResourceManager.h"
#include "Resource/ResourceUtil.h"

#include "SpriteFrame/SpriteFrameFile.h"
#include "Effect/EffectFile.h"
#include "Enemy/EnemyData.h"

#include "Ground/GroundData.h"


namespace
{
	pstr		NODE_STAGE				= "Stage";
	pstr		NODE_STATE				= "State";
	pstr		NODE_ACTION				= "Action";
	pstr		NODE_CONDITION			= "Condition";
	pstr		NODE_TELOP				= "Telop";

	pstr		NODE_ENEMY				= "Enemy";
	pstr		NODE_GROUND				= "Ground";
	pstr		NODE_SPRITE				= "Sprite";
	pstr		NODE_EFFECT				= "Effect";

	pstr		ATTR_PATH				= "Path";
	pstr		ATTR_TIME				= "Time";
	pstr		ATTR_ROOT				= "Root";
	pstr		ATTR_INIT_ACTION		= "InitAction";
	pstr		ATTR_NAME				= "Name";
	pstr		ATTR_FRAME				= "Frame";
	pstr		ATTR_TYPE				= "Type";

	pstr		ATTR_POS				= "Pos";
	pstr		ATTR_ROT				= "Rot";
	pstr		ATTR_DIR				= "Dir";
	pstr		ATTR_SIDE				= "Side";
	pstr		ATTR_PARAM				= "Param";
	pstr		ATTR_SPRITE				= "Sprite";
	pstr		ATTR_ANIME				= "Anime";
	pstr		ATTR_CLOSE_ANIME		= "CloseAnime";
	pstr		ATTR_CHILD				= "Child";
	pstr		ATTR_SKIP				= "Skip";
	pstr		ATTR_SKIP_TIME			= "SkipTime";
	pstr		ATTR_ANIME_END_CLOSE	= "AnimeEndClose";
	pstr		ATTR_VOL				= "Vol";
	pstr		ATTR_FLAG				= "Flag";
	pstr		ATTR_NO_WAIT			= "NoWait";
	pstr		ATTR_NO_START			= "NoStart";
	pstr		ATTR_INTP				= "Intp";
	pstr		ATTR_BONE				= "Bone";
	pstr		ATTR_TEXT				= "Text";
	pstr		ATTR_NUM				= "Num";
}


const StrPair<STAGE_ACTION_TYPE>	STAGE_ACTION_TYPE_TABLE[] =
{
	{ "Enemy",				STAGE_ACTION_ENEMY,					},
	{ "Ground",				STAGE_ACTION_GROUND,				},
	{ "Telop",				STAGE_ACTION_TELOP,					},
	{ "Effect",				STAGE_ACTION_EFFECT,				},
	{ "PlayBGM",			STAGE_ACTION_BGM_PLAY,				},
	{ "FadeBGM",			STAGE_ACTION_BGM_FADE,				},
	{ "StopBGM",			STAGE_ACTION_BGM_STOP,				},
	{ "TelopText",			STAGE_ACTION_TELOP_TEXT,			},
	{ "TelopAnime",			STAGE_ACTION_TELOP_ANIME,			},
	{ "TelopClose",			STAGE_ACTION_TELOP_CLOSE,			},
	{ "GroundAnime",		STAGE_ACTION_GROUND_ANIME,			},
	{ "GroundEnd",			STAGE_ACTION_GROUND_END,			},
	{ "EnemyChild",			STAGE_ACTION_ENEMY_CHILD,			},
	{ "EnemyNotify",		STAGE_ACTION_ENEMY_NOTIFY,			},
	{ "DestroyEnemyAll",	STAGE_ACTION_DESTROY_ENEMY_ALL,		},
	{ "EraseEnemy",			STAGE_ACTION_ERASE_ENEMY,			},
	{ "EraseEnemyAll",		STAGE_ACTION_ERASE_ENEMY_ALL,		},
	{ "EraseGround",		STAGE_ACTION_ERASE_GROUND,			},
	{ "EraseGroundAll",		STAGE_ACTION_ERASE_GROUND_ALL,		},
	{ "PlayerPos",			STAGE_ACTION_PLAYER_POS,			},
	{ "PlayerAction",		STAGE_ACTION_PLAYER_ACTION,			},
	{ "PlayerAnime",		STAGE_ACTION_PLAYER_ANIME,			},
	{ "PlayerMove",			STAGE_ACTION_PLAYER_MOVE,			},
	{ "PlayerAttack",		STAGE_ACTION_PLAYER_ATTACK,			},
	{ "PlayerLock",			STAGE_ACTION_PLAYER_LOCK,			},
	{ "PlayerIndicate",		STAGE_ACTION_PLAYER_INDICATE,		},
	{ "LockScore",			STAGE_ACTION_LOCK_SCORE,			},
	{ "Item",				STAGE_ACTION_ITEM,					},
	{ "Arrive",				STAGE_ACTION_ARRIVE,				},
	{ "Result",				STAGE_ACTION_RESULT,				},
	{ "ChangePlayer",		STAGE_ACTION_CHANGE_PLAYER,			},
	{ "Loop",				STAGE_ACTION_LOOP,					},
	{ "ChangeState",		STAGE_ACTION_CHANGE_STATE,			},
	{ "EndStage",			STAGE_ACTION_END_STAGE,				},
};

const StrPair<STAGE_CONDITION_TYPE>	STAGE_CONDITION_TYPE_TABLE[] =
{
	{ "DefeatEnemy",		STAGE_CONDITION_DEFEAT_ENEMY,		},
	{ "DefeatEnemyAll",		STAGE_CONDITION_DEFEAT_ENEMY_ALL,	},
	{ "EmptyEnemy",			STAGE_CONDITION_EMPTY_ENEMY,		},
	{ "EmptyGround",		STAGE_CONDITION_EMPTY_GROUND,		},
	{ "EndTelop",			STAGE_CONDITION_END_TELOP,			},
	{ "EndGroundAnime",		STAGE_CONDITION_END_GROUND_ANIME,	},
	{ "StateFrame",			STAGE_CONDITION_STATE_FRAME,		},
	{ "StageFrame",			STAGE_CONDITION_STAGE_FRAME,		},
	{ "PlayerState",		STAGE_CONDITION_PLAYER_STATE,		},
	{ "Notify",				STAGE_CONDITION_NOTIFY,				},
	{ "EndState",			STAGE_CONDITION_END_STATE,			},
};


////////////////////////////////////////////////////////////////////////////////
//
//	ステージデータ
//
////////////////////////////////////////////////////////////////////////////////
StageData::StageData( void )
{

}
StageData::~StageData()
{
	DeleteMap( m_mapState );
		
	ReleaseMap( m_mapEnemyRes );
	DeleteMap( m_mapGround );
	ReleaseMap( m_mapSpriteRes );
	ReleaseMap( m_mapEffectRes );
}

//******************************************************************************
//	読み込み
//******************************************************************************
bool StageData::Load( pstr pPath )
{
	//	ファイル読み込む
	XML Xml;
	if( !Xml.Load( pPath ) )
	{
		return false;
	}
	
	//	敵
	const XML::Node* pEnemyNode = Xml.GetRoot().FindChild( NODE_ENEMY );
	if( pEnemyNode ) _LoadEnemy( pEnemyNode );
	
	//	地形
	const XML::Node* pGroundNode = Xml.GetRoot().FindChild( NODE_GROUND );
	if( pGroundNode ) _LoadGround( pGroundNode );
	
	//	スプライト
	const XML::Node* pSpriteNode = Xml.GetRoot().FindChild( NODE_SPRITE );
	if( pSpriteNode ) _LoadSprite( pSpriteNode );
	
	//	エフェクト
	const XML::Node* pEffectNode = Xml.GetRoot().FindChild( NODE_EFFECT );
	if( pEffectNode ) _LoadEffect( pEffectNode );

	//	テロップ
	const XML::Node* pTelopNode = Xml.GetRoot().FindChild( NODE_TELOP );
	if( pTelopNode ) _LoadTelop( pTelopNode );

	//	パラメータ
	const XML::Node* pParamNode = Xml.GetRoot().FindChild( NODE_STAGE );
	if( pParamNode ) _LoadParam( pParamNode );

	//	ステート
	const XML::Node* pStateNode = Xml.GetRoot().FindChild( NODE_STATE );
	if( pStateNode ) _LoadState( pStateNode );

	return true;
}

//******************************************************************************
//	ステートデータ取得
//******************************************************************************
const STAGE_STATE* StageData::GetState( Label Name ) const
{
	MapState::const_iterator it = m_mapState.find( Name );
	if( it != m_mapState.end() )
	{
		return (it->second);
	}

	return NULL;
}

//******************************************************************************
//	テロップデータ取得
//******************************************************************************
const STAGE_TELOP* StageData::GetTelop( Label Name ) const
{
	MapTelop::const_iterator it = m_mapTelop.find( Name );
	if( it != m_mapTelop.end() )
	{
		return &(it->second);
	}

	return NULL;
}

//******************************************************************************
//	敵データの取得
//******************************************************************************
const EnemyData* StageData::GetEnemyData( Label Name ) const
{
	return FindResourceMap<EnemyData, EnemyResource>( m_mapEnemyRes, Name );
}

//******************************************************************************
//	地形データの取得
//******************************************************************************
const GroundData* StageData::GetGroundData( Label Name ) const
{
	MapGround::const_iterator it = m_mapGround.find( Name );
	if( it != m_mapGround.end() )
	{
		return (it->second);
	}

	return NULL;
}

//******************************************************************************
//	スプライトフレームファイルの取得
//******************************************************************************
const SpriteFrameFile* StageData::GetSpriteFrameFile( Label Name ) const
{
	return FindResourceMap<SpriteFrameFile, SpriteFrameResource>( m_mapSpriteRes, Name );
}

//******************************************************************************
//	エフェクトファイルの取得
//******************************************************************************
const EffectFile* StageData::GetEffectFile( Label Name ) const
{
	return FindResourceMap<EffectFile, EffectResource>( m_mapEffectRes, Name );
}

//------------------------------------------------------------------------------
//	パラメータの読み込み
//------------------------------------------------------------------------------
void StageData::_LoadParam( const XML::Node* pNode )
{
	_ASSERT( pNode );

	m_Param.Time			= pNode->GetAttr( ATTR_TIME ).GetInt();
	m_Param.RootState		= pNode->GetAttr( ATTR_ROOT ).GetLabel();
	m_Param.InitActionState	= pNode->GetAttr( ATTR_INIT_ACTION ).GetLabel();
}

//------------------------------------------------------------------------------
//	ステートの読み込み
//------------------------------------------------------------------------------
void StageData::_LoadState( const XML::Node* pNode )
{
	for( int StateID = 0; StateID < pNode->GetChildNum(); StateID++ )
	{
		const XML::Node* pStateNode = pNode->GetChild( StateID );
		
		STAGE_STATE* pState = new STAGE_STATE;
		pState->Name = pStateNode->GetAttr( ATTR_NAME ).GetLabel();

		//	アクションの取得
		for( int ActID = 0; ActID < pStateNode->GetChildNum(); ActID++ )
		{
			const XML::Node* pActionNode = pStateNode->GetChild( ActID );

			STAGE_ACTION Action;
			_LoadAction( Action, pActionNode );

			pState->ActionList.push_back( Action );
		}

		m_mapState.insert( MapState::value_type( pState->Name, pState ) );
	}	
}

//------------------------------------------------------------------------------
//	テロップの読み込み
//------------------------------------------------------------------------------
void StageData::_LoadTelop( const XML::Node* pNode )
{
	for( int TelopID = 0; TelopID < pNode->GetChildNum(); TelopID++ )
	{
		const XML::Node* pTelopNode = pNode->GetChild( TelopID );
		
		Label Name = pTelopNode->GetAttr( ATTR_NAME ).GetLabel();

		//	テロップデータ読み込み
		const STAGE_TELOP* pTelop = GetTelop( Name );
		if( !pTelop )
		{
			STAGE_TELOP	Telop;
			_LoadTelop( Telop, pTelopNode );
			m_mapTelop.insert( MapTelop::value_type( Name, Telop ) );
		}
	}	
}

//------------------------------------------------------------------------------
//	アクションの読み込み
//------------------------------------------------------------------------------
void StageData::_LoadAction( STAGE_ACTION& Param, const XML::Node* pNode )
{
	Param.Type	= StrToEnum<STAGE_ACTION_TYPE>( pNode->GetName(), STAGE_ACTION_TYPE_TABLE, ArrayNum(STAGE_ACTION_TYPE_TABLE), STAGE_ACTION_NONE );

	Param.Frame	= pNode->GetAttr( ATTR_FRAME, "-1" ).GetInt();
	Param.Name	= pNode->GetAttr( ATTR_NAME ).GetLabel();
	
	switch( Param.Type )
	{
	case STAGE_ACTION_ENEMY:
	case STAGE_ACTION_EFFECT:
		{
			Param.DataName		= pNode->GetAttr( pNode->GetName() ).GetLabel();
			Param.Pos			= pNode->GetAttr( ATTR_POS ).GetPoint();
			Param.Rot			= pNode->GetAttr( ATTR_ROT ).GetDegree();
		}
		break;
	case STAGE_ACTION_GROUND:
		{
			Param.DataName		= pNode->GetAttr( pNode->GetName() ).GetLabel();
			Param.Pos			= pNode->GetAttr( ATTR_POS ).GetPoint();
			Param.Rot			= pNode->GetAttr( ATTR_ROT ).GetDegree();
			Param.ParamVal		= pNode->GetAttr( ATTR_NO_START ).GetBOOL();
		}
		break;
	case STAGE_ACTION_BGM_PLAY:
		{
			Param.Text			= pNode->GetAttr( ATTR_PATH ).GetStr();
			Param.ParamVal		= pNode->GetAttr( ATTR_VOL ).GetFloat();
		}
		break;
	case STAGE_ACTION_BGM_FADE:
		{
			Param.ParamVal		= pNode->GetAttr( ATTR_TIME ).GetFloat();
		}
		break;
	case STAGE_ACTION_TELOP:
		{
			Param.Pos			= pNode->GetAttr( ATTR_POS ).GetPoint();
			Param.Rot			= pNode->GetAttr( ATTR_ROT ).GetDegree();

			//	テロップデータ読み込み
			const STAGE_TELOP* pTelop = GetTelop( Param.Name );
			if( !pTelop )
			{
				STAGE_TELOP	Telop;
				_LoadTelop( Telop, pNode );
				m_mapTelop.insert( MapTelop::value_type( Param.Name, Telop ) );
			}
		}
		break;
	case STAGE_ACTION_TELOP_TEXT:
		{
			Param.ParamStr		= pNode->GetAttr( ATTR_BONE ).GetLabel();
			Param.Text			= pNode->GetAttr( ATTR_TEXT ).GetStr();
		}
		break;
	case STAGE_ACTION_TELOP_ANIME:
		{
			Param.ParamStr		= pNode->GetAttr( ATTR_ANIME ).GetLabel();
			Param.ParamVal		= pNode->GetAttr( ATTR_INTP ).GetInt();
		}
		break;
	case STAGE_ACTION_PLAYER_POS:
		{
			Param.Pos			= pNode->GetAttr( ATTR_POS ).GetPoint();
			Param.ParamVal		= pNode->GetAttr( ATTR_TIME ).GetInt();
			Param.ParamStr		= pNode->GetAttr( ATTR_SIDE ).GetLabel();
		}
		break;
	case STAGE_ACTION_PLAYER_ACTION:
		{
			Param.Pos			= pNode->GetAttr( ATTR_DIR ).GetPoint();
		}
		break;
	case STAGE_ACTION_PLAYER_MOVE:
	case STAGE_ACTION_PLAYER_ATTACK:
	case STAGE_ACTION_PLAYER_LOCK:
	case STAGE_ACTION_PLAYER_INDICATE:
		{
			Param.ParamVal		= pNode->GetAttr( ATTR_FLAG ).GetBOOL();
		}
		break;
	case STAGE_ACTION_PLAYER_ANIME:
		{
			Param.ParamStr		= pNode->GetAttr( ATTR_ANIME ).GetLabel();
			Param.ParamVal		= pNode->GetAttr( ATTR_INTP ).GetInt();
		}
		break;

	case STAGE_ACTION_GROUND_ANIME:
		{
			Param.ParamStr		= pNode->GetAttr( ATTR_ANIME ).GetLabel();
			Param.ParamVal		= pNode->GetAttr( ATTR_NO_WAIT ).GetBOOL();
		}
		break;

	case STAGE_ACTION_ENEMY_CHILD:
		{
			Param.ParamStr		= pNode->GetAttr( ATTR_CHILD ).GetLabel();
		}
		break;

	case STAGE_ACTION_ENEMY_NOTIFY:
		{
			Param.ParamStr		= pNode->GetAttr( ATTR_PARAM ).GetLabel();
		}
		break;

	case STAGE_ACTION_LOOP:
	case STAGE_ACTION_LOCK_SCORE:
		{
			Param.ParamVal		= pNode->GetAttr( ATTR_TIME ).GetFloat();
		}
		break;

	case STAGE_ACTION_ARRIVE:
		{
			Param.ParamVal		= pNode->GetAttr( ATTR_PARAM ).GetFloat();
		}
		break;

	case STAGE_ACTION_ITEM:
		{
			Param.Pos			= pNode->GetAttr( ATTR_POS ).GetPoint();
			Param.ParamVal		= pNode->GetAttr( ATTR_NUM ).GetFloat();
		}
		break;
	}
	
	//	条件
	for( int ChildID = 0; ChildID < pNode->GetChildNum(); ChildID++ )
	{
		const XML::Node* pCondNode = pNode->GetChild( ChildID );
		if( Label(pCondNode->GetName()) == Label(NODE_CONDITION) )
		{
			STAGE_CONDITION Cond;
			_LoadCondition( Cond, pCondNode );

			Param.ConditionList.push_back( Cond );
		}
	}
}

//------------------------------------------------------------------------------
//	条件の読み込み
//------------------------------------------------------------------------------
void StageData::_LoadCondition( STAGE_CONDITION& Param, const XML::Node* pNode )
{
	Param.Type	= StrToEnum<STAGE_CONDITION_TYPE>( pNode->GetAttr( ATTR_TYPE ).GetStr(), STAGE_CONDITION_TYPE_TABLE, ArrayNum(STAGE_CONDITION_TYPE_TABLE), STAGE_CONDITION_NONE );

	Param.Name	= pNode->GetAttr( ATTR_NAME ).GetLabel();
	Param.Param	= pNode->GetAttr( ATTR_PARAM ).GetInt();
}

//------------------------------------------------------------------------------
//	テロップデータの読み込み
//------------------------------------------------------------------------------
void StageData::_LoadTelop( STAGE_TELOP& Param, const XML::Node* pNode )
{
	Param.SpriteName		= pNode->GetAttr( ATTR_SPRITE ).GetLabel();
	Param.AnimeName			= pNode->GetAttr( ATTR_ANIME ).GetLabel();
	Param.CloseAnimeName	= pNode->GetAttr( ATTR_CLOSE_ANIME ).GetLabel();
	Param.bSkip				= pNode->GetAttr( ATTR_SKIP ).GetBool();
	Param.SkipTime			= pNode->GetAttr( ATTR_SKIP_TIME ).GetInt();
	Param.bAnimeEndClose	= pNode->GetAttr( ATTR_ANIME_END_CLOSE, "true" ).GetBool();
	Param.Time				= pNode->GetAttr( ATTR_TIME ).GetInt();
}

//------------------------------------------------------------------------------
//	敵データの読み込み
//------------------------------------------------------------------------------
void StageData::_LoadEnemy( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pFileNode = pNode->GetChild( i );

		//	読み込む
		const XML::Value* pValue = pFileNode->FindAttr( ATTR_PATH );
		if( pValue && !pValue->IsEmpty() )
		{
			AddResourceMap< EnemyResource >( m_mapEnemyRes, pValue->GetStr(), GetApp()->GetResMng() );
		}
	}
}

//------------------------------------------------------------------------------
//	地形データの読み込み
//------------------------------------------------------------------------------
void StageData::_LoadGround( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pGroundNode = pNode->GetChild( i );

		GroundData* pData = new GroundData;
		MEM_CHECK( pData );

		pData->Load( pGroundNode );

		m_mapGround.insert( MapGround::value_type( pGroundNode->GetAttr( ATTR_NAME ).GetLabel(), pData ) );
	}
}

//------------------------------------------------------------------------------
//	スプライトフレームの読み込み
//------------------------------------------------------------------------------
void StageData::_LoadSprite( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pFileNode = pNode->GetChild( i );

		//	読み込む
		const XML::Value* pValue = pFileNode->FindAttr( ATTR_PATH );
		if( pValue && !pValue->IsEmpty() )
		{
			AddResourceMap< SpriteFrameResource >( m_mapSpriteRes, pValue->GetStr(), GetApp()->GetResMng() );
		}
	}
}

//------------------------------------------------------------------------------
//	エフェクトの読み込み
//------------------------------------------------------------------------------
void StageData::_LoadEffect( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pFileNode = pNode->GetChild( i );

		//	読み込む
		const XML::Value* pValue = pFileNode->FindAttr( ATTR_PATH );
		if( pValue && !pValue->IsEmpty() )
		{
			AddResourceMap< EffectResource >( m_mapEffectRes, pValue->GetStr(), GetApp()->GetResMng() );
		}
	}
}