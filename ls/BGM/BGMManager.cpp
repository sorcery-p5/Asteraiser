#include "stdafx.h"
#include "BGMManager.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"
#include "Resource/SoundResource.h"


////////////////////////////////////////////////////////////////////////////////
//
//	BGM管理
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
//	更新
//******************************************************************************
void BGMManager::Update( void )
{
	//	フェード
	if( m_pSound && m_FadeTime > 0 )
	{
		DecreaseZero( m_FadeCount, 1 );
		if( m_FadeCount == 0 )
		{
			if( m_bFadeStop )
			{
				//	フェード終了
				Stop();
			}
			else
			{
				//	音量固定
				SetVolume( m_FadeVolume );
			}
		}
		else
		{
			//	音量調整
			m_pSound->SetVolume( Lerp( m_FadeVolume, m_Volume, (float)m_FadeCount / (float)m_FadeTime ) );
		}
	}
}

//******************************************************************************
//	再生
//******************************************************************************
void BGMManager::Play( pstr pPath, float Volume )
{
	//	すでに再生中かどうか
	Label Name = GetFileTitle<256>( pPath );
	if( !m_pResource || m_pResource->GetName() != Name )
	{
		//	いったん停止
		Stop();

		//	読み込み
		m_pResource = GetApp()->GetResMng()->LoadResource<SoundResource>( pPath );
		if( !m_pResource ) return;

		const SoundFile* pFile = m_pResource->GetData();
		if( !pFile ) return;

		m_pSound = GetTone()->PlaySound( pFile, SOUND_CHANNEL_BGM, true, Volume );
	}
}

//******************************************************************************
//	フェードアウト
//******************************************************************************
void BGMManager::Fade( int Frame )
{
	m_FadeTime		= Frame;
	m_FadeCount		= Frame;
	m_FadeVolume	= 0.0f;
	m_bFadeStop		= true;
}

//******************************************************************************
//	フェード（ボリューム変更）
//******************************************************************************
void BGMManager::FadeVolume( int Frame, float Volume )
{
	m_FadeTime		= Frame;
	m_FadeCount		= Frame;
	m_FadeVolume	= Volume;
	m_bFadeStop		= false;
}

//******************************************************************************
//	停止
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
//	中断
//******************************************************************************
void BGMManager::Pause( void )
{
	if( m_pSound ) m_pSound->Pause();
}

//******************************************************************************
//	再開
//******************************************************************************
void BGMManager::Resume( void )
{
	if( m_pSound ) m_pSound->Resume();
}

//******************************************************************************
//	ボリューム設定
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