#pragma once

#include "Resource.h"


////////////////////////////////////////////////////////////////////////////////
//
//	サウンドリソース
//
////////////////////////////////////////////////////////////////////////////////
class SoundResource : public Resource
{
public:
	static const RES_TYPE	TYPE = RES_SOUND;

public:
	SoundResource( void );
	virtual ~SoundResource();

	virtual bool			Load( pstr pPath );

	const SoundFile*		GetData( void )		const	{ return m_pSoundFile; }

private:
	SoundFile*				m_pSoundFile;
};

typedef std::map< Label, SoundResource* >			SoundResMap;