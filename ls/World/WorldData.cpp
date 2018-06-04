#include "stdafx.h"
#include "WorldData.h"

#include "App/App.h"

#include "Resource/ResourceManager.h"
#include "Resource/ResourceUtil.h"
#include "Resource/SoundResource.h"

#include "Player/PlayerData.h"
#include "Stage/StageData.h"
#include "Indicate/IndicateData.h"
#include "Item/ItemData.h"
#include "Score/ScoreData.h"
#include "SpriteFrame/SpriteFrameFile.h"
#include "Menu/MenuData.h"

namespace
{
	pstr	NODE_WORLD					= "World";
	pstr	NODE_PLAYER					= "Player";
	pstr	NODE_STAGE					= "Stage";
	pstr	NODE_INDICATE				= "Indicate";
	pstr	NODE_ITEM					= "Item";
	pstr	NODE_SCORE					= "Score";
	pstr	NODE_SOUND					= "Sound";

	pstr	ATTR_PATH					= "Path";

	pstr	ATTR_FIELD_RECT				= "FieldRect";
	pstr	ATTR_INIT_POS				= "InitPos";
	pstr	ATTR_PLAYER_NAME			= "PlayerName";
	pstr	ATTR_PAUSE_MENU				= "PauseMenu";
	pstr	ATTR_PAUSE_EXIT				= "PauseExit";
	pstr	ATTR_CONTINUE_MENU			= "ContinueMenu";
	pstr	ATTR_CONTINUE_ENABLE		= "ContinueEnable";
	pstr	ATTR_RESULT_MENU			= "ResultMenu";
	pstr	ATTR_RESULT_SCORE_SOUND		= "ResultScoreSound";
	pstr	ATTR_REPLAY_MENU			= "ReplayMenu";
	pstr	ATTR_DEMO_MENU				= "DemoMenu";
};


///////////////////////////////////////////////////////////////////////////////
//
//	���[���h�f�[�^
//
///////////////////////////////////////////////////////////////////////////////
WorldData::WorldData( void )
{
	m_pItemRes		= NULL;
	m_pScoreRes		= NULL;
}
WorldData::~WorldData()
{
	ReleaseVec( m_vStageRes );
	ReleaseMap( m_mapIndicateRes );
	ReleaseMap( m_mapPlayerRes );
	SafeRelease( m_pItemRes );
	SafeRelease( m_pScoreRes );
	ReleaseMap( m_mapSpriteRes );
	ReleaseMap( m_mapMenuRes );
	ReleaseMap( m_SoundResMap );
}

//******************************************************************************
//	�ǂݍ���
//******************************************************************************
bool WorldData::Load( pstr pPath )
{
	//	�t�@�C���ǂݍ���
	XML Xml;
	if( !Xml.Load( pPath ) )
	{
		return false;
	}

	//	�v���C���[
	const XML::Node* pPlayerNode = Xml.GetRoot().FindChild( NODE_PLAYER );
	if( pPlayerNode ) _LoadPlayer( pPlayerNode );
	
	//	�X�e�[�W
	const XML::Node* pStageNode = Xml.GetRoot().FindChild( NODE_STAGE );
	if( pStageNode ) _LoadStage( pStageNode );

	//	�C���W�P�[�g
	const XML::Node* pIndicateNode = Xml.GetRoot().FindChild( NODE_INDICATE );
	if( pIndicateNode ) _LoadIndicate( pIndicateNode );

	//	�A�C�e��
	const XML::Node* pItemNode = Xml.GetRoot().FindChild( NODE_ITEM );
	if( pItemNode ) _LoadItem( pItemNode );
	
	//	�X�R�A
	const XML::Node* pScoreNode = Xml.GetRoot().FindChild( NODE_SCORE );
	if( pScoreNode ) _LoadScore( pScoreNode );

	//	�p�����[�^
	const XML::Node* pParamNode = Xml.GetRoot().FindChild( NODE_WORLD );
	if( pParamNode ) _LoadParam( pParamNode );

	return true;
}

//******************************************************************************
//	�v���C���[�f�[�^�擾
//******************************************************************************
const PlayerData* WorldData::GetPlayerData( Label Name ) const
{
	return FindResourceMap<PlayerData, PlayerResource>( m_mapPlayerRes, Name );
}

//******************************************************************************
//	�X�e�[�W�f�[�^�擾
//******************************************************************************
const StageData* WorldData::GetStageData( int Index ) const
{
	if( Index < GetStageNum() )
	{
		return m_vStageRes[Index]->GetData();
	}

	return NULL;
}

//******************************************************************************
//	�C���W�P�[�g�f�[�^�̎擾
//******************************************************************************
const IndicateData* WorldData::GetIndicateData( Label Name ) const
{
	return FindResourceMap<IndicateData, IndicateResource>( m_mapIndicateRes, Name );
}

//******************************************************************************
//	�X�v���C�g�t���[���t�@�C���̎擾
//******************************************************************************
const SpriteFrameFile* WorldData::GetSpriteFrameFile( Label Name ) const
{
	return FindResourceMap<SpriteFrameFile, SpriteFrameResource>( m_mapSpriteRes, Name );
}

//******************************************************************************
//	���j���[�f�[�^�̎擾
//******************************************************************************
const MenuData* WorldData::GetMenuData( Label Name ) const
{
	return FindResourceMap<MenuData, MenuResource>( m_mapMenuRes, Name );
}

//******************************************************************************
//	�A�C�e���f�[�^�擾
//******************************************************************************
const ItemData* WorldData::GetItemData( void ) const
{
	if( m_pItemRes )
	{
		return m_pItemRes->GetData();
	}

	return NULL;
}

//******************************************************************************
//	�X�R�A�f�[�^�擾
//******************************************************************************
const ScoreData* WorldData::GetScoreData( void ) const
{
	if( m_pScoreRes )
	{
		return m_pScoreRes->GetData();
	}

	return NULL;
}

//******************************************************************************
//	�T�E���h�t�@�C���̎擾
//******************************************************************************
const SoundFile* WorldData::GetSoundFile( Label Name ) const
{
	return FindResourceMap<SoundFile, SoundResource>( m_SoundResMap, Name );
}

//------------------------------------------------------------------------------
//	�p�����[�^�ǂݍ���
//------------------------------------------------------------------------------
void WorldData::_LoadParam( const XML::Node* pNode )
{
	m_Param.FieldRect		= pNode->GetAttr( ATTR_FIELD_RECT ).GetRect();
	m_Param.InitPos			= pNode->GetAttr( ATTR_INIT_POS ).GetPoint();
	m_Param.PlayerName		= pNode->GetAttr( ATTR_PLAYER_NAME ).GetLabel();

	m_Param.bPauseExit		= pNode->GetAttr( ATTR_PAUSE_EXIT ).GetBool();
	m_Param.bContinueEnable	= pNode->GetAttr( ATTR_CONTINUE_ENABLE ).GetBool();
	
	//	���j���[
	m_Param.PauseMenuName		= AddResourceMap< MenuResource >( m_mapMenuRes, pNode->GetAttr( ATTR_PAUSE_MENU ).GetStr(), GetApp()->GetResMng() );
	m_Param.ContinueMenuName	= AddResourceMap< MenuResource >( m_mapMenuRes, pNode->GetAttr( ATTR_CONTINUE_MENU ).GetStr(), GetApp()->GetResMng() );
	m_Param.ResultMenuName		= AddResourceMap< MenuResource >( m_mapMenuRes, pNode->GetAttr( ATTR_RESULT_MENU ).GetStr(), GetApp()->GetResMng() );
	m_Param.ReplayMenuName		= AddResourceMap< MenuResource >( m_mapMenuRes, pNode->GetAttr( ATTR_REPLAY_MENU ).GetStr(), GetApp()->GetResMng() );
	m_Param.DemoMenuName		= AddResourceMap< MenuResource >( m_mapMenuRes, pNode->GetAttr( ATTR_DEMO_MENU ).GetStr(), GetApp()->GetResMng() );

	//	�T�E���h
	m_Param.ResultScoreSoundName = AddResourceMap<SoundResource>( m_SoundResMap, pNode->GetAttr( ATTR_RESULT_SCORE_SOUND ).GetStr(), GetApp()->GetResMng() );
}

//------------------------------------------------------------------------------
//	�v���C���[�f�[�^�ǂݍ���
//------------------------------------------------------------------------------
void WorldData::_LoadPlayer( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pFileNode = pNode->GetChild( i );

		//	�ǂݍ���
		const XML::Value* pValue = pFileNode->FindAttr( ATTR_PATH );
		if( pValue && !pValue->IsEmpty() )
		{
			AddResourceMap< PlayerResource >( m_mapPlayerRes, pValue->GetStr(), GetApp()->GetResMng() );
		}
	}
}

//------------------------------------------------------------------------------
//	�X�e�[�W�f�[�^�ǂݍ���
//------------------------------------------------------------------------------
void WorldData::_LoadStage( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pFileNode = pNode->GetChild( i );

		//	�ǂݍ���
		const XML::Value* pValue = pFileNode->FindAttr( ATTR_PATH );
		if( pValue && !pValue->IsEmpty() )
		{
			StageResource* pRes = GetApp()->GetResMng()->LoadResource<StageResource>( pValue->GetStr() );
			if( pRes )
			{
				m_vStageRes.push_back( pRes );
			}
		}
	}
}

//------------------------------------------------------------------------------
//	�C���W�P�[�g�f�[�^�ǂݍ���
//------------------------------------------------------------------------------
void WorldData::_LoadIndicate( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pFileNode = pNode->GetChild( i );

		//	�ǂݍ���
		const XML::Value* pValue = pFileNode->FindAttr( ATTR_PATH );
		if( pValue && !pValue->IsEmpty() )
		{
			AddResourceMap< IndicateResource >( m_mapIndicateRes, pValue->GetStr(), GetApp()->GetResMng() );
		}
	}
}

//------------------------------------------------------------------------------
//	�A�C�e���f�[�^�ǂݍ���
//------------------------------------------------------------------------------
void WorldData::_LoadItem( const XML::Node* pNode )
{
	const XML::Value* pValue = pNode->FindAttr( ATTR_PATH );
	if( pValue && !pValue->IsEmpty() )
	{
		m_pItemRes = GetApp()->GetResMng()->LoadResource<ItemResource>( pValue->GetStr() );
	}
}

//------------------------------------------------------------------------------
//	�X�R�A�f�[�^�ǂݍ���
//------------------------------------------------------------------------------
void WorldData::_LoadScore( const XML::Node* pNode )
{
	const XML::Value* pValue = pNode->FindAttr( ATTR_PATH );
	if( pValue && !pValue->IsEmpty() )
	{
		m_pScoreRes = GetApp()->GetResMng()->LoadResource<ScoreResource>( pValue->GetStr() );
	}
}