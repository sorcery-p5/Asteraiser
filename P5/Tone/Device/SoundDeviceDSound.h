#pragma once

#include "SoundDevice.h"

namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	サウンドデバイス（DSound）
//
////////////////////////////////////////////////////////////////////////////////
class SoundDeviceDSound : public SoundDevice
{
public:
	SoundDeviceDSound( void );
	virtual ~SoundDeviceDSound();

	virtual bool			Init( void );
	virtual SoundObject*	CreateSound( const SoundFile* pFile, bool bLoop );
	
	virtual void			SetVolume( float Vol );

private:
	IDirectSound8*			m_pDSound;
	IDirectSoundBuffer*		m_pPrimaryBuffer;
};

};