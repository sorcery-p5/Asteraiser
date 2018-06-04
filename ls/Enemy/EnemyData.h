#pragma once

#include "EnemyTypes.h"
#include "Resource/Resource.h"

class EnemyStateData;
class EnemyResource;
class SpriteFrameFile;
class SpriteFrameResource;
class AttackData;
class AttackResource;
class EffectFile;
class EffectResource;
class SoundResource;


////////////////////////////////////////////////////////////////////////////////
//
//	敵データ
//
////////////////////////////////////////////////////////////////////////////////
class EnemyData
{
public:
	EnemyData( void );
	~EnemyData();
	
	bool						Load( pstr pPath );
	
	const ENEMY_PARAM&			GetParam( void )						const	{ return m_Param; }
	const EnemyStateData*		GetStateData( Label Name )				const;	
	Label						GetHandlerStateName( Label Name )		const;
	const SpriteFrameFile*		GetSpriteFrameFile( Label Name )		const;
	const AttackData*			GetAttackData( Label Name )				const;
	const EnemyData*			GetEnemyData( Label Name )				const;
	const EffectFile*			GetEffectFile( Label Name )				const;
	const SoundFile*			GetSoundFile( Label Name )				const;

private:
	typedef std::map< Label, SpriteFrameResource* >	MapSpriteRes;
	typedef std::map< Label, AttackResource* >		MapAttackRes;
	typedef std::map< Label, EnemyResource* >		MapEnemyRes;
	typedef std::map< Label, EffectResource* >		MapEffectRes;
	typedef std::map< Label, SoundResource* >		MapSoundRes;
	typedef std::map< Label, EnemyStateData* >		MapState;
	typedef std::map< Label, Label >				MapHandlerState;

private:
	void						_LoadParam( const XML::Node* pNode );
	void						_LoadAttack( const XML::Node* pNode );
	void						_LoadEnemy( const XML::Node* pNode );
	void						_LoadEffect( const XML::Node* pNode );
	void						_LoadSound( const XML::Node* pNode );
	void						_LoadState( const XML::Node* pNode );
	void						_LoadHandlerState( const XML::Node* pNode );

	void						_LoadAction( const XML::Node* pNode, ENEMY_ACTION& Param );
	void						_LoadMoveAction( const XML::Node* pNode, ENEMY_MOVE_ACTION& Param );
	void						_LoadRotAction( const XML::Node* pNode, ENEMY_ROT_ACTION& Param );
	void						_LoadAttackAction( const XML::Node* pNode, ENEMY_ATTACK_ACTION& Param );
	void						_LoadSummonAction( const XML::Node* pNode, ENEMY_SUMMON_ACTION& Param );
	void						_LoadWarpAction( const XML::Node* pNode, ENEMY_WARP_ACTION& Param );
	void						_LoadTarget( const XML::Node* pNode, ENEMY_TARGET& Param );
	void						_LoadCondition( const XML::Node* pNode, ENEMY_CONDITION& Param );

private:
	ENEMY_PARAM					m_Param;
	MapSpriteRes				m_mapSpriteRes;
	MapAttackRes				m_mapAttackRes;
	MapEnemyRes					m_mapEnemyRes;
	MapEffectRes				m_mapEffectRes;
	MapSoundRes					m_mapSoundRes;
	MapState					m_mapState;
	MapHandlerState				m_mapHandlerState;
};

//------------------------------------------------------------------------------
//	敵リソース
//------------------------------------------------------------------------------
class EnemyResource : public ResourceTemplate< EnemyData, RES_ENEMY >
{
};