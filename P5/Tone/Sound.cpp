#include "P5.h"
#include "Sound.h"

#include "Object/SoundObject.h"

namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	�T�E���h�I�u�W�F�N�g
//
////////////////////////////////////////////////////////////////////////////////
Sound::Sound( void )
{
	m_pObject	= NULL;

	m_State		= STATE_NONE;
	m_Channel	= SOUND_CHANNEL_SE;
	m_bMute		= false;
	m_Volume	= 1.0f;
	m_Pan		= 0.0f;
}

Sound::~Sound()
{
	SafeDelete( m_pObject );
}

//******************************************************************************
//	������
//******************************************************************************
void Sound::Init( SoundObject* pObject, SOUND_CHANNEL Ch )
{
	_ASSERT( pObject );

	SafeDelete( m_pObject );

	m_pObject	= pObject;
	m_State		= STATE_STOP;
	m_Channel	= Ch;
}

//******************************************************************************
//	�X�V
//******************************************************************************
void Sound::Update( void )
{
	//	�Đ��I���`�F�b�N
	if( m_pObject )
	{
		m_pObject->Update();

		if( m_State == STATE_PLAY )
		{
			if( !m_pObject->IsPlaying() )
			{
				Stop();
			}
		}
	}
}

//******************************************************************************
//	�Đ�(�ŏ�����)
//******************************************************************************
bool Sound::Play( void )
{
	if( !m_pObject ) return false;

	//	��U�~�߂�
	if( m_State != STATE_STOP ) Stop();

	//	���Z�b�g
	if( !m_pObject->Reset() ) return false;

	//	�X�^�[�g
	if( !m_pObject->Start() ) return false;

	//	���ʂ̐ݒ�
	m_pObject->SetVolume( GetOutVolume() );

	m_State = STATE_PLAY;

	return true;
}

//******************************************************************************
//	�Đ���~
//******************************************************************************
bool Sound::Stop( void )
{
	if( !m_pObject ) return false;

	//	��~
	if( !m_pObject->Stop() ) return false;

	m_State = STATE_STOP;

	return true;
}

//******************************************************************************
//	�ꎞ��~
//******************************************************************************
bool Sound::Pause( void )
{
	if( !m_pObject ) return false;

	//	�Đ����̂݉\
	if( m_State != STATE_PLAY ) return false;

	//	�J�n
	if( !m_pObject->Stop() ) return false;

	m_State = STATE_PAUSE;

	return true;
}

//******************************************************************************
//	�ꎞ��~�̉���
//******************************************************************************
bool Sound::Resume( void )
{
	if( !m_pObject ) return false;

	//	�ꎞ��~���̂݉\
	if( m_State != STATE_PAUSE ) return false;

	//	�X�^�[�g
	if( !m_pObject->Start() ) return false;

	m_State = STATE_PLAY;

	return true;
}

//******************************************************************************
//	�ꎞ��~�̐ݒ�
//******************************************************************************
bool Sound::SetPause( bool bPause )
{
	return bPause? Pause() : Resume();
}

//******************************************************************************
//	�~���[�g�ݒ�
//******************************************************************************
void Sound::SetMute( bool bMute )
{
	if( !m_pObject ) return;

	if( bMute != m_bMute )
	{
		m_pObject->SetVolume( GetOutVolume() );
		m_bMute = bMute;
	}
}

//******************************************************************************
//	�{�����[���ݒ�
//******************************************************************************
void Sound::SetVolume( float Vol )
{
	if( !m_pObject ) return;

	m_Volume = Clamp( Vol, 0.0f, 1.0f );

	if( !m_bMute )
	{
		m_pObject->SetVolume( GetOutVolume() );
	}
}

//******************************************************************************
//	�p���ݒ�
//******************************************************************************
void Sound::SetPan( float Pan )
{
	if( !m_pObject ) return;

	Pan = Clamp( Pan, -1.0f, 1.0f );
	m_pObject->SetPan( Pan );

	m_Pan = Pan;
}

//******************************************************************************
//	�{�����[���̍X�V
//******************************************************************************
void Sound::UpdateVolume( void )
{
	if( !m_pObject ) return;

	if( !m_bMute )
	{
		m_pObject->SetVolume( GetOutVolume() );
	}
}

//******************************************************************************
//	���[�v���ǂ���
//******************************************************************************
bool Sound::IsLoop( void ) const
{
	if( !m_pObject ) return false;

	return m_pObject->IsLoop();
}

//******************************************************************************
//	�폜��Ԃ�
//******************************************************************************
bool Sound::IsDelete( void ) const
{
	return !IS_REFERENCE() && ( IsStop() || IsLoop() );
}

//******************************************************************************
//	�o�̓{�����[���𓾂�
//******************************************************************************
float Sound::GetOutVolume( void ) const
{
	return m_bMute? 0.0f : m_Volume * GetTone()->GetChannelVolume( m_Channel );
}

};