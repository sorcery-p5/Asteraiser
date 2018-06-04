#pragma once

#include "AttackTypes.h"
#include "Resource/Resource.h"

class EffectFile;
class EffectResource;
class SoundResource;
class AttackResource;


////////////////////////////////////////////////////////////////////////////////
//
//	攻撃データ
//
////////////////////////////////////////////////////////////////////////////////
class AttackData
{
public:
	AttackData( void );
	~AttackData();

	bool						Load( pstr pPath );

	const ATTACK_PARAM&			GetParam( void )									const	{ return m_Param; }
	const EffectFile*			GetEffectFile( Label Name )							const;
	const SoundFile*			GetSoundFile( Label Name )							const;
	const AttackData*			GetAttackData( Label Name )							const;

	uint						GetTimeKeyNum( ATTACK_TIME_KEY_TYPE Type )			const;
	const ATTACK_TIME_KEY*		GetTimeKey( ATTACK_TIME_KEY_TYPE Type, int Index )	const;

private:
	typedef std::map< Label, EffectResource* >	EffectResMap;
	typedef std::map< Label, AttackResource* >	AttackResMap;
	typedef std::map< Label, SoundResource* >	SoundResMap;

private:
	void						_LoadParam( const XML::Node* pNode );
	void						_LoadBullet( ATTACK_BULLET& Param, const XML::Node* pNode );
	void						_LoadHoming( ATTACK_HOMING& Param, const XML::Node* pNode );
	void						_LoadLaser( ATTACK_LASER& Param, const XML::Node* pNode );
	void						_LoadBlade( ATTACK_BLADE& Param, const XML::Node* pNode );
	void						_LoadShield( ATTACK_SHIELD& Param, const XML::Node* pNode );
	void						_LoadRuler( ATTACK_RULER& Param, const XML::Node* pNode );
	void						_LoadConcurrent( ATTACK_CONCURRENT& Param, const XML::Node* pNode );
	void						_LoadEffect( const XML::Node* pNode );
	void						_LoadTimeKey( const XML::Node* pNode );

private:
	ATTACK_PARAM				m_Param;
	EffectResMap				m_EffectResMap;
	SoundResMap					m_SoundResMap;
	AttackResMap				m_AttackResMap;

	VecAttackTimeKey			m_vTimeKey[ ATTACK_TIME_KEY_TYPE_NUM ];
};

//------------------------------------------------------------------------------
//	攻撃データリソース
//------------------------------------------------------------------------------
class AttackResource : public ResourceTemplate< AttackData, RES_ATTACK >
{
};
