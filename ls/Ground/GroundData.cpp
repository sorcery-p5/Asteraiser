#include "stdafx.h"
#include "GroundData.h"

#include "App/App.h"

#include "Resource/ResourceManager.h"
#include "Resource/ResourceUtil.h"

#include "SpriteFrame/SpriteFrameFile.h"
#include "Effect/EffectFile.h"

namespace
{
	pstr		ATTR_SPRITE				= "Sprite";
	pstr		ATTR_ANIME				= "Anime";
	pstr		ATTR_START_ANIME		= "StartAnime";
	pstr		ATTR_END_ANIME			= "EndAnime";
	pstr		ATTR_SPEED				= "Speed";
	pstr		ATTR_ROT_SPEED			= "RotSpeed";
	pstr		ATTR_TIME				= "Time";
	pstr		ATTR_HIT				= "Hit";
	pstr		ATTR_HIT_EFFECT			= "HitEffect";
	pstr		ATTR_NOT_PLAYER_HIT		= "NotPlayerHit";
	pstr		ATTR_NOT_ENEMY_HIT		= "NotEnemyHit";
	pstr		ATTR_NOT_ATTACK_HIT		= "NotAttackHit";
	pstr		ATTR_ATTACK_ERASE		= "AttackErase";
	pstr		ATTR_ATTACK_HIT_EFFECT	= "AttackHitEffect";
};


////////////////////////////////////////////////////////////////////////////////
//
//	地形データ
//
////////////////////////////////////////////////////////////////////////////////
GroundData::GroundData( void )
{

}
GroundData::~GroundData()
{
	ReleaseMap( m_mapSpriteRes );
	ReleaseMap( m_mapEffectRes );
}

//******************************************************************************
//	読み込み
//******************************************************************************
void GroundData::Load( const XML::Node* pNode )
{
	//	パラメータ
	m_Param.AnimeName		= pNode->GetAttr( ATTR_ANIME ).GetLabel();
	m_Param.StartAnimeName	= pNode->GetAttr( ATTR_START_ANIME ).GetLabel();
	m_Param.EndAnimeName	= pNode->GetAttr( ATTR_END_ANIME ).GetLabel();
	m_Param.Speed			= pNode->GetAttr( ATTR_SPEED ).GetPoint();
	m_Param.RotSpeed		= pNode->GetAttr( ATTR_ROT_SPEED ).GetDegree();
	m_Param.Time			= pNode->GetAttr( ATTR_TIME ).GetInt();

	m_Param.bHitEnable			= pNode->GetAttr( ATTR_HIT ).GetBool();
	m_Param.bNotPlayerHit		= pNode->GetAttr( ATTR_NOT_PLAYER_HIT ).GetBool();
	m_Param.bNotEnemyHit		= pNode->GetAttr( ATTR_NOT_ENEMY_HIT ).GetBool();
	m_Param.bNotAttackHit		= pNode->GetAttr( ATTR_NOT_ATTACK_HIT ).GetBool();
	m_Param.bAttackErase		= pNode->GetAttr( ATTR_ATTACK_ERASE, "true" ).GetBool();
	m_Param.bAttackHitEffect	= pNode->GetAttr( ATTR_ATTACK_HIT_EFFECT, "true" ).GetBool();

	//	スプライト
	m_Param.SpriteName		= AddResourceMap< SpriteFrameResource >( m_mapSpriteRes, pNode->GetAttr( ATTR_SPRITE ).GetStr(), GetApp()->GetResMng() );
	
	//	エフェクト
	m_Param.HitEffectName	= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_HIT_EFFECT ).GetStr(), GetApp()->GetResMng() );
}

//******************************************************************************
//	スプライトフレームファイル取得
//******************************************************************************
const SpriteFrameFile* GroundData::GetSpriteFrameFile( Label Name ) const
{
	return FindResourceMap<SpriteFrameFile, SpriteFrameResource>( m_mapSpriteRes, Name );
}

//******************************************************************************
//	エフェクトファイル取得
//******************************************************************************
const EffectFile* GroundData::GetEffectFile( Label Name ) const
{
	return FindResourceMap<EffectFile, EffectResource>( m_mapEffectRes, Name );
}