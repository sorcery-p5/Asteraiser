#include "stdafx.h"
#include "ItemData.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"
#include "Resource/SoundResource.h"
#include "Resource/ResourceUtil.h"

#include "Effect/EffectFile.h"


namespace
{
	pstr	NODE_ITEM				= "Item";
	pstr	NODE_EFFECT				= "Effect";

	pstr	ATTR_PATH				= "Path";
	pstr	ATTR_EFFECT				= "Effect";
	pstr	ATTR_SOUND				= "Sound";

	pstr	ATTR_TIME				= "Time";
	pstr	ATTR_SPEED				= "Speed";
	pstr	ATTR_DECEL				= "Decel";
	pstr	ATTR_GRAVITY			= "Gravity";
	pstr	ATTR_GRAVITY_MAX		= "GravityMax";
	pstr	ATTR_GRAVITY_TIME		= "GravityTime";
	pstr	ATTR_DELETE_HEIGHT		= "DeleteHeight";
	pstr	ATTR_FIELD_OUT_RADIUS	= "FieldOutRadius";
	pstr	ATTR_SOUND_VOL			= "SoundVol";

	pstr	ATTR_COLLIDE_RADIUS			= "CollideRadius";
	pstr	ATTR_COLLECT_RADIUS			= "CollectRadius";
	pstr	ATTR_AUTO_COLLECT_RADIUS	= "AutoCollectRadius";
	pstr	ATTR_COLLECT_ACCEL			= "CollectAccel";
	pstr	ATTR_COLLECT_SPEED_MAX		= "CollectSpeedMax";
	pstr	ATTR_COLLECT_TIME			= "CollectTime";
}


////////////////////////////////////////////////////////////////////////////////
//
//	�A�C�e���f�[�^
//
////////////////////////////////////////////////////////////////////////////////
ItemData::ItemData( void )
{
	m_pSoundRes = NULL;
}

ItemData::~ItemData()
{
	ReleaseMap( m_mapEffectRes );
	SafeRelease( m_pSoundRes );
}

//******************************************************************************
//	�ǂݍ���
//******************************************************************************
bool ItemData::Load( pstr pPath )
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

	//	�p�����[�^
	const XML::Node* pAttackNode = Xml.GetRoot().FindChild( NODE_ITEM );
	if( pAttackNode ) _LoadParam( pAttackNode );

	return true;
}

//******************************************************************************
//	�G�t�F�N�g�t�@�C���擾
//******************************************************************************
const EffectFile* ItemData::GetEffectFile( Label Name ) const
{
	return FindResourceMap<EffectFile, EffectResource>( m_mapEffectRes, Name );
}

//******************************************************************************
//	�T�E���h�t�@�C���擾
//******************************************************************************
const SoundFile* ItemData::GetSoundFile( void ) const
{
	if( m_pSoundRes )
	{
		return m_pSoundRes->GetData();
	}
	return NULL;
}

//------------------------------------------------------------------------------
//	�p�����[�^�ǂݍ���
//------------------------------------------------------------------------------
void ItemData::_LoadParam( const XML::Node* pNode )
{;
	m_Param.Time				= pNode->GetAttr( ATTR_TIME ).GetInt();
	m_Param.Speed				= pNode->GetAttr( ATTR_SPEED ).GetFloat();
	m_Param.Decel				= pNode->GetAttr( ATTR_DECEL ).GetFloat();
	m_Param.Gravity				= pNode->GetAttr( ATTR_GRAVITY ).GetFloat();
	m_Param.GravityMax			= pNode->GetAttr( ATTR_GRAVITY_MAX ).GetFloat();
	m_Param.GravityTime			= pNode->GetAttr( ATTR_GRAVITY_TIME ).GetInt();

	m_Param.CollideRadius		= pNode->GetAttr( ATTR_COLLIDE_RADIUS ).GetFloat();
	m_Param.CollectRadius		= pNode->GetAttr( ATTR_COLLECT_RADIUS ).GetFloat();
	m_Param.AutoCollectRadius	= pNode->GetAttr( ATTR_AUTO_COLLECT_RADIUS ).GetFloat();
	m_Param.CollectAccel		= pNode->GetAttr( ATTR_COLLECT_ACCEL ).GetFloat();
	m_Param.CollectSpeedMax		= pNode->GetAttr( ATTR_COLLECT_SPEED_MAX ).GetFloat();
	m_Param.CollectTime			= pNode->GetAttr( ATTR_COLLECT_TIME ).GetInt();
	m_Param.DeleteHeight		= pNode->GetAttr( ATTR_DELETE_HEIGHT ).GetFloat();
	m_Param.FieldOutRadius		= pNode->GetAttr( ATTR_FIELD_OUT_RADIUS ).GetFloat();

	m_Param.SoundVolume			= pNode->GetAttr( ATTR_SOUND_VOL ).GetFloat();

	//	�G�t�F�N�g
	m_Param.EffectName		= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_EFFECT ).GetStr(), GetApp()->GetResMng() );

	//	�T�E���h
	m_pSoundRes				= GetApp()->GetResMng()->LoadResource<SoundResource>( pNode->GetAttr( ATTR_SOUND ).GetStr() );
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�ǂݍ���
//------------------------------------------------------------------------------
void ItemData::_LoadEffect( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pFileNode = pNode->GetChild( i );

		//	�ǂݍ���
		const XML::Value* pValue = pFileNode->FindAttr( ATTR_PATH );
		if( pValue && !pValue->IsEmpty() )
		{
			AddResourceMap< EffectResource >( m_mapEffectRes, pValue->GetStr(), GetApp()->GetResMng() );
		}
	}
}