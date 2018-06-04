#pragma once

#include "Player/PlayerTypes.h"
#include "Resource/Resource.h"

class SpriteFrameResource;
class EffectResource;
class SpriteFrameFile;
class EffectFile;
class IndicateResource;
class IndicateData;


////////////////////////////////////////////////////////////////////////////////
//
//	プレイヤーデータ
//
////////////////////////////////////////////////////////////////////////////////
class PlayerData
{
public:
	PlayerData( void );
	~PlayerData();

	bool					Load( pstr pPath );
	
	const PLAYER_PARAM&		GetParam( void )					const	{ return m_Param; }
	const SpriteFrameFile*	GetSpriteFrameFile( Label Name )	const;
	const EffectFile*		GetEffectFile( Label Name )			const;
	const IndicateData*		GetIndicateData( Label Name )		const;

private:
	void					_LoadParam( const XML::Node* pNode );

	void					_LoadBlade( BLADE_PARAM& Param, const XML::Node* pNode );
	void					_LoadBladeEnergy( BLADE_ENERGY_PARAM& Param, const XML::Node* pNode );
	void					_LoadBladeSmash( BLADE_SMASH_PARAM& Param, const XML::Node* pNode );
	void					_LoadOverdrive( OVERDRIVE_PARAM& Param, const XML::Node* pNode );
	void					_LoadSpear( SPEAR_PARAM& Param, const XML::Node* pNode );
	void					_LoadRuler( RULER_PARAM& Param, const XML::Node* pNode );
	void					_LoadDash( DASH_PARAM& Param, const XML::Node* pNode );
	void					_LoadDashSplash( DASH_SPLASH_PARAM& Param, const XML::Node* pNode );
	void					_LoadShield( SHIELD_PARAM& Param, const XML::Node* pNode );

	void					_LoadEffect( const XML::Node* pNode );
	void					_LoadIndicate( const XML::Node* pNode );

private:
	typedef std::map< Label, SpriteFrameResource* >	SpriteResMap;
	typedef std::map< Label, EffectResource* >		EffectResMap;
	typedef std::map< Label, IndicateResource* >	IndicateResMap;

private:
	PLAYER_PARAM			m_Param;
	SpriteResMap			m_mapSpriteRes;
	EffectResMap			m_mapEffectRes;
	IndicateResMap			m_mapIndicateRes;
};

//------------------------------------------------------------------------------
//	プレイヤーリソース
//------------------------------------------------------------------------------
class PlayerResource : public ResourceTemplate< PlayerData, RES_PLAYER >
{
};