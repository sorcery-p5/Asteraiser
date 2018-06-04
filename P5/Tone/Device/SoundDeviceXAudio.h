#pragma once

#include "SoundDevice.h"

namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	サウンドデバイス（XAudio）
//
////////////////////////////////////////////////////////////////////////////////
class SoundDeviceXAudio : public SoundDevice
{
public:
	SoundDeviceXAudio( void );
	virtual ~SoundDeviceXAudio();

	virtual bool				Init( void );
	virtual SoundObject*		CreateSound( const SoundFile* pFile, bool bLoop );
	
	virtual void				SetVolume( float Vol );

private:
	IXAudio2*					m_pXAudio2;
	IXAudio2MasteringVoice* 	m_pMasteringVoice;
};

};