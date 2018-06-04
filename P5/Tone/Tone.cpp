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
//	サウンド管理
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
//	初期化
//******************************************************************************
bool Tone::Init( void )
{
	Log( "　□サウンド初期化開始" );

	CoInitializeEx( NULL, COINIT_MULTITHREADED );

	//	デバイスの生成
	m_pDevice = _CreateDevice();
	if( !m_pDevice ) return false;

	Log( "　○サウンド初期化完了" );
	Log( "　----------------------------------------" );

	return true;
}

//******************************************************************************
//	更新
//******************************************************************************
bool Tone::Update( void )
{
	//	削除済みサウンドの消去
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

	//	更新
	for( SoundList::iterator it = m_SoundList.begin(); it != m_SoundList.end(); ++it )
	{
		(*it)->Update();
	}

	//	参照リストを削除
	m_SoundRefMap.clear();

	return true;
}

//******************************************************************************
//	サウンドファイルを読み込み
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

	//	読み込み
	if( !pFile->Load( pPath ) )
	{
		SafeDelete( pFile );
	}

	return pFile;
}

//******************************************************************************
//	サウンド生成
//******************************************************************************
Sound* Tone::CreateSound( const SoundFile* pFile, SOUND_CHANNEL Ch, bool bLoop )
{
	_ASSERT( pFile );

	if( !m_pDevice ) return NULL;

	//	重複チェック
	Label Name = pFile->GetName();
	SoundMap::iterator it = m_SoundRefMap.find( Name );
	if( it != m_SoundRefMap.end() ) return it->second;

	Sound* pSound = new Sound;
	if( !pSound ) return NULL;
	
	//	サウンドオブジェクト作成
	SoundObject* pObject = m_pDevice->CreateSound( pFile, bLoop );
	if( !pObject ) 
	{
		SafeDelete( pSound );
		return NULL;
	}

	pSound->Init( pObject, Ch );

	//	登録
	m_SoundList.push_back( pSound );

	//	参照リストに登録
	m_SoundRefMap.insert( SoundMap::value_type( Name, pSound ) );

	return pSound;
}

//******************************************************************************
//	サウンド再生
//******************************************************************************
Sound* Tone::PlaySound( const SoundFile* pFile, SOUND_CHANNEL Ch, bool bLoop, float Volume )
{
	//	生成
	Sound* pSound = CreateSound( pFile, Ch, bLoop );
	if( !pSound ) return NULL;

	//	再生
	pSound->Play();
	pSound->SetVolume( Volume );

	return pSound;
}

//******************************************************************************
//	チャンネルのサウンドを停止
//******************************************************************************
void Tone::StopChannelSound( SOUND_CHANNEL Ch )
{	
	//	削除済みサウンドの消去
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
//	全サウンドを停止
//******************************************************************************
void Tone::StopAllSound( void )
{
	DeleteVec( m_SoundList );
}

//******************************************************************************
//	ミュート設定
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
//	ボリューム変更
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
//	チャンネルのボリューム変更
//******************************************************************************
void Tone::SetChannelVolume( SOUND_CHANNEL Ch, float Vol )
{
	m_ChannelVolume[Ch] = Vol;

	//	ボリューム更新
	for( SoundList::iterator it = m_SoundList.begin(); it != m_SoundList.end(); ++it )
	{
		if( (*it)->GetChannel() == Ch )
		{
			(*it)->UpdateVolume();
		}
	}
}

//------------------------------------------------------------------------------
//	デバイスの生成
//------------------------------------------------------------------------------
SoundDevice* Tone::_CreateDevice( void ) const
{
	//	DSoundで生成
	SoundDeviceDSound* pDeviceDSound = new SoundDeviceDSound;
	if( !pDeviceDSound ) return NULL;
	if( pDeviceDSound->Init() ) return pDeviceDSound;
	SafeDelete( pDeviceDSound );

	//	XAudioで生成
	SoundDeviceXAudio* pDeviceXAudio = new SoundDeviceXAudio;
	if( !pDeviceXAudio ) return NULL;
	if( pDeviceXAudio->Init() ) return pDeviceXAudio;
	SafeDelete( pDeviceXAudio );

	return NULL;
}


};