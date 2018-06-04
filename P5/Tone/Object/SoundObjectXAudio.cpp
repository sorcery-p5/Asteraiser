#include "P5.h"
#include "SoundObjectXAudio.h"
#include "../OggLoader.h"


namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	サウンドオブジェクト(XAudio)
//
////////////////////////////////////////////////////////////////////////////////
SoundObjectXAudio::SoundObjectXAudio( const SoundFile* pFile, bool bLoop )
: SoundObject( pFile, bLoop )
{
	m_pVoice	= NULL;
	m_bPlaying	= false;
}

SoundObjectXAudio::~SoundObjectXAudio()
{
	if( m_pVoice ) m_pVoice->DestroyVoice();
}

//******************************************************************************
//	初期化
//******************************************************************************
bool SoundObjectXAudio::Init( IXAudio2* pXAudio )
{
	HRESULT hr;

	//	ソースボイスの作成
	hr = pXAudio->CreateSourceVoice( &m_pVoice, m_pFile->GetFormat(), 0, XAUDIO2_DEFAULT_FREQ_RATIO, this );
	if( FAILED(hr) ) return false;
	
	return true;
}

//******************************************************************************
//	再生
//******************************************************************************
bool SoundObjectXAudio::Start( void )
{
	if( !m_pVoice ) return false;

	HRESULT hr = m_pVoice->Start( 0 );
	if( FAILED(hr) ) return false;

	m_bPlaying = true;

	return true;
}

//******************************************************************************
//	再生停止
//******************************************************************************
bool SoundObjectXAudio::Stop( void )
{
	if( !m_pVoice ) return false;

	HRESULT hr = m_pVoice->Stop( XAUDIO2_PLAY_TAILS );
	if( FAILED(hr) ) return false;

	m_bPlaying = false;

	return true;
}

//******************************************************************************
//	リセット
//******************************************************************************
bool SoundObjectXAudio::Reset( void )
{
	HRESULT hr;

	if( !m_pVoice ) return false;

	//	一旦フラッシュ
//	m_pVoice->FlushSourceBuffers();

	//	バッファの登録
	XAUDIO2_BUFFER Buffer;
	ZeroStruct( Buffer );

	Buffer.AudioBytes	= m_pFile->GetSize();
	Buffer.pAudioData	= m_pFile->GetData();
	Buffer.pContext		= this;
	Buffer.Flags		= XAUDIO2_END_OF_STREAM;
	Buffer.LoopCount	= m_bLoop? XAUDIO2_LOOP_INFINITE : 0;

	hr = m_pVoice->SubmitSourceBuffer( &Buffer );
	if( FAILED(hr) ) return false;

	return true;
}

//******************************************************************************
//	ボリューム設定
//******************************************************************************
void SoundObjectXAudio::SetVolume( float Vol )
{
	SafePtr( m_pVoice )->SetVolume( Vol );
}

//******************************************************************************
//	パン設定
//******************************************************************************
void SoundObjectXAudio::SetPan( float Pan )
{
	if( _GetFormat()->nChannels <= 1 ) return;

	float aVol[] = { Clamp( 1.0f-Pan, 0.0f, 1.0f ), Clamp( 1.0f+Pan, 0.0f, 1.0f ) };

	SafePtr( m_pVoice )->SetChannelVolumes( 2, aVol );
}

//------------------------------------------------------------------------------
//	コールバック：最後に到達
//------------------------------------------------------------------------------
void SoundObjectXAudio::OnStreamEnd( void )
{
	if( !m_bLoop )
	{
		//	停止状態に
		m_bPlaying = false;
	}
}


////////////////////////////////////////////////////////////////////////////////
//
//	ストリーミングサウンドオブジェクト(XAudio)
//
////////////////////////////////////////////////////////////////////////////////
SoundObjectStreamXAudio::SoundObjectStreamXAudio( const SoundFile* pFile, bool bLoop )
: SoundObjectXAudio( pFile, bLoop )
{
	m_pLoader = NULL;
	m_BufferPos = 0;
	m_bBufferEnd = false;
}
SoundObjectStreamXAudio::~SoundObjectStreamXAudio()
{
	SafeDelete( m_pLoader );
}

//******************************************************************************
//	初期化
//******************************************************************************
bool SoundObjectStreamXAudio::Init( IXAudio2* pXAudio )
{
	HRESULT hr;

	//	ローダーの作成
	m_pLoader = new OggLoader;
	if( !m_pLoader ) return false;
	if( !m_pLoader->Load( m_pFile ) ) return false;

	//	ソースボイスの作成
	hr = pXAudio->CreateSourceVoice( &m_pVoice, _GetFormat(), 0, XAUDIO2_DEFAULT_FREQ_RATIO, this );
	if( FAILED(hr) ) return false;

	//	領域確保
	for( int i = 0; i < BUF_NUM; i++ )
	{
		m_aBuffer[i].Mem.Alloc( BUF_SIZE );
	}
	m_BufferPos		= 0;
	m_bBufferEnd	= false;

	return true;
}

//******************************************************************************
//	リセット
//******************************************************************************
bool SoundObjectStreamXAudio::Reset( void )
{
	HRESULT hr;

	if( !m_pVoice ) return false;

	//	一旦フラッシュ
//	m_pVoice->FlushSourceBuffers();
	m_pLoader->Reset();

	m_BufferPos = 0;
	m_bBufferEnd = false;
	for( int i = 0; i < BUF_NUM; i++ )
	{
		//	バッファ取得
		OggLoader::BUFFER& Buf = m_aBuffer[i];
		m_pLoader->Read( Buf, m_bLoop );

		//	バッファの登録
		XAUDIO2_BUFFER Buffer;
		ZeroStruct( Buffer );

		Buffer.AudioBytes	= Buf.Size;
		Buffer.pAudioData	= Buf.Mem.GetData();
		Buffer.Flags		= Buf.bEnd? XAUDIO2_END_OF_STREAM : 0;

		hr = m_pVoice->SubmitSourceBuffer( &Buffer );
		if( FAILED(hr) ) return false;

		m_BufferPos = (m_BufferPos + 1) % BUF_NUM;

		if( Buf.bEnd )
		{
			m_bBufferEnd = true;
			break;
		}
	}

	return true;
}

//------------------------------------------------------------------------------
//	コールバック：バッファ最後に到達
//------------------------------------------------------------------------------
void SoundObjectStreamXAudio::OnBufferEnd( void* pContext )
{
	if( m_bBufferEnd ) return;

	//	読み込み
	OggLoader::BUFFER& Buf = m_aBuffer[m_BufferPos];
	m_pLoader->Read( Buf, m_bLoop );

	//	バッファを追加登録
	XAUDIO2_BUFFER Buffer;
	ZeroStruct( Buffer );

	Buffer.AudioBytes	= Buf.Size;
	Buffer.pAudioData	= Buf.Mem.GetData();
	Buffer.Flags		= Buf.bEnd? XAUDIO2_END_OF_STREAM : 0;

	m_pVoice->SubmitSourceBuffer( &Buffer );
	
	m_BufferPos = (m_BufferPos + 1) % BUF_NUM;

	if( Buf.bEnd ) m_bBufferEnd = true;
}

//------------------------------------------------------------------------------
//	フォーマットの取得
//------------------------------------------------------------------------------
const WAVEFORMATEX* SoundObjectStreamXAudio::_GetFormat( void ) const
{
	return m_pLoader->GetFormat();
}

};