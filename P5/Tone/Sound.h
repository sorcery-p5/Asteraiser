#pragma once

namespace P5
{

class SoundObject;


////////////////////////////////////////////////////////////////////////////////
//
//	サウンドオブジェクト
//
////////////////////////////////////////////////////////////////////////////////
class Sound
{
	REFERENCE_IMPL( Sound );
public:
	Sound( void );
	virtual ~Sound();

	void						Init( SoundObject* pObject, SOUND_CHANNEL Ch );
	void						Update( void );

	bool						Play( void );
	bool						Stop( void );
	bool						Pause( void );
	bool						Resume( void );
	bool						SetPause( bool bPause );
	void						SetMute( bool bMute );
	void						SetVolume( float Vol );
	void						SetPan( float Pan );
	void						UpdateVolume( void );

	bool						IsActive( void )						const	{ return m_State != STATE_NONE && m_State != STATE_STOP; }
	bool						IsStop( void )							const	{ return m_State == STATE_STOP; }
	bool						IsPause( void )							const	{ return m_State == STATE_PAUSE; }
	bool						IsLoop( void )							const;
	bool						IsDelete( void )						const;
	float						GetOutVolume( void )					const;
	float						GetVolume( void )						const	{ return m_Volume; }
	float						GetPan( void )							const	{ return m_Pan; }
	bool						IsMute( void )							const	{ return m_bMute; }
	SOUND_CHANNEL				GetChannel( void )						const	{ return m_Channel; }

	static float				CalcPanValue( Point Pos, Point Screen )			{ return (Pos.x / Screen.x * 2.0f) - 1.0f; }

private:
	enum STATE
	{
		STATE_NONE,
		STATE_PLAY,
		STATE_STOP,
		STATE_PAUSE,
	};

private:
	SoundObject*				m_pObject;

	STATE						m_State;
	SOUND_CHANNEL				m_Channel;
	bool						m_bMute;
	float						m_Volume;
	float						m_Pan;
};

//------------------------------------------------------------------------------
//	削除用
//------------------------------------------------------------------------------
#define SafeStop( p )		{ if(p){ (p)->Stop(); (p) = NULL; } }


}