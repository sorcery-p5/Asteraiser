#include "stdafx.h"
#include "SoundResource.h"


////////////////////////////////////////////////////////////////////////////////
//
//	サウンドファイルリソース
//
////////////////////////////////////////////////////////////////////////////////
SoundResource::SoundResource( void )
: Resource()
{
	m_pSoundFile = NULL;
}
SoundResource::~SoundResource()
{
	SafeDelete( m_pSoundFile );
}

//******************************************************************************
//	読み込み
//******************************************************************************
bool SoundResource::Load( pstr pPath )
{
	m_pSoundFile = GetTone()->LoadSoundFile( pPath );

	return m_pSoundFile != NULL;
}