#pragma once

#include "EffectTypes.h"
#include "TimeKey/TimeKey.h"

class Effect;
class EffectManager;
class EffectMove;
class EffectFile;
class EffectDrawObj;

class SpriteFrame;

typedef TimeKey< EFFECT_TIME_KEY_PARAM, EFFECT_TIME_KEY_TYPE, EFFECT_TIME_KEY_NUM >	EffectTimeKey;


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクトパーティクル
//
////////////////////////////////////////////////////////////////////////////////
class EffectParticle : public EffectTimeKey::Owner
{
public:
	EffectParticle( const Effect* pParent, const EFFECT_PARTICLE* pParam );
	virtual ~EffectParticle();

	void							Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Parent, const EFFECT_APPEAR* pAppear );
	void							Update( EffectManager* pManager );
	void							Draw( const Matrix3& OffsetMat );

	void							Delete( void );
	void							EndAppear( void );
	void							ApplyColor( void );
	void							SetVisible( bool bVisible ); 
	void							Pause( bool bPause );
	
	void							SetScale( Point Scale )								{ m_Scale = Scale; }
	
	bool							IsDelete( void )							const	{ return m_bDelete; }
	bool							IsEndless( void )							const	{ return m_pParam->EndFrame == 0 || m_pParam->bLoop; }
	bool							IsVisible( void )							const;
	const Matrix3&					GetMatrix( void )							const;
	Point							GetScale( void )							const	{ return m_Scale; }
	Label							GetName( void )								const	{ return m_pAppear? m_pAppear->Name : ""; }

	//	タイムキー
	virtual uint					GetTimeKeyNum( EFFECT_TIME_KEY_TYPE Type )													const;
	virtual const EFFECT_TIME_KEY*	GetTimeKey( EFFECT_TIME_KEY_TYPE Type, int Index )											const;
	virtual void					ApplyTimeKey( EFFECT_TIME_KEY_TYPE Type, const EFFECT_TIME_KEY_PARAM& Key );
	virtual void					ApplyTimeKey( EFFECT_TIME_KEY_TYPE Type, const EFFECT_TIME_KEY_PARAM& Key, const EFFECT_TIME_KEY_PARAM& Next, float f );

private:
	EffectDrawObj*					_CreateDrawObj( EFFECT_PARTICLE_TYPE Type )	const;
	EffectMove*						_CreateMove( EFFECT_MOVE_TYPE Type );

protected:
	const EFFECT_PARTICLE*			m_pParam;
	const EFFECT_APPEAR*			m_pAppear;

private:
	const Effect*					m_pParent;
	EffectDrawObj*					m_pDrawObj;
	EffectMove*						m_pMove;
	EffectTimeKey*					m_pTimeKey;

	uint							m_FrameCount;
	Color							m_Color;
	Point							m_Scale;
	bool							m_bDelete;
	bool							m_bUpdateEnable;
};