#include "stdafx.h"
#include "EffectMoveFollow.h"

#include "../EffectManager.h"
#include "../EffectParticle.h"

#include "App/App.h"


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト移動物（追従型）
//
////////////////////////////////////////////////////////////////////////////////
EffectMoveFollow::EffectMoveFollow( EffectParticle* pParticle )
: EffectMove( pParticle )
{
	m_pAppear = NULL;
	m_pParent	= NULL;
	m_Scale.One();
	m_RotSpeed	= 0.0f;
}

EffectMoveFollow::~EffectMoveFollow()
{
}

//******************************************************************************
//	初期化
//******************************************************************************
void EffectMoveFollow::Init( EffectManager* pManager, const Matrix3& Parent, const EFFECT_APPEAR* pAppear )
{
	_ASSERT( pManager && pAppear );
	m_pAppear = pAppear;

	Rand& Rnd = pManager->GetRand();

	m_pParent	= &Parent;
	m_Rot		= Rnd.GetRandRange( pAppear->Follow.Rot, pAppear->Follow.RotRange );
	m_RotSpeed	= Rnd.GetRandRange( pAppear->Follow.RotSpeed, pAppear->Follow.RotSpeedRange );
	m_Pos.Set( Rnd.GetRandRange( pAppear->Follow.X, pAppear->Follow.XRange ),
				Rnd.GetRandRange( pAppear->Follow.Y, pAppear->Follow.YRange ) );

	//	マトリクス更新
	_UpdateMatrix();
}

//******************************************************************************
//	更新
//******************************************************************************
void EffectMoveFollow::Update( const EffectManager* pManager )
{
	_ASSERT( pManager );

	//	回転
	m_Rot += m_RotSpeed;

	//	マトリクス更新
	_UpdateMatrix();
}

//******************************************************************************
//	タイムキーパラメータの反映
//******************************************************************************
void EffectMoveFollow::ApplyTimeKey( EFFECT_TIME_KEY_TYPE Type, Point Val )
{
	switch( Type )
	{
	case EFFECT_TIME_KEY_ROT:		m_Rot		= Val.x;	break;
	case EFFECT_TIME_KEY_ROTSPEED:	m_RotSpeed	= Val.x;	break;
	case EFFECT_TIME_KEY_SCALE:		m_Scale		= Val;		break;
	case EFFECT_TIME_KEY_POS:		m_Pos		= Val;		break;
	}
}

//------------------------------------------------------------------------------
//	マトリクスを更新
//------------------------------------------------------------------------------
void EffectMoveFollow::_UpdateMatrix( void )
{
	Point Offset;

	//	ランダム挙動
	if( m_pAppear->Follow.MoveRandomX != 0.0f || m_pAppear->Follow.MoveRandomY != 0.0f )
	{
		Rand& Rnd = GetApp()->GetEffectMng()->GetRand();

		Offset.x = Rnd.GetRandRange( 0.0f, m_pAppear->Follow.MoveRandomX );
		Offset.y = Rnd.GetRandRange( 0.0f, m_pAppear->Follow.MoveRandomY );
	}

	//	向き固定
	if( m_pAppear->DirType == EFFECT_DIR_FIX )
	{
		m_Matrix = Matrix3::TransRotZ( m_pParent->Transform(m_Pos) + Offset, m_Rot );
	}
	//	移動方向へ向く
	else if( m_pAppear->DirType == EFFECT_DIR_FOLLOW )
	{
		Point Pos = m_pParent->Transform(m_Pos);
		Point Dir = Pos - m_Matrix.GetPos();
		
		m_Matrix = Matrix3::TransRotZ( Pos + Offset, Dir.GetAngle() + m_Rot.Rad() );
	}
	//	回転の向き
	else
	{
		m_Matrix = Matrix3::TransRotZ( m_Pos + Offset, m_Rot );
		m_Matrix *= (*m_pParent);
	}

	//	スケール
	if( !m_Scale.IsOne() || !m_pParticle->GetScale().IsOne() )
	{
		m_Matrix = Matrix3::Scale( m_Scale * m_pParticle->GetScale() ) * m_Matrix;
	}
}