#pragma once

#include "Scene.h"

class SpriteFrameFile;
class SpriteFrame;
class FontResource;
class EffectManager;
class EffectResource;
class Effect;
class SoundResource;


///////////////////////////////////////////////////////////////////////////////
//
//	テスト用シーン
//
///////////////////////////////////////////////////////////////////////////////
class TestScene : public Scene
{
public:
	TestScene( Label Name );
	virtual ~TestScene();

	virtual void			OnInit( void );
	virtual void			Update( void );

private:
	SpriteFrameFile*		m_pSpriteFrameFile;
	SpriteFrame*			m_pSpriteFrame;

	FontResource*			m_pFontRes;
	FontSprite*				m_pFontSprite;

	EffectResource*			m_pEffectRes;
	EffectResource*			m_pEffectRes2;
	RefPtr<Effect>			m_pEffect;

	SoundResource*			m_pSERes;
	SoundResource*			m_pBGMRes;
	RefPtr<Sound>			m_pSE;
	RefPtr<Sound>			m_pBGM;

	String256				m_SpritePath;
	String256				m_EffectPath;
	String256				m_EffectPath2;

	XORRand					m_Rand;
	Matrix3					m_BaseMat;
	Angle					m_WheelAng;
	int						m_IntpTime;
	Color					m_BackColor;
};