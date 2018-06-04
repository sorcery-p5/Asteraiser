#pragma once

#include "EffectTypes.h"

class Effect;
class EffectManager;
class EffectFile;
class EffectParticle;

class SpriteFrame;


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�`�敨(���)
//
////////////////////////////////////////////////////////////////////////////////
class EffectDrawObj
{
public:
	EffectDrawObj( const EFFECT_PARTICLE* pParam, const EffectParticle* pParent )	{ m_pParam = pParam; m_pParent = pParent; m_Color.SetWhite(); }
	virtual ~EffectDrawObj()														{}

	virtual void					Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Mat )	= 0;
	virtual void					Update( EffectManager* pManager, const Matrix3& Mat )							= 0;
	virtual void					Draw( const Matrix3& OffsetMat )												= 0;

	virtual void					Delete( void )									{}
	virtual void					EndAppear( void )								{}
	virtual void					SetVisible( bool bVisible )						{}
	virtual void					Pause( bool bPause )							{}
	virtual void					SetColor( Color Col )							{}
	virtual void					SetIndex( int ID, int Index )					{}
	virtual void					SetVolume( float Vol )							{}

protected:
	bool							_IsVisible( void )						const;

protected:
	const EFFECT_PARTICLE*			m_pParam;
	const EffectParticle*			m_pParent;

private:
	Color							m_Color;
};


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�`�敨�i�X�v���C�g�j
//
////////////////////////////////////////////////////////////////////////////////
class EffectDrawObjSprite : public EffectDrawObj
{
public:
	EffectDrawObjSprite( const EFFECT_PARTICLE* pParam, const EffectParticle* pParent );
	virtual ~EffectDrawObjSprite();

	virtual void					Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Mat );
	virtual void					Update( EffectManager* pManager, const Matrix3& Mat );
	virtual void					Draw( const Matrix3& OffsetMat );
	
	virtual void					SetColor( Color Col );
	virtual void					SetIndex( int ID, int Index );
	
private:
	Matrix3							m_Matrix;
	RectSprite*						m_pSprite;
};


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�`�敨�i�X�v���C�g�t���[���j
//
////////////////////////////////////////////////////////////////////////////////
class EffectDrawObjSpriteFrame : public EffectDrawObj
{
public:
	EffectDrawObjSpriteFrame( const EFFECT_PARTICLE* pParam, const EffectParticle* pParent );
	virtual ~EffectDrawObjSpriteFrame();

	virtual void					Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Mat );
	virtual void					Update( EffectManager* pManager, const Matrix3& Mat );
	virtual void					Draw( const Matrix3& OffsetMat );
	
	virtual void					SetVisible( bool bVisible );
	virtual void					Pause( bool bPause );
	virtual void					SetColor( Color Col );

private:
	SpriteFrame*					m_pSpriteFrame;
};


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�`�敨�i���C���j
//
////////////////////////////////////////////////////////////////////////////////
class EffectDrawObjLine : public EffectDrawObj
{
public:
	EffectDrawObjLine( const EFFECT_PARTICLE* pParam, const EffectParticle* pParent );
	virtual ~EffectDrawObjLine();

	virtual void					Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Mat );
	virtual void					Update( EffectManager* pManager, const Matrix3& Mat );
	virtual void					Draw( const Matrix3& OffsetMat );
	
	virtual void					SetColor( Color Col );
	virtual void					SetIndex( int ID, int Index );

private:
	typedef std::vector<Point>		VecPoint;

private:
	LineSprite*						m_pSprite;
	VecPoint						m_vPointA;
	VecPoint						m_vPointB;
	int								m_Count;
};


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�`�敨�i�t�H���g�j
//
////////////////////////////////////////////////////////////////////////////////
class EffectDrawObjFont : public EffectDrawObj
{
public:
	EffectDrawObjFont( const EFFECT_PARTICLE* pParam, const EffectParticle* pParent );
	virtual ~EffectDrawObjFont();

	virtual void					Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Mat );
	virtual void					Update( EffectManager* pManager, const Matrix3& Mat );
	virtual void					Draw( const Matrix3& OffsetMat );
	
	virtual void					SetColor( Color Col );
	void							SetText( pstr pText );

private:
	FontSprite*						m_pSprite;
};

////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�`�敨�i�T�E���h�j
//
////////////////////////////////////////////////////////////////////////////////
class EffectDrawObjSound : public EffectDrawObj
{
public:
	EffectDrawObjSound( const EFFECT_PARTICLE* pParam, const EffectParticle* pParent );
	virtual ~EffectDrawObjSound();

	virtual void					Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Mat );
	virtual void					Update( EffectManager* pManager, const Matrix3& Mat );
	virtual void					Draw( const Matrix3& OffsetMat );
	
	virtual void					SetVisible( bool bVisible );
	virtual void					Pause( bool bPause );
	virtual void					SetVolume( float Vol );

private:
	RefPtr<Sound>					m_pSound;
};

////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�`�敨�i�G�t�F�N�g�j
//
////////////////////////////////////////////////////////////////////////////////
class EffectDrawObjEffect : public EffectDrawObj
{
public:
	EffectDrawObjEffect( const EFFECT_PARTICLE* pParam, const EffectParticle* pParent );
	virtual ~EffectDrawObjEffect();

	virtual void					Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Mat );
	virtual void					Update( EffectManager* pManager, const Matrix3& Mat );
	virtual void					Draw( const Matrix3& OffsetMat );
	
	virtual void					Delete( void );
	virtual void					EndAppear( void );
	virtual void					SetVisible( bool bVisible );
	virtual void					Pause( bool bPause );
	virtual void					SetColor( Color Col );

private:
	RefPtr<Effect>					m_pEffect;
};