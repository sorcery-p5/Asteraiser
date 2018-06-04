#include "stdafx.h"
#include "EffectMoveDefault.h"

#include "../EffectManager.h"
#include "../EffectParticle.h"


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト移動物（固定型）
//
////////////////////////////////////////////////////////////////////////////////
EffectMoveDefault::EffectMoveDefault( EffectParticle* pParticle )
: EffectMove( pParticle )
{
	m_Scale.One();
}

EffectMoveDefault::~EffectMoveDefault()
{
	
}

//******************************************************************************
//	初期化
//******************************************************************************
void EffectMoveDefault::Init( EffectManager* pManager, const Matrix3& Parent, const EFFECT_APPEAR* pAppear )
{
	_ASSERT( pManager && pAppear );

	m_BaseMat	= Parent;

	_UpdateMatrix();
}

//******************************************************************************
//	更新
//******************************************************************************
void EffectMoveDefault::Update( const EffectManager* pManager )
{
	_ASSERT( pManager );
}

//******************************************************************************
//	タイムキーパラメータの反映
//******************************************************************************
void EffectMoveDefault::ApplyTimeKey( EFFECT_TIME_KEY_TYPE Type, Point Val )
{
	_ASSERT( !_isnan(Val.x) );

	switch( Type )
	{
	case EFFECT_TIME_KEY_POS:			m_Pos		= Val;		break;
	case EFFECT_TIME_KEY_ROT:			m_Rot		= Val.x;	break;
	case EFFECT_TIME_KEY_SCALE:			m_Scale		= Val;		break;
	};

	_UpdateMatrix();
}

//------------------------------------------------------------------------------
//	マトリクス更新
//------------------------------------------------------------------------------
void EffectMoveDefault::_UpdateMatrix( void )
{
	//	拡縮
	m_Matrix = Matrix3::Scale( m_Scale * m_pParticle->GetScale() );

	//	回転
	if( m_Rot != 0.0f ) m_Matrix *= Matrix3::RotZ( m_Rot );

	//	位置
	if( !m_Pos.IsZero() ) m_Matrix *= Matrix3::Trans( m_Pos );

	//	基準位置を反映
	m_Matrix *= m_BaseMat;
}