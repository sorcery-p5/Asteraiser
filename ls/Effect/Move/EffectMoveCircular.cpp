#include "stdafx.h"
#include "EffectMoveCircular.h"

#include "../EffectManager.h"
#include "../EffectParticle.h"


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト移動物（回転型）
//
////////////////////////////////////////////////////////////////////////////////
EffectMoveCircular::EffectMoveCircular( EffectParticle* pParticle )
: EffectMove( pParticle )
{
	m_pParent		= NULL;

	m_Dist			= 0.0f;
	m_AngleSpeed	= 0.0f;
	m_RotSpeed		= 0.0f;
	m_AxisScale.One();
	m_Scale.One();

	m_DirType	= EFFECT_DIR_ROT;
}

EffectMoveCircular::~EffectMoveCircular()
{
}

//******************************************************************************
//	初期化
//******************************************************************************
void EffectMoveCircular::Init( EffectManager* pManager, const Matrix3& Parent, const EFFECT_APPEAR* pAppear )
{
	_ASSERT( pManager && pAppear );

	m_pParent	= &Parent;

	Rand& Rnd = pManager->GetRand();

	m_Dist			= Rnd.GetRandRange( pAppear->Circular.Dist, pAppear->Circular.DistRange );
	m_Angle			= Rnd.GetRandRange( pAppear->Circular.TrackAngle, pAppear->Circular.TrackAngleRange );
	m_AngleSpeed	= Rnd.GetRandRange( pAppear->Circular.AngleSpeed, pAppear->Circular.AngleSpeedRange );
	m_Rot			= Rnd.GetRandRange( pAppear->Circular.Rot, pAppear->Circular.RotRange );
	m_RotSpeed		= Rnd.GetRandRange( pAppear->Circular.RotSpeed, pAppear->Circular.RotSpeedRange );
	m_AxisScale.x	= Rnd.GetRandRange( pAppear->Circular.AxisScale[0], pAppear->Circular.AxisScaleRange[0] );
	m_AxisScale.y	= Rnd.GetRandRange( pAppear->Circular.AxisScale[1], pAppear->Circular.AxisScaleRange[1] );
	m_AxisAngle		= Rnd.GetRandRange( pAppear->Circular.AxisAngle, pAppear->Circular.AxisAngleRange );
	m_DirType		= pAppear->DirType;

	m_Scale.One();

	//	マトリクス更新
	_UpdateMatrix();
}

//******************************************************************************
//	更新
//******************************************************************************
void EffectMoveCircular::Update( const EffectManager* pManager )
{
	_ASSERT( pManager );

	//	角度速度更新
	m_Angle += m_AngleSpeed;
	m_Rot += m_RotSpeed;

	//	マトリクス更新
	_UpdateMatrix();
}

//******************************************************************************
//	タイムキーパラメータの反映
//******************************************************************************
void EffectMoveCircular::ApplyTimeKey( EFFECT_TIME_KEY_TYPE Type, Point Val )
{
	switch( Type )
	{
	case EFFECT_TIME_KEY_ROT:			m_Rot			= Val.x;		break;
	case EFFECT_TIME_KEY_ROTSPEED:		m_RotSpeed		= Val.x;		break;
	case EFFECT_TIME_KEY_SCALE:			m_Scale			= Val;			break;
	case EFFECT_TIME_KEY_DIST:			m_Dist			= Val.x;		break;
	case EFFECT_TIME_KEY_ANGLE:			m_Angle			= Val.x;		break;
	case EFFECT_TIME_KEY_ANGLE_SPEED:	m_AngleSpeed	= Val.x;		break;
	case EFFECT_TIME_KEY_AXIS_SCALE:	m_AxisScale		= Val;			break;
	case EFFECT_TIME_KEY_AXIS_ANGLE:	m_AxisAngle		= Val.x;		break;
	}
}

//------------------------------------------------------------------------------
//	マトリクスを更新
//------------------------------------------------------------------------------
void EffectMoveCircular::_UpdateMatrix( void )
{
	//	位置の計算
	Point Pos( 0.0f, -m_Dist );

	Pos = Pos.Rot( m_Angle ) * m_AxisScale;
	if( m_AxisAngle != 0.0f ) Pos = Pos.Rot( m_AxisAngle );

	//	向き固定
	if( m_DirType == EFFECT_DIR_FIX )
	{
		m_Matrix = Matrix3::TransRotZ( m_pParent->Transform(Pos), m_Rot );
	}
	//	移動方向へ向く
	else if( m_DirType == EFFECT_DIR_FOLLOW )
	{
		Point NewPos = m_pParent->Transform(Pos);
		Point Dir = Pos - m_Matrix.GetPos();
		
		m_Matrix = Matrix3::TransRotZ( NewPos, Dir.GetAngle() );
	}
	//	中心方向へ向く
	else if( m_DirType == EFFECT_DIR_CENTER )
	{
		Point NewPos = m_pParent->Transform(Pos);
		Point Dir	 = Pos - m_pParent->GetPos();
		
		m_Matrix = Matrix3::TransRotZ( NewPos, Dir.GetAngle() );
	}
	//	回転の向き
	else
	{
		m_Matrix = Matrix3::TransRotZ( Pos, m_Rot );
		m_Matrix *= *m_pParent;
	}
	
	//	スケール
	if( !m_Scale.IsOne() || !m_pParticle->GetScale().IsOne() )
	{
		m_Matrix = Matrix3::Scale( m_Scale * m_pParticle->GetScale() ) * m_Matrix;
	}

}