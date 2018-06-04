#include "stdafx.h"
#include "EnemyData.h"
#include "EnemyStateData.h"

#include "App/App.h"

#include "Resource/ResourceManager.h"
#include "Resource/ResourceUtil.h"
#include "Resource/SoundResource.h"

#include "Attack/AttackData.h"
#include "SpriteFrame/SpriteFrameFile.h"
#include "Effect/EffectFile.h"


namespace
{
	pstr		NODE_ENEMY				= "Enemy";
	pstr		NODE_ATTACK				= "Attack";
	pstr		NODE_EFFECT				= "Effect";
	pstr		NODE_SOUND				= "Sound";
	pstr		NODE_CHILD				= "Child";
	pstr		NODE_STATE				= "State";
	pstr		NODE_ACTION				= "Action";
	pstr		NODE_CONDITION			= "Condition";
	pstr		NODE_TARGET				= "Target";

	pstr		NODE_POS_TARGET			= "PosTarget";
	pstr		NODE_DIR_TARGET			= "DirTarget";

	pstr		ATTR_PATH				= "Path";
	pstr		ATTR_SPRITE				= "Sprite";
	pstr		ATTR_NAME				= "Name";
	pstr		ATTR_PREVIEW_NAME		= "PreviewName";
	pstr		ATTR_TYPE				= "Type";
	pstr		ATTR_PARAM				= "Param";
	pstr		ATTR_INTERVAL			= "Interval";
	pstr		ATTR_NUM				= "Num";
	pstr		ATTR_FLAG				= "Flag";
	pstr		ATTR_COLOR				= "Color";
	pstr		ATTR_FORCE_CHANGE		= "ForceChange";

	pstr		ATTR_LIFE				= "Life";
	pstr		ATTR_DAMAGE_INVINCIBLE	= "DamageInvincible";
	pstr		ATTR_DEFENSE			= "Defense";
	pstr		ATTR_EXHAUST_RATE		= "ExhaustRate";
	pstr		ATTR_REDUCE_VEL			= "ReduceVel";
	pstr		ATTR_REDUCE_KNOCKBACK	= "ReduceKnockback";
	pstr		ATTR_BODY_DAMAGE		= "BodyDamage";
	pstr		ATTR_BODY_DAMAGE_VEL	= "BodyDamageVel";
	pstr		ATTR_ITEM_NUM			= "ItemNum";
	pstr		ATTR_ITEM_SPEED			= "ItemSpeed";
	pstr		ATTR_SCORE_RATE			= "ScoreRate";
	pstr		ATTR_CRASH_KNOCKBACK	= "CrashKnockback";
	pstr		ATTR_CRASH_VEL			= "CrashVel";
	pstr		ATTR_COLLIDE_RADIUS		= "CollideRadius";
	pstr		ATTR_COLLIDE_FRAME		= "CollideFrame";
	pstr		ATTR_HIT				= "Hit";
	pstr		ATTR_GROUND_HIT			= "GroundHit";
	pstr		ATTR_PLAYER_HIT			= "PlayerHit";
	pstr		ATTR_REPULSE			= "Repulse";
	pstr		ATTR_NOT_REPULSE_NOTIFY	= "NotRepulseNotify";
	pstr		ATTR_NO_COUNT			= "NoCount";
	pstr		ATTR_BOSS				= "Boss";
	pstr		ATTR_OUT_FIELD			= "OutField";
	pstr		ATTR_NO_INDICATE		= "NoIndicate";
	pstr		ATTR_DEAD_COLOR			= "DeadColor";

	pstr		ATTR_TIME				= "Time";
	pstr		ATTR_CONTINUE			= "Continue";
	pstr		ATTR_ONCE				= "Once";
	
	pstr		ATTR_ACCEL				= "Accel";
	pstr		ATTR_DECEL				= "Decel";
	pstr		ATTR_MAX_SPEED			= "MaxSpeed";
	pstr		ATTR_SPEED				= "Speed";
	pstr		ATTR_FIX				= "Fix";

	pstr		ATTR_POS				= "Pos";
	pstr		ATTR_ROT				= "Rot";
	pstr		ATTR_DIR				= "Dir";
	pstr		ATTR_BONE				= "Bone";
	pstr		ATTR_INTP				= "Intp";
	pstr		ATTR_VOL				= "Vol";
	pstr		ATTR_ATTACH				= "Attach";
	pstr		ATTR_APPEAR_NAME		= "AppearName";

	pstr		ATTR_RECT				= "Rect";
	pstr		ATTR_POLER				= "Poler";
	pstr		ATTR_RANGE				= "Range";
	pstr		ATTR_AXIS				= "Axis";
	pstr		ATTR_NOT				= "Not";

	pstr		DEFAULT_DEAD_COLOR		= "255,64,64,255";

	const LabelPair<ENEMY_TARGET_TYPE>	ENEMY_TARGET_TABLE[] =
	{
		{ "Fix",	ENEMY_TARGET_FIX,		},
		{ "Player",	ENEMY_TARGET_PLAYER,	},
		{ "Self",	ENEMY_TARGET_SELF,		},
		{ "Enemy",	ENEMY_TARGET_ENEMY,		},
	};

	const LabelPair<ENEMY_TARGET_AXIS_TYPE>	ENEMY_TARGET_AXIS_TABLE[] =
	{
		{ "Abs",	ENEMY_TARGET_AXIS_ABS,	},
		{ "Rel",	ENEMY_TARGET_AXIS_REL,	},
	};

	const LabelPair<ENEMY_MOVE_TYPE>	ENEMY_MOVE_TABLE[] =
	{
		{ "Target",		ENEMY_MOVE_TARGET,	},
		{ "TargetX",	ENEMY_MOVE_TARGET_X	},
		{ "TargetY",	ENEMY_MOVE_TARGET_Y	},
	};
	
	const LabelPair<ENEMY_ROT_TYPE>		ENEMY_ROT_TABLE[] =
	{
		{ "Fix",		ENEMY_ROT_FIX,		},
		{ "Target",		ENEMY_ROT_TARGET,	},
		{ "Move",		ENEMY_ROT_MOVE,		},
	};
	
	const LabelPair<ENEMY_ATTACK_POS_TYPE>	ENEMY_ATTACK_POS_TABLE[] =
	{
		{ "Center",		ENEMY_ATTACK_POS_CENTER,	},
		{ "Bone",		ENEMY_ATTACK_POS_BONE,		},
		{ "Target",		ENEMY_ATTACK_POS_TARGET,	},
	};
	
	const LabelPair<ENEMY_ATTACK_DIR_TYPE>	ENEMY_ATTACK_DIR_TABLE[] =
	{
		{ "Fix",		ENEMY_ATTACK_DIR_FIX,		},
		{ "Straight",	ENEMY_ATTACK_DIR_STRAIGHT,	},
		{ "Target",		ENEMY_ATTACK_DIR_TARGET,	},
		{ "Move",		ENEMY_ATTACK_DIR_MOVE,		},
		{ "Rot",		ENEMY_ATTACK_DIR_ROT,		},
	};

	const StrPair<ENEMY_CONDITION_TYPE>	ENEMY_CONDITION_TABLE[] =
	{
		{ "MoveEnd",			ENEMY_CONDITION_MOVE_END,			},
		{ "RotEnd",				ENEMY_CONDITION_ROT_END,			},
		{ "AttackEnd",			ENEMY_CONDITION_ATTACK_END,			},
		{ "AnimeEnd",			ENEMY_CONDITION_ANIME_END,			},
		{ "AnimeNotify",		ENEMY_CONDITION_ANIME_NOTIFY,		},
		{ "Notify",				ENEMY_CONDITION_NOTIFY,				},
		{ "ChildDefeat",		ENEMY_CONDITION_CHILD_DEFEAT,		},
		{ "ChildAllDefeat",		ENEMY_CONDITION_CHILD_ALL_DEFEAT,	},
		{ "ChildEmpty",			ENEMY_CONDITION_CHILD_EMPTY,		},
		{ "SelfPosX",			ENEMY_CONDITION_SELF_POS_X,			},
		{ "SelfPosY",			ENEMY_CONDITION_SELF_POS_Y,			},
		{ "PlayerRange",		ENEMY_CONDITION_PLAYER_RANGE,		},
		{ "PlayerRangeX",		ENEMY_CONDITION_PLAYER_RANGE_X,		},
		{ "PlayerRangeY",		ENEMY_CONDITION_PLAYER_RANGE_Y,		},
		{ "PlayerAngle",		ENEMY_CONDITION_PLAYER_ANGLE,		},
		{ "PlayerAngleRel",		ENEMY_CONDITION_PLAYER_ANGLE_REL,	},
		{ "PlayerPosX",			ENEMY_CONDITION_PLAYER_POS_X,		},
		{ "PlayerPosY",			ENEMY_CONDITION_PLAYER_POS_Y,		},
		{ "PlayerState",		ENEMY_CONDITION_PLAYER_STATE,		},
		{ "PlayerStateTime",	ENEMY_CONDITION_PLAYER_STATE_TIME,	},
		{ "Frame",				ENEMY_CONDITION_FRAME,				},
		{ "Random",				ENEMY_CONDITION_RANDOM,				},
		{ "StateEnd",			ENEMY_CONDITION_STATE_END,			},
		{ "LifeLess",			ENEMY_CONDITION_LIFE_LESS,			},
		{ "Damage",				ENEMY_CONDITION_DAMAGE,				},
		{ "Knockback",			ENEMY_CONDITION_KNOCKBACK,			},
		{ "Repulse",			ENEMY_CONDITION_REPULSE,			},
		{ "Dead",				ENEMY_CONDITION_DEAD,				},
	};

	const StrPair<ENEMY_ACTION_TYPE>		ENEMY_ACTION_TABLE[] =
	{
		{ "Move",			ENEMY_ACTION_MOVE,			},
		{ "Rot",			ENEMY_ACTION_ROT,			},
		{ "Attack",			ENEMY_ACTION_ATTACK,		},
		{ "Summon",			ENEMY_ACTION_SUMMON,		},
		{ "Warp",			ENEMY_ACTION_WARP,			},
		{ "EndMove",		ENEMY_ACTION_END_MOVE,		},
		{ "EndRot",			ENEMY_ACTION_END_ROT,		},
		{ "EndAttack",		ENEMY_ACTION_END_ATTACK,	},
		{ "EndAction",		ENEMY_ACTION_END_ACTION,	},
		{ "Invincible",		ENEMY_ACTION_INVINCIBLE,	},
		{ "Hide",			ENEMY_ACTION_HIDE,			},
		{ "NoDamage",		ENEMY_ACTION_NO_DAMAGE,		},
		{ "Hit",			ENEMY_ACTION_HIT,			},
		{ "GroundHit",		ENEMY_ACTION_GROUND_HIT,	},
		{ "OutField",		ENEMY_ACTION_OUT_FIELD,		},
		{ "NoCount",		ENEMY_ACTION_NO_COUNT,		},
		{ "Repulse",		ENEMY_ACTION_REPULSE,		},
		{ "Anime",			ENEMY_ACTION_CHANGE_ANIME,	},
		{ "AnimePause",		ENEMY_ACTION_PAUSE_ANIME,	},
		{ "State",			ENEMY_ACTION_CHANGE_STATE,	},
		{ "Effect",			ENEMY_ACTION_EFFECT,		},
		{ "Sound",			ENEMY_ACTION_SOUND,			},
		{ "Color",			ENEMY_ACTION_COLOR,			},
		{ "AddLife",		ENEMY_ACTION_ADD_LIFE,		},
		{ "AddDefense",		ENEMY_ACTION_ADD_DEFENSE,	},
		{ "LockScore",		ENEMY_ACTION_LOCK_SCORE,	},
		{ "ParentNotify",	ENEMY_ACTION_PARENT_NOTIFY,	},
		{ "ChildNotify",	ENEMY_ACTION_CHILD_NOTIFY,	},
		{ "StageNotify",	ENEMY_ACTION_STAGE_NOTIFY,	},
		{ "Destroy",		ENEMY_ACTION_DESTROY,		},
		{ "Escape",			ENEMY_ACTION_ESCAPE,		},
	};
};


////////////////////////////////////////////////////////////////////////////////
//
//	�G�f�[�^
//
////////////////////////////////////////////////////////////////////////////////
EnemyData::EnemyData( void )
{

}
EnemyData::~EnemyData()
{
	ReleaseMap( m_mapSpriteRes );
	ReleaseMap( m_mapAttackRes );
	ReleaseMap( m_mapEnemyRes );
	ReleaseMap( m_mapEffectRes );
	ReleaseMap( m_mapSoundRes );
	DeleteMap( m_mapState );
}

//******************************************************************************
//	�ǂݍ���
//******************************************************************************
bool EnemyData::Load( pstr pPath )
{
	//	�t�@�C���ǂݍ���
	XML Xml;
	if( !Xml.Load( pPath ) )
	{
		return false;
	}

	//	�p�����[�^
	const XML::Node* pParamNode = Xml.GetRoot().FindChild( NODE_ENEMY );
	if( pParamNode ) _LoadParam( pParamNode );

	//	�U��
	const XML::Node* pAttackNode = Xml.GetRoot().FindChild( NODE_ATTACK );
	if( pAttackNode ) _LoadAttack( pAttackNode );

	//	�G
	const XML::Node* pEnemyNode = Xml.GetRoot().FindChild( NODE_CHILD );
	if( pEnemyNode ) _LoadEnemy( pEnemyNode );

	//	�G�t�F�N�g
	const XML::Node* pEffectNode = Xml.GetRoot().FindChild( NODE_EFFECT );
	if( pEffectNode ) _LoadEffect( pEffectNode );

	//	�T�E���h
	const XML::Node* pSoundNode = Xml.GetRoot().FindChild( NODE_SOUND );
	if( pSoundNode ) _LoadSound( pSoundNode );

	//	�X�e�[�g
	const XML::Node* pStateNode = Xml.GetRoot().FindChild( NODE_STATE );
	if( pStateNode ) _LoadState( pStateNode );

	return true;
}

//******************************************************************************
//	�X�e�[�g�f�[�^�擾
//******************************************************************************
const EnemyStateData* EnemyData::GetStateData( Label Name ) const
{
	MapState::const_iterator it = m_mapState.find( Name );
	if( it != m_mapState.end() )
	{
		return it->second;
	}
	return NULL;
}

//******************************************************************************
//	�n���h���[�X�e�[�g���𓾂�
//******************************************************************************
Label EnemyData::GetHandlerStateName( Label Name ) const
{
	MapHandlerState::const_iterator it = m_mapHandlerState.find( Name );
	if( it != m_mapHandlerState.end() )
	{
		return it->second;
	}
	return Label();
}

//******************************************************************************
//	�X�v���C�g�t�@�C���擾
//******************************************************************************
const SpriteFrameFile* EnemyData::GetSpriteFrameFile( Label Name ) const
{
	return FindResourceMap<SpriteFrameFile, SpriteFrameResource>( m_mapSpriteRes, Name );
}

//******************************************************************************
//	�U���f�[�^�擾
//******************************************************************************
const AttackData* EnemyData::GetAttackData( Label Name ) const
{
	return FindResourceMap<AttackData, AttackResource>( m_mapAttackRes, Name );
}

//******************************************************************************
//	�G�f�[�^�擾
//******************************************************************************
const EnemyData* EnemyData::GetEnemyData( Label Name ) const
{
	return FindResourceMap<EnemyData, EnemyResource>( m_mapEnemyRes, Name );
}

//******************************************************************************
//	�G�t�F�N�g�f�[�^�擾
//******************************************************************************
const EffectFile* EnemyData::GetEffectFile( Label Name ) const
{
	return FindResourceMap<EffectFile, EffectResource>( m_mapEffectRes, Name );
}

//******************************************************************************
//	�T�E���h�t�@�C���擾
//******************************************************************************
const SoundFile* EnemyData::GetSoundFile( Label Name ) const
{
	return FindResourceMap<SoundFile, SoundResource>( m_mapSoundRes, Name );
}

//------------------------------------------------------------------------------
//	�p�����[�^�̓ǂݍ���
//------------------------------------------------------------------------------
void EnemyData::_LoadParam( const XML::Node* pNode )
{
	_ASSERT( pNode );
	
	m_Param.Life				= pNode->GetAttr( ATTR_LIFE ).GetInt();
	m_Param.Decel				= pNode->GetAttr( ATTR_DECEL ).GetFloat();
	m_Param.DamageInvincible	= pNode->GetAttr( ATTR_DAMAGE_INVINCIBLE ).GetInt();
	m_Param.Defense				= pNode->GetAttr( ATTR_DEFENSE, "1.0" ).GetFloat();
	m_Param.ExhaustRate			= pNode->GetAttr( ATTR_EXHAUST_RATE, "1.0" ).GetFloat();
	m_Param.ReduceVel			= pNode->GetAttr( ATTR_REDUCE_VEL, "1.0" ).GetFloat();
	m_Param.ReduceKnockback		= pNode->GetAttr( ATTR_REDUCE_KNOCKBACK, "1.0" ).GetFloat();
	m_Param.BodyDamage			= pNode->GetAttr( ATTR_BODY_DAMAGE ).GetFloat();
	m_Param.BodyDamageVel		= pNode->GetAttr( ATTR_BODY_DAMAGE_VEL ).GetFloat();
	m_Param.ItemNum				= pNode->GetAttr( ATTR_ITEM_NUM ).GetInt();
	m_Param.ItemSpeed			= pNode->GetAttr( ATTR_ITEM_SPEED ).GetFloat();
	m_Param.ScoreRate			= pNode->GetAttr( ATTR_SCORE_RATE, "1.0" ).GetFloat();
	m_Param.CrashKnockback		= pNode->GetAttr( ATTR_CRASH_KNOCKBACK ).GetInt();
	m_Param.CrashVel			= pNode->GetAttr( ATTR_CRASH_VEL ).GetFloat();
	m_Param.CollideRadius		= pNode->GetAttr( ATTR_COLLIDE_RADIUS ).GetFloat();
	m_Param.bCollideFrame		= pNode->GetAttr( ATTR_COLLIDE_FRAME ).GetBool();
	m_Param.bHitEnable			= pNode->GetAttr( ATTR_HIT, "true" ).GetBool();
	m_Param.bGroundHit			= pNode->GetAttr( ATTR_GROUND_HIT, "true" ).GetBool();
	m_Param.bPlayerHit			= pNode->GetAttr( ATTR_PLAYER_HIT, "true" ).GetBool();
	m_Param.bRepulse			= pNode->GetAttr( ATTR_REPULSE ).GetBool();
	m_Param.bNotRepulseNotify	= pNode->GetAttr( ATTR_NOT_REPULSE_NOTIFY ).GetBool();
	m_Param.bNoCount			= pNode->GetAttr( ATTR_NO_COUNT ).GetBool();
	m_Param.bBoss				= pNode->GetAttr( ATTR_BOSS ).GetBool();
	m_Param.bOutField			= pNode->GetAttr( ATTR_OUT_FIELD ).GetBool();
	m_Param.bNoIndicate			= pNode->GetAttr( ATTR_NO_INDICATE ).GetBool();
	m_Param.DeadColor			= pNode->GetAttr( ATTR_DEAD_COLOR, DEFAULT_DEAD_COLOR ).GetColor();
	m_Param.PreviewName			= pNode->GetAttr( ATTR_PREVIEW_NAME ).GetStr();

	m_Param.SpriteName			= AddResourceMap< SpriteFrameResource >( m_mapSpriteRes, pNode->GetAttr( ATTR_SPRITE ).GetStr(), GetApp()->GetResMng() );
}

//------------------------------------------------------------------------------
//	�U���f�[�^�̓ǂݍ���
//------------------------------------------------------------------------------
void EnemyData::_LoadAttack( const XML::Node* pNode )
{
	_ASSERT( pNode );

	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pAttackNode = pNode->GetChild( i );
		
		AddResourceMap< AttackResource >( m_mapAttackRes, pAttackNode->GetAttr( ATTR_PATH ).GetStr(), GetApp()->GetResMng() );
	}
}

//------------------------------------------------------------------------------
//	�G�f�[�^�̓ǂݍ���
//------------------------------------------------------------------------------
void EnemyData::_LoadEnemy( const XML::Node* pNode )
{
	_ASSERT( pNode );

	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pAttackNode = pNode->GetChild( i );
		
		AddResourceMap< EnemyResource >( m_mapEnemyRes, pAttackNode->GetAttr( ATTR_PATH ).GetStr(), GetApp()->GetResMng() );
	}
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�̓ǂݍ���
//------------------------------------------------------------------------------
void EnemyData::_LoadEffect( const XML::Node* pNode )
{
	_ASSERT( pNode );

	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pAttackNode = pNode->GetChild( i );
		
		AddResourceMap< EffectResource >( m_mapEffectRes, pAttackNode->GetAttr( ATTR_PATH ).GetStr(), GetApp()->GetResMng() );
	}
}

//------------------------------------------------------------------------------
//	�T�E���h�ǂݍ���
//------------------------------------------------------------------------------
void EnemyData::_LoadSound( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pFileNode = pNode->GetChild( i );

		//	�ǂݍ���
		const XML::Value* pValue = pFileNode->FindAttr( ATTR_PATH );
		if( pValue && !pValue->IsEmpty() )
		{
			AddResourceMap< SoundResource >( m_mapSoundRes, pValue->GetStr(), GetApp()->GetResMng() );
		}
	}
}

//------------------------------------------------------------------------------
//	�X�e�[�g�̓ǂݍ���
//------------------------------------------------------------------------------
void EnemyData::_LoadState( const XML::Node* pNode )
{
	_ASSERT( pNode );

	//	�n���h���X�e�[�g
	_LoadHandlerState( pNode );

	for( int StateID = 0; StateID < pNode->GetChildNum(); StateID++ )
	{
		const XML::Node* pStateNode = pNode->GetChild( StateID );
		
		Label Name = pStateNode->GetAttr( ATTR_NAME ).GetStr();
		if( Name.IsEmpty() ) continue;

		//	���[�g�������̎��A�ŏ��̂��̂�����
		if( m_mapHandlerState[ENEMY_HANDLER_STATE_ROOT].IsEmpty() )
		{
			m_mapHandlerState[ENEMY_HANDLER_STATE_ROOT] = Name;
		}

		EnemyStateData* pStateData = new EnemyStateData();
		MEM_CHECK( pStateData );

		//	�A�N�V�������o��
		for( int ActionID = 0; ActionID < pStateNode->GetChildNum(); ActionID++ )
		{
			const XML::Node* pActionNode = pStateNode->GetChild( ActionID );

			ENEMY_ACTION* pAction = new ENEMY_ACTION;
			MEM_CHECK( pAction );

			_LoadAction( pActionNode, *pAction );
			pStateData->AddAction( pAction );
		}

		m_mapState.insert( MapState::value_type(Name, pStateData) );
	}
}

//------------------------------------------------------------------------------
//	�n���h���X�e�[�g�ǂݍ���
//------------------------------------------------------------------------------
void EnemyData::_LoadHandlerState( const XML::Node* pNode )
{
	_ASSERT( pNode );

	for( int i = 0; i < pNode->GetAttrNum(); i++ )
	{
		Label Name = pNode->GetAttrName( i );
		const XML::Value& Value = pNode->GetAttrVal( i );

		m_mapHandlerState.insert( MapHandlerState::value_type(Name, Value.GetLabel()) );
	}
}

//------------------------------------------------------------------------------
//	�A�N�V�����p�����[�^�̓ǂݍ���
//------------------------------------------------------------------------------
void EnemyData::_LoadAction( const XML::Node* pNode, ENEMY_ACTION& Param )
{
	_ASSERT( pNode );

	Param.Type = StrToEnum<ENEMY_ACTION_TYPE>( pNode->GetName(), ENEMY_ACTION_TABLE, ArrayNum(ENEMY_ACTION_TABLE), ENEMY_ACTION_NONE );
	_ASSERT( Param.Type != ENEMY_ACTION_NONE );

	Param.Time		= pNode->GetAttr( ATTR_TIME ).GetInt();
	Param.bContinue	= pNode->GetAttr( ATTR_CONTINUE ).GetBool();
	Param.bOnce		= pNode->GetAttr( ATTR_ONCE, "true" ).GetBool();

	//	�A�N�V�������̃p�����[�^
	switch( Param.Type )
	{
	case ENEMY_ACTION_MOVE:		_LoadMoveAction( pNode, Param.Move );		break;
	case ENEMY_ACTION_ROT:		_LoadRotAction( pNode, Param.Rot );			break;
	case ENEMY_ACTION_ATTACK:	_LoadAttackAction( pNode, Param.Attack );	break;
	case ENEMY_ACTION_SUMMON:	_LoadSummonAction( pNode, Param.Summon );	break;
	case ENEMY_ACTION_WARP:		_LoadWarpAction( pNode, Param.Warp );		break;

	case ENEMY_ACTION_INVINCIBLE:
	case ENEMY_ACTION_HIDE:
	case ENEMY_ACTION_NO_DAMAGE:
	case ENEMY_ACTION_HIT:
	case ENEMY_ACTION_GROUND_HIT:
	case ENEMY_ACTION_OUT_FIELD:
	case ENEMY_ACTION_NO_COUNT:
	case ENEMY_ACTION_REPULSE:
	case ENEMY_ACTION_PAUSE_ANIME:
		{
			Param.bFlag	= pNode->GetAttr( ATTR_FLAG ).GetBool();
		}
		break;
	case ENEMY_ACTION_CHANGE_ANIME:
		{
			pNode->GetAttr( ATTR_NAME ).GetLabelStr( Param.Name );
			Param.Param		= pNode->GetAttr( ATTR_INTP ).GetFloat();
		}
		break;
	case ENEMY_ACTION_CHANGE_STATE:
		{
			pNode->GetAttr( ATTR_NAME ).GetLabelStr( Param.Name );
			Param.bFlag	= pNode->GetAttr( ATTR_FORCE_CHANGE ).GetBool();
		}
		break;
	case ENEMY_ACTION_END_ATTACK:
	case ENEMY_ACTION_EFFECT:
	case ENEMY_ACTION_PARENT_NOTIFY:
	case ENEMY_ACTION_STAGE_NOTIFY:
		{
			pNode->GetAttr( ATTR_NAME ).GetLabelStr( Param.Name );
		}
		break;
	case ENEMY_ACTION_CHILD_NOTIFY:
		{
			pNode->GetAttr( ATTR_NAME ).GetLabelStr( Param.Name );
			pNode->GetAttr( ATTR_APPEAR_NAME ).GetLabelStr( Param.ParamStr );
		}
		break;
	case ENEMY_ACTION_SOUND:
		{
			pNode->GetAttr( ATTR_NAME ).GetLabelStr( Param.Name );
			Param.Param		= pNode->GetAttr( ATTR_VOL ).GetFloat();
		}
		break;
	case ENEMY_ACTION_COLOR:	
		{
			Param.Col = pNode->GetAttr( ATTR_COLOR ).GetColor();
		}
		break;
	case ENEMY_ACTION_ADD_LIFE:	
	case ENEMY_ACTION_ADD_DEFENSE:	
		{
			Param.Param = pNode->GetAttr( ATTR_PARAM ).GetFloat();
		}
		break;
	case ENEMY_ACTION_LOCK_SCORE:
		{
			Param.Param	= pNode->GetAttr( ATTR_TIME ).GetFloat();
		}
		break;
	}
	
	//	����
	for( int ChildID = 0; ChildID < pNode->GetChildNum(); ChildID++ )
	{
		const XML::Node* pCondNode = pNode->GetChild( ChildID );
		if( Label(pCondNode->GetName()) == Label(NODE_CONDITION) )
		{
			ENEMY_CONDITION Cond;
			_LoadCondition( pCondNode, Cond );

			Param.ConditionList.push_back( Cond );
		}
	}
}

//------------------------------------------------------------------------------
//	�ړ��A�N�V�����̓ǂݍ���
//------------------------------------------------------------------------------
void EnemyData::_LoadMoveAction( const XML::Node* pNode, ENEMY_MOVE_ACTION& Param )
{
	_ASSERT( pNode );

	Param.Type		= LabelToEnum<ENEMY_MOVE_TYPE>( pNode->GetAttr( ATTR_TYPE ).GetStr(), ENEMY_MOVE_TABLE, ArrayNum(ENEMY_MOVE_TABLE), ENEMY_MOVE_NONE );
	_ASSERT( Param.Type != ENEMY_MOVE_NONE );

	Param.Accel		= pNode->GetAttr( ATTR_ACCEL ).GetFloat();
	Param.Decel		= pNode->GetAttr( ATTR_DECEL ).GetFloat();
	Param.MaxSpeed	= pNode->GetAttr( ATTR_MAX_SPEED ).GetFloat();

	//	�ڕW
	const XML::Node* pTargetNode = pNode->FindChild( NODE_TARGET );
	if( pTargetNode )
	{
		_LoadTarget( pTargetNode, Param.Target );
	}
}

//------------------------------------------------------------------------------
//	����A�N�V�����̓ǂݍ���
//------------------------------------------------------------------------------
void EnemyData::_LoadRotAction( const XML::Node* pNode, ENEMY_ROT_ACTION& Param )
{
	_ASSERT( pNode );

	Param.Type		= LabelToEnum<ENEMY_ROT_TYPE>( pNode->GetAttr( ATTR_TYPE ).GetStr(), ENEMY_ROT_TABLE, ArrayNum(ENEMY_ROT_TABLE), ENEMY_ROT_NONE );
	_ASSERT( Param.Type != ENEMY_ROT_NONE );

	Param.Speed		= pNode->GetAttr( ATTR_SPEED ).GetDegree();

	if( Param.Type == ENEMY_ROT_TARGET )
	{
		//	�ڕW
		const XML::Node* pTargetNode = pNode->FindChild( NODE_TARGET );
		if( pTargetNode )
		{
			_LoadTarget( pTargetNode, Param.Target );
		}
	}
	else if( Param.Type == ENEMY_ROT_FIX )
	{
		//	�Œ�p�x
		Param.Fix		= pNode->GetAttr( ATTR_FIX ).GetDegree();
	}
}

//------------------------------------------------------------------------------
//	�U���A�N�V�����̓ǂݍ���
//------------------------------------------------------------------------------
void EnemyData::_LoadAttackAction( const XML::Node* pNode, ENEMY_ATTACK_ACTION& Param )
{
	_ASSERT( pNode );

	Param.AppearNum	= pNode->GetAttr( ATTR_NUM ).GetInt();
	Param.Interval	= pNode->GetAttr( ATTR_INTERVAL ).GetInt();

	Param.Pos.Type = LabelToEnum<ENEMY_ATTACK_POS_TYPE>( pNode->GetAttr( ATTR_POS ).GetStr(), ENEMY_ATTACK_POS_TABLE, ArrayNum(ENEMY_ATTACK_POS_TABLE), ENEMY_ATTACK_POS_CENTER );
	Param.Dir.Type = LabelToEnum<ENEMY_ATTACK_DIR_TYPE>( pNode->GetAttr( ATTR_DIR ).GetStr(), ENEMY_ATTACK_DIR_TABLE, ArrayNum(ENEMY_ATTACK_DIR_TABLE), ENEMY_ATTACK_DIR_STRAIGHT );

	const XML::Value* pAttrName = pNode->FindAttr( ATTR_NAME );
	const XML::Value* pAttrPath = pNode->FindAttr( ATTR_PATH );

	//	���O�w��
	if( pAttrName )
	{
		pAttrName->GetLabelStr( Param.Name );
	}
	//	�p�X�w��
	else if( pAttrPath )
	{
		Label Name = AddResourceMap< AttackResource >( m_mapAttackRes, pAttrPath->GetStr(), GetApp()->GetResMng() );
		Name.GetStr( Param.Name );
	}
	//	����
	else
	{
		return;
	}

	//	�����ʒu
	if( Param.Pos.Type == ENEMY_ATTACK_POS_TARGET )
	{
		//	�ڕW
		const XML::Node* pTargetNode = pNode->FindChild( NODE_POS_TARGET );
		if( pTargetNode )
		{
			_LoadTarget( pTargetNode, Param.Pos.Target );
		}
	}
	else if( Param.Pos.Type == ENEMY_ATTACK_POS_BONE )
	{
		//	�{�[����
		pNode->GetAttr( ATTR_BONE ).GetLabelStr( Param.Pos.Bone );
	}

	//	�������
	if( Param.Dir.Type == ENEMY_ATTACK_DIR_TARGET )
	{
		//	�ڕW
		const XML::Node* pTargetNode = pNode->FindChild( NODE_DIR_TARGET );
		if( pTargetNode )
		{
			_LoadTarget( pTargetNode, Param.Dir.Target );
		}
	}
	else if( Param.Dir.Type == ENEMY_ROT_FIX )
	{
		//	�Œ�p�x
		Param.Dir.Fix = pNode->GetAttr( ATTR_FIX ).GetDegree();
	}
}

//------------------------------------------------------------------------------
//	�����A�N�V�����̓ǂݍ���
//------------------------------------------------------------------------------
void EnemyData::_LoadSummonAction( const XML::Node* pNode, ENEMY_SUMMON_ACTION& Param )
{
	_ASSERT( pNode );

	Param.bAttach	= pNode->GetAttr( ATTR_ATTACH ).GetBool();
	Point Pos		= pNode->GetAttr( ATTR_POS ).GetPoint();
	Param.X			= Pos.x;
	Param.Y			= Pos.y;
	Param.Rot		= pNode->GetAttr( ATTR_ROT ).GetDegree();

	pNode->GetAttr( ATTR_BONE ).GetLabelStr( Param.BoneName );
	pNode->GetAttr( ATTR_APPEAR_NAME ).GetLabelStr( Param.AppearName );

	const XML::Value* pAttrName = pNode->FindAttr( ATTR_NAME );
	const XML::Value* pAttrPath = pNode->FindAttr( ATTR_PATH );

	//	���O�w��
	if( pAttrName )
	{
		pAttrName->GetLabelStr( Param.Name );
	}
	//	�p�X�w��
	else if( pAttrPath )
	{
		Label Name = AddResourceMap< EnemyResource >( m_mapEnemyRes, pNode->GetAttr( ATTR_PATH ).GetStr(), GetApp()->GetResMng() );
		Name.GetStr( Param.Name );
	}
	//	����
	else
	{
		return;
	}
}

//------------------------------------------------------------------------------
//	���[�v�A�N�V�����̓ǂݍ���
//------------------------------------------------------------------------------
void EnemyData::_LoadWarpAction( const XML::Node* pNode, ENEMY_WARP_ACTION& Param )
{
	//	�ڕW
	const XML::Node* pTargetNode = pNode->FindChild( NODE_TARGET );
	if( pTargetNode )
	{
		_LoadTarget( pTargetNode, Param.Target );
	}
}

//------------------------------------------------------------------------------
//	�ڕW�p�����[�^�̓ǂݍ���
//------------------------------------------------------------------------------
void EnemyData::_LoadTarget( const XML::Node* pNode, ENEMY_TARGET& Param )
{
	_ASSERT( pNode );

	Param.Type		= LabelToEnum<ENEMY_TARGET_TYPE>( pNode->GetAttr( ATTR_TYPE ).GetStr(), ENEMY_TARGET_TABLE, ArrayNum(ENEMY_TARGET_TABLE), ENEMY_TARGET_NONE );
	Param.Interval	= pNode->GetAttr( ATTR_INTERVAL ).GetInt();
	_ASSERT( Param.Type != ENEMY_TARGET_NONE );

	//	����W�^�C�v
	const XML::Value*	pRect	= pNode->FindAttr( ATTR_RECT );
	const XML::Value*	pPoler	= pNode->FindAttr( ATTR_POLER );

	//	�����_���͈�
	Point RandRange		= pNode->GetAttr( ATTR_RANGE ).GetPoint();
	Param.RandRange[0]	= RandRange.x;
	Param.RandRange[1]	= RandRange.y;

	//	�����n
	if( pRect )
	{
		Param.PosType	= ENEMY_TARGET_POS_RECT;
		Point Pos		= pRect->GetPoint();
		Param.X			= Pos.x;
		Param.Y			= Pos.y;
	}
	//	�ɍ��W�n
	else if( pPoler )
	{
		Param.PosType	= ENEMY_TARGET_POS_POLER;
		Point Pos		= pPoler->GetPoint();
		Param.Ang		= Angle::ToRad( Pos.x );
		Param.Len		= Pos.y;

		Param.RandRange[0]	= Angle::ToRad( Param.RandRange[0] );
	}
	//	�I�t�Z�b�g�Ȃ�
	else
	{
		Param.PosType	= ENEMY_TARGET_POS_NONE;
		ZeroArray( Param.Pos );
	}
	
	//	�t���O��
	Param.AxisType	= LabelToEnum<ENEMY_TARGET_AXIS_TYPE>( pNode->GetAttr( ATTR_AXIS ).GetStr(), ENEMY_TARGET_AXIS_TABLE, ArrayNum(ENEMY_TARGET_AXIS_TABLE), ENEMY_TARGET_AXIS_ABS );
	Param.bOutField	= pNode->GetAttr( ATTR_OUT_FIELD ).GetBool();

	//	�^�[�Q�b�g��
	if( Param.Type == ENEMY_TARGET_ENEMY )
	{
		Label Name( pNode->GetAttr( ATTR_NAME ).GetStr() );
		Name.GetStr( Param.Name );
	}
}

//------------------------------------------------------------------------------
//	�����p�����[�^�̓ǂݍ���
//------------------------------------------------------------------------------
void EnemyData::_LoadCondition( const XML::Node* pNode, ENEMY_CONDITION& Param )
{
	_ASSERT( pNode );

	Param.Type = StrToEnum<ENEMY_CONDITION_TYPE>( pNode->GetAttr( ATTR_TYPE ).GetStr(), ENEMY_CONDITION_TABLE, ArrayNum(ENEMY_CONDITION_TABLE), ENEMY_CONDITION_NONE );
	if( Param.Type == ENEMY_CONDITION_NONE ) return;

	const XML::Value& Val = pNode->GetAttr( ATTR_PARAM );

	Param.bNot	= pNode->GetAttr( ATTR_NOT ).GetBool();

	switch( Param.Type )
	{
	case ENEMY_CONDITION_CHILD_DEFEAT:
	case ENEMY_CONDITION_NOTIFY:
	case ENEMY_CONDITION_ANIME_NOTIFY:
	case ENEMY_CONDITION_PLAYER_STATE:
		{
			Val.GetLabelStr( Param.Name );
			break;
		}
	case ENEMY_CONDITION_SELF_POS_X:
	case ENEMY_CONDITION_SELF_POS_Y:
	case ENEMY_CONDITION_PLAYER_RANGE:
	case ENEMY_CONDITION_PLAYER_RANGE_X:
	case ENEMY_CONDITION_PLAYER_RANGE_Y:
	case ENEMY_CONDITION_PLAYER_POS_X:
	case ENEMY_CONDITION_PLAYER_POS_Y:
		{
			Point Pt = Val.GetPoint();
			Param.Range[0] = Pt.x;
			Param.Range[1] = Pt.y;
			break;
		}
	case ENEMY_CONDITION_PLAYER_ANGLE:
	case ENEMY_CONDITION_PLAYER_ANGLE_REL:
		{
			Point Pt = Val.GetPoint();
			Param.Range[0] = Angle::ToRad( Pt.x );
			Param.Range[1] = Angle::ToRad( Pt.y );
			break;
		}
	case ENEMY_CONDITION_FRAME:
	case ENEMY_CONDITION_RANDOM:
	case ENEMY_CONDITION_LIFE_LESS:
	case ENEMY_CONDITION_PLAYER_STATE_TIME:
		{
			Param.Param = Val.GetFloat();
			break;
		}
	case ENEMY_CONDITION_DAMAGE:
	case ENEMY_CONDITION_KNOCKBACK:
	case ENEMY_CONDITION_REPULSE:
	case ENEMY_CONDITION_DEAD:
		{
			Param.bFlag	= Val.GetBool();
			break;
		}
	}
}
