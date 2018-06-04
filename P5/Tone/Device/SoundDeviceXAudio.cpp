#include "P5.h"
#include "SoundDeviceXAudio.h"

#include "../Object/SoundObjectXAudio.h"


namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	�T�E���h�f�o�C�X�iXAudio�j
//
////////////////////////////////////////////////////////////////////////////////
SoundDeviceXAudio::SoundDeviceXAudio( void )
{
	m_pXAudio2			= NULL;
	m_pMasteringVoice	= NULL;
}
SoundDeviceXAudio::~SoundDeviceXAudio()
{
	if( m_pMasteringVoice ) m_pMasteringVoice->DestroyVoice();
	SafeRelease( m_pXAudio2 );
}

//******************************************************************************
//	������
//******************************************************************************
bool SoundDeviceXAudio::Init( void )
{
	Log( "�@�@��XAudio�������J�n" );

	HRESULT hr;

	//	XAudio�̏�����
	hr = XAudio2Create( &m_pXAudio2 );
	if( FAILED(hr) )
	{
		Log( "�@�@�~XAudio���������s" );
		return false;
	}

	//	�}�X�^�[�{�C�X���쐬
	hr = m_pXAudio2->CreateMasteringVoice( &m_pMasteringVoice );
	if( FAILED(hr) )
	{
		Log( "�@�@�~XAudio�}�X�^�[�{�C�X���������s" );
		return false;
	}

	Log( "�@�@��XAudio����������" );
	Log( "�@�@----------------------------------------" );

	return true;
}

//******************************************************************************
//	�T�E���h�I�u�W�F�N�g�̍쐬
//******************************************************************************
SoundObject* SoundDeviceXAudio::CreateSound( const SoundFile* pFile, bool bLoop )
{
	SoundObjectXAudio* pObject = NULL;
	if( pFile->IsStream() )
	{
		pObject = new SoundObjectStreamXAudio( pFile, bLoop );
	}
	else
	{
		pObject = new SoundObjectXAudio( pFile, bLoop );
	}

	if( !pObject ) return NULL;

	if( !pObject->Init( m_pXAudio2 ) )
	{
		SafeDelete( pObject );
	}

	return pObject;
}

//******************************************************************************
//	�{�����[���̐ݒ�
//******************************************************************************
void SoundDeviceXAudio::SetVolume( float Vol )
{
	SafePtr( m_pMasteringVoice )->SetVolume( Vol );
}

};