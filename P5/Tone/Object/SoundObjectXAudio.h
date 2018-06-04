#pragma once

#include "SoundObject.h"
#include "../OggLoader.h"


namespace P5
{


////////////////////////////////////////////////////////////////////////////////
//
//	サウンドオブジェクト(XAudio)
//
////////////////////////////////////////////////////////////////////////////////
class SoundObjectXAudio : public SoundObject, public IXAudio2VoiceCallback
{
public:
	SoundObjectXAudio( const SoundFile* pFile, bool bLoop );
	virtual ~SoundObjectXAudio();

	virtual bool				Init( IXAudio2* pXAudio );

	virtual bool				Start( void );
	virtual bool				Stop( void );
	virtual bool				Reset( void );

	virtual void				SetVolume( float Vol );
	virtual void				SetPan( float Pan );

	virtual bool				IsPlaying( void )					const	{ return m_bPlaying; }

	//	コールバック
	STDMETHOD_( void, OnStreamEnd )					( void );
	STDMETHOD_( void, OnVoiceProcessingPassEnd )	( void )				{}
	STDMETHOD_( void, OnVoiceProcessingPassStart )	( void )				{}
	STDMETHOD_( void, OnVoiceProcessingPassStart )	( UINT32 )				{}
	STDMETHOD_( void, OnBufferEnd )					( void* )				{}
	STDMETHOD_( void, OnBufferStart )				( void* )				{}
	STDMETHOD_( void, OnLoopEnd )					( void* )				{}
	STDMETHOD_( void, OnVoiceError )				( void*, HRESULT )		{}

protected:
	IXAudio2SourceVoice*		m_pVoice;
	bool						m_bPlaying;
};


////////////////////////////////////////////////////////////////////////////////
//
//	ストリーミングサウンドオブジェクト(XAudio)
//
////////////////////////////////////////////////////////////////////////////////
class SoundObjectStreamXAudio : public SoundObjectXAudio
{
public:
	SoundObjectStreamXAudio( const SoundFile* pFile, bool bLoop );
	virtual ~SoundObjectStreamXAudio();

	virtual bool				Init( IXAudio2* pXAudio );
	
	virtual bool				Reset( void );
	
	//	コールバック
	STDMETHOD_( void, OnBufferEnd )					( void* );

protected:
	virtual const WAVEFORMATEX*	_GetFormat( void )		const;

private:
	enum
	{
		BUF_NUM				= 16,
		BUF_SIZE			= (1024 * 32),
	};

private:
	OggLoader*					m_pLoader;
	OggLoader::BUFFER			m_aBuffer[BUF_NUM];
	uint						m_BufferPos;
	bool						m_bBufferEnd;
};



}