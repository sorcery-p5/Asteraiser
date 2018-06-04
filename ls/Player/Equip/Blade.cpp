#include "stdafx.h"
#include "Blade.h"

#include "Player/PlayerTypes.h"
#include "Player/Player.h"
#include "Player/PlayerData.h"

#include "World/World.h"
#include "Config/Config.h"

#include "SpriteFrame/SpriteFrame.h"
#include "Collide/CollideSystem.h"
#include "Effect/EffectManager.h"
#include "Effect/Effect.h"

#include "Enemy/Enemy.h"
#include "Ground/Ground.h"
#include "Attack/Attack.h"
#include "Item/ItemManager.h"
#include "Score/ScoreManager.h"
#include "Indicate/Indicate.h"
#include "Indicate/IndicateManager.h"

namespace
{
	const float	ENERGY_MAX			= 100.0f;
	const float	ENERGY_REDZONE		= 25.0f;

	const int	DISABLE_SLASH_TIME	= 7;

	const Label	BONE_NAME_BLADE				= "Blade";
	
	const Label	INDICATE_BLADE				= "Blade";
	const Label	INDICATE_RING_BLADE			= "RingBlade";
	const Label	INDICATE_BONE_BLUE			= "Blue";
	const Label	INDICATE_BONE_CANCEL		= "Cancel";
	const Label	INDICATE_BONE_RED			= "Red";
	const Label	INDICATE_BONE_BASE			= "Base";
	const Label	INDICATE_BONE_BASE_LIMIT	= "BaseLimit";
	const Label	INDICATE_BONE_LIMIT			= "Limit";
	const Label	INDICATE_BONE_DELAY			= "Delay";
	const Label	INDICATE_BONE_MARKER		= "Marker";

	const Label	STATE_OVERDRIVE				= "Overdrive";

	const Label	NOTIFY_END					= "End";
	const Label	NOTIFY_KEY_BLADE			= "Blade";
	const Label	NOTIFY_VISIBLE				= "Visible";
	const Label	NOTIFY_INVISIBLE			= "Invisible";

	const Color COLOR_EFFECT_CANCEL			= Color(32,32,255);
	const Color COLOR_EFFECT_DELAY			= Color::Gray(64);
	const Color COLOR_EFFECT_DISABLE		= Color::Gray(32);
	const Color COLOR_EFFECT_ENERGY_MIN		= Color(0,140,255);
	const Color COLOR_EFFECT_ENERGY_MAX		= Color::White();

	const Color COLOR_INDICATE_CANCEL		= Color(32,64,255);
	const Color COLOR_INDICATE_DELAY		= Color(255,0,0);
	const Color COLOR_INDICATE_CHARGE		= Color(128,192,255);
	const Color COLOR_INDICATE_BASE			= Color(32,32,32);
	const Color COLOR_INDICATE_CRASH		= Color(255,0,0);
	const Color COLOR_INDICATE_ENERGY_MIN	= Color(0,140,255);
	const Color COLOR_INDICATE_ENERGY_MAX	= Color(255,255,255);

	const float INDICATE_DELAY_SCALE		= 0.5f;
	
}

///////////////////////////////////////////////////////////////////////////////
//
//	ブレード
//
///////////////////////////////////////////////////////////////////////////////
Blade::Blade( void )
{
	m_pParent			= NULL;
	m_pParam			= NULL;

	m_State				= STATE_NORMAL;
	m_bVisible			= true;
	m_bRepulse			= false;
	m_RepulseCount		= 0;
	m_bAttackRepulse	= false;
	m_Delay				= 0;
	m_CancelCount		= 0;
	m_NotExhaustCount	= 0;
	m_NotReloadCount	= 0;
	m_DisableSlashCount	= 0;
	m_ExhaustMax		= 0;
	m_bHit				= false;
	m_bSmash			= false;
	m_bRuler			= false;
	m_bEndNotify		= false;

	m_pCollide			= NULL;
	m_pCaster			= NULL;
	m_pEnergy			= NULL;
}
Blade::~Blade()
{
	EffectEndAppear( m_pEffect );

	SafeDelete( m_pCollide );
	SafeDelete( m_pCaster );
	SafeDelete( m_pEnergy );
}

//******************************************************************************
//	初期化
//******************************************************************************
void Blade::Init( Player* pParent, const BLADE_PARAM* pParam )
{
	_ASSERT( pParent && pParam && pParent->GetSprite() );
	m_pParent	= pParent;
	m_pParam	= pParam;
	
	m_State				= STATE_NORMAL;
	m_StateCount		= 0;
	m_bRepulse			= false;
	m_RepulseCount		= 0;
	m_bAttackRepulse	= false;
	m_Delay				= 0;
	m_CancelCount		= 0;
	m_NotExhaustCount	= 0;
	m_NotReloadCount	= 0;
	m_DisableSlashCount	= 0;
	m_ExhaustMax		= 0;
	m_bHit				= false;
	m_bSmash			= false;
	m_bRuler			= false;
	m_bEndNotify		= false;

	//	マトリクス
	const Matrix3* pMat	= m_pParent->GetSprite()->GetBoneMatrix(BONE_NAME_BLADE);
	_ASSERT( pMat );
	m_Matrix		= *pMat;

	//	エネルギー
	m_pEnergy		= new BladeEnergy( this, &pParam->EnergyParam );
	MEM_CHECK( m_pEnergy  );

	//	当たり判定
	m_pCollide		= new CollideObject;
	m_pCaster		= new CollideCaster;
	MEM_CHECK( m_pCollide && m_pCaster );

	m_pCollide->Init( new CollideShapeLine( pParam->Length, pParam->Radius ), COLLIDE_LAYER_BLADE );
	m_pCollide->SetHitEnable( false );
	m_pCollide->SetOwner( COLLIDE_OWNER_BLADE, this );
	m_pCollide->UpdateMatrix( &m_Matrix );

	m_pCaster->Init( new CollideShapeLine( pParam->Length, pParam->Radius ) );
	m_pCaster->SetCalcType( COLLIDE_CALC_POS );
	m_pCaster->SetFilter( COLLIDE_FILTER_BLADE );

	//	インジケート
	m_pParent->GetWorld()->GetIndicateManager()->AddIndicate( INDICATE_RING_BLADE, m_pParent->GetData()->GetIndicateData( INDICATE_RING_BLADE ) );
}

//******************************************************************************
//	リセット
//******************************************************************************
void Blade::Reset( void )
{
	m_State				= STATE_NORMAL;
	m_StateCount		= 0;
	m_bRepulse			= false;
	m_RepulseCount		= 0;
	m_bAttackRepulse	= false;
	m_Delay				= 0;
	m_CancelCount		= 0;
	m_NotExhaustCount	= 0;
	m_NotReloadCount	= 0;
	m_DisableSlashCount	= 0;
	m_ExhaustMax		= 0;
	m_bHit				= false;
	m_bSmash			= false;
	m_bRuler			= false;
	m_bEndNotify		= false;
	
	//	当たりを戻す
	m_pCollide->SetHitEnable( false );
	_UpdateHitSize();

	m_pEnergy->Reset();
}

//******************************************************************************
//	更新
//******************************************************************************
void Blade::Update( void )
{
	//	ルーラー状態
	_UpdateRuler();

	//	エネルギー
	m_pEnergy->Update();

	//	マトリクス更新
	_UpdateMatrix( m_pParent->GetSprite()->GetBoneMatrix(BONE_NAME_BLADE) );

	switch( m_State )
	{
	case STATE_NORMAL:	_UpdateNormal();	break;
	case STATE_CHARGE:	_UpdateCharge();	break;
	case STATE_SLASH:	_UpdateSlash();		break;
	}

	//	エフェクトの表示
	_UpdateEffect();

	//	インジケートの更新
	_UpdateIndicate();

	//	カウント
	DecreaseZero( m_DisableSlashCount, 1 );
	m_StateCount++;
}

//******************************************************************************
//	ブレードタメ
//******************************************************************************
void Blade::Charge( Point Dir )
{
	//	カウントを設定
	m_CancelCount	= 0;
	m_bHit			= false;
	m_bSmash		= false;
	m_bEndNotify	= false;

	//	ノックバック方向登録
	m_KnockbackDir	= Dir;

	m_State = STATE_CHARGE;
	m_StateCount = 0;
}

//******************************************************************************
//	ブレード振る
//******************************************************************************
void Blade::Slash( bool bSmash )
{
	//	カウントを設定
	m_Delay			= m_bRuler? m_pParam->RulerDelay : m_pParam->NormalDelay;
	m_CancelCount	= 0;
	m_bHit			= false;
	m_bSmash		= bSmash;
	m_bEndNotify	= false;

	//	リロード不可時間を設定
	m_NotReloadCount = m_pParam->NotReloadTime;

	//	当たり判定生成
	m_pCollide->SetHitEnable( true );

	//	当たりサイズ設定
	_UpdateHitSize();

	m_State = STATE_SLASH;
	m_StateCount = 0;
}

//******************************************************************************
//	リロードを行う
//******************************************************************************
void Blade::Reload( void )
{	
	//	アイテム発生
	int ItemNum = (int)( m_pParam->ReloadItemNum * ( 1.0f - m_pEnergy->GetLimitRate() ) );
	m_pParent->GetWorld()->GetItemManager()->AppearItem( ItemNum, m_pParent->GetPos(), m_pParam->ItemSpeed );

	m_pEnergy->Reload();
}

//******************************************************************************
//	キャンセルリロードを行う
//******************************************************************************
void Blade::CancelReload( void )
{
	//	ディレイ消す
	m_Delay			= 0;
	m_CancelCount	= 0;

	//	リロード不可時間を設定
	m_NotReloadCount = m_pParam->NotReloadTime;

	//	適用
	m_pEnergy->CancelReload();

	//	スコア通知
	m_pParent->GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_CANCEL );
}

//******************************************************************************
//	状態を戻す
//******************************************************************************
void Blade::CancelState( void )
{
	m_bRepulse		= false;
	m_RepulseCount	= 0;
	m_bHit			= false;
	m_bSmash		= false;
	m_bEndNotify	= false;
	m_State			= STATE_NORMAL;
	m_StateCount	= 0;
	
	//	当たりを戻す
	m_pCollide->SetHitEnable( false );
	_UpdateHitSize();
}

//******************************************************************************
//	ディレイを付与する
//******************************************************************************
void Blade::Delay( void )
{
	m_Delay = m_bRuler? m_pParam->RulerDelay : m_pParam->NormalDelay;
}

//******************************************************************************
//	敵に当たった
//******************************************************************************
bool Blade::OnHitEnemy( Enemy* pEnemy, float Power, Point Vel, int Knockback, bool bRepulse, bool bAntiHide, const Matrix3& HitMat, Label HitEffect )
{
	bool bCombo = pEnemy->IsKnockback();

	//	ダメージを与える
	float Damage = pEnemy->Damage( Power, Vel, Knockback, bRepulse, bAntiHide );
	if( Damage >= 0.0f )
	{
		//	消耗
		_Exhaust( Damage, bRepulse, HitMat.GetPos() );
		
		if( !m_bRepulse )
		{
			//	ヒットエフェクト
			Effect* pEffect = m_pParent->CreateEffect( HitEffect, Matrix3::RotZ( GetRand().GetRandRange(0.0f, Angle::ToRad(15.0f)) ) * HitMat );
			if( pEffect ) pEnemy->AddHitEffect( pEffect );
		
			//	アイテム発生
			int ItemNum = (int)ceilf( m_pParam->HitItemNum * (Damage / Power) );
			m_pParent->GetWorld()->GetItemManager()->AppearItem( ItemNum, HitMat.GetPos(), m_pParam->ItemSpeed );
		}
		else
		{
			if( pEnemy->IsDead() )
			{
				//	反発終了エフェクト
				Effect* pEffect = m_pParent->CreateEffect( m_pParam->RepulseEndEffectName, HitMat );
				if( pEffect ) pEnemy->AddHitEffect( pEffect );

				m_RepulseCount = 0;
			}
			else if( m_RepulseCount % m_pParam->RepulseInterval == 0 )
			{
				//	反発ヒットエフェクト
				Effect* pEffect = m_pParent->CreateEffect( m_pParam->RepulseHitEffectName, HitMat );
				if( pEffect ) pEnemy->AddHitEffect( pEffect );
			}
		}

		//	スコア通知
		if( m_RepulseCount <= 1 )
		{
			m_pParent->GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_HIT, pEnemy->GetScoreRate() );
			if( bCombo ) m_pParent->GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_COMBO );
		}

		m_Delay			= Min( m_Delay, m_pParam->HitDelay );
		m_CancelCount	= m_pParam->CancelTime;
		m_bHit			= true;

		return true;
	}

	return false;
}

//******************************************************************************
//	攻撃に当たった
//******************************************************************************
bool Blade::OnHitAttack( float Exhaust, bool bRepulse, Point HitPos )
{
	bRepulse = bRepulse && !m_pParent->IsRulerHit();

	//	消費
	_Exhaust( Exhaust, bRepulse, HitPos );

	if( m_bRepulse )
	{
		//	反発状態
		m_bAttackRepulse = true;
	}
	else
	{
		//	ヒットエフェクト
		m_pParent->CreateEffect( m_pParam->HitEffectName, _GetHitMatrix(HitPos) );
	}

	m_bHit			= true;
	m_CancelCount	= m_pParam->CancelTime;

	return true;
}

//******************************************************************************
//	アイテムにヒットした
//******************************************************************************
void Blade::OnHitItem( Item* pItem, Point HitPos )
{
	//	取得を通知
	m_pParent->OnGetItem( pItem );
}

//******************************************************************************
//	払い抜け発動
//******************************************************************************
void Blade::OnSplash( void )
{
	//	リロード不可時間を設定
	m_NotReloadCount = m_pParam->NotReloadTime;
}

//******************************************************************************
//	ルーラー終了
//******************************************************************************
void Blade::OnCrashRuler( void )
{
	//	当たりを戻す
	m_pCollide->SetHitEnable( false );
	_UpdateHitSize();

	//	破損させる
	m_pEnergy->Crash();

	//	破損エフェクト
	m_pParent->CreateEffect( m_pParam->CrashEffectName, m_Matrix );
}

//******************************************************************************
//	振れないときに振ろうとした
//******************************************************************************
void Blade::OnDisableSlash( void )
{
	m_DisableSlashCount = DISABLE_SLASH_TIME;
}

//******************************************************************************
//	自然回復
//******************************************************************************
float Blade::Regenerate( float Energy )
{
	return m_pEnergy->Regenerate( Energy );
}

//******************************************************************************
//	アイテム取得
//******************************************************************************
float Blade::OnGetItem( float Energy )
{
	return m_pEnergy->OnGetItem( Energy );
}

//******************************************************************************
//	折れた状態か？
//******************************************************************************
bool Blade::IsCrash( void ) const
{
	return m_pEnergy->IsCrash();
}

//******************************************************************************
//	ブレード振ることが可能か
//******************************************************************************
bool Blade::IsSlashEnable( void ) const
{
	//	ルーラー中
	if( m_bRuler )
	{
		//	通常状態またはキャンセル状態
		bool bEnable = false;
		if( m_State == STATE_NORMAL ) bEnable = true;
		if( m_State == STATE_SLASH && m_bEndNotify ) bEnable = true;

		if( !bEnable )
		{
			return false;
		}
	}
	else
	{
		//	通常状態のみ
		if( m_State != STATE_NORMAL ) return false;
	}

	//	エネルギー不足
	if( !m_pEnergy->IsSlashEnable() ) return false;
	
	//	ディレイ中
	if( m_Delay > 0 ) return false;

	return true;
}

//******************************************************************************
//	リロード可能か
//******************************************************************************
bool Blade::IsReloadEnable( void ) const
{
	if( m_State != STATE_NORMAL ) return false;
	
	if( !m_pEnergy->IsNormalState() ) return false;

	if( m_pEnergy->IsEnergyFull() ) return false;

	if( m_NotReloadCount > 0 ) return false;

	return true;
}

//******************************************************************************
//	キャンセルリロード可能か
//******************************************************************************
bool Blade::IsCancelEnable( void ) const
{
	if( m_State != STATE_NORMAL ) return false;
	
	if( !m_pEnergy->IsNormalState() ) return false;

	return m_CancelCount > 0;
}

//******************************************************************************
//	攻撃力の割合を得る
//******************************************************************************
float Blade::GetAttackRate( void ) const
{
	return	Lerp( m_pParam->AttackEnergyRate, 1.0f, m_pEnergy->GetEnergyRate() ) *
			Lerp( m_pParam->AttackLimitRate, 1.0f, m_pEnergy->GetLimitRate() );
}

//******************************************************************************
//	攻撃力の取得
//******************************************************************************
float Blade::GetAttackPower( void ) const
{
	float Attack =	m_bSmash?	m_pParam->SmashParam.Attack :
					m_bRuler?	m_pParam->RulerAttack :
								m_pParam->Attack;
		
	return Attack * GetAttackRate();
}

//******************************************************************************
//	反発時の攻撃力の取得
//******************************************************************************
float Blade::GetRepulseAttackPower( void ) const
{
	return m_pParam->RepulseAttack * GetAttackRate();
}

//******************************************************************************
//	攻撃の反動力を取得
//******************************************************************************
float Blade::GetAttackVel( void ) const
{
	return m_bSmash? m_pParam->SmashParam.AttackVel : m_pParam->AttackVel;
}

//******************************************************************************
//	攻撃のノックバック時間を取得
//******************************************************************************
int Blade::GetKnockback( void ) const
{
	return m_bSmash? m_pParam->SmashParam.Knockback : m_pParam->Knockback;
}

//******************************************************************************
//	乱数の取得
//******************************************************************************
Rand& Blade::GetRand( void ) const
{
	return m_pParent->GetRand();
}

//------------------------------------------------------------------------------
//	更新：待機
//------------------------------------------------------------------------------
void Blade::_UpdateNormal( void )
{
	//	カウントダウン
	DecreaseZero( m_Delay, 1 );
	DecreaseZero( m_CancelCount, 1 );
	DecreaseZero( m_NotExhaustCount, 1 );
	DecreaseZero( m_NotReloadCount, 1 );

	if( m_NotExhaustCount == 0 ) m_ExhaustMax = 0;

	//	通常状態にもかかわらずエネルギー切れ
	if( m_pEnergy->IsNormalRedzone() )
	{
		m_pEnergy->Crash();
	}
}

//------------------------------------------------------------------------------
//	更新：タメ
//------------------------------------------------------------------------------
void Blade::_UpdateCharge( void )
{
	if( m_pParent->IsRulerHit() ) return;

	//	エネルギー消耗
	if( m_pParent->GetStateCount() >= m_pParam->SmashParam.ChargeStartTime &&
		m_pParent->GetStateCount() < m_pParam->SmashParam.ChargeTime )
	{
		m_pEnergy->ChargeExhaust( m_pParam->SmashParam.ChargeExhaustEnergy, m_pParam->SmashParam.ChargeExhaustLimit );
	}
}

//------------------------------------------------------------------------------
//	更新：ブレード振り中
//------------------------------------------------------------------------------
void Blade::_UpdateSlash( void )
{
	m_bRepulse = false;

	//	当たりキャスト
	COLLIDE_CAST_RESULT Result;
	if( m_pCaster->Cast( Result ) )
	{
		for( COLLIDE_RESULT_LIST::iterator it = Result.ResultList.begin(); it != Result.ResultList.end(); ++it )
		{
			const COLLIDE_RESULT& Ret = *it;

			switch( Ret.pOwner->Type )
			{
			case COLLIDE_OWNER_ENEMY:	_OnHitEnemy( (Enemy*)(Ret.pOwner->pData), Ret.HitPos );		break;
			case COLLIDE_OWNER_ITEM:	OnHitItem( (Item*)(Ret.pOwner->pData), Ret.HitPos );		break;
			case COLLIDE_OWNER_GROUND:	_OnHitGround( (Ground*)(Ret.pOwner->pData), Ret.HitPos );	break;
			}
		}
	}

	//	反発状態
	if( m_bAttackRepulse )
	{
		m_bRepulse = true;
		m_bAttackRepulse = false;
	}

	//	終了通知
	if( m_pParent->GetSprite() )
	{
		if( m_pParent->GetSprite()->GetAnimeNotify( NOTIFY_END ) )
		{
			m_bEndNotify = true;
		}
	}
}

//------------------------------------------------------------------------------
//	マトリクス更新
//------------------------------------------------------------------------------
void Blade::_UpdateMatrix( const Matrix3* pMat )
{
	if( !pMat ) return;

	m_Matrix = *pMat;
	
	//	エフェクト
	if( m_pEffect ) m_pEffect->SetMatrix( m_Matrix );

	//	当たり判定
	m_pCaster->UpdateMatrix( &m_Matrix );
	m_pCollide->UpdateMatrix( &m_Matrix );
}

//------------------------------------------------------------------------------
//	当たり判定のサイズを更新
//------------------------------------------------------------------------------
void Blade::_UpdateHitSize( void )
{
	float Rad = m_pParam->Radius;
	float Len = m_pParam->Length;

	//	スマッシュ
	if( m_bSmash )
	{
		Rad = m_pParam->SmashParam.Radius;
		Len = m_pParam->SmashParam.Length;
	}
	//	ルーラー
	else if( m_bRuler )
	{
		Rad = m_pParam->RulerRadius;
		Len = m_pParam->RulerLength;
	}

	m_pCollide->GetShape()->SetRadius( Rad );
	m_pCollide->GetShape()->SetLength( Len );
	m_pCaster->GetShape()->SetRadius( Rad );
	m_pCaster->GetShape()->SetLength( Len );
}

//------------------------------------------------------------------------------
//	エフェクトの更新
//------------------------------------------------------------------------------
void Blade::_UpdateEffect( void )
{
	if( !m_pEffect )
	{
		if( m_StateCount > 1 )
		{
			m_pEffect = m_pParent->CreateEffect( m_pParam->EffectName, m_Matrix );
		}
		return;
	}

	Color Col = Color::White();

	//	ルーラー中・オーヴァドライヴ中は更新しない
	if( !m_bRuler && !m_pParent->IsState( STATE_OVERDRIVE ) )
	{
		//	キャンセル可能中
		if( IsCancelEnable() )
		{
			Col = COLOR_EFFECT_CANCEL;
		}
		//	エネルギー不足
		else if( m_State == STATE_NORMAL && (!m_pEnergy->IsSlashEnable()) )
		{
			Col = COLOR_EFFECT_DISABLE;
		}
		//	ディレイ中
		else if( m_State == STATE_NORMAL && m_Delay > 0 )
		{
			Col = COLOR_EFFECT_DELAY;
		}
		//	エネルギー量に応じて変化
		else
		{
			Col = Color::Lerp( COLOR_EFFECT_ENERGY_MIN, COLOR_EFFECT_ENERGY_MAX, m_pEnergy->GetLimitRate() );
		}
	}

	m_pEffect->SetColor( Col );

	//	自機のアニメ
	Label VisibleNotify( m_pParent->GetSprite()->GetAnimeNotify( NOTIFY_KEY_BLADE ) );
	if( VisibleNotify == NOTIFY_VISIBLE )
	{
		m_bVisible = true;
	}
	else if( VisibleNotify == NOTIFY_INVISIBLE )
	{
		m_bVisible = false;
	}

	//	非表示
	m_pEffect->SetVisible( !m_pParent->IsDead() && m_bVisible );
}

//------------------------------------------------------------------------------
//	インジケートの更新
//------------------------------------------------------------------------------
void Blade::_UpdateIndicate( void )
{
	//	インジケート
	Indicate* pIndicate	= m_pParent->GetWorld()->GetIndicateManager()->GetIndicate( INDICATE_BLADE );
	if( pIndicate )
	{
		pIndicate->SetGauge( INDICATE_BONE_BLUE,		m_pEnergy->GetNormalEnergyRate(),	1.0f );
		pIndicate->SetGauge( INDICATE_BONE_CANCEL,		m_pEnergy->GetCancelEnergyRate(),	1.0f );
		pIndicate->SetGauge( INDICATE_BONE_RED,			m_pEnergy->GetRedEnergyRate(),		1.0f );
		pIndicate->SetGauge( INDICATE_BONE_BASE_LIMIT,	m_pEnergy->GetLimitRate(),			1.0f );
		pIndicate->SetMarker( INDICATE_BONE_LIMIT,		m_pEnergy->GetLimitRate() );
	}
	
	//	リング
	Indicate* pRingIndicate	= m_pParent->GetWorld()->GetIndicateManager()->GetIndicate( INDICATE_RING_BLADE );
	if( pRingIndicate )
	{
		pRingIndicate->SetMatrix( m_pParent->GetMatrix() );
		pRingIndicate->SetColor( m_pParent->GetBodyAlpha() );

		//	ゲージ
		pRingIndicate->SetAngle( INDICATE_BONE_BLUE,		m_pEnergy->GetNormalEnergyRate() );
		pRingIndicate->SetAngle( INDICATE_BONE_RED,			m_pEnergy->GetRedEnergyRate() );
		pRingIndicate->SetAngle( INDICATE_BONE_BASE_LIMIT,	m_pEnergy->GetLimitRate() );

		//	キャンセル
		pRingIndicate->SetAngle( INDICATE_BONE_CANCEL,		Max( m_pEnergy->GetCancelEnergyRate() - m_pEnergy->GetNormalEnergyRate(), 0.0f ) );
		pRingIndicate->SetSpin( INDICATE_BONE_CANCEL,		m_pEnergy->GetNormalEnergyRate() );

		//	振れない時間
		float Disable = (float)m_DisableSlashCount / (float)DISABLE_SLASH_TIME;
		pRingIndicate->SetColor( INDICATE_BONE_BASE,		Color::Lerp( COLOR_INDICATE_BASE, COLOR_INDICATE_DELAY, Disable ) );

		//	キャンセル
		if( IsCancelEnable() )
		{
			float CancelRate = Lerp( INDICATE_DELAY_SCALE, 1.0f, (float)m_CancelCount / (float)m_pParam->CancelTime );
			pRingIndicate->SetGauge( INDICATE_BONE_DELAY,		CancelRate, CancelRate );
			pRingIndicate->SetColor( INDICATE_BONE_DELAY,		COLOR_INDICATE_CANCEL );
			pRingIndicate->SetBoneVisible( INDICATE_BONE_DELAY,	m_State == STATE_NORMAL );
		}
		//	ディレイ
		else if( m_Delay )
		{
			float DelayRate = Lerp( INDICATE_DELAY_SCALE, 1.0f, (float)m_Delay / (float)m_pParam->NormalDelay );
			pRingIndicate->SetGauge( INDICATE_BONE_DELAY,		DelayRate, DelayRate );
			pRingIndicate->SetColor( INDICATE_BONE_DELAY,		COLOR_INDICATE_DELAY );
			pRingIndicate->SetBoneVisible( INDICATE_BONE_DELAY,	m_State == STATE_NORMAL );
		}
		//	チャージ
		else if( m_State == STATE_CHARGE )
		{
			float t =  Coef( (float)m_pParent->GetStateCount(), (float)m_pParam->SmashParam.ChargeTime, (float)m_pParam->SmashParam.ChargeMaxTime );
			float ChargeRate = Lerp( INDICATE_DELAY_SCALE, 1.0f, t );
			pRingIndicate->SetGauge( INDICATE_BONE_DELAY,		ChargeRate, ChargeRate );
			pRingIndicate->SetColor( INDICATE_BONE_DELAY,		COLOR_INDICATE_CHARGE );
			pRingIndicate->SetBoneVisible( INDICATE_BONE_DELAY,	t > 0.0f );
		}
		else
		{
			pRingIndicate->SetBoneVisible( INDICATE_BONE_DELAY,		false );
		}

		//	耐久
		pRingIndicate->SetSpin( INDICATE_BONE_LIMIT,		m_pEnergy->GetLimitRate() );
		pRingIndicate->SetColor( INDICATE_BONE_MARKER,		m_pEnergy->IsCrash()?	COLOR_INDICATE_CRASH :
																					Color::Lerp( COLOR_INDICATE_ENERGY_MIN, COLOR_INDICATE_ENERGY_MAX, m_pEnergy->GetLimitRate() ) );

		//	開閉
		if( m_pParent->IsIndicateEnable() )
		{
			pRingIndicate->Open();
		}
		else
		{
			pRingIndicate->Close( false );
		}
	}
}

//------------------------------------------------------------------------------
//	ルーラー状態の更新
//------------------------------------------------------------------------------
void Blade::_UpdateRuler( void )
{
	//	ルーラーに入った
	if( !m_bRuler && m_pParent->IsRulerHit() )
	{
		//	専用エフェクト
		EffectEndAppear( m_pEffect );
		m_pEffect = m_pParent->CreateEffect( m_pParam->RulerEffectName, m_Matrix );

		//	エネルギー無制限
		m_pEnergy->Reset();
	}
	//	ルーラーから出た
	else if( m_bRuler && !m_pParent->IsRulerHit() )
	{
		//	通常エフェクト
		EffectEndAppear( m_pEffect );
		m_pEffect = m_pParent->CreateEffect( m_pParam->EffectName, m_Matrix );
	}

	//	フラグ更新
	m_bRuler = m_pParent->IsRulerHit();
}

//------------------------------------------------------------------------------
//	敵に当たった
//------------------------------------------------------------------------------
void Blade::_OnHitEnemy( Enemy* pEnemy, Point HitPos )
{
	bool bRepulse = !IsSmash() && pEnemy->IsRepulse() && !m_pParent->IsRulerHit();

	float Power = bRepulse? GetRepulseAttackPower() : GetAttackPower();
	Point Vel = m_KnockbackDir * GetAttackVel();

	Label EffectName =	IsSmash()?	m_pParam->SmashParam.HitEffectName :
						m_bRuler?	m_pParam->RulerHitEffectName : 
									m_pParam->HitEffectName;

	OnHitEnemy( pEnemy, Power, Vel, GetKnockback(), bRepulse, IsSmash(), _GetHitMatrix(HitPos), EffectName );
}

//------------------------------------------------------------------------------
//	地形にヒットした
//------------------------------------------------------------------------------
void Blade::_OnHitGround( Ground* pGround, Point HitPos )
{
	Matrix3 HitMat = _GetHitMatrix(HitPos);

	//	命中エフェクト
	if( pGround->IsAttackHitEffect() )
	{
		m_pParent->CreateEffect( m_pParam->RepulseEffectName, HitMat );
		m_pParent->CreateEffect( m_pParam->RepulseHitEffectName, HitMat );
	}

	//	地形側のエフェクト
	pGround->CreateHitEffect( HitMat );
}

//------------------------------------------------------------------------------
//	ブレード消耗
//------------------------------------------------------------------------------
void Blade::_Exhaust( float Damage, bool bRepulse, Point HitPos )
{
	bool bPreCrash = m_pEnergy->IsCrash();

	//	減算
	if( !GetConfig()->GetDebugInfo().bBladeInfinite )
	{
		if( !m_pParent->IsRulerHit() )
		{
			float Exhaust = Damage * _GetExhaustRate();

			//	最大消費量に合わせる
			if( !bRepulse )
			{
				float ExhaustMax = m_ExhaustMax;
				m_ExhaustMax = Max( Exhaust, m_ExhaustMax );
				Exhaust = (Exhaust > ExhaustMax)? Exhaust - ExhaustMax : 0.0f;
			}

			//	エネルギー消費
			if( Exhaust > 0.0f )
			{
				m_pEnergy->Exhaust( Exhaust );
			}
	
			if( m_NotExhaustCount == 0 )
			{
				m_NotExhaustCount = m_pParam->NotExhaustTime;
			}
		}
	}

	//	破損
	if( !bPreCrash && m_pEnergy->IsCrash() )
	{
		//	破損エフェクト
		m_pParent->CreateEffect( m_pParam->CrashEffectName, m_Matrix );
		
		//	アイテム発生
		m_pParent->GetWorld()->GetItemManager()->AppearItem( m_pParam->CrashItemNum, m_Matrix.GetPos(), m_pParam->ItemSpeed );

		//	スコア通知
		m_pParent->GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_CRASH );
	}
	//	反発
	else if( bRepulse )
	{
		m_bRepulse = true;

		if( m_RepulseCount % m_pParam->RepulseInterval == 0 )
		{
			//	反発エフェクト
			m_pParent->CreateEffect( m_pParam->RepulseEffectName, _GetHitMatrix(HitPos) );
			
			//	アイテム発生
			m_pParent->GetWorld()->GetItemManager()->AppearItem( m_pParam->RepulseItemNum, HitPos, m_pParam->ItemSpeed );
		}
		m_RepulseCount++;
	}
}

//------------------------------------------------------------------------------
//	消費割合を得る
//------------------------------------------------------------------------------
float Blade::_GetExhaustRate( void ) const
{
	return m_bSmash? m_pParam->SmashParam.ExhaustRate : 1.0f;
}

//------------------------------------------------------------------------------
//	攻撃ヒット用のマトリクスを得る
//------------------------------------------------------------------------------
Matrix3 Blade::_GetHitMatrix( Point HitPos ) const
{
	Matrix3 Mat = m_Matrix;
	Mat.SetPos( HitPos );

	return Mat;
}


////////////////////////////////////////////////////////////////////////////////
//
//	ブレードエネルギー
//
////////////////////////////////////////////////////////////////////////////////
BladeEnergy::BladeEnergy( Blade* pParent, const BLADE_ENERGY_PARAM* pParam )
{
	_ASSERT( pParent && pParam );

	m_pParent			= pParent;
	m_pParam			= pParam;

	m_EnergyLimit		= ENERGY_MAX;
	m_Energy			= ENERGY_MAX;
	m_CancelEnergy		= 0.0f;
	
	m_State				= STATE_NORMAL;
}
BladeEnergy::~BladeEnergy()
{

}

//******************************************************************************
//	リセット
//******************************************************************************
void BladeEnergy::Reset( void )
{
	m_EnergyLimit	= ENERGY_MAX;
	m_Energy		= ENERGY_MAX;
	m_CancelEnergy	= 0.0f;

	m_State			= STATE_NORMAL;
}

//******************************************************************************
//	更新
//******************************************************************************
void BladeEnergy::Update( void )
{
	switch( m_State )
	{
	case STATE_NORMAL:	_UpdateNormal();	break;
	case STATE_RELOAD:	_UpdateReload();	break;
	case STATE_CRASH:	_UpdateCrash();		break;
	}
}

//******************************************************************************
//	リロード
//******************************************************************************
void BladeEnergy::Reload( void )
{
	m_State = STATE_RELOAD;

	m_CancelEnergy	= 0.0f;
}

//******************************************************************************
//	キャンセルリロード
//******************************************************************************
void BladeEnergy::CancelReload( void )
{
	m_CancelEnergy = ENERGY_MAX;
}

//******************************************************************************
//	エネルギー消耗
//******************************************************************************
void BladeEnergy::Exhaust( float Damage )
{
	//	消費
	_ExhaustEnergy( Damage * m_pParam->ExhaustEnergyRate, Damage * m_pParam->ExhaustLimitRate );

	//	破損
	if( m_EnergyLimit <= ENERGY_REDZONE )
	{
		Crash();
	}
}

//******************************************************************************
//	タメ中の消耗
//******************************************************************************
void BladeEnergy::ChargeExhaust( float Energy, float Limit )
{
	//	消費
	_ExhaustEnergy( Energy, Limit );

	//	レッドゾーンには行かない
	if( m_EnergyLimit <= ENERGY_REDZONE )
	{
		m_EnergyLimit = ENERGY_REDZONE;
	}
}

//******************************************************************************
//	自然回復
//******************************************************************************
float BladeEnergy::Regenerate( float Energy )
{
	if( Energy <= 0.0f ) return 0.0f;

	//	通常状態
	if( m_State == STATE_NORMAL )
	{
		//	エネルギーの回復
		if( m_Energy < m_EnergyLimit )
		{
			Increase( m_Energy, m_EnergyLimit, Energy * m_pParam->NormalRegen );
			DecreaseZero( Energy, m_pParam->UseRegenEnergy );
		}

		//	耐久の回復
		if( m_EnergyLimit < ENERGY_MAX )
		{
			Increase( m_EnergyLimit, ENERGY_MAX, Energy * m_pParam->NormalLimitRegen );
			DecreaseZero( Energy, m_pParam->UseLimitEnergy );
		}
	}
	//	リロード・破損
	else
	{
		float Regen = m_State == STATE_RELOAD? m_pParam->ReloadRegen : m_pParam->CrashRegen;

		Increase( m_EnergyLimit, ENERGY_MAX, Energy * Regen );
		m_Energy = m_EnergyLimit;

		DecreaseZero( Energy, m_pParam->UseReloadEnergy );
	}

	return Energy;
}

//******************************************************************************
//	アイテム取得
//******************************************************************************
float BladeEnergy::OnGetItem( float Energy )
{
	if( Energy <= 0.0f ) return 0.0f;

	//	通常状態
	if( m_State == STATE_NORMAL )
	{
		//	エネルギーの回復
		if( m_Energy < m_EnergyLimit )
		{
			Increase( m_Energy, m_EnergyLimit, Energy * m_pParam->ItemRegen );
			DecreaseZero( Energy, m_pParam->UseRegenEnergy );
		}

		//	耐久の回復
		if( m_EnergyLimit < ENERGY_MAX )
		{
			Increase( m_EnergyLimit, ENERGY_MAX, Energy * m_pParam->ItemLimitRegen );
			DecreaseZero( Energy, m_pParam->UseLimitEnergy );
		}
	}
	//	リロード・破損
	else
	{
		Increase( m_EnergyLimit, ENERGY_MAX, Energy * m_pParam->ItemLimitRegen );
		m_Energy = m_EnergyLimit;

		DecreaseZero( Energy, m_pParam->UseReloadEnergy );
	}

	return Energy;
}

//******************************************************************************
//	破損させる
//******************************************************************************
void BladeEnergy::Crash( void )
{
	m_EnergyLimit	= ENERGY_REDZONE;
	m_Energy		= 0.0f;
	m_CancelEnergy	= 0.0f;

	m_State			= STATE_CRASH;
}

//******************************************************************************
//	エネルギー残存割合を得る
//******************************************************************************
float BladeEnergy::GetEnergyRate( void ) const
{
	return Max( m_Energy, m_CancelEnergy ) / ENERGY_MAX;
}

//******************************************************************************
//	エネルギー残存割合を得る（純残量）
//******************************************************************************
float BladeEnergy::GetNormalEnergyRate( void ) const
{
	return m_Energy / ENERGY_MAX;
}

//******************************************************************************
//	レッドゾーン残存割合を得る
//******************************************************************************
float BladeEnergy::GetRedEnergyRate( void ) const
{
	if( m_State == STATE_CRASH ||
		m_State == STATE_RELOAD )
	{
		return m_Energy / ENERGY_MAX;
	}

	return Min( m_Energy, ENERGY_REDZONE ) / ENERGY_MAX;
}

//******************************************************************************
//	キャンセルエネルギーの割合を得る
//******************************************************************************
float BladeEnergy::GetCancelEnergyRate( void ) const
{
	return m_CancelEnergy / ENERGY_MAX;
}

//******************************************************************************
//	エネルギー最大の割合を得る
//******************************************************************************
float BladeEnergy::GetLimitRate( void ) const
{
	return m_EnergyLimit / ENERGY_MAX;
}

//******************************************************************************
//	使用可能か
//******************************************************************************
bool BladeEnergy::IsSlashEnable( void ) const
{
	return IsNormalState() && Max( m_Energy, m_CancelEnergy ) > ENERGY_REDZONE;
}

//******************************************************************************
//	通常状態なのに破損状態
//******************************************************************************
bool BladeEnergy::IsNormalRedzone( void ) const
{
	return IsNormalState() && m_EnergyLimit <= ENERGY_REDZONE;
}

//******************************************************************************
//	エネルギーが満タンか
//******************************************************************************
bool BladeEnergy::IsEnergyLimit( void ) const
{
	return m_Energy >= m_EnergyLimit;
}

//******************************************************************************
//	エネルギーが完全か
//******************************************************************************
bool BladeEnergy::IsEnergyFull( void ) const
{
	return m_Energy >= m_EnergyLimit && m_Energy >= ENERGY_MAX;
}

//------------------------------------------------------------------------------
//	通常状態
//------------------------------------------------------------------------------
void BladeEnergy::_UpdateNormal( void )
{
	//	キャンセル一時エネルギー減衰
	if( !m_pParent->IsCharge() )
	{
		DecreaseZero( m_CancelEnergy, m_pParam->CancelEnergyDecay );
		if( m_Energy >= m_CancelEnergy ) m_CancelEnergy = 0.0f;
	}
}

//------------------------------------------------------------------------------
//	リロード状態
//------------------------------------------------------------------------------
void BladeEnergy::_UpdateReload( void )
{
	//	復旧完了
	if( IsEnergyFull() )
	{
		m_State = STATE_NORMAL;
	}
}

//------------------------------------------------------------------------------
//	破損状態
//------------------------------------------------------------------------------
void BladeEnergy::_UpdateCrash( void )
{
	//	復旧完了
	if( IsEnergyFull() )
	{
		m_State = STATE_NORMAL;
	}
}

//------------------------------------------------------------------------------
//	エネルギーの消耗
//------------------------------------------------------------------------------
void BladeEnergy::_ExhaustEnergy( float Energy, float Limit )
{
	//	キャンセルエネルギー減少
	if( m_CancelEnergy > m_Energy )
	{
		DecreaseZero( m_CancelEnergy, Energy );

		//	キャンセルエネルギー無くなった
		if( m_CancelEnergy <= m_Energy )
		{
			m_Energy		= m_CancelEnergy;
			m_CancelEnergy	= 0.0f;
		}
	}
	//	エネルギー減少
	else
	{
		DecreaseZero( m_Energy, Energy );
	}

	//	耐久の減少
	DecreaseZero( m_EnergyLimit, Limit );
	if( m_Energy > m_EnergyLimit )
	{
		m_Energy = m_EnergyLimit;
	}
}