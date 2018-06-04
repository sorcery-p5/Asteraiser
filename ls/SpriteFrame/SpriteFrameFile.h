#pragma once

#include "SpriteFrameTypes.h"
#include "Resource/Resource.h"

class TextureResource;
class SpriteFrameAnimeData;
class EffectFile;
class EffectResource;
class SoundResource;
class FontResource;
typedef std::map< Label, TextureResource* > TexResMap;
typedef std::map< Label, FontResource* >	FontResMap;


///////////////////////////////////////////////////////////////////////////////
//
//	スプライトフレーム：ファイル
//
///////////////////////////////////////////////////////////////////////////////
class SpriteFrameFile
{
public:
	SpriteFrameFile( void );
	~SpriteFrameFile();

	bool							Load( pstr pPath );
	
	int								GetBoneNum( void )				const	{ return (int)m_vBoneParam.size(); }
	const SPRITE_FRAME_BONE*		GetBoneParam( int BoneID )		const;
	const Piece*					GetPiece( Label Name )			const;
	const Texture*					GetTex( Label Name )			const;
	const Font*						GetFont( Label Name )			const;
	const EffectFile*				GetEffectFile( Label Name )		const;
	const SoundFile*				GetSoundFile( Label Name )		const;
	const SpriteFrameAnimeData*		GetAnimeData( Label Name )		const;
	BLEND_MODE						GetBlendMode( int BoneID )		const;
	int								GetDrawPhase( int BoneID )		const;
	pstr							GetText( int BoneID )			const;
	FONT_ALIGN						GetAlign( int BoneID )			const;
	const SPRITE_FRAME_COLLIDE*		GetCollide( int BoneID )		const;
	const SPRITE_FRAME_RING*		GetRing( int BoneID )			const;

	int								FindBoneIndex( Label Name )		const;
	bool							HasAnime( void )				const	{ return !m_AnimeDataMap.empty(); }
	int								GetAnimeNum( void )				const	{ return (int)m_AnimeDataMap.size(); }
	Label							GetAnimeName( int Index )		const;

private:
	typedef std::vector<SPRITE_FRAME_BONE>				VecBoneParam;
	typedef std::map< Label, SpriteFrameAnimeData* >	AnimeDataMap;
	typedef std::map< Label, EffectResource* >			EffectResMap;
	typedef std::map< Label, SoundResource* >			SoundResMap;
	typedef std::map< int, BLEND_MODE >					BlendMap;
	typedef std::map< int, int >						PhaseMap;
	typedef std::map< int, std::string >				StringMap;
	typedef std::map< int, FONT_ALIGN >					AlignMap;
	typedef std::map< int, SPRITE_FRAME_COLLIDE >		CollideMap;
	typedef std::map< int, SPRITE_FRAME_RING >			RingMap;

private:
	void						_LoadFrameParam( const XML::Node* pNode );
	void						_LoadTex( const XML::Node* pNode );
	void						_LoadFont( const XML::Node* pNode );
	void						_LoadPiece( const XML::Node* pNode );
	int							_LoadBone( const XML::Node* pNode, int ParentID );
	void						_LoadAnime( const XML::Node* pNode );
	void						_LoadCollide( const XML::Node* pNode );
	void						_LoadAnimeBone( const XML::Node* pDataNode, SpriteFrameAnimeData* pData );
	void						_LoadAnimeKey( const XML::Node* pBoneNode, SpriteFrameAnimeData* pData, int BoneID );
	void						_LoadBoneEffect( SPRITE_FRAME_BONE& Bone, const XML::Node* pNode );
	void						_LoadBoneSound( SPRITE_FRAME_BONE& Bone, const XML::Node* pNode );
	void						_LoadEffectParam( SPRITE_FRAME_EFFECT& Effect, const XML::Node* pNode );
	void						_LoadSoundParam( SPRITE_FRAME_SOUND& Sound, const XML::Node* pNode );
	bool						_LoadRing( SPRITE_FRAME_RING& Param, const XML::Node* pNode );

	int							_LabelToKeyType( Label Type );
	int							_LabelToAppearType( Label Type );
	SPRITE_FRAME_INTP			_LabelToIntpType( Label Type );
	int							_LabelToCollideType( Label Type );
	SPRITE_FRAME_BONE_DIR		_LabelToBoneDirType( Label Type );

private:
	SPRITE_FRAME				m_Param;
	VecBoneParam				m_vBoneParam;
	TexResMap					m_TexResMap;
	FontResMap					m_FontResMap;
	EffectResMap				m_EffectResMap;
	SoundResMap					m_SoundResMap;
	PieceMap					m_PieceMap;
	AnimeDataMap				m_AnimeDataMap;
	BlendMap					m_BlendMap;
	PhaseMap					m_PhaseMap;
	StringMap					m_TextMap;
	AlignMap					m_AlignMap;
	CollideMap					m_CollideMap;
	RingMap						m_RingMap;
};

//------------------------------------------------------------------------------
//	スプライトフレームリソース
//------------------------------------------------------------------------------
class SpriteFrameResource : public ResourceTemplate< SpriteFrameFile, RES_SPRITE_FRAME >
{
};
