#include "stdafx.h"
#include "EnemyRotAction.h"
#include "EnemyTarget.h"

#include "../Enemy.h"


////////////////////////////////////////////////////////////////////////////////
//
//	敵：旋回アクション
//
////////////////////////////////////////////////////////////////////////////////
EnemyRotAction::EnemyRotAction( Enemy* pEnemy ) : EnemyAction( pEnemy )
{
	m_pTarget	= NULL;
	m_bRotEnd	= false;
}
EnemyRotAction::~EnemyRotAction()
{
	SafeDelete( m_pTarget );
}

//******************************************************************************
//	目標位置の取得
//******************************************************************************
const Point* EnemyRotAction::GetTargetPos( void ) const
{
	if( !m_pTarget ) return NULL;

	return &m_pTarget->GetPos();
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
void EnemyRotAction::_OnInit( void )
{
	const ENEMY_ROT_ACTION& Param = m_pParam->Rot;

	if( Param.Type == ENEMY_ROT_TARGET )
	{
		m_pTarget = new EnemyTarget( m_pEnemy );
		MEM_CHECK( m_pTarget );

		m_pTarget->Init( &Param.Target, m_pEnemy->GetPos() );
	}
	
	m_bRotEnd	= false;
}

//------------------------------------------------------------------------------
//	更新
//------------------------------------------------------------------------------
void EnemyRotAction::_OnUpdate( void )
{
	const ENEMY_ROT_ACTION& Param = m_pParam->Rot;

	bool bEnd	= false;
	Angle Ang	= 0.0f;
	Angle Rot	= m_pEnemy->GetRot();

	//	目標方向
	if( Param.Type == ENEMY_ROT_TARGET )
	{
		//	目標位置の更新
		m_pTarget->Update();

		Point Diff = m_pTarget->GetPos() - m_pEnemy->GetPos();
		if( Diff.IsZero() )
		{
			bEnd = true;
		}
		else
		{
			Ang = Diff.GetAngle();
		}
	}
	//	固定方向
	else if( Param.Type == ENEMY_ROT_FIX )
	{
		Ang = Param.Fix;
	}
	//	移動方向
	else if( Param.Type == ENEMY_ROT_MOVE )
	{
		const Point* pTarget = m_pEnemy->GetMoveTarget();
		if( pTarget )
		{
			Point Diff = *pTarget - m_pEnemy->GetPos();
			if( Diff.IsZero() )
			{
				bEnd = true;
			}
			else
			{
				Ang = Diff.GetAngle();
			}
		}
	}

	//	旋回目標にいる
	if( bEnd )
	{
		m_bRotEnd = true;
		return;
	}

	//	設定
	Rot.NearTo( Ang, Param.Speed );
	m_pEnemy->SetRot( Rot );

	//	移動終了
	if( Ang == Rot ) m_bRotEnd = true;
}