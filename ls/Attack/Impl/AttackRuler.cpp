#include "stdafx.h"
#include "AttackRuler.h"

#include "../AttackData.h"

#include "App/App.h"

#include "Collide/Shape/CollideShapePoint.h"
#include "Collide/Shape/CollideShapeLine.h"

#include "BGM/BGMManager.h"

#include "Player/Player.h"
#include "Player/Equip/Blade.h"
#include "Player/Equip/Spear.h"
#include "Player/Equip/Ruler.h"
#include "Enemy/Enemy.h"


////////////////////////////////////////////////////////////////////////////////
//
//	敵の攻撃：ルーラー
//
////////////////////////////////////////////////////////////////////////////////
AttackRuler::AttackRuler( AttackManager* pManager ) : Attack( pManager )
{
	m_pParam		= NULL;
	m_bHit			= false;
	m_HitCount		= 0;
	m_bChangeVolume	= false;
}
AttackRuler::~AttackRuler()
{
	//	拘束を解除
	if( m_pPlayer )	m_pPlayer->SetEnemyRulerHit( false );
	
	//	BGM戻す
	if( m_bChangeVolume ) GetApp()->GetBGMMng()->SetVolume( 1.0f );
}

//******************************************************************************
//	オーナーの登録
//******************************************************************************
void AttackRuler::SetOwner( Enemy* pEnemy )
{
	m_pEnemy = pEnemy;
}

//******************************************************************************
//	終了フレーム取得
//******************************************************************************
int AttackRuler::GetEndFrame( void ) const
{
	return m_bHit? 0 : m_pData->GetParam().EndFrame;
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
void AttackRuler::_OnInit( void )
{
	//	パラメータ
	m_pParam	= &m_pData->GetParam().Ruler;
}

//------------------------------------------------------------------------------
//	更新
//------------------------------------------------------------------------------
void AttackRuler::_OnUpdate( void )
{
	//	命中した場合
	if( m_bHit )
	{
		m_HitCount++;

		if( !m_pPlayer ) return;

		if( m_HitCount == m_pParam->AttackFrame )
		{
			//	攻撃エフェクト発生
			Label AttackEffect( m_pParam->AttackEffectName );
			if( !AttackEffect.IsEmpty() )
			{
				_AppearEffect( AttackEffect, Matrix3::TransRotZ( m_pPlayer->GetPos(), m_HitVel.GetAngle() ) );
			}
		}

		if( m_HitCount >= m_pParam->HitEndFrame )
		{
			//	ダメージを与える
			m_pPlayer->Damage( m_pData->GetParam().Damage, m_HitVel * m_pData->GetParam().HitVel, true );

			//	破損通知
			if( m_pEnemy && !m_pData->GetParam().bNotNotify )
			{
				m_pEnemy->OnAttackNotify( ENEMY_ATTACK_NOTIFY_CRASH );
			}

			//	消滅
			Label BreakEffect( m_pData->GetParam().HitEffectName );
			if( !BreakEffect.IsEmpty() )
			{
				_AppearEffect( BreakEffect );
			}

			Erase();
		}
	}
}

//------------------------------------------------------------------------------
//	終了フレームに到達
//------------------------------------------------------------------------------
void AttackRuler::_OnEndFrame( void )
{
	//	破損通知
	if( m_pEnemy && !m_pData->GetParam().bNotNotify )
	{
		m_pEnemy->OnAttackNotify( ENEMY_ATTACK_NOTIFY_CRASH );
	}

	//	消滅
	Label BreakEffect( m_pData->GetParam().HitEffectName );
	if( !BreakEffect.IsEmpty() )
	{
		_AppearEffect( BreakEffect );
	}
}

//------------------------------------------------------------------------------
//	当たり形状作成
//------------------------------------------------------------------------------
CollideShape* AttackRuler::_CreateCollideShape( void ) const
{
	return new CollideShapePoint( m_pParam->Radius );
}

//------------------------------------------------------------------------------
//	当たりキャストに当たった
//------------------------------------------------------------------------------
bool AttackRuler::_OnCastHit( const COLLIDE_RESULT& Result )
{
	enum RESULT
	{
		RESULT_NONE,
		RESULT_BREAK,
	};

	RESULT Ret = RESULT_NONE;

	switch( Result.pOwner->Type )
	{
	case COLLIDE_OWNER_PLAYER:
		{
			//	自機
			Player* pPlayer = (Player*)Result.pOwner->pData;

			//	未ヒット
			if( !m_bHit )
			{
				//	自機を拘束
				pPlayer->SetEnemyRulerHit( true );

				//	登録
				m_bHit		= true;
				m_HitCount	= 0;
				m_pPlayer	= pPlayer;
				m_HitVel	= _GetHitVel( Result.Normal );

				//	BGM消す
				GetApp()->GetBGMMng()->SetVolume( 0.0f );
				m_bChangeVolume = true;
			}
		}
		break;
	case COLLIDE_OWNER_BLADE:
		{
			//	ブレード・折れる
			Blade* pBlade = (Blade*)Result.pOwner->pData;
			pBlade->OnCrashRuler();
		}
		break;
	case COLLIDE_OWNER_SPEAR:
		{
			//	スピア・折れる
			Spear* pSpear = (Spear*)Result.pOwner->pData;
			pSpear->Crash();
		}
		break;
	case COLLIDE_OWNER_RULER:
		{
			//	ルーラー・対消滅
			Ruler* pRuler = (Ruler*)Result.pOwner->pData;
			pRuler->Crash();
			Ret = RESULT_BREAK;
		}
		break;
	}

	//	破損
	if( Ret == RESULT_BREAK )
	{
		//	通知
		if( m_pEnemy && !m_pData->GetParam().bNotNotify )
		{
			m_pEnemy->OnAttackNotify( ENEMY_ATTACK_NOTIFY_CRASH );
		}
		
		//	ブレイクエフェクト出す
		Label BreakEffect( m_pData->GetParam().HitEffectName );
		if( !BreakEffect.IsEmpty() )
		{
			_AppearEffect( BreakEffect );
		}
		
		Erase();
	}

	return true;
}

//------------------------------------------------------------------------------
//	当たり判定をするか
//------------------------------------------------------------------------------
bool AttackRuler::IsHit( const COLLIDE_OWNER& Owner ) const
{
	switch( Owner.Type )
	{
	case COLLIDE_OWNER_PLAYER:
		{
			if( m_pData->GetParam().bNotPlayerHit ) return false;
			
			//	無敵状態
			Player* pPlayer = (Player*)Owner.pData;
			if( pPlayer->IsInvincible( true ) ) return false;
		}
		return true;
	case COLLIDE_OWNER_BLADE:
	case COLLIDE_OWNER_SPEAR:	return !m_pData->GetParam().bNotAttackHit;
	case COLLIDE_OWNER_GROUND:	return !m_pData->GetParam().bNotGroundHit;
	}
	return true;
}