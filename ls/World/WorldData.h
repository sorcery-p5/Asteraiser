#pragma once

#include "World/WorldTypes.h"
#include "Resource/Resource.h"

class PlayerResource;
class PlayerData;
class StageResource;
class StageData;
class ItemResource;
class ItemData;
class IndicateResource;
class IndicateData;
class ScoreResource;
class ScoreData;
class SpriteFrameResource;
class SpriteFrameFile;
class SoundResource;
class MenuResource;
class MenuData;


///////////////////////////////////////////////////////////////////////////////
//
//	ワールドデータ
//
///////////////////////////////////////////////////////////////////////////////
class WorldData
{
public:
	WorldData( void );
	~WorldData();

	bool					Load( pstr pPath );

	WORLD_PARAM				GetParam( void )					const	{ return m_Param; }
	const PlayerData*		GetPlayerData( Label Name )			const;
	int						GetStageNum( void )					const	{ return (int)m_vStageRes.size(); }
	const StageData*		GetStageData( int Index )			const;
	const IndicateData*		GetIndicateData( Label Name )		const;
	const SpriteFrameFile*	GetSpriteFrameFile( Label Name )	const;
	const MenuData*			GetMenuData( Label Name )			const;
	const ItemData*			GetItemData( void )					const;
	const ScoreData*		GetScoreData( void )				const;
	const SoundFile*		GetSoundFile( Label Name )			const;

private:
	typedef std::vector<StageResource*>				VecStageRes;
	typedef std::map< Label, PlayerResource*>		MapPlayerRes;
	typedef std::map< Label, IndicateResource*>		MapIndicateRes;
	typedef std::map< Label, SpriteFrameResource* >	MapSpriteRes;
	typedef std::map< Label, MenuResource* >		MapMenuRes;
	typedef std::map< Label, SoundResource* >		SoundResMap;

private:
	void					_LoadParam( const XML::Node* pNode );
	void					_LoadPlayer( const XML::Node* pNode );
	void					_LoadStage( const XML::Node* pNode );
	void					_LoadIndicate( const XML::Node* pNode );
	void					_LoadItem( const XML::Node* pNode );
	void					_LoadScore( const XML::Node* pNode );

private:
	WORLD_PARAM				m_Param;
	VecStageRes				m_vStageRes;
	MapPlayerRes			m_mapPlayerRes;
	MapIndicateRes			m_mapIndicateRes;
	MapSpriteRes			m_mapSpriteRes;
	MapMenuRes				m_mapMenuRes;
	ItemResource*			m_pItemRes;
	ScoreResource*			m_pScoreRes;
	SoundResMap				m_SoundResMap;
};

//------------------------------------------------------------------------------
//	ワールドリソース
//------------------------------------------------------------------------------
class WorldResource : public ResourceTemplate< WorldData, RES_WORLD >
{
};