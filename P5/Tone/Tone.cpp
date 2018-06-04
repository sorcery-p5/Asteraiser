#include "P5.h"

#include "Device/SoundDevice.h"
#include "Device/SoundDeviceXAudio.h"
#include "Device/SoundDeviceDSound.h"

#include "File/SoundFileWave.h"
#include "File/SoundFileOgg.h"

namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	�T�E���h�Ǘ�
//
////////////////////////////////////////////////////////////////////////////////
Tone::Tone( void )
{
	m_pDevice = NULL;

	m_Volume	= 1.0f;
	m_bMute		= false;
	FillArray( m_ChannelVolume, ArrayNum( m_ChannelVolume ), 0.5f );
}
Tone::~Tone()
{
	DeleteVec( m_SoundList );

	SafeDelete( m_pDevice );

	CoUninitialize();
}

//******************************************************************************
//	������
//******************************************************************************
bool Tone::Init( void )
{
	Log( "�@���T�E���h�������J�n" );

	CoInitializeEx( NULL, COINIT_MULTITHREADED );

	//	�f�o�C�X�̐���
	m_pDevice = _CreateDevice();
	if( !m_pDevice ) return false;

	Log( "�@���T�E���h����������" );
	Log( "�@----------------------------------------" );

	return true;
}

//******************************************************************************
//	�X�V
//******************************************************************************
bool Tone::Update( void )
{
	//	�폜�ς݃T�E���h�̏���
	for( SoundList::iterator it = m_SoundList.begin(); it != m_SoundList.end(); )
	{
		if( (*it)->IsDelete() )
		{
			SafeDelete( *it );
			it = m_SoundList.erase( it );
		}
		else
		{
			++it;
		}
	}

	//	�X�V
	for( SoundList::iterator it = m_SoundList.begin(); it != m_SoundList.end(); ++it )
	{
		(*it)->Update();
	}

	//	�Q�ƃ��X�g���폜
	m_SoundRefMap.clear();

	return true;
}

//******************************************************************************
//	�T�E���h�t�@�C����ǂݍ���
//******************************************************************************
SoundFile* Tone::LoadSoundFile( pstr pPath )
{
	SoundFile* pFile = NULL;

	String256 Ext = GetFileExt<256>( pPath );
	if( Ext.IComp("wav") || Ext.IComp("wave") )
	{
		pFile = new SoundFileWave;
	}
	else if( Ext.IComp("ogg") )
	{
		pFile = new SoundFileOgg;
	}
	if( !pFile ) return NULL;

	//	�ǂݍ���
	if( !pFile->Load( pPath ) )
	{
		SafeDelete( pFile );
	}

	return pFile;
}

//******************************************************************************
//	�T�E���h����
//******************************************************************************
Sound* Tone::CreateSound( const SoundFile* pFile, SOUND_CHANNEL Ch, bool bLoop )
{
	_ASSERT( pFile );

	if( !m_pDevice ) return NULL;

	//	�d���`�F�b�N
	Label Name = pFile->GetName();
	SoundMap::iterator it = m_SoundRefMap.find( Name );
	if( it != m_SoundRefMap.end() ) return it->second;

	Sound* pSound = new Sound;
	if( !pSound ) return NULL;
	
	//	�T�E���h�I�u�W�F�N�g�쐬
	SoundObject* pObject = m_pDevice->CreateSound( pFile, bLoop );
	if( !pObject ) 
	{
		SafeDelete( pSound );
		return NULL;
	}

	pSound->Init( pObject, Ch );

	//	�o�^
	m_SoundList.push_back( pSound );

	//	�Q�ƃ��X�g�ɓo�^
	m_SoundRefMap.insert( SoundMap::value_type( Name, pSound ) );

	return pSound;
}

//******************************************************************************
//	�T�E���h�Đ�
//******************************************************************************
Sound* Tone::PlaySound( const SoundFile* pFile, SOUND_CHANNEL Ch, bool bLoop, float Volume )
{
	//	����
	Sound* pSound = CreateSound( pFile, Ch, bLoop );
	if( !pSound ) return NULL;

	//	�Đ�
	pSound->Play();
	pSound->SetVolume( Volume );

	return pSound;
}

//******************************************************************************
//	�`�����l���̃T�E���h���~
//******************************************************************************
void Tone::StopChannelSound( SOUND_CHANNEL Ch )
{	
	//	�폜�ς݃T�E���h�̏���
	for( SoundList::iterator it = m_SoundList.begin(); it != m_SoundList.end(); )
	{
		if( (*it)->GetChannel() == Ch )
		{
			SafeDelete( *it );
			it = m_SoundList.erase( it );
		}
		else
		{
			++it;
		}
	}
}

//******************************************************************************
//	�S�T�E���h���~
//******************************************************************************
void Tone::StopAllSound( void )
{
	DeleteVec( m_SoundList );
}

//******************************************************************************
//	�~���[�g�ݒ�
//******************************************************************************
void Tone::SetMute( bool bMute )
{
	if( !m_pDevice ) return;

	if( bMute != m_bMute )
	{
		m_pDevice->SetVolume( bMute? 0.0f : m_Volume );
		m_bMute = bMute;
	}
}

//******************************************************************************
//	�{�����[���ύX
//******************************************************************************
void Tone::SetVolume( float Vol )
{
	if( !m_pDevice ) return;

	if( !m_bMute )
	{
		m_pDevice->SetVolume( Vol );
	}

	m_Volume = Vol;
}

//******************************************************************************
//	�`�����l���̃{�����[���ύX
//******************************************************************************
void Tone::SetChannelVolume( SOUND_CHANNEL Ch, float Vol )
{
	m_ChannelVolume[Ch] = Vol;

	//	�{�����[���X�V
	for( SoundList::iterator it = m_SoundList.begin(); it != m_SoundList.end(); ++it )
	{
		if( (*it)->GetChannel() == Ch )
		{
			(*it)->UpdateVolume();
		}
	}
}

//------------------------------------------------------------------------------
//	�f�o�C�X�̐���
//------------------------------------------------------------------------------
SoundDevice* Tone::_CreateDevice( void ) const
{
	//	DSound�Ő���
	SoundDeviceDSound* pDeviceDSound = new SoundDeviceDSound;
	if( !pDeviceDSound ) return NULL;
	if( pDeviceDSound->Init() ) return pDeviceDSound;
	SafeDelete( pDeviceDSound );

	//	XAudio�Ő���
	SoundDeviceXAudio* pDeviceXAudio = new SoundDeviceXAudio;
	if( !pDeviceXAudio ) return NULL;
	if( pDeviceXAudio->Init() ) return pDeviceXAudio;
	SafeDelete( pDeviceXAudio );

	return NULL;
}


};