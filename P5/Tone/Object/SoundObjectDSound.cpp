#include "P5.h"
#include "SoundObjectDSound.h"

#include "../OggLoader.h"
#include "../File/SoundFile.h"


namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	�T�E���h�I�u�W�F�N�g(DSound)
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
//	������
//******************************************************************************
bool SoundObjectDSound::Init( IDirectSound8* pDSound )
{
	HRESULT hr;

	//	DESC���쐬
	DSBUFFERDESC Desc;
	ZeroStruct( Desc );
	Desc.dwSize			= sizeof(DSBUFFERDESC);
	Desc.dwFlags		= DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
	Desc.dwBufferBytes	= m_pFile->GetSize();
	Desc.lpwfxFormat	= (WAVEFORMATEX*)_GetFormat();

	//	�o�b�t�@���쐬
	LPDIRECTSOUNDBUFFER pDsb = NULL;
	hr = pDSound->CreateSoundBuffer( &Desc, &pDsb, NULL );
	if( FAILED(hr) ) return false;

	hr = pDsb->QueryInterface( IID_IDirectSoundBuffer8, (LPVOID*)&m_pBuffer );
	pDsb->Release();
	if( FAILED(hr) ) return false;

	//	�o�b�t�@�ɏ�������
	void*	pData;
	dword	Size;
	hr = m_pBuffer->Lock( 0, 0, &pData, &Size, NULL, NULL, DSBLOCK_ENTIREBUFFER );
	if( FAILED(hr) ) return false;

	//	�f�[�^��������
	CopyMemory( pData, m_pFile->GetData(), Size );

	//	���b�N����
	m_pBuffer->Unlock( pData, Size, NULL, 0);

	return true;
}

//******************************************************************************
//	�Đ�
//******************************************************************************
bool SoundObjectDSound::Start( void )
{
	if( !m_pBuffer ) return false;

	HRESULT hr = m_pBuffer->Play( 0, 0, (m_bLoop || m_pFile->IsStream())? DSBPLAY_LOOPING : 0 );
	if( FAILED(hr) ) return false;

	return true;
}

//******************************************************************************
//	�Đ���~
//******************************************************************************
bool SoundObjectDSound::Stop( void )
{
	if( !m_pBuffer ) return false;

	HRESULT hr = m_pBuffer->Stop();
	if( FAILED(hr) ) return false;

	return true;
}

//******************************************************************************
//	���Z�b�g
//******************************************************************************
bool SoundObjectDSound::Reset( void )
{
	if( !m_pBuffer ) return false;

	m_pBuffer->SetCurrentPosition( 0 );

	return true;
}

//******************************************************************************
//	�{�����[���ݒ�
//******************************************************************************
void SoundObjectDSound::SetVolume( float Vol )
{
	LONG Val = (LONG)( Vol == 0.0f? DSBVOLUME_MIN : log10( Vol ) * 1000 );
	SafePtr( m_pBuffer )->SetVolume( Val );
}

//******************************************************************************
//	�p���ݒ�
//******************************************************************************
void SoundObjectDSound::SetPan( float Pan )
{
	SafePtr( m_pBuffer )->SetPan( Lerp( DSBPAN_LEFT, DSBPAN_RIGHT, Coef( Pan, -1.0f, 1.0f ) ) );
}

//******************************************************************************
//	�Đ������ǂ���
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
//	�X�g���[�~���O�T�E���h�I�u�W�F�N�g(DSound)
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
//	������
//******************************************************************************
bool SoundObjectStreamDSound::Init( IDirectSound8* pDSound )
{
	HRESULT hr;

	//	���[�_�[�̍쐬
	m_pLoader = new OggLoader;
	if( !m_pLoader ) return false;
	if( !m_pLoader->Load( m_pFile ) ) return false;

	//	DESC���쐬
	DSBUFFERDESC Desc;
	ZeroStruct( Desc );
	Desc.dwSize			= sizeof(DSBUFFERDESC);
	Desc.dwFlags		= DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
	Desc.dwBufferBytes	= _GetBufSize();
	Desc.lpwfxFormat	= (WAVEFORMATEX*)_GetFormat();

	//	�o�b�t�@���쐬
	LPDIRECTSOUNDBUFFER pDsb = NULL;
	hr = pDSound->CreateSoundBuffer( &Desc, &pDsb, NULL );
	if( FAILED(hr) ) return false;

	hr = pDsb->QueryInterface( IID_IDirectSoundBuffer8, (LPVOID*)&m_pBuffer );
	pDsb->Release();
	if( FAILED(hr) ) return false;

	return true;
}

//******************************************************************************
//	�X�V
//******************************************************************************
void SoundObjectStreamDSound::Update( void )
{
	if( !IsPlaying() ) return;

	//	�o�b�t�@�X�V
	_UpdateBuffer();
}

//******************************************************************************
//	���Z�b�g
//******************************************************************************
bool SoundObjectStreamDSound::Reset( void )
{
	if( !m_pBuffer || !m_pLoader ) return false;

	//	�ŏ��֖߂�
	m_pLoader->Reset();
	m_pBuffer->SetCurrentPosition( 0 );

	//	�o�b�t�@������
	if( !_InitBuffer() ) return false;

	return true;
}

//------------------------------------------------------------------------------
//	�t�H�[�}�b�g�̎擾
//------------------------------------------------------------------------------
const WAVEFORMATEX*	SoundObjectStreamDSound::_GetFormat( void ) const
{
	return m_pLoader->GetFormat();
}

//------------------------------------------------------------------------------
//	�o�b�t�@�̓��e��������
//------------------------------------------------------------------------------
bool SoundObjectStreamDSound::_InitBuffer( void )
{
	HRESULT hr;

	//	�����o�b�t�@�擾
	OggLoader::BUFFER Buf( _GetBufSize() );
	m_pLoader->Read( Buf, m_bLoop );

	//	�o�b�t�@�ɏ�������
	void*	pData = NULL;
	dword	Size = 0;
	hr = m_pBuffer->Lock( 0, 0, &pData, &Size, NULL, NULL, DSBLOCK_ENTIREBUFFER );
	if( FAILED(hr) ) return false;

	CopyMemory( pData, Buf.Mem.GetData(), Buf.Size );

	//	���b�N����
	m_pBuffer->Unlock( pData, Size, NULL, 0 );

	m_bLoadTop = true;

	return true;
}

//------------------------------------------------------------------------------
//	�o�b�t�@�̍X�V
//------------------------------------------------------------------------------
void SoundObjectStreamDSound::_UpdateBuffer( void )
{
	HRESULT hr;

	uint LoadSize = _GetBufSize() / 2;
	
	//	�������݈ʒu�擾
	dword PlayPos = 0, WritePos = 0;
	m_pBuffer->GetCurrentPosition( &PlayPos, &WritePos );

	dword LockPos = 0;
	if( m_bLoadTop && PlayPos >= LoadSize && WritePos >= LoadSize )
	{
		//	�O�������֏�������
		LockPos = 0;
	}
	else if( !m_bLoadTop && PlayPos < LoadSize && WritePos < LoadSize )
	{
		//	�㔼�����֏�������
		LockPos = LoadSize;
	}
	else
	{
		//	�܂��������ރ^�C�~���O�ł͂Ȃ�
		return;
	}
	m_bLoadTop = !m_bLoadTop;

	//	�ǂݍ���
	OggLoader::BUFFER Buf( LoadSize );
	m_pLoader->Read( Buf, m_bLoop );

	//	�o�b�t�@�ɏ�������
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

	//	���b�N����
	m_pBuffer->Unlock( pData1, Size1, pData2, Size2 );
}

//------------------------------------------------------------------------------
//	�o�b�t�@�T�C�Y�擾
//------------------------------------------------------------------------------
uint SoundObjectStreamDSound::_GetBufSize( void )
{
	return _GetFormat()->nAvgBytesPerSec * BUF_LENGTH;
}

};