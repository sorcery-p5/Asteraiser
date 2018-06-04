#include "P5.h"
#include "SoundDeviceDSound.h"

#include "../Object/SoundObjectDSound.h"


namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	�T�E���h�f�o�C�X�iDSound�j
//
////////////////////////////////////////////////////////////////////////////////
SoundDeviceDSound::SoundDeviceDSound( void )
{
	m_pPrimaryBuffer	= NULL;
	m_pDSound			= NULL;
}
SoundDeviceDSound::~SoundDeviceDSound()
{
	SafeRelease( m_pPrimaryBuffer );
	SafeRelease( m_pDSound );
}

//******************************************************************************
//	������
//******************************************************************************
bool SoundDeviceDSound::Init( void )
{
	Log( "�@�@��DirectSound�������J�n" );

	HRESULT hr;

	//	DSound�̏�����
	hr = DirectSoundCreate8( NULL, &m_pDSound, NULL );
	if( FAILED(hr) )
	{
		Log( "�@�@�~DirectSound���������s" );
		return false;
	}

	//	�������x���ݒ�
	hr = m_pDSound->SetCooperativeLevel( P5::GetWindow()->GetHwnd(), DSSCL_PRIORITY );
	if( FAILED(hr) )
	{
		Log( "�@�@�~DirectSound�������x���ݒ莸�s" );
		return false;
	}

	//	�v���C�}���o�b�t�@���쐬
	DSBUFFERDESC Desc;
	ZeroStruct( Desc );
	Desc.dwSize = sizeof(DSBUFFERDESC);
	Desc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;

	hr = m_pDSound->CreateSoundBuffer( &Desc, &m_pPrimaryBuffer, NULL );
	if( FAILED(hr) )
	{
		Log( "�@�@��DirectSound�v���C�}���o�b�t�@���������s�E�\�t�g�E�F�A�����ŏ�����" );

		//	���s�E�\�t�g�E�F�A�ŏ�����
		Desc.dwFlags |= DSBCAPS_LOCSOFTWARE;
		hr = m_pDSound->CreateSoundBuffer( &Desc, &m_pPrimaryBuffer, NULL );
		if( FAILED(hr) )
		{
			Log( "�@�@�~DirectSound�v���C�}���o�b�t�@���������s" );
			return false;
		}
	}

	Log( "�@�@��DirectSound����������" );
	Log( "�@�@----------------------------------------" );

	return true;
}

//******************************************************************************
//	�T�E���h�I�u�W�F�N�g�̍쐬
//******************************************************************************
SoundObject* SoundDeviceDSound::CreateSound( const SoundFile* pFile, bool bLoop )
{
	if( !pFile ) return NULL;

	SoundObjectDSound* pObject = NULL;
	
	if( pFile->IsStream() )
	{
		pObject = new SoundObjectStreamDSound( pFile, bLoop );
	}
	else
	{
		pObject = new SoundObjectDSound( pFile, bLoop );
	}

	if( !pObject ) return NULL;
	
	if( !pObject->Init( m_pDSound ) )
	{
		SafeDelete( pObject );
	}

	return pObject;
}

//******************************************************************************
//	�{�����[���̐ݒ�
//******************************************************************************
void SoundDeviceDSound::SetVolume( float Vol )
{
	SafePtr( m_pPrimaryBuffer )->SetVolume( Lerp( DSBVOLUME_MIN,  DSBVOLUME_MAX, Vol ) );
}

};