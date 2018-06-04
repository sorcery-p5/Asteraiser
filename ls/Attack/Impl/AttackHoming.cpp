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
//	敵の攻撃：追尾
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
//	タイムキーパラメータの適用（単独）
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
//	タイムキーパラメータの適用（補間）
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
//	初期化
//------------------------------------------------------------------------------
void AttackHoming::_OnInit( void )
{
	//	パラメータ
	m_pParam	= &m_pData->GetParam().Homing;

	m_Speed		= m_pParam->Speed;
	m_RotSpeed	= m_pParam->RotSpeed;
	m_Accel		= m_pParam->Accel;
	m_RotAccel	= m_pParam->RotAccel;
}

//------------------------------------------------------------------------------
//	更新
//------------------------------------------------------------------------------
void AttackHoming::_OnUpdate( void )
{
	Matrix3 Mat = GetMatrix();

	//	追尾処理
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
			
			//	初期 
			if( _GetFrameCount() <= m_pParam->StartTime )
			{
				Add		= -m_Accel;
			}
			else
			{
				//	加速
				if( Abs(Diff) < m_pParam->AccelAngle )
				{
					Add		= m_Accel;
				}
				//	減速
				else
				{
					Add		= -m_Accel;
				}
				
				//	角度加速
				if( Abs(Diff) >= m_pParam->RotAccelAngle )
				{
					AddRot	= m_RotAccel;
				}
				//	角度減速
				else
				{
					AddRot	= -m_RotAccel;
				}
			}

			//	速度・角速度の更新
			m_Speed		= Clamp( m_Speed + Add, m_pParam->MinSpeed, m_pParam->MaxSpeed );
			m_RotSpeed	= Clamp( m_RotSpeed + AddRot, m_pParam->MinRotSpeed, m_pParam->MaxRotSpeed );
			
			//	方向を転換
			if( m_RotSpeed != 0.0f )
			{
				Mat.SetRotZ( Angle::NearTo( RotZ, ToDir, m_RotSpeed ) );
			}
		}
	}
	//	追尾終了
	else
	{
		//	加速する
		m_Speed = Clamp( m_Speed + m_Accel, m_pParam->MinSpeed, m_pParam->MaxSpeed );
	}

	//	直進
	Mat.SetPos( Mat.GetPos() + Mat.GetAxisY() * m_Speed );

	SetMatrix( Mat );
}
