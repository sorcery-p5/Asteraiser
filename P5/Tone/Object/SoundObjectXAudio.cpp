#include "P5.h"
#include "SoundObjectXAudio.h"
#include "../OggLoader.h"


namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	�T�E���h�I�u�W�F�N�g(XAudio)
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
//	������
//******************************************************************************
bool SoundObjectXAudio::Init( IXAudio2* pXAudio )
{
	HRESULT hr;

	//	�\�[�X�{�C�X�̍쐬
	hr = pXAudio->CreateSourceVoice( &m_pVoice, m_pFile->GetFormat(), 0, XAUDIO2_DEFAULT_FREQ_RATIO, this );
	if( FAILED(hr) ) return false;
	
	return true;
}

//******************************************************************************
//	�Đ�
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
//	�Đ���~
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
//	���Z�b�g
//******************************************************************************
bool SoundObjectXAudio::Reset( void )
{
	HRESULT hr;

	if( !m_pVoice ) return false;

	//	��U�t���b�V��
//	m_pVoice->FlushSourceBuffers();

	//	�o�b�t�@�̓o�^
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
//	�{�����[���ݒ�
//******************************************************************************
void SoundObjectXAudio::SetVolume( float Vol )
{
	SafePtr( m_pVoice )->SetVolume( Vol );
}

//******************************************************************************
//	�p���ݒ�
//******************************************************************************
void SoundObjectXAudio::SetPan( float Pan )
{
	if( _GetFormat()->nChannels <= 1 ) return;

	float aVol[] = { Clamp( 1.0f-Pan, 0.0f, 1.0f ), Clamp( 1.0f+Pan, 0.0f, 1.0f ) };

	SafePtr( m_pVoice )->SetChannelVolumes( 2, aVol );
}

//------------------------------------------------------------------------------
//	�R�[���o�b�N�F�Ō�ɓ��B
//------------------------------------------------------------------------------
void SoundObjectXAudio::OnStreamEnd( void )
{
	if( !m_bLoop )
	{
		//	��~��Ԃ�
		m_bPlaying = false;
	}
}


////////////////////////////////////////////////////////////////////////////////
//
//	�X�g���[�~���O�T�E���h�I�u�W�F�N�g(XAudio)
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
//	������
//******************************************************************************
bool SoundObjectStreamXAudio::Init( IXAudio2* pXAudio )
{
	HRESULT hr;

	//	���[�_�[�̍쐬
	m_pLoader = new OggLoader;
	if( !m_pLoader ) return false;
	if( !m_pLoader->Load( m_pFile ) ) return false;

	//	�\�[�X�{�C�X�̍쐬
	hr = pXAudio->CreateSourceVoice( &m_pVoice, _GetFormat(), 0, XAUDIO2_DEFAULT_FREQ_RATIO, this );
	if( FAILED(hr) ) return false;

	//	�̈�m��
	for( int i = 0; i < BUF_NUM; i++ )
	{
		m_aBuffer[i].Mem.Alloc( BUF_SIZE );
	}
	m_BufferPos		= 0;
	m_bBufferEnd	= false;

	return true;
}

//******************************************************************************
//	���Z�b�g
//******************************************************************************
bool SoundObjectStreamXAudio::Reset( void )
{
	HRESULT hr;

	if( !m_pVoice ) return false;

	//	��U�t���b�V��
//	m_pVoice->FlushSourceBuffers();
	m_pLoader->Reset();

	m_BufferPos = 0;
	m_bBufferEnd = false;
	for( int i = 0; i < BUF_NUM; i++ )
	{
		//	�o�b�t�@�擾
		OggLoader::BUFFER& Buf = m_aBuffer[i];
		m_pLoader->Read( Buf, m_bLoop );

		//	�o�b�t�@�̓o�^
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
//	�R�[���o�b�N�F�o�b�t�@�Ō�ɓ��B
//------------------------------------------------------------------------------
void SoundObjectStreamXAudio::OnBufferEnd( void* pContext )
{
	if( m_bBufferEnd ) return;

	//	�ǂݍ���
	OggLoader::BUFFER& Buf = m_aBuffer[m_BufferPos];
	m_pLoader->Read( Buf, m_bLoop );

	//	�o�b�t�@��ǉ��o�^
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
//	�t�H�[�}�b�g�̎擾
//------------------------------------------------------------------------------
const WAVEFORMATEX* SoundObjectStreamXAudio::_GetFormat( void ) const
{
	return m_pLoader->GetFormat();
}

};