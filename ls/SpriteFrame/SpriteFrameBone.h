#pragma once

#include "SpriteFrameTypes.h"

class SpriteFrameAnime;
class SpriteFrameFile;
class SpriteFrame;
class Effect;


////////////////////////////////////////////////////////////////////////////////
//
//	スプライトフレーム：ボーン（基底）
//
////////////////////////////////////////////////////////////////////////////////
class SpriteFrameBone
{
public:
	SpriteFrameBone( void );
	virtual ~SpriteFrameBone();

	void						Init( const SPRITE_FRAME_BONE* pParam, const SpriteFrame* pFrame );
	void						Update( void );
	void						Draw( const Matrix3& OffsetMat );

	void						InitResidentAppear( void );
	void						ApplyAnime( const SpriteFrameAnime* pAnime );
	void						ResetAnime( void );
	void						Pause( bool bPause );

	void						SetLocalMatrix( const Matrix3& Mat )				{ m_LocalMatrix = Mat; }
	void						CalcWorldMatrix( const Matrix3& ParentMat );
	void						SetColor( Color Col, const SpriteFrameAnime* pAnime );
	void						ApplyColor( const SpriteFrameAnime* pAnime );
	Color						GetColor( const SpriteFrameAnime* pAnime )	const;

	void						ResetAppear( void );
	void						AddAppear( const SPRITE_FRAME_APPEAR* pParam );
	void						SetVisible( bool bVisible );
	void						UpdateVisible( void );
	
	void						SetPos( Point Scale );
	void						SetRot( float Rot );
	void						SetScale( Point Scale );
	
	Label						GetName( void )								const	{ return m_pParam->Name; }
	int							GetID( void )								const	{ return m_pParam->ID; }
	int							GetParentID( void )							const	{ return m_pParam->ParentID; }
	int							GetSiblingID( void )						const	{ return m_pParam->SiblingID; }
	int							GetChildID( void )							const	{ return m_pParam->ChildID; }

	const Matrix3&				GetLocalMatrix( void )						const	{ return m_LocalMatrix; }
	const Matrix3&				GetWorldMatrix( void )						const	{ return m_WorldMatrix; }

	virtual Rect				GetRect( void )								const	{ return Rect(); }
	virtual float				GetRadius( void )							const	{ return 0.0f; }

	virtual void				SetDrawPhase( int Phase )							{}
	virtual void				SetText( pstr pStr )								{}
	virtual void				SetAngle( float Ang )								{}
	virtual void				SetPieceIndex( int Index )							{}

private:
	struct EFFECT
	{
		RefPtr< Effect >			pEffect;
		const SPRITE_FRAME_EFFECT*	pParam;
	};

	struct SOUND
	{
		RefPtr< Sound >				pSound;
		const SPRITE_FRAME_SOUND*	pParam;
	};

	typedef std::list< EFFECT >		EffectList;
	typedef std::list< SOUND >		SoundList;

protected:
	virtual void				_OnInit( void )										= 0;
	virtual void				_OnDraw( const Matrix3& OffsetMat )					= 0;
	
	virtual void				_OnApplyAnime( const SpriteFrameAnime* pAnime )		{}
	virtual void				_OnResetAnime( void )								{}
	virtual void				_OnApplyColor( Color Col )							{}

private:	
	void						_ResetLocalMatrix( void );
	void						_UpdateEffect( void );
	void						_UpdateSound( const Matrix3& OffsetMat );
	Effect*						_CreateEffect( const SPRITE_FRAME_EFFECT* pParam );
	Sound*						_CreateSound( const SPRITE_FRAME_SOUND* pParam, bool bLoop );
	Matrix3						_CalcEffectMatrix( const SPRITE_FRAME_EFFECT* pParam );

protected:
	const SpriteFrame*			m_pFrame;
	const SPRITE_FRAME_BONE*	m_pParam;
	bool						m_bVisible;
	Color						m_Color;

	Matrix3						m_LocalMatrix;
	Matrix3						m_WorldMatrix;

private:
	EffectList					m_ResidentEffectList;
	SoundList					m_ResidentSoundList;
	EffectList					m_AppearEffectList;
	SoundList					m_AppearSoundList;
};

////////////////////////////////////////////////////////////////////////////////
//
//	スプライトフレーム：ボーン（ダミー）
//
////////////////////////////////////////////////////////////////////////////////
class SpriteFrameBoneDummy : public SpriteFrameBone
{
public:
	SpriteFrameBoneDummy( void );
	virtual ~SpriteFrameBoneDummy();

protected:
	virtual void				_OnInit( void );
	virtual void				_OnDraw( const Matrix3& OffsetMat );

private:
	ShapeArrowLine*				m_pShape;
};


////////////////////////////////////////////////////////////////////////////////
//
//	スプライトフレーム：ボーン（矩形）
//
////////////////////////////////////////////////////////////////////////////////
class SpriteFrameBoneRect : public SpriteFrameBone
{
public:
	SpriteFrameBoneRect( void );
	virtual ~SpriteFrameBoneRect();

	virtual void				SetDrawPhase( int Phase );
	virtual void				SetPieceIndex( int Index )							{ m_Index = Index; }
	virtual Rect				GetRect( void )								const;

protected:
	virtual void				_OnInit( void );
	virtual void				_OnDraw( const Matrix3& OffsetMat );
	
	virtual void				_OnApplyAnime( const SpriteFrameAnime* pAnime );
	virtual void				_OnResetAnime( void );
	virtual void				_OnApplyColor( Color Col );

private:
	RectSprite*					m_pSprite;
	Point						m_Center;
	int							m_Index;
	Label						m_PieceName;
};


////////////////////////////////////////////////////////////////////////////////
//
//	スプライトフレーム：ボーン（輪形）
//
////////////////////////////////////////////////////////////////////////////////
class SpriteFrameBoneRing : public SpriteFrameBone
{
public:
	SpriteFrameBoneRing( void );
	virtual ~SpriteFrameBoneRing();

	virtual void				SetDrawPhase( int Phase );
	virtual void				SetAngle( float Ang );

	virtual float				GetRadius( void )							const;

protected:
	virtual void				_OnInit( void );
	virtual void				_OnDraw( const Matrix3& OffsetMat );
	
	virtual void				_OnApplyAnime( const SpriteFrameAnime* pAnime );
	virtual void				_OnResetAnime( void );
	virtual void				_OnApplyColor( Color Col );

private:
	RingSprite*					m_pSprite;
	Label						m_PieceName;
};


////////////////////////////////////////////////////////////////////////////////
//
//	スプライトフレーム：ボーン（フォント）
//
////////////////////////////////////////////////////////////////////////////////
class SpriteFrameBoneFont : public SpriteFrameBone
{
public:
	SpriteFrameBoneFont( void );
	virtual ~SpriteFrameBoneFont();

	virtual void				SetDrawPhase( int Phase );
	virtual void				SetText( pstr pStr );

protected:
	virtual void				_OnInit( void );
	virtual void				_OnDraw( const Matrix3& OffsetMat );
	
	virtual void				_OnApplyAnime( const SpriteFrameAnime* pAnime );
	virtual void				_OnResetAnime( void );
	virtual void				_OnApplyColor( Color Col );

private:
	FontSprite*					m_pSprite;
};