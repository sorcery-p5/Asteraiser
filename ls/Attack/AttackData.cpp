#include "stdafx.h"
#include "AttackData.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"
#include "Resource/ResourceUtil.h"
#include "Resource/SoundResource.h"

#include "Effect/EffectFile.h"
#include "TimeKey/TimeKey.h"

namespace
{	
	pstr	NODE_ATTACK				= "Attack";
	pstr	NODE_EFFECT				= "Effect";
	pstr	NODE_TIME_KEY			= "TimeKey";
	pstr	NODE_CONCURRENT			= "Concurrent";
	pstr	NODE_FILE				= "File";

	pstr	NODE_BULLET				= "Bullet";
	pstr	NODE_HOMING				= "Homing";
	pstr	NODE_LASER				= "Laser";
	pstr	NODE_BLADE				= "Blade";
	pstr	NODE_SHIELD				= "Shield";
	pstr	NODE_RULER				= "Ruler";

	pstr	NODE_COLOR				= "Color";
	pstr	NODE_HIT				= "Hit";
	pstr	NODE_RADIUS				= "Radius";
	pstr	NODE_LENGTH				= "Length";
	pstr	NODE_SPEED				= "Speed";
	pstr	NODE_ROT_SPEED			= "RotSpeed";
	pstr	NODE_ACCEL				= "Accel";

	pstr	ATTR_EFFECT				= "Effect";
	pstr	ATTR_HIT_EFFECT			= "HitEffect";
	pstr	ATTR_APPEAR_EFFECT		= "AppearEffect";
	pstr	ATTR_APPEAR_SOUND		= "AppearSound";
	pstr	ATTR_ANGLE_RANGE		= "AngleRange";
	pstr	ATTR_DAMAGE				= "Damage";
	pstr	ATTR_EXHAUST			= "Exhaust";
	pstr	ATTR_HIT_VEL			= "HitVel";
	pstr	ATTR_HIT_VEL_TYPE		= "HitVelType";
	pstr	ATTR_END				= "End";
	pstr	ATTR_ITEM_NUM			= "ItemNum";
	pstr	ATTR_ITEM_SPEED			= "ItemSpeed";
	pstr	ATTR_SCORE_RATE			= "ScoreRate";
	pstr	ATTR_NOT_PLAYER_HIT		= "NotPlayerHit";
	pstr	ATTR_NOT_ATTACK_HIT		= "NotAttackHit";
	pstr	ATTR_NOT_GROUND_HIT		= "NotGroundHit";
	pstr	ATTR_APPEAR_SOUND_VOL	= "AppearSoundVol";
	pstr	ATTR_TIMEKEY_LOOP_FRAME	= "TimeKeyLoopFrame";
	pstr	ATTR_OWNER_ERASE		= "OwnerErase";
	pstr	ATTR_NOT_NOTIFY			= "NotNotify";

	pstr	ATTR_PATH				= "Path";

	pstr	ATTR_NUM				= "Num";
	pstr	ATTR_OFFSET				= "Offset";
	pstr	ATTR_RANGE				= "Range";

	pstr	ATTR_SPEED				= "Speed";
	pstr	ATTR_MIN_SPEED			= "MinSpeed";
	pstr	ATTR_MAX_SPEED			= "MaxSpeed";
	pstr	ATTR_ACCEL				= "Accel";
	pstr	ATTR_DECEL				= "Decel";
	pstr	ATTR_ROT_SPEED			= "RotSpeed";
	pstr	ATTR_MIN_ROT_SPEED		= "MinRotSpeed";
	pstr	ATTR_MAX_ROT_SPEED		= "MaxRotSpeed";
	pstr	ATTR_ROT_ACCEL			= "RotAccel";
	pstr	ATTR_ACCEL_ANGLE		= "AccelAngle";
	pstr	ATTR_ROT_ACCEL_ANGLE	= "RotAccelAngle";
	pstr	ATTR_SEARCH_TIME		= "SearchTime";
	pstr	ATTR_START_TIME			= "StartTime";
	pstr	ATTR_LENGTH				= "Length";
	pstr	ATTR_FRONT_LENGTH		= "FrontLength";
	pstr	ATTR_RADIUS				= "Radius";
	pstr	ATTR_OUT_RADIUS			= "OutRadius";
	pstr	ATTR_PIERCE				= "Pierce";
	pstr	ATTR_REPULSE			= "Repulse";
	pstr	ATTR_SMASH				= "Smash";
	pstr	ATTR_APPEAR				= "Appear";
	pstr	ATTR_ENERGY				= "Energy";
	pstr	ATTR_REPULSE_EXHAUST	= "RepulseExhaust";
	pstr	ATTR_DEFENSE			= "Defense";
	pstr	ATTR_ATTACH				= "Attach";
	pstr	ATTR_HIDE				= "Hide";
	pstr	ATTR_HIT_END_FRAME		= "HitEndFrame";
	pstr	ATTR_ATTACK_FRAME		= "AttackFrame";
	pstr	ATTR_CRASH_EFFECT		= "CrashEffect";
	pstr	ATTR_BREAK_EFFECT		= "BreakEffect";
	pstr	ATTR_REPULSE_EFFECT		= "RepulseEffect";
	pstr	ATTR_ATTACK_EFFECT		= "AttackEffect";
	pstr	ATTR_END_EFFECT			= "EndEffect";

	pstr	ATTR_FRAME				= "Frame";
	pstr	ATTR_INTP				= "Intp";
	pstr	ATTR_FLAG				= "Flag";
	pstr	ATTR_VALUE				= "Value";
	pstr	ATTR_NAME				= "Name";
	pstr	ATTR_COLOR				= "Color";
	pstr	ATTR_ROT				= "Rot";
		
	pstr	COLOR_DEFAULT			= "255,255,255,255";

	pstr ATTACK_TIME_KEY_TABLE[] =
	{
		NODE_EFFECT,
		NODE_ATTACK,
		NODE_COLOR,
		NODE_HIT,

		NODE_RADIUS,
		NODE_LENGTH,

		NODE_SPEED,
		NODE_ROT_SPEED,
		NODE_ACCEL,
	};

	pstr ATTACK_VEL_TYPE_TABLE[] =
	{
		"Normal",
		"Dir",
	};
}


////////////////////////////////////////////////////////////////////////////////
//
//	�U���f�[�^
//
////////////////////////////////////////////////////////////////////////////////
AttackData::AttackData( void )
{
}

AttackData::~AttackData()
{
	ReleaseMap( m_SoundResMap );
	ReleaseMap( m_EffectResMap );
	ReleaseMap( m_AttackResMap );
}

//******************************************************************************
//	�ǂݍ���
//******************************************************************************
bool AttackData::Load( pstr pPath )
{
	//	�t�@�C���ǂݍ���
	XML Xml;
	if( !Xml.Load( pPath ) )
	{
		return false;
	}
	
	//	�G�t�F�N�g
	const XML::Node* pEffectNode = Xml.GetRoot().FindChild( NODE_EFFECT );
	if( pEffectNode ) _LoadEffect( pEffectNode );

	//	�^�C���L�[
	const XML::Node* pTimeKeyNode = Xml.GetRoot().FindChild( NODE_TIME_KEY );
	if( pTimeKeyNode ) _LoadTimeKey( pTimeKeyNode );
	
	//	�p�����[�^
	const XML::Node* pAttackNode = Xml.GetRoot().FindChild( NODE_ATTACK );
	if( pAttackNode ) _LoadParam( pAttackNode );

	return true;
}

//******************************************************************************
//	�G�t�F�N�g�t�@�C���̎擾
//******************************************************************************
const EffectFile* AttackData::GetEffectFile( Label Name ) const
{
	return FindResourceMap<EffectFile, EffectResource>( m_EffectResMap, Name );
}

//******************************************************************************
//	�T�E���h�t�@�C���̎擾
//******************************************************************************
const SoundFile* AttackData::GetSoundFile( Label Name ) const
{
	return FindResourceMap<SoundFile, SoundResource>( m_SoundResMap, Name );
}

//******************************************************************************
//	�U���f�[�^�̎擾
//******************************************************************************
const AttackData* AttackData::GetAttackData( Label Name ) const
{
	return FindResourceMap<AttackData, AttackResource>( m_AttackResMap, Name );
}

//******************************************************************************
//	�^�C���L�[�̐����擾
//******************************************************************************
uint AttackData::GetTimeKeyNum( ATTACK_TIME_KEY_TYPE Type ) const
{
	return (uint)m_vTimeKey[ Type ].size();
}

//******************************************************************************
//	�^�C���L�[�p�����[�^�̎擾
//******************************************************************************
const ATTACK_TIME_KEY* AttackData::GetTimeKey( ATTACK_TIME_KEY_TYPE Type, int Index )	const
{
	return &( m_vTimeKey[ Type ].at(Index) );
}

//------------------------------------------------------------------------------
//	�p�����[�^�̓ǂݍ���
//------------------------------------------------------------------------------
void AttackData::_LoadParam( const XML::Node* pNode )
{
	//	���p�����[�^
	m_Param.AngleRange			= pNode->GetAttr( ATTR_ANGLE_RANGE ).GetDegree();
	m_Param.Damage				= pNode->GetAttr( ATTR_DAMAGE ).GetFloat();
	m_Param.Exhaust				= pNode->GetAttr( ATTR_EXHAUST ).GetFloat();
	m_Param.HitVel				= pNode->GetAttr( ATTR_HIT_VEL ).GetFloat();
	m_Param.EndFrame			= pNode->GetAttr( ATTR_END ).GetInt();
	m_Param.bNotPlayerHit		= pNode->GetAttr( ATTR_NOT_PLAYER_HIT ).GetBool();
	m_Param.bNotAttackHit		= pNode->GetAttr( ATTR_NOT_ATTACK_HIT ).GetBool();
	m_Param.bNotGroundHit		= pNode->GetAttr( ATTR_NOT_GROUND_HIT ).GetBool();
	m_Param.ItemNum				= pNode->GetAttr( ATTR_ITEM_NUM ).GetInt();
	m_Param.ItemSpeed			= pNode->GetAttr( ATTR_ITEM_SPEED ).GetFloat();
	m_Param.ScoreRate			= pNode->GetAttr( ATTR_SCORE_RATE, "1.0" ).GetFloat();
	m_Param.HitVelType			= ATTACK_VEL_TYPE( StrToInt( pNode->GetAttr( ATTR_HIT_VEL_TYPE ).GetStr(), ATTACK_VEL_TYPE_TABLE, ArrayNum(ATTACK_VEL_TYPE_TABLE), ATTACK_VEL_NORMAL ) );
	m_Param.AppearSoundVol		= pNode->GetAttr( ATTR_APPEAR_SOUND_VOL, "0.5" ).GetFloat();
	m_Param.TimeKeyLoopFrame	= pNode->GetAttr( ATTR_TIMEKEY_LOOP_FRAME ).GetInt();
	m_Param.bOwnerErase			= pNode->GetAttr( ATTR_OWNER_ERASE ).GetBool();
	m_Param.bNotNotify			= pNode->GetAttr( ATTR_NOT_NOTIFY ).GetBool();

	//	�G�t�F�N�g�ǉ�
	m_Param.EffectName			= AddResourceMap< EffectResource >( m_EffectResMap, pNode->GetAttr( ATTR_EFFECT ).GetStr(), GetApp()->GetResMng() );
	m_Param.HitEffectName		= AddResourceMap< EffectResource >( m_EffectResMap, pNode->GetAttr( ATTR_HIT_EFFECT ).GetStr(), GetApp()->GetResMng() );
	m_Param.AppearEffectName	= AddResourceMap< EffectResource >( m_EffectResMap, pNode->GetAttr( ATTR_APPEAR_EFFECT ).GetStr(), GetApp()->GetResMng() );
	m_Param.AppearSoundName		= AddResourceMap< SoundResource >( m_SoundResMap, pNode->GetAttr( ATTR_APPEAR_SOUND ).GetStr(), GetApp()->GetResMng() );

	//	���������p�����[�^
	const XML::Node* pConcurrentNode = pNode->FindChild( NODE_CONCURRENT );
	if( pConcurrentNode ) _LoadConcurrent( m_Param.Conncurrent, pConcurrentNode );

	//	�e��
	const XML::Node* pBulletNode = pNode->FindChild( NODE_BULLET );
	if( pBulletNode )
	{
		m_Param.Type = ATTACK_TYPE_BULLET;
		_LoadBullet( m_Param.Bullet, pBulletNode );
		return;
	}
	//	�ǔ�
	const XML::Node* pHomingNode = pNode->FindChild( NODE_HOMING );
	if( pHomingNode )
	{
		m_Param.Type = ATTACK_TYPE_HOMING;
		_LoadHoming( m_Param.Homing, pHomingNode );
		return;
	}
	//	���[�U�[
	const XML::Node* pLaserNode = pNode->FindChild( NODE_LASER );
	if( pLaserNode )
	{
		m_Param.Type = ATTACK_TYPE_LASER;
		_LoadLaser( m_Param.Laser, pLaserNode );
		return;
	}
	//	�u���[�h
	const XML::Node* pBladeNode = pNode->FindChild( NODE_BLADE );
	if( pBladeNode )
	{
		m_Param.Type = ATTACK_TYPE_BLADE;
		_LoadBlade( m_Param.Blade, pBladeNode );
		return;
	}
	//	�V�[���h
	const XML::Node* pShieldNode = pNode->FindChild( NODE_SHIELD );
	if( pShieldNode )
	{
		m_Param.Type = ATTACK_TYPE_SHIELD;
		_LoadShield( m_Param.Shield, pShieldNode );
		return;
	}
	//	���[���[
	const XML::Node* pRulerNode = pNode->FindChild( NODE_RULER );
	if( pRulerNode )
	{
		m_Param.Type = ATTACK_TYPE_RULER;
		_LoadRuler( m_Param.Ruler, pRulerNode );
		return;
	}
}

//------------------------------------------------------------------------------
//	�e�ۃp�����[�^�ǂݍ���
//------------------------------------------------------------------------------
void AttackData::_LoadBullet( ATTACK_BULLET& Param, const XML::Node* pNode )
{
	Point Speed = pNode->GetAttr( ATTR_SPEED ).GetPoint();
	Point RotSpeed = pNode->GetAttr( ATTR_ROT_SPEED ).GetPoint();

	Param.Speed			= Speed.x;
	Param.SpeedRange	= Speed.y;
	Param.RotSpeed		= Angle::ToRad( RotSpeed.x );
	Param.RotSpeedRange	= Angle::ToRad( RotSpeed.y );
	Param.Radius		= pNode->GetAttr( ATTR_RADIUS ).GetFloat();
	Param.Length		= pNode->GetAttr( ATTR_LENGTH ).GetFloat();
	Param.FrontLength	= pNode->GetAttr( ATTR_FRONT_LENGTH ).GetFloat();
	Param.Decel			= pNode->GetAttr( ATTR_DECEL, "1.0" ).GetFloat();
	Param.OutRadius		= pNode->GetAttr( ATTR_OUT_RADIUS ).GetFloat();
	Param.bPierce		= pNode->GetAttr( ATTR_PIERCE ).GetBool();

	//	�U���ǉ�
	Label Name = AddResourceMap< AttackResource >( m_AttackResMap, pNode->GetAttr( ATTR_APPEAR ).GetStr(), GetApp()->GetResMng() );
	Name.GetStr( Param.AppearAttack );
	
	//	�G�t�F�N�g�ǉ�
	Label EndEffectName = AddResourceMap< EffectResource >( m_EffectResMap, pNode->GetAttr( ATTR_END_EFFECT ).GetStr(), GetApp()->GetResMng() );
	EndEffectName.GetStr( Param.EndEffectName );
}

//------------------------------------------------------------------------------
//	�U���e�p�����[�^�ǂݍ���
//------------------------------------------------------------------------------
void AttackData::_LoadHoming( ATTACK_HOMING& Param, const XML::Node* pNode )
{
	Param.Speed			= pNode->GetAttr( ATTR_SPEED ).GetFloat();
	Param.MinSpeed		= pNode->GetAttr( ATTR_MIN_SPEED ).GetFloat();
	Param.MaxSpeed		= pNode->GetAttr( ATTR_MAX_SPEED ).GetFloat();
	Param.Accel			= pNode->GetAttr( ATTR_ACCEL ).GetFloat();

	Param.RotSpeed		= pNode->GetAttr( ATTR_ROT_SPEED ).GetDegree();
	Param.MinRotSpeed	= pNode->GetAttr( ATTR_MIN_ROT_SPEED ).GetDegree();
	Param.MaxRotSpeed	= pNode->GetAttr( ATTR_MAX_ROT_SPEED ).GetDegree();
	Param.RotAccel		= pNode->GetAttr( ATTR_ROT_ACCEL ).GetDegree();

	Param.AccelAngle	= pNode->GetAttr( ATTR_ACCEL_ANGLE ).GetDegree();
	Param.RotAccelAngle	= pNode->GetAttr( ATTR_ROT_ACCEL_ANGLE ).GetDegree();
	Param.StartTime		= pNode->GetAttr( ATTR_START_TIME ).GetInt();
	Param.SearchTime	= pNode->GetAttr( ATTR_SEARCH_TIME ).GetInt();

	Param.Radius		= pNode->GetAttr( ATTR_RADIUS ).GetFloat();
	Param.Length		= pNode->GetAttr( ATTR_LENGTH ).GetFloat();
	Param.FrontLength	= pNode->GetAttr( ATTR_FRONT_LENGTH ).GetFloat();
	Param.OutRadius		= pNode->GetAttr( ATTR_OUT_RADIUS ).GetFloat();
	Param.bPierce		= pNode->GetAttr( ATTR_PIERCE ).GetBool();

	//	�U���ǉ�
	Label Name = AddResourceMap< AttackResource >( m_AttackResMap, pNode->GetAttr( ATTR_APPEAR ).GetStr(), GetApp()->GetResMng() );
	Name.GetStr( Param.AppearAttack );
}

//------------------------------------------------------------------------------
//	���[�U�[�p�����[�^�ǂݍ���
//------------------------------------------------------------------------------
void AttackData::_LoadLaser( ATTACK_LASER& Param, const XML::Node* pNode )
{
	Param.Radius	= pNode->GetAttr( ATTR_RADIUS ).GetFloat();
	Param.Length	= pNode->GetAttr( ATTR_LENGTH ).GetFloat();
	Param.bPierce	= pNode->GetAttr( ATTR_PIERCE ).GetBool();
	Param.bAttach	= pNode->GetAttr( ATTR_ATTACH, "true" ).GetBool();
}

//------------------------------------------------------------------------------
//	�u���[�h�p�����[�^�ǂݍ���
//------------------------------------------------------------------------------
void AttackData::_LoadBlade( ATTACK_BLADE& Param, const XML::Node* pNode )
{
	Param.Radius			= pNode->GetAttr( ATTR_RADIUS ).GetFloat();
	Param.Length			= pNode->GetAttr( ATTR_LENGTH ).GetFloat();
	Param.Energy			= pNode->GetAttr( ATTR_ENERGY ).GetFloat();
	Param.RepulseExhaust	= pNode->GetAttr( ATTR_REPULSE_EXHAUST ).GetFloat();
	Param.bRepulse			= pNode->GetAttr( ATTR_REPULSE ).GetBool();
	Param.bSmash			= pNode->GetAttr( ATTR_SMASH ).GetBool();

	//	�G�t�F�N�g�ǉ�
	Label ClashName = AddResourceMap< EffectResource >( m_EffectResMap, pNode->GetAttr( ATTR_CRASH_EFFECT ).GetStr(), GetApp()->GetResMng() );
	Label RepulseName = AddResourceMap< EffectResource >( m_EffectResMap, pNode->GetAttr( ATTR_REPULSE_EFFECT ).GetStr(), GetApp()->GetResMng() );
	ClashName.GetStr( Param.CrashEffectName );
	RepulseName.GetStr( Param.RepulseEffectName );
}

//------------------------------------------------------------------------------
//	�V�[���h�p�����[�^�ǂݍ���
//------------------------------------------------------------------------------
void AttackData::_LoadShield( ATTACK_SHIELD& Param, const XML::Node* pNode )
{
	Param.Radius	= pNode->GetAttr( ATTR_RADIUS ).GetFloat();
	Param.Energy	= pNode->GetAttr( ATTR_ENERGY ).GetFloat();
	Param.Defense	= pNode->GetAttr( ATTR_DEFENSE ).GetFloat();
	Param.bAttach	= pNode->GetAttr( ATTR_ATTACH, "true" ).GetBool();
	Param.bHide		= pNode->GetAttr( ATTR_HIDE ).GetBool();

	//	�G�t�F�N�g�ǉ�
	Label Name = AddResourceMap< EffectResource >( m_EffectResMap, pNode->GetAttr( ATTR_BREAK_EFFECT ).GetStr(), GetApp()->GetResMng() );
	Name.GetStr( Param.BreakEffectName );
}

//------------------------------------------------------------------------------
//	���[���[�p�����[�^�ǂݍ���
//------------------------------------------------------------------------------
void AttackData::_LoadRuler( ATTACK_RULER& Param, const XML::Node* pNode )
{
	Param.Radius		= pNode->GetAttr( ATTR_RADIUS ).GetFloat();
	Param.AttackFrame	= pNode->GetAttr( ATTR_ATTACK_FRAME ).GetInt();
	Param.HitEndFrame	= pNode->GetAttr( ATTR_HIT_END_FRAME ).GetInt();

	//	�G�t�F�N�g�ǉ�
	Label Name = AddResourceMap< EffectResource >( m_EffectResMap, pNode->GetAttr( ATTR_ATTACK_EFFECT ).GetStr(), GetApp()->GetResMng() );
	Name.GetStr( Param.AttackEffectName );
}

//------------------------------------------------------------------------------
//	���������p�����[�^�ǂݍ���
//------------------------------------------------------------------------------
void AttackData::_LoadConcurrent( ATTACK_CONCURRENT& Param, const XML::Node* pNode )
{
	Param.Num			= pNode->GetAttr( ATTR_NUM ).GetInt();
	Param.OffsetAngle	= pNode->GetAttr( ATTR_OFFSET ).GetDegree();
	Param.AngleRange	= pNode->GetAttr( ATTR_RANGE ).GetDegree();
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�̓ǂݍ���
//------------------------------------------------------------------------------
void AttackData::_LoadEffect( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pFileNode = pNode->GetChild( i );

		//	�ǂݍ���
		const XML::Value* pValue = pFileNode->FindAttr( ATTR_PATH );
		if( pValue && !pValue->IsEmpty() )
		{
			AddResourceMap< EffectResource >( m_EffectResMap, pValue->GetStr(), GetApp()->GetResMng() );
		}
	}
}

//------------------------------------------------------------------------------
//	�^�C���L�[�p�����[�^�ǂݍ���
//------------------------------------------------------------------------------
void AttackData::_LoadTimeKey( const XML::Node* pNode )
{
	for( int KeyID = 0; KeyID < pNode->GetChildNum(); KeyID++ )
	{
		const XML::Node* pKeyNode = pNode->GetChild( KeyID );

		//	��ʎ擾
		int Type = StrToInt( pKeyNode->GetName(), ATTACK_TIME_KEY_TABLE, ArrayNum(ATTACK_TIME_KEY_TABLE) );
		if( Type < 0 ) continue;

		ATTACK_TIME_KEY KeyInfo;
		ZeroStruct( KeyInfo );

		KeyInfo.Frame	= pKeyNode->GetAttr( ATTR_FRAME ).GetInt();
		KeyInfo.Intp	= StrToTimeKeyIntpType( pKeyNode->GetAttr( ATTR_INTP ).GetStr() );

		switch( Type )
		{
		case ATTACK_TIME_KEY_EFFECT:
			{
				//	�G�t�F�N�g�ǉ�
				Label Name = AddResourceMap< EffectResource >( m_EffectResMap, pKeyNode->GetAttr( pKeyNode->GetName() ).GetStr(), GetApp()->GetResMng() );
				Name.GetStr( KeyInfo.Param.Appear.Name );
				KeyInfo.Param.Appear.Rot = pKeyNode->GetAttr( ATTR_ROT ).GetDegree();
			}
			break;
		case ATTACK_TIME_KEY_ATTACK:
			{
				//	�U���ǉ�
				Label Name = AddResourceMap< AttackResource >( m_AttackResMap, pKeyNode->GetAttr( pKeyNode->GetName() ).GetStr(), GetApp()->GetResMng() );
				Name.GetStr( KeyInfo.Param.Appear.Name );
				KeyInfo.Param.Appear.Rot = pKeyNode->GetAttr( ATTR_ROT ).GetDegree();
			}
			break;
		case ATTACK_TIME_KEY_COLOR:
			{
				Color Col = pKeyNode->GetAttr( ATTR_COLOR, COLOR_DEFAULT ).GetColor();
				Col.GetArray( KeyInfo.Param.Col );
			}
			break;
		case ATTACK_TIME_KEY_HIT:
			{
				KeyInfo.Param.bFlag = pKeyNode->GetAttr( pKeyNode->GetName() ).GetBool();
			}
			break;
		case ATTACK_TIME_KEY_RADIUS:
		case ATTACK_TIME_KEY_LENGTH:
		case ATTACK_TIME_KEY_SPEED:
		case ATTACK_TIME_KEY_ROTSPEED:
		case ATTACK_TIME_KEY_ACCEL:
			{	
				KeyInfo.Param.Val[0] = pKeyNode->GetAttr( pKeyNode->GetName() ).GetFloat();
				KeyInfo.Param.Val[1] = 0.0f;
			}
			break;
		}

		m_vTimeKey[ Type ].push_back( KeyInfo );
	}
}