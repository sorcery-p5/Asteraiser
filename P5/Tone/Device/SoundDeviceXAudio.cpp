#include "P5.h"
#include "SoundDeviceXAudio.h"

#include "../Object/SoundObjectXAudio.h"


namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	サウンドデバイス（XAudio）
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
//	初期化
//******************************************************************************
bool SoundDeviceXAudio::Init( void )
{
	Log( "　　□XAudio初期化開始" );

	HRESULT hr;

	//	XAudioの初期化
	hr = XAudio2Create( &m_pXAudio2 );
	if( FAILED(hr) )
	{
		Log( "　　×XAudio初期化失敗" );
		return false;
	}

	//	マスターボイスを作成
	hr = m_pXAudio2->CreateMasteringVoice( &m_pMasteringVoice );
	if( FAILED(hr) )
	{
		Log( "　　×XAudioマスターボイス初期化失敗" );
		return false;
	}

	Log( "　　○XAudio初期化完了" );
	Log( "　　----------------------------------------" );

	return true;
}

//******************************************************************************
//	サウンドオブジェクトの作成
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
//	ボリュームの設定
//******************************************************************************
void SoundDeviceXAudio::SetVolume( float Vol )
{
	SafePtr( m_pMasteringVoice )->SetVolume( Vol );
}

};