#pragma once

#include "GroundTypes.h"

class SpriteFrameFile;
class SpriteFrameResource;
class EffectFile;
class EffectResource;


////////////////////////////////////////////////////////////////////////////////
//
//	地形データ
//
////////////////////////////////////////////////////////////////////////////////
class GroundData
{
public:
	GroundData( void );
	~GroundData();

	void					Load( const XML::Node* pNode );

	const GROUND_PARAM&		GetParam( void )					const	{ return m_Param; }
	const SpriteFrameFile*	GetSpriteFrameFile( Label Name )	const;
	const EffectFile*		GetEffectFile( Label Name )			const;

private:
	typedef std::map< Label, SpriteFrameResource* >		MapSpriteRes;
	typedef std::map< Label, EffectResource* >			MapEffectRes;

private:
	GROUND_PARAM			m_Param;
	MapSpriteRes			m_mapSpriteRes;
	MapEffectRes			m_mapEffectRes;
};