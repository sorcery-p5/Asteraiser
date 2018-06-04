#include "stdafx.h"
#include "EffectMoveFollow.h"

#include "../EffectManager.h"
#include "../EffectParticle.h"

#include "App/App.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�ړ����i�Ǐ]�^�j
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
//	������
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

	//	�}�g���N�X�X�V
	_UpdateMatrix();
}

//******************************************************************************
//	�X�V
//******************************************************************************
void EffectMoveFollow::Update( const EffectManager* pManager )
{
	_ASSERT( pManager );

	//	��]
	m_Rot += m_RotSpeed;

	//	�}�g���N�X�X�V
	_UpdateMatrix();
}

//******************************************************************************
//	�^�C���L�[�p�����[�^�̔��f
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
//	�}�g���N�X���X�V
//------------------------------------------------------------------------------
void EffectMoveFollow::_UpdateMatrix( void )
{
	Point Offset;

	//	�����_������
	if( m_pAppear->Follow.MoveRandomX != 0.0f || m_pAppear->Follow.MoveRandomY != 0.0f )
	{
		Rand& Rnd = GetApp()->GetEffectMng()->GetRand();

		Offset.x = Rnd.GetRandRange( 0.0f, m_pAppear->Follow.MoveRandomX );
		Offset.y = Rnd.GetRandRange( 0.0f, m_pAppear->Follow.MoveRandomY );
	}

	//	�����Œ�
	if( m_pAppear->DirType == EFFECT_DIR_FIX )
	{
		m_Matrix = Matrix3::TransRotZ( m_pParent->Transform(m_Pos) + Offset, m_Rot );
	}
	//	�ړ������֌���
	else if( m_pAppear->DirType == EFFECT_DIR_FOLLOW )
	{
		Point Pos = m_pParent->Transform(m_Pos);
		Point Dir = Pos - m_Matrix.GetPos();
		
		m_Matrix = Matrix3::TransRotZ( Pos + Offset, Dir.GetAngle() + m_Rot.Rad() );
	}
	//	��]�̌���
	else
	{
		m_Matrix = Matrix3::TransRotZ( m_Pos + Offset, m_Rot );
		m_Matrix *= (*m_pParent);
	}

	//	�X�P�[��
	if( !m_Scale.IsOne() || !m_pParticle->GetScale().IsOne() )
	{
		m_Matrix = Matrix3::Scale( m_Scale * m_pParticle->GetScale() ) * m_Matrix;
	}
}