#pragma once

class SpriteFrameFile;

#include "SpriteFrameTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	スプライトフレーム：アニメデータ
//
////////////////////////////////////////////////////////////////////////////////
class SpriteFrameAnimeData
{
public:
	SpriteFrameAnimeData( void );
	~SpriteFrameAnimeData();

	void						Init( uint BoneNum, int EndFrame, bool bLoop, int LoopFrame );
	void						AddKeyFrame( uint BoneID, uint KeyType, const SPRITE_FRAME_KEY& Key );
	void						AddAppearParam( uint BoneID, const SPRITE_FRAME_APPEAR& Param );
	void						AddNotifyParam( const SPRITE_FRAME_NOTIFY& Param );

	int							GetEndFrame( void )									const	{ return m_EndFrame; }
	bool						IsLoop( void )										const	{ return m_bLoop; }
	int							GetLoopFrame( void )								const	{ return m_LoopFrame; }

	bool						IsBoneEmpty( uint BoneID )							const;
	bool						IsKeyEmpty( uint BoneID, uint KeyType )				const;
	const SPRITE_FRAME_KEY*		GetKey( uint BoneID, uint KeyType, uint Index )		const;
	uint						GetAppearNum( uint BoneID )							const;
	const SPRITE_FRAME_APPEAR*	GetAppear( uint BoneID, uint Index )				const;
	uint						GetNotifyNum( void )								const;
	const SPRITE_FRAME_NOTIFY*	GetNotify( uint Index )								const;

private:
	typedef std::vector< SPRITE_FRAME_KEY > VecKey;

	struct BONE_KEY
	{
		VecKey		aKey[ SPRITE_FRAME_KEY_NUM ];
	};
	typedef std::vector< BONE_KEY* > VecBoneKey;

	typedef std::vector< SPRITE_FRAME_APPEAR >	VecAppear;
	typedef std::vector< VecAppear >			VecBoneAppear;
	typedef std::vector< SPRITE_FRAME_NOTIFY >	VecNotify;

private:
	void						_Delete( void );

private:
	VecBoneKey					m_vBoneKey;
	VecBoneAppear				m_vAppear;
	VecNotify					m_vNotify;
	int							m_EndFrame;
	bool						m_bLoop;
	int							m_LoopFrame;
};