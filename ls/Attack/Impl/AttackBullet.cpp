#include "stdafx.h"
#include "AttackBullet.h"

#include "../AttackData.h"
#include "../AttackManager.h"

#include "World/World.h"

#include "Collide/Shape/CollideShapePoint.h"
#include "Collide/Shape/CollideShapeLine.h"
#include "Collide/CollideCaster.h"

#include "Player/Player.h"
#include "Player/Equip/Blade.h"
#include "Player/Equip/Spear.h"
#include "Score/ScoreManager.h"
#include "Ground/Ground.h"


////////////////////////////////////////////////////////////////////////////////
//
//	敵の攻撃：弾丸
//
////////////////////////////////////////////////////////////////////////////////
AttackBullet::AttackBullet( AttackManager* pManager ) : Attack( pManager )
{
	m_pParam		= NULL;

	m_Speed			= 0.0f;
	m_RotSpeed		= 0.0f;
}
AttackBullet::~AttackBullet()
{
	
}

//******************************************************************************
//	タイムキーパラメータの適用（単独）
//******************************************************************************
void AttackBullet::ApplyTimeKey( ATTACK_TIME_KEY_TYPE Type, const ATTACK_TIME_KEY_PARAM& Key )
{
	switch( Type )
	{
	case ATTACK_TIME_KEY_SPEED:
		m_Speed = Key.Val[0];
		return;
	case ATTACK_TIME_KEY_ROTSPEED:
		m_RotSpeed = Key.Val[0];
		return;
	}

	__super::ApplyTimeKey( Type, Key );
}

//******************************************************************************
//	タイムキーパラメータの適用（補間）
//******************************************************************************
void AttackBullet::ApplyTimeKey( ATTACK_TIME_KEY_TYPE Type, const ATTACK_TIME_KEY_PARAM& Key, const ATTACK_TIME_KEY_PARAM& Next, float f )
{
	switch( Type )
	{
	case ATTACK_TIME_KEY_SPEED:
		m_Speed = Lerp( Key.Val[0], Next.Val[0], f );
		return;
	case ATTACK_TIME_KEY_ROTSPEED:
		m_RotSpeed = Lerp( Key.Val[0], Next.Val[0], f );
		return;
	}

	__super::ApplyTimeKey( Type, Key, Next, f );
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
void AttackBullet::_OnInit( void )
{
	//	パラメータ
	m_pParam	= &m_pData->GetParam().Bullet;

	m_Speed		= m_pManager->GetRand().GetRandRange( m_pParam->Speed, m_pParam->SpeedRange );
	m_RotSpeed	= m_pManager->GetRand().GetRandRange( m_pParam->RotSpeed, m_pParam->RotSpeedRange );
}

//------------------------------------------------------------------------------
//	更新
//------------------------------------------------------------------------------
void AttackBullet::_OnUpdate( void )
{
	Matrix3 Mat = GetMatrix();

	//	回転を加える
	if( m_RotSpeed != 0.0f )
	{
		Mat.SetRotZ( Mat.GetRotZ() + m_RotSpeed );
	}

	//	直進
	Mat.SetPos( Mat.GetPos() + Mat.GetAxisY() * m_Speed );

	//	減速
	m_Speed *= m_pParam->Decel;

	SetMatrix( Mat );
}

//------------------------------------------------------------------------------
//	終了フレームに到達
//------------------------------------------------------------------------------
void AttackBullet::_OnEndFrame( void )
{
	//	攻撃の生成
	_AppearAttack( _GetAppearAttack() );

	//	終了エフェクト出す
	Label EndEffect = _GetEndEffect();
	if( !EndEffect.IsEmpty() )
	{
		_AppearEffect( EndEffect );
	}
}

//------------------------------------------------------------------------------
//	画面外に出た
//------------------------------------------------------------------------------
void AttackBullet::_OnOutField( void )
{
	//	終了エフェクト出す
	Label EndEffect = _GetEndEffect();
	if( !EndEffect.IsEmpty() )
	{
		_AppearEffect( EndEffect );
	}
}

//------------------------------------------------------------------------------
//	当たり形状作成
//------------------------------------------------------------------------------
CollideShape* AttackBullet::_CreateCollideShape( void ) const
{
	if( _GetLength() == 0.0f )
	{
		return new CollideShapePoint( _GetRadius() );
	}
	else
	{
		return new CollideShapeLine( -_GetLength(), _GetFrontLength(), _GetRadius() );
	}
}

//------------------------------------------------------------------------------
//	当たりキャストに当たった
//------------------------------------------------------------------------------
bool AttackBullet::_OnCastHit( const COLLIDE_RESULT& Result )
{
	enum
	{
		RESULT_HIT		= (1 << 0),	//	命中
		RESULT_ERASE	= (1 << 1),	//	削除する
		RESULT_APPEAR	= (1 << 2),	//	派生攻撃を生成する
	};

	uint	Ret		= 0;

	switch( Result.pOwner->Type )
	{
	case COLLIDE_OWNER_PLAYER:
		{
			Player* pPlayer = (Player*)Result.pOwner->pData;
			if( pPlayer->Damage( m_pData->GetParam().Damage, _GetHitVel(Result.Normal) * m_pData->GetParam().HitVel ) )
			{
				Ret |= (RESULT_HIT | RESULT_APPEAR);

				if( !_IsPierce() )
				{
					Ret |= RESULT_ERASE;
				}
			}
		}
		break;
	case COLLIDE_OWNER_BLADE:
		{
			Blade* pBlade = (Blade*)Result.pOwner->pData;
			if( pBlade->OnHitAttack( m_pData->GetParam().Exhaust, IsRepulse(), Result.HitPos ) )
			{
				//	スコア通知
				m_pManager->GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_BULLET_ERASE, m_pData->GetParam().ScoreRate );

				Ret |= (RESULT_HIT | RESULT_ERASE);
			}
		}
		break;
	case COLLIDE_OWNER_SPEAR:
		{
			Spear* pSpear = (Spear*)Result.pOwner->pData;
			if( pSpear->OnHitAttack( m_pData->GetParam().Exhaust, Result.HitPos ) )
			{
				Ret |= (RESULT_HIT | RESULT_ERASE);
			}
		}
		break;
	case COLLIDE_OWNER_RULER:
		{
			Ret |= RESULT_ERASE;
			
			if( !_IsPierce() )
			{
				Ret |= RESULT_HIT;
			}
		}
		break;
	case COLLIDE_OWNER_GROUND:
		{
			Ground* pGround = (Ground*)Result.pOwner->pData;

			if( !_IsPierce() )
			{
				Ret |= (RESULT_APPEAR);
				
				//	ヒットエフェクト出すかどうか
				if( pGround->IsAttackHitEffect() )
				{
					Ret |= RESULT_HIT;
				}

				//	攻撃を消去するかどうか
				if( pGround->IsAttackErase() )
				{
					Ret |= RESULT_ERASE;
				}
				
				//	地形側のエフェクト
				pGround->CreateHitEffect( Result.GetHitMatrix() );
			}
		}
		break;
	}

	if( Ret & RESULT_HIT )
	{
		//	攻撃発生
		if( Ret & RESULT_APPEAR && 
			!_GetAppearAttack().IsEmpty() )
		{
			_AppearAttack( _GetAppearAttack() );
		}
		else
		{
			//	ヒットエフェクト出す
			Label HitEffect( m_pData->GetParam().HitEffectName );
			if( !HitEffect.IsEmpty() )
			{
				_AppearEffect( HitEffect, Result.GetHitMatrix( PI ) );
			}
		}
	}
	
	//	削除処理
	if( Ret & RESULT_ERASE )
	{
		//	終了エフェクト出す
		Label EndEffect = _GetEndEffect();
		if( !EndEffect.IsEmpty() )
		{
			_AppearEffect( EndEffect );
		}

		Erase();
	}

	return (Ret & RESULT_ERASE) == 0;
}

//------------------------------------------------------------------------------
//	当たり判定の長さを変更
//------------------------------------------------------------------------------
void AttackBullet::_SetHitLength( float Len )
{
	if( _GetCollideCaster() )
	{
		CollideShape* pShape = _GetCollideCaster()->GetShape();
		if( pShape ) pShape->SetLength( 0.0f, Len );
	}
}