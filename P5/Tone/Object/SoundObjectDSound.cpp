#include "P5.h"
#include "SoundObjectDSound.h"

#include "../OggLoader.h"
#include "../File/SoundFile.h"


namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	サウンドオブジェクト(DSound)
//
////////////////////////////////////////////////////////////////////////////////
SoundObjectDSound::SoundObjectDSound( const SoundFile* pFile, bool bLoop )
: SoundObject( pFile, bLoop )
{
	m_pBuffer	= NULL;
}

SoundObjectDSound::~SoundObjectDSound()
{
	SafeRelease( m_pBuffer );
}

//******************************************************************************
//	初期化
//******************************************************************************
bool SoundObjectDSound::Init( IDirectSound8* pDSound )
{
	HRESULT hr;

	//	DESCを作成
	DSBUFFERDESC Desc;
	ZeroStruct( Desc );
	Desc.dwSize			= sizeof(DSBUFFERDESC);
	Desc.dwFlags		= DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
	Desc.dwBufferBytes	= m_pFile->GetSize();
	Desc.lpwfxFormat	= (WAVEFORMATEX*)_GetFormat();

	//	バッファを作成
	LPDIRECTSOUNDBUFFER pDsb = NULL;
	hr = pDSound->CreateSoundBuffer( &Desc, &pDsb, NULL );
	if( FAILED(hr) ) return false;

	hr = pDsb->QueryInterface( IID_IDirectSoundBuffer8, (LPVOID*)&m_pBuffer );
	pDsb->Release();
	if( FAILED(hr) ) return false;

	//	バッファに書き込み
	void*	pData;
	dword	Size;
	hr = m_pBuffer->Lock( 0, 0, &pData, &Size, NULL, NULL, DSBLOCK_ENTIREBUFFER );
	if( FAILED(hr) ) return false;

	//	データ書き込み
	CopyMemory( pData, m_pFile->GetData(), Size );

	//	ロック解除
	m_pBuffer->Unlock( pData, Size, NULL, 0);

	return true;
}

//******************************************************************************
//	再生
//******************************************************************************
bool SoundObjectDSound::Start( void )
{
	if( !m_pBuffer ) return false;

	HRESULT hr = m_pBuffer->Play( 0, 0, (m_bLoop || m_pFile->IsStream())? DSBPLAY_LOOPING : 0 );
	if( FAILED(hr) ) return false;

	return true;
}

//******************************************************************************
//	再生停止
//******************************************************************************
bool SoundObjectDSound::Stop( void )
{
	if( !m_pBuffer ) return false;

	HRESULT hr = m_pBuffer->Stop();
	if( FAILED(hr) ) return false;

	return true;
}

//******************************************************************************
//	リセット
//******************************************************************************
bool SoundObjectDSound::Reset( void )
{
	if( !m_pBuffer ) return false;

	m_pBuffer->SetCurrentPosition( 0 );

	return true;
}

//******************************************************************************
//	ボリューム設定
//******************************************************************************
void SoundObjectDSound::SetVolume( float Vol )
{
	LONG Val = (LONG)( Vol == 0.0f? DSBVOLUME_MIN : log10( Vol ) * 1000 );
	SafePtr( m_pBuffer )->SetVolume( Val );
}

//******************************************************************************
//	パン設定
//******************************************************************************
void SoundObjectDSound::SetPan( float Pan )
{
	SafePtr( m_pBuffer )->SetPan( Lerp( DSBPAN_LEFT, DSBPAN_RIGHT, Coef( Pan, -1.0f, 1.0f ) ) );
}

//******************************************************************************
//	再生中かどうか
//******************************************************************************
bool SoundObjectDSound::IsPlaying( void ) const
{
	if( !m_pBuffer ) return false;

	dword Status = 0;
	m_pBuffer->GetStatus( &Status );

	return (Status & DSBSTATUS_PLAYING);
}


////////////////////////////////////////////////////////////////////////////////
//
//	ストリーミングサウンドオブジェクト(DSound)
//
////////////////////////////////////////////////////////////////////////////////
SoundObjectStreamDSound::SoundObjectStreamDSound( const SoundFile* pFile, bool bLoop )
: SoundObjectDSound( pFile, bLoop )
{
	m_pLoader	= NULL;
	m_bLoadTop	= true;
}
SoundObjectStreamDSound::~SoundObjectStreamDSound()
{
	SafeDelete( m_pLoader );
}

//******************************************************************************
//	初期化
//******************************************************************************
bool SoundObjectStreamDSound::Init( IDirectSound8* pDSound )
{
	HRESULT hr;

	//	ローダーの作成
	m_pLoader = new OggLoader;
	if( !m_pLoader ) return false;
	if( !m_pLoader->Load( m_pFile ) ) return false;

	//	DESCを作成
	DSBUFFERDESC Desc;
	ZeroStruct( Desc );
	Desc.dwSize			= sizeof(DSBUFFERDESC);
	Desc.dwFlags		= DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
	Desc.dwBufferBytes	= _GetBufSize();
	Desc.lpwfxFormat	= (WAVEFORMATEX*)_GetFormat();

	//	バッファを作成
	LPDIRECTSOUNDBUFFER pDsb = NULL;
	hr = pDSound->CreateSoundBuffer( &Desc, &pDsb, NULL );
	if( FAILED(hr) ) return false;

	hr = pDsb->QueryInterface( IID_IDirectSoundBuffer8, (LPVOID*)&m_pBuffer );
	pDsb->Release();
	if( FAILED(hr) ) return false;

	return true;
}

//******************************************************************************
//	更新
//******************************************************************************
void SoundObjectStreamDSound::Update( void )
{
	if( !IsPlaying() ) return;

	//	バッファ更新
	_UpdateBuffer();
}

//******************************************************************************
//	リセット
//******************************************************************************
bool SoundObjectStreamDSound::Reset( void )
{
	if( !m_pBuffer || !m_pLoader ) return false;

	//	最初へ戻す
	m_pLoader->Reset();
	m_pBuffer->SetCurrentPosition( 0 );

	//	バッファ初期化
	if( !_InitBuffer() ) return false;

	return true;
}

//------------------------------------------------------------------------------
//	フォーマットの取得
//------------------------------------------------------------------------------
const WAVEFORMATEX*	SoundObjectStreamDSound::_GetFormat( void ) const
{
	return m_pLoader->GetFormat();
}

//------------------------------------------------------------------------------
//	バッファの内容を初期化
//------------------------------------------------------------------------------
bool SoundObjectStreamDSound::_InitBuffer( void )
{
	HRESULT hr;

	//	初期バッファ取得
	OggLoader::BUFFER Buf( _GetBufSize() );
	m_pLoader->Read( Buf, m_bLoop );

	//	バッファに書き込み
	void*	pData = NULL;
	dword	Size = 0;
	hr = m_pBuffer->Lock( 0, 0, &pData, &Size, NULL, NULL, DSBLOCK_ENTIREBUFFER );
	if( FAILED(hr) ) return false;

	CopyMemory( pData, Buf.Mem.GetData(), Buf.Size );

	//	ロック解除
	m_pBuffer->Unlock( pData, Size, NULL, 0 );

	m_bLoadTop = true;

	return true;
}

//------------------------------------------------------------------------------
//	バッファの更新
//------------------------------------------------------------------------------
void SoundObjectStreamDSound::_UpdateBuffer( void )
{
	HRESULT hr;

	uint LoadSize = _GetBufSize() / 2;
	
	//	書き込み位置取得
	dword PlayPos = 0, WritePos = 0;
	m_pBuffer->GetCurrentPosition( &PlayPos, &WritePos );

	dword LockPos = 0;
	if( m_bLoadTop && PlayPos >= LoadSize && WritePos >= LoadSize )
	{
		//	前半部分へ書き込む
		LockPos = 0;
	}
	else if( !m_bLoadTop && PlayPos < LoadSize && WritePos < LoadSize )
	{
		//	後半部分へ書き込む
		LockPos = LoadSize;
	}
	else
	{
		//	まだ書き込むタイミングではない
		return;
	}
	m_bLoadTop = !m_bLoadTop;

	//	読み込み
	OggLoader::BUFFER Buf( LoadSize );
	m_pLoader->Read( Buf, m_bLoop );

	//	バッファに書き込み
	void*	pData1 = NULL;
	void*	pData2 = NULL;
	dword	Size1 = 0, Size2 = 0;
	hr = m_pBuffer->Lock( LockPos, Buf.Size, &pData1, &Size1, &pData2, &Size2, 0 );
	if( FAILED(hr) ) return;

	CopyMemory( pData1, Buf.Mem.GetData(), Size1 );
	if( pData2 )
	{
		CopyMemory( pData2, Buf.Mem.GetData() + Size1, Size2 );
	}

	//	ロック解除
	m_pBuffer->Unlock( pData1, Size1, pData2, Size2 );
}

//------------------------------------------------------------------------------
//	バッファサイズ取得
//------------------------------------------------------------------------------
uint SoundObjectStreamDSound::_GetBufSize( void )
{
	return _GetFormat()->nAvgBytesPerSec * BUF_LENGTH;
}

};