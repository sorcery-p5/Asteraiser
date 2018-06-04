#include "stdafx.h"
#include "SoundResource.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�T�E���h�t�@�C�����\�[�X
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
//	�ǂݍ���
//******************************************************************************
bool SoundResource::Load( pstr pPath )
{
	m_pSoundFile = GetTone()->LoadSoundFile( pPath );

	return m_pSoundFile != NULL;
}