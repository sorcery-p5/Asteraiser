#pragma once

#include "SpriteFrameTypes.h"

class SpriteFrameFile;
class SpriteFrameAnimeData;
class SpriteFrameBone;


////////////////////////////////////////////////////////////////////////////////
//
//	スプライトフレームアニメ
//
////////////////////////////////////////////////////////////////////////////////
class SpriteFrameAnime
{
public:
	typedef std::vector<const SPRITE_FRAME_APPEAR*> VecAppear;

public:
	SpriteFrameAnime( void );
	~SpriteFrameAnime();

	void						Init( const SpriteFrameFile* pFile );
	void						Update( void );

	void						Play( Label Name, uint Intp );
	void						Stop( void );
	void						Pause( bool bPause )											{ m_bPause = bPause; }
	void						SetFrameSpeed( float Speed )									{ m_FrameSpeed = Speed; }

	bool						IsPlaying( void )										const	{ return m_pPlayData != NULL; }
	bool						IsLastFrame( void )										const;
	bool						IsAnimeLoopFrame( void )								const;

	Label						GetAnimeName( void )									const	{ return m_PlayName; }
	void						GetBoneMatrix( Matrix3& Mat, uint BoneID )				const;
	Point						GetBoneCenterPos( uint BoneID )							const;
	Color						GetBoneColor( uint BoneID )								const;
	uint						GetBoneSpriteIndex( uint BoneID )						const;
	Label						GetBonePieceName( uint BoneID )							const;
	pstr						GetBoneText( uint BoneID )								const;
	pstr						GetNotify( Label Key )									const;

	void						ApplyAppear( SpriteFrameBone* pBone );

	static void					CalcMatrix( Matrix3& Mat, const Point& Pos, float Rot, const Point& Scale );

private:
	struct BONE_INFO
	{
		int						aKeyIndex[ SPRITE_FRAME_KEY_NUM ];
		
		Point					Pos;
		float					Rot;
		Point					Scale;
		Point					Center;
		Color					Col;

		int						AppearIndex;
		VecAppear				vAppear;
	};
	typedef std::vector< BONE_INFO >		VecBoneInfo;
	typedef std::map< Label, std::string >	MapNotify;

private:
	void						_ResetBoneParam( void );
	void						_ResetBoneKey( void );
	void						_UpdateBoneKey( void );
	void						_UpdateNotify( void );
	void						_UpdateAppear( void );

	Point						_GetKeyFloat2( uint BoneID, uint KeyType )	const;
	Angle						_GetKeyRot( uint BoneID )					const;
	Color						_GetKeyColor( uint BoneID )					const;

	int							_GetFrameCount( void )						const	{ return (int)Round(m_FrameCount); }

private:
	const SpriteFrameFile*		m_pFile;
	const SpriteFrameAnimeData*	m_pPlayData;
	Label						m_PlayName;

	VecBoneInfo					m_vBoneInfo;
	float						m_FrameCount;
	float						m_FrameSpeed;
	uint						m_IntpCount;
	bool						m_bPause;

	MapNotify					m_NotifyMap;
};