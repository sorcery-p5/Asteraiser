#include "P5.h"
#include "SoundDeviceDSound.h"

#include "../Object/SoundObjectDSound.h"


namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	サウンドデバイス（DSound）
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
//	初期化
//******************************************************************************
bool SoundDeviceDSound::Init( void )
{
	Log( "　　□DirectSound初期化開始" );

	HRESULT hr;

	//	DSoundの初期化
	hr = DirectSoundCreate8( NULL, &m_pDSound, NULL );
	if( FAILED(hr) )
	{
		Log( "　　×DirectSound初期化失敗" );
		return false;
	}

	//	協調レベル設定
	hr = m_pDSound->SetCooperativeLevel( P5::GetWindow()->GetHwnd(), DSSCL_PRIORITY );
	if( FAILED(hr) )
	{
		Log( "　　×DirectSound強調レベル設定失敗" );
		return false;
	}

	//	プライマリバッファを作成
	DSBUFFERDESC Desc;
	ZeroStruct( Desc );
	Desc.dwSize = sizeof(DSBUFFERDESC);
	Desc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;

	hr = m_pDSound->CreateSoundBuffer( &Desc, &m_pPrimaryBuffer, NULL );
	if( FAILED(hr) )
	{
		Log( "　　△DirectSoundプライマリバッファ初期化失敗・ソフトウェア処理で初期化" );

		//	失敗・ソフトウェアで初期化
		Desc.dwFlags |= DSBCAPS_LOCSOFTWARE;
		hr = m_pDSound->CreateSoundBuffer( &Desc, &m_pPrimaryBuffer, NULL );
		if( FAILED(hr) )
		{
			Log( "　　×DirectSoundプライマリバッファ初期化失敗" );
			return false;
		}
	}

	Log( "　　○DirectSound初期化完了" );
	Log( "　　----------------------------------------" );

	return true;
}

//******************************************************************************
//	サウンドオブジェクトの作成
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
//	ボリュームの設定
//******************************************************************************
void SoundDeviceDSound::SetVolume( float Vol )
{
	SafePtr( m_pPrimaryBuffer )->SetVolume( Lerp( DSBVOLUME_MIN,  DSBVOLUME_MAX, Vol ) );
}

};