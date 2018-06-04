#include "stdafx.h"
#include "EffectMoveVelocity.h"

#include "../EffectManager.h"
#include "../EffectParticle.h"

#include "App/App.h"

namespace
{
	const EFFECT_MOVE_VELOCITY_PARAM s_DefaultParam = { 1.0f, 0.0f, 0.0f, 1.0f, false };
}


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�ړ����i�ړ��^�j
//
////////////////////////////////////////////////////////////////////////////////
EffectMoveVelocity::EffectMoveVelocity( EffectParticle* pParticle, const EFFECT_MOVE_VELOCITY_PARAM* pParam )
: EffectMove( pParticle )
{
	m_pAppear = NULL;

	m_pParam	= pParam;
	if( !pParam ) m_pParam = &s_DefaultParam;

	m_RotSpeed	= 0.0f;
	m_Scale.One();
}

EffectMoveVelocity::~EffectMoveVelocity()
{
	
}

//******************************************************************************
//	������
//******************************************************************************
void EffectMoveVelocity::Init( EffectManager* pManager, const Matrix3& Parent, const EFFECT_APPEAR* pAppear )
{
	_ASSERT( pManager && pAppear );

	m_pAppear = pAppear;

	Rand& Rnd = pManager->GetRand();

	//	�����ʒu�ݒ�
	Point Offset( Rnd.GetRandRange( pAppear->Velocity.X, pAppear->Velocity.XRange ), 
					Rnd.GetRandRange( pAppear->Velocity.Y, pAppear->Velocity.YRange ) );
	Point Pos( 0.0f, Rnd.GetRandRange( pAppear->Velocity.Dist, pAppear->Velocity.DistRange ) );
	Point Vel( 0.0f, Rnd.GetRandRange( pAppear->Velocity.Speed, pAppear->Velocity.SpeedRange ) );
	
	//	�ʒu�ƌ�����ϊ�
	float Dir	= Rnd.GetRandRange( pAppear->Velocity.Dir, pAppear->Velocity.DirRange );

	m_Pos		= Parent.Transform( Pos.Rot( Dir ) + Offset );
	m_Vel		= Parent.TransformNormal( Vel.Rot( Dir ) );

	//	��]
	m_Rot		= Rnd.GetRandRange( pAppear->Velocity.Rot, pAppear->Velocity.RotRange );
	m_RotSpeed	= Rnd.GetRandRange( pAppear->Velocity.RotSpeed, pAppear->Velocity.RotSpeedRange );

	if( m_pAppear->DirType == EFFECT_DIR_ROT ) m_Rot += Parent.GetRotZ();

	//	�}�g���N�X�X�V
	_UpdateMatrix();
}

//******************************************************************************
//	�X�V
//******************************************************************************
void EffectMoveVelocity::Update( const EffectManager* pManager )
{
	_ASSERT( pManager );

	//	�d��
	m_Vel.y += pManager->GetGravity() * m_pParam->Gravity;

	//	�͏�
	m_Vel += pManager->GetForceValue( m_Pos ) * m_pParam->Force;

	//	�ʒu�X�V
	m_Pos += m_Vel;
	m_Rot += m_RotSpeed;

	//	�����_������
	if( m_pAppear->Velocity.MoveRandomX != 0.0f || m_pAppear->Velocity.MoveRandomY != 0.0f || 
		m_pAppear->Velocity.RotRandom != 0.0f )
	{
		Rand& Rnd = GetApp()->GetEffectMng()->GetRand();

		Point Offset( Rnd.GetRandRange( 0.0f, m_pAppear->Velocity.MoveRandomX ), Rnd.GetRandRange( 0.0f, m_pAppear->Velocity.MoveRandomY ) );
		m_Pos += Offset.Rot( m_Vel.GetAngle() );
		m_Rot += Rnd.GetRandRange( 0.0f, m_pAppear->Velocity.RotRandom );
	}

	//	����
	m_Vel *= m_pParam->Decel;

	//	�}�g���N�X�X�V
	_UpdateMatrix();
}

//******************************************************************************
//	�^�C���L�[�p�����[�^�̔��f
//******************************************************************************
void EffectMoveVelocity::ApplyTimeKey( EFFECT_TIME_KEY_TYPE Type, Point Val )
{
	switch( Type )
	{
	case EFFECT_TIME_KEY_ROT:			m_Rot		= Val.x;					break;
	case EFFECT_TIME_KEY_ROTSPEED:		m_RotSpeed	= Val.x;					break;
	case EFFECT_TIME_KEY_SCALE:			m_Scale		= Val;						break;
	case EFFECT_TIME_KEY_VEL:			m_Vel		+= Val;						break;
	case EFFECT_TIME_KEY_SPEED:			m_Vel		= m_Vel.GetNormal() * Val;	break;
	};
}

//------------------------------------------------------------------------------
//	�}�g���N�X�X�V
//------------------------------------------------------------------------------
void EffectMoveVelocity::_UpdateMatrix( void )
{
	float Rot = m_Rot;
	if( m_pAppear->DirType == EFFECT_DIR_FOLLOW )
	{
		Rot = m_Vel.GetAngle() + m_Rot.Rad();
	}
		
	//	�g�k
	m_Matrix = Matrix3::Scale( m_Scale * m_pParticle->GetScale() );

	//	��]
	if( Rot != 0.0f ) m_Matrix *= Matrix3::RotZ( Rot );

	//	�ʒu
	if( !m_Pos.IsZero() ) m_Matrix *= Matrix3::Trans( m_Pos );
}