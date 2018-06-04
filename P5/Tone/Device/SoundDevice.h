#pragma once

namespace P5
{

class SoundObject;

////////////////////////////////////////////////////////////////////////////////
//
//	サウンドデバイス（基底）
//
////////////////////////////////////////////////////////////////////////////////
class SoundDevice
{
public:
	virtual ~SoundDevice()														{}

	virtual bool			Init( void )										= 0;
	virtual SoundObject*	CreateSound( const SoundFile* pFile, bool bLoop )	= 0;
	
	virtual void			SetVolume( float Vol )								= 0;
};

};