#include "stdafx.h"
#include "EnemyMoveAction.h"
#include "EnemyTarget.h"

#include "../Enemy.h"


////////////////////////////////////////////////////////////////////////////////
//
//	敵：移動アクション
//
////////////////////////////////////////////////////////////////////////////////
EnemyMoveAction::EnemyMoveAction( Enemy* pEnemy ) : EnemyAction( pEnemy )
{
	m_pTarget	= NULL;
	m_Speed		= 0.0f;
	m_bMoveEnd	= false;
}
EnemyMoveAction::~EnemyMoveAction()
{
	SafeDelete( m_pTarget );
}

//******************************************************************************
//	目標位置を取得
//******************************************************************************
const Point* EnemyMoveAction::GetTargetPos( void ) const
{
	if( !m_pTarget ) return NULL;

	return &m_pTarget->GetPos();
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
void EnemyMoveAction::_OnInit( void )
{
	const ENEMY_MOVE_ACTION& Param = m_pParam->Move;

	m_pTarget = new EnemyTarget( m_pEnemy );
	MEM_CHECK( m_pTarget );

	m_pTarget->Init( &Param.Target, m_pEnemy->GetPos() );
	
	m_Speed		= 0.0f;
	m_bMoveEnd	= false;
}

//------------------------------------------------------------------------------
//	更新
//------------------------------------------------------------------------------
void EnemyMoveAction::_OnUpdate( void )
{
	if( !m_pTarget ) return;

	//	目標位置の更新
	m_pTarget->Update();

	if( m_bMoveEnd ) return;

	//	移動
	const ENEMY_MOVE_ACTION& Param = m_pParam->Move;
	Point Diff;
	float Range = 1.0f;

	if( Param.Type == ENEMY_MOVE_TARGET )
	{
		Diff = m_pTarget->GetPos() - m_pEnemy->GetPos();
		Range = Diff.Length();
	}
	//	横のみ移動
	else if( Param.Type == ENEMY_MOVE_TARGET_X )
	{
		Diff	= Point( m_pTarget->GetPos().x - m_pEnemy->GetPos().x, 0.0f );
		Range	= Abs( Diff.x );
	}
	//	縦のみ移動
	else if( Param.Type == ENEMY_MOVE_TARGET_Y )
	{
		Diff	= Point( 0.0f, m_pTarget->GetPos().y - m_pEnemy->GetPos().y );
		Range	= Abs( Diff.y );
	}

	//	速度の調整
	float DecelRange = _GetDecelRange();
	if( Range > DecelRange )	Increase( m_Speed, Param.MaxSpeed, Param.Accel );
	else						DecreaseZero( m_Speed, Param.Decel );

	if( Range > 0.001f )
	{
		m_pEnemy->AddMoveVelocity( Diff * (m_Speed / Range) );
	}

	//	移動終了
	if( m_Speed <= 0.001f ) m_bMoveEnd = true;
}

//------------------------------------------------------------------------------
//	減速範囲を得る
//------------------------------------------------------------------------------
float EnemyMoveAction::_GetDecelRange( void ) const
{
	if( m_pParam->Move.Decel <= 0 ) return m_Speed;

	float Speed = m_Speed;
	float Range = 0.0f;
	while( Speed > 0.0f )
	{
		Range += Speed;
		DecreaseZero( Speed, m_pParam->Move.Decel );
	}
	return Range;
}