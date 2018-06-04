#pragma once

#include <xaudio2.h>
#include <dsound.h>

namespace P5
{

class Sound;
class SoundFile;
class SoundDevice;

//------------------------------------------------------------------------------
//	サウンドのチャンネル
//------------------------------------------------------------------------------
enum SOUND_CHANNEL
{
	SOUND_CHANNEL_BGM,
	SOUND_CHANNEL_SE,
	
	SOUND_CHANNEL_NUM,
};

////////////////////////////////////////////////////////////////////////////////
//
//	サウンド管理
//
////////////////////////////////////////////////////////////////////////////////
class Tone
{
public:
	Tone( void );
	~Tone();

	bool							Init( void );
	bool							Update( void );

	SoundFile*						LoadSoundFile( pstr pPath );

	Sound*							CreateSound( const SoundFile* pFile, SOUND_CHANNEL Ch, bool bLoop );
	Sound*							PlaySound( const SoundFile* pFile, SOUND_CHANNEL Ch, bool bLoop, float Volume = 1.0f );
	void							StopChannelSound( SOUND_CHANNEL Ch );
	void							StopAllSound( void );

	void							SetMute( bool bMute );
	void							SetVolume( float Vol );
	void							SetChannelVolume( SOUND_CHANNEL Ch, float Vol );
	float							GetChannelVolume( SOUND_CHANNEL Ch )					const	{ return m_ChannelVolume[Ch]; }

private:
	typedef std::list< Sound* >			SoundList;
	typedef std::map< Label, Sound* >	SoundMap;

private:
	SoundDevice*					_CreateDevice( void )			const;

private:
	SoundDevice*					m_pDevice;
	SoundList						m_SoundList;
	SoundMap						m_SoundRefMap;

	float							m_Volume;
	float							m_ChannelVolume[SOUND_CHANNEL_NUM];
	bool							m_bMute;
};

}

#include "Sound.h"
#include "File/SoundFile.h"