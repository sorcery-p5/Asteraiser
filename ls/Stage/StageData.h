#pragma once

#include "StageTypes.h"
#include "Resource/Resource.h"

class EnemyData;
class GroundData;
class SpriteFrameFile;
class EffectFile;
class EnemyResource;
class SpriteFrameResource;
class EffectResource;
class SoundResource;


////////////////////////////////////////////////////////////////////////////////
//
//	ステージデータ
//
////////////////////////////////////////////////////////////////////////////////
class StageData
{
public:
	StageData( void );
	~StageData();

	bool					Load( pstr pPath );
	
	const STAGE_PARAM&		GetParam( void )						const	{ return m_Param; }
	const STAGE_STATE*		GetState( Label Name )					const;
	const STAGE_TELOP*		GetTelop( Label Name )					const;

	const EnemyData*		GetEnemyData( Label Name )				const;
	const GroundData*		GetGroundData( Label Name )				const;
	const SpriteFrameFile*	GetSpriteFrameFile( Label Name )		const;
	const EffectFile*		GetEffectFile( Label Name )				const;

private:
	typedef std::map< Label, STAGE_STATE* >			MapState;
	typedef std::map< Label, EnemyResource* >		MapEnemyRes;
	typedef std::map< Label, GroundData* >			MapGround;
	typedef std::map< Label, SpriteFrameResource* >	MapSpriteRes;
	typedef std::map< Label, EffectResource* >		MapEffectRes;
	typedef std::map< Label, STAGE_TELOP >			MapTelop;

private:
	void					_LoadParam( const XML::Node* pNode );
	void					_LoadState( const XML::Node* pNode );
	void					_LoadTelop( const XML::Node* pNode );
	
	void					_LoadAction( STAGE_ACTION& Param, const XML::Node* pNode );
	void					_LoadCondition( STAGE_CONDITION& Param, const XML::Node* pNode );
	void					_LoadTelop( STAGE_TELOP& Param, const XML::Node* pNode );

	void					_LoadEnemy( const XML::Node* pNode );
	void					_LoadGround( const XML::Node* pNode );
	void					_LoadSprite( const XML::Node* pNode );
	void					_LoadEffect( const XML::Node* pNode );

private:
	STAGE_PARAM				m_Param;
	MapState				m_mapState;
	MapTelop				m_mapTelop;
	
	MapEnemyRes				m_mapEnemyRes;
	MapGround				m_mapGround;
	MapSpriteRes			m_mapSpriteRes;
	MapEffectRes			m_mapEffectRes;
};


//------------------------------------------------------------------------------
//	ステージリソース
//------------------------------------------------------------------------------
class StageResource : public ResourceTemplate< StageData, RES_STAGE >
{
};