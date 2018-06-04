#pragma once

class SoundResource;


////////////////////////////////////////////////////////////////////////////////
//
//	BGMä«óù
//
////////////////////////////////////////////////////////////////////////////////
class BGMManager
{
public:
	BGMManager( void );
	~BGMManager();

	void				Update( void );

	void				Play( pstr pPath, float Volume );
	void				Fade( int Frame );
	void				FadeVolume( int Frame, float Volume );
	void				Stop( void );

	void				Pause( void );
	void				Resume( void );
	void				SetVolume( float Volume );
	void				ChangeVolume( float Volume, int Frame );

private:
	SoundResource*		m_pResource;
	RefPtr< Sound >		m_pSound;

	int					m_FadeTime;
	int					m_FadeCount;
	bool				m_bFadeStop;
	float				m_Volume;
	float				m_FadeVolume;
};