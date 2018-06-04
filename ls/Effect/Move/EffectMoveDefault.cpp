#include "stdafx.h"
#include "EffectMoveDefault.h"

#include "../EffectManager.h"
#include "../EffectParticle.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�ړ����i�Œ�^�j
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
//	������
//******************************************************************************
void EffectMoveDefault::Init( EffectManager* pManager, const Matrix3& Parent, const EFFECT_APPEAR* pAppear )
{
	_ASSERT( pManager && pAppear );

	m_BaseMat	= Parent;

	_UpdateMatrix();
}

//******************************************************************************
//	�X�V
//******************************************************************************
void EffectMoveDefault::Update( const EffectManager* pManager )
{
	_ASSERT( pManager );
}

//******************************************************************************
//	�^�C���L�[�p�����[�^�̔��f
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
//	�}�g���N�X�X�V
//------------------------------------------------------------------------------
void EffectMoveDefault::_UpdateMatrix( void )
{
	//	�g�k
	m_Matrix = Matrix3::Scale( m_Scale * m_pParticle->GetScale() );

	//	��]
	if( m_Rot != 0.0f ) m_Matrix *= Matrix3::RotZ( m_Rot );

	//	�ʒu
	if( !m_Pos.IsZero() ) m_Matrix *= Matrix3::Trans( m_Pos );

	//	��ʒu�𔽉f
	m_Matrix *= m_BaseMat;
}