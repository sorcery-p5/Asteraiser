#include "P5.h"
#include "Sound.h"

#include "Object/SoundObject.h"

namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	サウンドオブジェクト
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
//	初期化
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
//	更新
//******************************************************************************
void Sound::Update( void )
{
	//	再生終了チェック
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
//	再生(最初から)
//******************************************************************************
bool Sound::Play( void )
{
	if( !m_pObject ) return false;

	//	一旦止める
	if( m_State != STATE_STOP ) Stop();

	//	リセット
	if( !m_pObject->Reset() ) return false;

	//	スタート
	if( !m_pObject->Start() ) return false;

	//	音量の設定
	m_pObject->SetVolume( GetOutVolume() );

	m_State = STATE_PLAY;

	return true;
}

//******************************************************************************
//	再生停止
//******************************************************************************
bool Sound::Stop( void )
{
	if( !m_pObject ) return false;

	//	停止
	if( !m_pObject->Stop() ) return false;

	m_State = STATE_STOP;

	return true;
}

//******************************************************************************
//	一時停止
//******************************************************************************
bool Sound::Pause( void )
{
	if( !m_pObject ) return false;

	//	再生中のみ可能
	if( m_State != STATE_PLAY ) return false;

	//	開始
	if( !m_pObject->Stop() ) return false;

	m_State = STATE_PAUSE;

	return true;
}

//******************************************************************************
//	一時停止の解除
//******************************************************************************
bool Sound::Resume( void )
{
	if( !m_pObject ) return false;

	//	一時停止中のみ可能
	if( m_State != STATE_PAUSE ) return false;

	//	スタート
	if( !m_pObject->Start() ) return false;

	m_State = STATE_PLAY;

	return true;
}

//******************************************************************************
//	一時停止の設定
//******************************************************************************
bool Sound::SetPause( bool bPause )
{
	return bPause? Pause() : Resume();
}

//******************************************************************************
//	ミュート設定
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
//	ボリューム設定
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
//	パン設定
//******************************************************************************
void Sound::SetPan( float Pan )
{
	if( !m_pObject ) return;

	Pan = Clamp( Pan, -1.0f, 1.0f );
	m_pObject->SetPan( Pan );

	m_Pan = Pan;
}

//******************************************************************************
//	ボリュームの更新
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
//	ループかどうか
//******************************************************************************
bool Sound::IsLoop( void ) const
{
	if( !m_pObject ) return false;

	return m_pObject->IsLoop();
}

//******************************************************************************
//	削除状態か
//******************************************************************************
bool Sound::IsDelete( void ) const
{
	return !IS_REFERENCE() && ( IsStop() || IsLoop() );
}

//******************************************************************************
//	出力ボリュームを得る
//******************************************************************************
float Sound::GetOutVolume( void ) const
{
	return m_bMute? 0.0f : m_Volume * GetTone()->GetChannelVolume( m_Channel );
}

};