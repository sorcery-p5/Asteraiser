#include "stdafx.h"
#include "Spear.h"

#include "Player/PlayerTypes.h"
#include "Player/Player.h"

#include "World/World.h"

#include "SpriteFrame/SpriteFrame.h"
#include "Collide/CollideSystem.h"
#include "Effect/EffectManager.h"
#include "Effect/Effect.h"

#include "Enemy/Enemy.h"
#include "Attack/Attack.h"
#include "Ground/Ground.h"
#include "Item/ItemManager.h"
#include "Score/ScoreManager.h"

namespace
{
	const float		OUTCHECK_WIDEN		= 30.0f;
	const float		SCALE_MAX			= 1.5f;
}


////////////////////////////////////////////////////////////////////////////////
//
//	オーヴァドライヴ・スピア
//
////////////////////////////////////////////////////////////////////////////////
Spear::Spear( const Player* pParent )
{
	m_pParent		= pParent;
	m_pParam		= NULL;

	m_pCollide		= NULL;
	m_pCaster		= NULL;

	m_FrameCount	= 0;
	m_bDelete		= false;
	m_Energy		= 0.0f;
	m_Attack		= 0.0f;
}

Spear::~Spear()
{
	EffectEndAppear( m_pEffect );
	SafeDelete( m_pCaster );
	SafeDelete( m_pCollide );
}

//******************************************************************************
//	初期化
//******************************************************************************
void Spear::Init( const SPEAR_PARAM* pParam, const Matrix3& Mat, float AddEnergy )
{
	//	パラメータ
	m_pParam		= pParam;
	m_Attack		= Min( pParam->Attack + AddEnergy * pParam->AttackItemRate, pParam->AttackMax );
	m_Energy		= Min( pParam->Energy + AddEnergy * pParam->EnergyItemRate, pParam->EnergyMax );
	m_FrameCount	= 0;

	//	大きさの設定
	float Scale		 = Lerp( 1.0f, SCALE_MAX, Coef( m_Attack, pParam->Attack, pParam->AttackMax ) );

	//	マトリクス設定
	m_Matrix		= Matrix3::Scale( Point(Scale,Scale) ) * Mat;
	
	//	エフェクトを作る
	m_pEffect = m_pParent->CreateEffect( pParam->EffectName, Mat );

	//	当たり判定を作る
	m_pCaster = new CollideCaster;
	m_pCollide = new CollideObject;
	MEM_CHECK( m_pCaster && m_pCollide );

	m_pCaster->Init( new CollideShapeLine( -m_pParam->Length, 0.0f, m_pParam->Radius ) );
	m_pCaster->SetCalcType( COLLIDE_CALC_POS_NORMAL );
	m_pCaster->SetOwner( COLLIDE_OWNER_SPEAR, this );
	m_pCaster->SetFilter( COLLIDE_FILTER_PLAYER );
	m_pCaster->UpdateMatrix( &m_Matrix );

	m_pCollide->Init( new CollideShapeLine( -m_pParam->Length, 0.0f, m_pParam->Radius ), COLLIDE_LAYER_BLADE );
	m_pCollide->SetOwner( COLLIDE_OWNER_SPEAR, this );
	m_pCollide->UpdateMatrix( &m_Matrix );
}

//******************************************************************************
//	更新
//******************************************************************************
void Spear::Update( void )
{
	if( m_bDelete ) return;

	//	移動
	m_Matrix.SetPos( m_Matrix.GetPos() + m_Matrix.GetAxisY() * m_pParam->Speed );

	//	当たり判定実行
	m_pCaster->UpdateMatrix( &m_Matrix );

	COLLIDE_CAST_RESULT Result;
	if( m_pCaster->Cast( Result ) )
	{
		for( COLLIDE_RESULT_LIST::iterator it = Result.ResultList.begin(); it != Result.ResultList.end(); ++it )
		{
			if( !_OnCastHit( *it ) )
			{
				//	判定中断
				break;
			}
		}
	}
	
	//	マトリクスの更新
	_UpdateMatrix();

	//	フレームカウント
	m_FrameCount++;

	//	終了
	if( m_FrameCount >= m_pParam->Time )
	{
		Delete();
	}

	//	画面外チェック
	Rect FieldRect = m_pParent->GetWorld()->GetFieldRect();
	FieldRect.Widen( OUTCHECK_WIDEN );
	if( !FieldRect.IsInside( m_Matrix.GetPos() ) )
	{
		//	破壊する
		Crash();
	}
}

//******************************************************************************
//	攻撃がヒット
//******************************************************************************
bool Spear::OnHitAttack( float Damage, Point HitPos )
{
	_Exhaust( Damage, HitPos, false );

	return true;
}

//******************************************************************************
//	破壊する
//******************************************************************************
void Spear::Crash( void )
{
	if( m_bDelete ) return;

	//	破損エフェクト
	m_pParent->CreateEffect( m_pParam->CrashEffectName, m_Matrix );

	//	アイテム発生
	m_pParent->GetWorld()->GetItemManager()->AppearItem( m_pParam->CrashItemNum, m_Matrix.GetPos(), m_pParam->ItemSpeed );

	Delete();
}

//------------------------------------------------------------------------------
//	当たり判定にヒット
//------------------------------------------------------------------------------
bool Spear::_OnCastHit( const COLLIDE_RESULT& Result )
{
	switch( Result.pOwner->Type )
	{
	case COLLIDE_OWNER_ENEMY:
		{
			Enemy* pEnemy = (Enemy*)(Result.pOwner->pData);
			float Power		= GetAttackPower();
			float Damage	= pEnemy->Damage( Power, m_Matrix.GetAxisY() * m_pParam->AttackVel, m_pParam->Knockback, false, true );
			if( Damage > 0.0f )
			{
				_Exhaust( Damage, Result.HitPos, true );
				
				//	アイテム発生
				int ItemNum = (int)ceilf( m_pParam->HitItemNum * (Damage / Power) );
				m_pParent->GetWorld()->GetItemManager()->AppearItem( ItemNum, Result.HitPos, m_pParam->ItemSpeed );
			}
		}
		break;
	case COLLIDE_OWNER_GROUND:
		{
			Ground* pGround = (Ground*)(Result.pOwner->pData);

			//	ヒットエフェクト発生
			pGround->CreateHitEffect( Result.GetHitMatrix() );
		}
		break;
	}

	return !IsDelete();
}

//------------------------------------------------------------------------------
//	エネルギーの消耗
//------------------------------------------------------------------------------
void Spear::_Exhaust( float Damage, Point HitPos, bool bHitEffect )
{
	DecreaseZero( m_Energy, Damage * m_pParam->EnergyDecayRate );
	DecreaseZero( m_Attack, Damage * m_pParam->AttackDecayRate );

	//	スコア通知
	if( bHitEffect )
	{
		m_pParent->GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_SPEAR );
	}

	if( m_Energy <= 0.0f )
	{
		//	破損
		Crash();
	}
	else
	{
		//	ヒットエフェクト
		if( bHitEffect )
		{
			m_pParent->CreateEffect( m_pParam->HitEffectName, _GetHitMatrix(HitPos) );
		}
	}
}

//------------------------------------------------------------------------------
//	マトリクスの更新
//------------------------------------------------------------------------------
void Spear::_UpdateMatrix( void )
{
	if( m_pEffect ) m_pEffect->SetMatrix( m_Matrix );

	m_pCollide->UpdateMatrix( &m_Matrix );
}

//------------------------------------------------------------------------------
//	攻撃ヒット用のマトリクスを得る
//------------------------------------------------------------------------------
Matrix3 Spear::_GetHitMatrix( Point HitPos )
{
	Matrix3 Mat = m_Matrix;
	Mat.SetPos( HitPos );

	return Mat;
}
