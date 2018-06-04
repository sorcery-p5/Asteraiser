#pragma once

#include "EffectTypes.h"
#include "Resource/Resource.h"

class TextureResource;
class SpriteFrameFile;
class SpriteFrameResource;
class FontResource;
class SoundResource;
class EffectResource;
typedef std::map< Label, TextureResource* > TexResMap;
typedef std::map< Label, FontResource* >	FontResMap;


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクトファイル
//
////////////////////////////////////////////////////////////////////////////////
class EffectFile
{
public:
	EffectFile( void );
	~EffectFile();

	bool							Load( pstr pPath );
	void							LoadChild( const EffectFile* pParent, const XML::Node* pNode );

	const EFFECT_PARAM&				GetParam( void )					const		{ return m_Param; }
	const Texture*					GetTex( Label Name )				const;
	const SpriteFrameFile*			GetSpriteFrame( Label Name )		const;
	const Font*						GetFont( Label Name )				const;
	const SoundFile*				GetSoundFile( Label Name )			const;
	const EffectFile*				GetEffect( Label Name )				const;
	const Piece*					GetPiece( Label Name )				const;
	const EFFECT_PARTICLE*			GetParticleParam( Label Name )		const;
	const EFFECT_APPEAR*			GetAppearParam( uint Index )		const;
	uint							GetAppearParamNum( void )			const		{ return (uint)m_AppearVec.size(); }
	
private:
	typedef std::map< Label, SpriteFrameResource* >		SpriteFrameResMap;
	typedef std::map< Label, SoundResource* >			SoundResMap;
	typedef std::map< Label, EffectResource* >			EffectResMap;
	typedef std::map< Label, EffectFile* >				EffectFileMap;
	typedef std::map< Label, EFFECT_PARTICLE* >			ParticleMap;
	typedef std::vector< EFFECT_APPEAR >				AppearVec;

private:
	void							_Load( const XML::Node* pNode );
	void							_LoadEffectParam( const XML::Node* pNode );
	void							_LoadTex( const XML::Node* pNode );
	void							_LoadFont( const XML::Node* pNode );
	void							_LoadPiece( const XML::Node* pNode );
	void							_LoadParticle( const XML::Node* pNode );
	void							_LoadAppear( const XML::Node* pNode );
	void							_LoadTimeKey( const XML::Node* pNode, EFFECT_PARTICLE* pParticle );

	EFFECT_DIR_TYPE					_StrToDirType( pstr pStr );
	int								_StrToKeyType( pstr pStr );

private:
	EFFECT_PARAM					m_Param;
	TexResMap						m_TexResMap;
	SpriteFrameResMap				m_SpriteFrameResMap;
	FontResMap						m_FontResMap;
	SoundResMap						m_SoundResMap;
	EffectResMap					m_EffectResMap;
	EffectFileMap					m_EffectFileMap;
	PieceMap						m_PieceMap;
	ParticleMap						m_ParticleMap;
	AppearVec						m_AppearVec;

	const EffectFile*				m_pParent;
};

//------------------------------------------------------------------------------
//	エフェクトリソース
//------------------------------------------------------------------------------
class EffectResource : public ResourceTemplate< EffectFile, RES_EFFECT >
{
};