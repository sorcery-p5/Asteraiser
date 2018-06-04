#include "stdafx.h"
#include "BGMManager.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"
#include "Resource/SoundResource.h"


////////////////////////////////////////////////////////////////////////////////
//
//	BGM�Ǘ�
//
////////////////////////////////////////////////////////////////////////////////
BGMManager::BGMManager( void )
{
	m_pResource		= NULL;

	m_FadeTime		= 0;
	m_FadeCount		= 0;
	m_Volume		= 0.0f;
	m_FadeVolume	= 0.0f;
	m_bFadeStop		= false;
}

BGMManager::~BGMManager()
{
	SafeStop( m_pSound );
	SafeRelease( m_pResource );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void BGMManager::Update( void )
{
	//	�t�F�[�h
	if( m_pSound && m_FadeTime > 0 )
	{
		DecreaseZero( m_FadeCount, 1 );
		if( m_FadeCount == 0 )
		{
			if( m_bFadeStop )
			{
				//	�t�F�[�h�I��
				Stop();
			}
			else
			{
				//	���ʌŒ�
				SetVolume( m_FadeVolume );
			}
		}
		else
		{
			//	���ʒ���
			m_pSound->SetVolume( Lerp( m_FadeVolume, m_Volume, (float)m_FadeCount / (float)m_FadeTime ) );
		}
	}
}

//******************************************************************************
//	�Đ�
//******************************************************************************
void BGMManager::Play( pstr pPath, float Volume )
{
	//	���łɍĐ������ǂ���
	Label Name = GetFileTitle<256>( pPath );
	if( !m_pResource || m_pResource->GetName() != Name )
	{
		//	���������~
		Stop();

		//	�ǂݍ���
		m_pResource = GetApp()->GetResMng()->LoadResource<SoundResource>( pPath );
		if( !m_pResource ) return;

		const SoundFile* pFile = m_pResource->GetData();
		if( !pFile ) return;

		m_pSound = GetTone()->PlaySound( pFile, SOUND_CHANNEL_BGM, true, Volume );
	}
}

//******************************************************************************
//	�t�F�[�h�A�E�g
//******************************************************************************
void BGMManager::Fade( int Frame )
{
	m_FadeTime		= Frame;
	m_FadeCount		= Frame;
	m_FadeVolume	= 0.0f;
	m_bFadeStop		= true;
}

//******************************************************************************
//	�t�F�[�h�i�{�����[���ύX�j
//******************************************************************************
void BGMManager::FadeVolume( int Frame, float Volume )
{
	m_FadeTime		= Frame;
	m_FadeCount		= Frame;
	m_FadeVolume	= Volume;
	m_bFadeStop		= false;
}

//******************************************************************************
//	��~
//******************************************************************************
void BGMManager::Stop( void )
{
	SafeStop( m_pSound );
	SafeRelease( m_pResource );
		
	m_FadeCount		= 0;
	m_FadeTime		= 0;
	m_Volume		= 0.0f;
	m_FadeVolume	= 0.0f;
}

//******************************************************************************
//	���f
//******************************************************************************
void BGMManager::Pause( void )
{
	if( m_pSound ) m_pSound->Pause();
}

//******************************************************************************
//	�ĊJ
//******************************************************************************
void BGMManager::Resume( void )
{
	if( m_pSound ) m_pSound->Resume();
}

//******************************************************************************
//	�{�����[���ݒ�
//******************************************************************************
void BGMManager::SetVolume( float Volume )
{
	if( m_pSound ) m_pSound->SetVolume( Volume );
	m_Volume = Volume;

	m_FadeVolume = 0.0f;
	
	m_FadeCount		= 0;
	m_FadeTime		= 0;
	m_bFadeStop		= false;
}