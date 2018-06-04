#pragma once

#include "SoundObject.h"


namespace P5
{

class OggLoader;

////////////////////////////////////////////////////////////////////////////////
//
//	�T�E���h�I�u�W�F�N�g(DSound)
//
////////////////////////////////////////////////////////////////////////////////
class SoundObjectDSound : public SoundObject
{
public:
	SoundObjectDSound( const SoundFile* pFile, bool bLoop );
	virtual ~SoundObjectDSound();

	virtual bool				Init( IDirectSound8* pDSound );

	virtual bool				Start( void );
	virtual bool				Stop( void );
	virtual bool				Reset( void );

	virtual void				SetVolume( float Vol );
	virtual void				SetPan( float Pan );

	virtual bool				IsPlaying( void )			const;

protected:
	IDirectSoundBuffer8*		m_pBuffer;
};


////////////////////////////////////////////////////////////////////////////////
//
//	�X�g���[�~���O�T�E���h�I�u�W�F�N�g(DSound)
//
////////////////////////////////////////////////////////////////////////////////
class SoundObjectStreamDSound : public SoundObjectDSound
{
public:
	SoundObjectStreamDSound( const SoundFile* pFile, bool bLoop );
	virtual ~SoundObjectStreamDSound();

	virtual bool				Init( IDirectSound8* pDSound );
	
	virtual void				Update( void );
	
	virtual bool				Reset( void );

protected:
	virtual const WAVEFORMATEX*	_GetFormat( void )		const;

private:
	enum
	{
		BUF_LENGTH				= 1,
	};

private:
	bool						_InitBuffer( void );
	void						_UpdateBuffer( void );
	uint						_GetBufSize( void );

private:
	OggLoader*					m_pLoader;
	bool						m_bLoadTop;
};


}