#include "stdafx.h"
#include "AttackHoming.h"

#include "../AttackData.h"
#include "../AttackManager.h"

#include "World/World.h"

#include "Collide/Shape/CollideShapePoint.h"
#include "Collide/Shape/CollideShapeLine.h"

#include "Player/Player.h"
#include "Player/PlayerManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�̍U���F�ǔ�
//
////////////////////////////////////////////////////////////////////////////////
AttackHoming::AttackHoming( AttackManager* pManager ) : AttackBullet( pManager )
{
	m_pParam	= NULL;

	m_Speed			= 0.0f;
	m_RotSpeed		= 0.0f;
	m_RotAccel		= 0.0f;
}
AttackHoming::~AttackHoming()
{
	
}

//******************************************************************************
//	�^�C���L�[�p�����[�^�̓K�p�i�P�Ɓj
//******************************************************************************
void AttackHoming::ApplyTimeKey( ATTACK_TIME_KEY_TYPE Type, const ATTACK_TIME_KEY_PARAM& Key )
{
	switch( Type )
	{
	case ATTACK_TIME_KEY_ACCEL:
		m_Accel = Key.Val[0];
		return;
	}
	
	__super::ApplyTimeKey( Type, Key );
}

//******************************************************************************
//	�^�C���L�[�p�����[�^�̓K�p�i��ԁj
//******************************************************************************
void AttackHoming::ApplyTimeKey( ATTACK_TIME_KEY_TYPE Type, const ATTACK_TIME_KEY_PARAM& Key, const ATTACK_TIME_KEY_PARAM& Next, float f )
{
	switch( Type )
	{
	case ATTACK_TIME_KEY_ACCEL:
		m_Accel = Lerp( Key.Val[0], Next.Val[0], f );
		return;
	}

	__super::ApplyTimeKey( Type, Key, Next, f );
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
void AttackHoming::_OnInit( void )
{
	//	�p�����[�^
	m_pParam	= &m_pData->GetParam().Homing;

	m_Speed		= m_pParam->Speed;
	m_RotSpeed	= m_pParam->RotSpeed;
	m_Accel		= m_pParam->Accel;
	m_RotAccel	= m_pParam->RotAccel;
}

//------------------------------------------------------------------------------
//	�X�V
//------------------------------------------------------------------------------
void AttackHoming::_OnUpdate( void )
{
	Matrix3 Mat = GetMatrix();

	//	�ǔ�����
	if( _GetFrameCount() <= m_pParam->SearchTime )
	{
		const Player* pPlayer = m_pManager->GetWorld()->GetPlayerManager()->GetPlayer();
		if( pPlayer )
		{
			float ToDir = (pPlayer->GetPos() - Mat.GetPos()).GetAngle();
			float RotZ	= Mat.GetRotZ();
			float Diff	= Angle::Diff( ToDir, RotZ );

			float Add = 0.0f;
			float AddRot = 0.0f;
			
			//	���� 
			if( _GetFrameCount() <= m_pParam->StartTime )
			{
				Add		= -m_Accel;
			}
			else
			{
				//	����
				if( Abs(Diff) < m_pParam->AccelAngle )
				{
					Add		= m_Accel;
				}
				//	����
				else
				{
					Add		= -m_Accel;
				}
				
				//	�p�x����
				if( Abs(Diff) >= m_pParam->RotAccelAngle )
				{
					AddRot	= m_RotAccel;
				}
				//	�p�x����
				else
				{
					AddRot	= -m_RotAccel;
				}
			}

			//	���x�E�p���x�̍X�V
			m_Speed		= Clamp( m_Speed + Add, m_pParam->MinSpeed, m_pParam->MaxSpeed );
			m_RotSpeed	= Clamp( m_RotSpeed + AddRot, m_pParam->MinRotSpeed, m_pParam->MaxRotSpeed );
			
			//	������]��
			if( m_RotSpeed != 0.0f )
			{
				Mat.SetRotZ( Angle::NearTo( RotZ, ToDir, m_RotSpeed ) );
			}
		}
	}
	//	�ǔ��I��
	else
	{
		//	��������
		m_Speed = Clamp( m_Speed + m_Accel, m_pParam->MinSpeed, m_pParam->MaxSpeed );
	}

	//	���i
	Mat.SetPos( Mat.GetPos() + Mat.GetAxisY() * m_Speed );

	SetMatrix( Mat );
}
