#pragma once

#include "EffectTypes.h"

class EffectParticle;
class EffectFile;
class EffectManager;


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト
//
////////////////////////////////////////////////////////////////////////////////
class Effect
{
	REFERENCE_IMPL( Effect );
public:
	Effect( void );
	~Effect();

	void					Init( const EffectFile* pFile );
	void					Update( EffectManager* pManager );
	void					Draw( const Matrix3& OffsetMat )	const;

	void					SetPos( const Point& Pos )					{ m_Matrix.SetPos( Pos ); }
	void					SetMatrix( const Matrix3& Mat )				{ m_Matrix = Mat; }
	void					SetVisible( bool bVisible );
	void					SetColor( Color Col );
	void					Pause( bool bPause );

	void					Delete( void );
	void					EndAppear( void );

	Point					GetParticleScale( Label Name );
	void					SetParticleScale( Label Name, Point Scale );

	bool					IsDelete( void )					const;
	bool					IsEndAppear( void )					const;
	bool					IsLastFrame( void )					const	{ return m_FrameCount >= m_pParam->EndFrame; }
	bool					IsVisible( void )					const	{ return m_bVisible; }
	Color					GetColor( void )					const	{ return m_Color; }
	bool					IsPause( void )						const	{ return m_bPause; }

private:
	typedef std::vector<EffectParticle*>	VecParticle;
	
private:
	void					_UpdateParticle( EffectManager* pManager );
	void					_UpdateAppear( EffectManager* pManager );
	void					_AppearParticle( EffectManager* pManager, const EFFECT_APPEAR* pParam );

	EffectParticle*			_FindParticle( Label Name );

private:
	const EffectFile*		m_pFile;
	const EFFECT_PARAM*		m_pParam;
	VecParticle				m_vParticle;

	uint					m_FrameCount;
	Matrix3					m_Matrix;

	bool					m_bDelete;
	bool					m_bEndAppear;
	bool					m_bVisible;
	bool					m_bPause;
	Color					m_Color;
};


//------------------------------------------------------------------------------
//	削除用
//------------------------------------------------------------------------------
#define EffectDelete( p )		{ if(p){ (p)->Delete(); (p) = NULL; } }
#define EffectEndAppear( p )	{ if(p){ (p)->EndAppear(); (p) = NULL; } }