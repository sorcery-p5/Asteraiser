#include "stdafx.h"
#include "Overdrive.h"
#include "Spear.h"
#include "Ruler.h"

#include "Player/Player.h"
#include "Player/PlayerData.h"

#include "World/World.h"
#include "Config/Config.h"

#include "Enemy/Enemy.h"
#include "Enemy/EnemyManager.h"
#include "Indicate/Indicate.h"
#include "Indicate/IndicateManager.h"

#include "SpriteFrame/SpriteFrame.h"
#include "Effect/Effect.h"
#include "Effect/EffectManager.h"

namespace
{
	const float		ENERGY_MAX			= 300.0f;
	const float		ENERGY_USE_SPEAR	= 100.0f;
	const float		ENERGY_USE_RULER	= 200.0f;
	const float		ENERGY_SECTOR		= 100.0f;
	const float		ENERGY_DEFAULT		= 100.0f;

	const Label		NOTIFY_KEY_EXEC			= "Exec";
	const Label		NOTIFY_KEY_INVINCEBLE	= "Invincible";
	const Label		NOTIFY_VAL_TRUE			= "true";
	const Label		NOTIFY_VAL_FALSE		= "false";

	const Label		BONE_SPEAR				= "Blade";

	const Label		INDICATE_OVERDRIVE		= "Overdrive";
	const Label		INDICATE_RULER			= "RingRuler";
	const Label		INDICATE_BONE_GAUGE		= "Gauge";
	const Label		INDICATE_BONE_NUM		= "Num";

	const Color		INDICATE_NUM_COLOR[]	=
	{
		Color( 255,255,255 ),
		Color( 255,192,192 ),
		Color( 255,128,128 ),
		Color( 255,64,64 ),
	};

}


///////////////////////////////////////////////////////////////////////////////
//
//	オーヴァドライヴ
//
///////////////////////////////////////////////////////////////////////////////
Overdrive::Overdrive( void )
{
	m_pParent	= NULL;
	m_pParam	= NULL;

	m_Type			= TYPE_SPEAR;
	m_State			= STATE_NONE;
	m_ReadyEnergy	= 0.0f;
	m_bInvincible	= false;
	m_Delay			= 0;
}
Overdrive::~Overdrive()
{
	EffectEndAppear( m_pIndicateEffect );
	EffectEndAppear( m_pSpearLockEffect );
	SafeDelete( m_pEnergy );
	DeleteVec( m_SpearList );
	DeleteVec( m_RulerList );
}

//******************************************************************************
//	初期化
//******************************************************************************
void Overdrive::Init( Player* pParent, const OVERDRIVE_PARAM* pParam )
{
	_ASSERT( pParent && pParam );
	m_pParent	= pParent;
	m_pParam	= pParam;

	m_Type			= TYPE_SPEAR;
	m_State			= STATE_NONE;
	m_bInvincible	= false;
	m_Delay			= 0;

	m_pEnergy	= new OverdriveEnergy( pParam );
	MEM_CHECK( m_pEnergy );
}

//******************************************************************************
//	リセット
//******************************************************************************
void Overdrive::Reset( void )
{
	m_Type			= TYPE_SPEAR;
	m_State			= STATE_NONE;
	m_ReadyEnergy	= 0.0f;
	m_bInvincible	= false;
	m_Delay			= 0;

	m_pEnergy->Reset();
}

//******************************************************************************
//	更新
//******************************************************************************
void Overdrive::Update( void )
{
	//	エネルギー
	m_pEnergy->Update();

	//	武装
	_UpdateSpear();
	_UpdateRuler();

	switch( m_State )
	{
	case STATE_NONE:	DecreaseZero( m_Delay, 1 );	break;
	case STATE_READY:	_UpdateReady();				break;
	case STATE_AFTER:	_UpdateAfter();				break;
	}

	//	インジケートの更新
	_UpdateIndicate();
}

//******************************************************************************
//	スピア発動
//******************************************************************************
void Overdrive::DriveSpear( void )
{
	m_Type			= TYPE_SPEAR;
	m_State			= STATE_READY;
	m_ReadyEnergy	= 0.0f;

	m_pEnergy->Drive( ENERGY_USE_SPEAR );

	//	インジケートエフェクト更新
	EffectEndAppear( m_pIndicateEffect );

	//	ロックエフェクト生成
	Enemy* pEnemy = m_pParent->GetWorld()->GetEnemyManager()->GetSpearTarget( m_pParent->GetPos() );
	if( pEnemy )
	{
		m_pSpearLockEffect = m_pParent->CreateEffect( m_pParam->Spear.LockEffectName, Matrix3::FromTo( pEnemy->GetPos(), m_pParent->GetPos() ) );
	}
}

//******************************************************************************
//	ルーラー発動
//******************************************************************************
void Overdrive::DriveRuler( void )
{
	m_Type			= TYPE_RULER;
	m_State			= STATE_READY;
	m_ReadyEnergy	= 0.0f;
	
	m_pEnergy->Drive( ENERGY_USE_RULER );
	
	//	インジケートエフェクト更新
	EffectEndAppear( m_pIndicateEffect );
	
	//	リングインジケート発生
	m_pParent->GetWorld()->GetIndicateManager()->AddIndicate( INDICATE_RULER, m_pParent->GetData()->GetIndicateData( INDICATE_RULER ) );
	
	Indicate* pRingIndicate	= m_pParent->GetWorld()->GetIndicateManager()->GetIndicate( INDICATE_RULER );
	if( pRingIndicate )
	{
		pRingIndicate->SetMatrix( m_pParent->GetMatrix() );
	}
}

//******************************************************************************
//	状態をキャンセル
//******************************************************************************
void Overdrive::CancelState( void )
{
	//	ロックエフェクト消去
	EffectEndAppear( m_pSpearLockEffect );

	//	インジケート消す
	if( m_State == STATE_READY )
	{
		Indicate* pRingIndicate	= m_pParent->GetWorld()->GetIndicateManager()->GetIndicate( INDICATE_RULER );
		if( pRingIndicate )
		{
			pRingIndicate->Close( true );
		}
	}

	m_State			= STATE_NONE;
	m_ReadyEnergy	= 0.0f;
	m_bInvincible	= false;
}

//******************************************************************************
//	スピア／ルーラーを削除
//******************************************************************************
void Overdrive::AllErase( void )
{
	for( SpearList::iterator it = m_SpearList.begin(); it != m_SpearList.end(); ++it )
	{
		(*it)->Crash();
	}
	
	for( RulerList::iterator it = m_RulerList.begin(); it != m_RulerList.end(); ++it )
	{
		(*it)->Crash();
	}
}

//******************************************************************************
//	自然回復
//******************************************************************************
float Overdrive::Regenerate( float Energy )
{
	if( IsActive() ) return Energy;

	int Sector = m_pEnergy->GetEnergySectorNum();

	float Ret = m_pEnergy->Regenerate( Energy );

	//	回復した
	if( Sector < m_pEnergy->GetEnergySectorNum() )
	{
		//	インジケートエフェクト更新
		EffectEndAppear( m_pIndicateEffect );

		//	回復エフェクト生成
		if( m_pParent->IsIndicateEnable() )
		{
			m_pParent->CreateEffect( m_pEnergy->IsEnergyFull()? m_pParam->MaxEffectName : m_pParam->ChargeEffectName, m_pParent->GetMatrix() );
		}
	}

	return Ret;
}

//******************************************************************************
//	アイテム取得
//******************************************************************************
float Overdrive::OnGetItem( float Energy )
{
	//	準備中はエネルギーを貯蔵
	if( IsReady() )
	{
		m_ReadyEnergy += Energy;
		return 0.0f;
	}

	int Sector = m_pEnergy->GetEnergySectorNum();

	float Ret = m_pEnergy->OnGetItem( Energy );

	//	回復した
	if( Sector < m_pEnergy->GetEnergySectorNum() )
	{
		//	インジケートエフェクト更新
		EffectEndAppear( m_pIndicateEffect );

		//	回復エフェクト生成
		if( m_pParent->IsIndicateEnable() )
		{
			m_pParent->CreateEffect( m_pEnergy->IsEnergyFull()? m_pParam->MaxEffectName : m_pParam->ChargeEffectName, m_pParent->GetMatrix() );
		}
	}

	return Ret;
}

//******************************************************************************
//	スピア発動可能か？
//******************************************************************************
bool Overdrive::IsSpearEnable( void ) const
{
	return	m_State == STATE_NONE &&
			m_Delay <= 0 &&
			m_pEnergy->IsDriveEnable( ENERGY_USE_SPEAR );
}

//******************************************************************************
//	ルーラー発動可能か？
//******************************************************************************
bool Overdrive::IsRulerEnable( void ) const
{
	return	m_State == STATE_NONE &&
			m_Delay <= 0 &&
			m_pEnergy->IsDriveEnable( ENERGY_USE_RULER ) && m_RulerList.empty();
}

//------------------------------------------------------------------------------
//	準備中
//------------------------------------------------------------------------------
void Overdrive::_UpdateReady( void )
{
	//	無敵状態
	_UpdateInvincible();

	//	ロックエフェクト
	if( m_Type == TYPE_SPEAR && m_pSpearLockEffect )
	{
		Enemy* pEnemy = m_pParent->GetWorld()->GetEnemyManager()->GetSpearTarget( m_pParent->GetPos() );
		if( pEnemy )
		{
			m_pSpearLockEffect->SetMatrix( Matrix3::FromTo( pEnemy->GetPos(), m_pParent->GetPos() ) );
		}
		else
		{
			EffectEndAppear( m_pSpearLockEffect );
		}
	}

	if( m_pParent->GetSprite()->GetAnimeNotify( NOTIFY_KEY_EXEC ) )
	{
		//	発動
		if( m_Type == TYPE_SPEAR )	_ExecSpear();
		if( m_Type == TYPE_RULER )	_ExecRuler();
		
		m_ReadyEnergy = 0.0f;
		m_State = STATE_AFTER;
	}
}

//------------------------------------------------------------------------------
//	発動後
//------------------------------------------------------------------------------
void Overdrive::_UpdateAfter( void )
{
	//	無敵状態
	_UpdateInvincible();
}

//------------------------------------------------------------------------------
//	無敵状態の更新
//------------------------------------------------------------------------------
void Overdrive::_UpdateInvincible( void )
{
	Label InvNotify( m_pParent->GetSprite()->GetAnimeNotify( NOTIFY_KEY_INVINCEBLE ) );

	//	無敵発動
	if( InvNotify == NOTIFY_VAL_TRUE )
	{
		m_bInvincible = true;
	}
	//	無敵解除
	else if( InvNotify == NOTIFY_VAL_FALSE )
	{
		m_bInvincible = false;
	}
}

//------------------------------------------------------------------------------
//	インジケートの更新
//------------------------------------------------------------------------------
void Overdrive::_UpdateIndicate( void )
{
	//	インジケート
	Indicate* pIndicate	= m_pParent->GetWorld()->GetIndicateManager()->GetIndicate( INDICATE_OVERDRIVE );

	if( pIndicate )
	{
		pIndicate->SetGauge( INDICATE_BONE_GAUGE,		m_pEnergy->GetEnergySectorRate(),		1.0f );
		pIndicate->SetText( INDICATE_BONE_NUM,			"%1d", m_pEnergy->GetEnergySectorNum() );
		pIndicate->SetColor( INDICATE_BONE_NUM,			INDICATE_NUM_COLOR[ m_pEnergy->GetEnergySectorNum() ] );
	}

	//	エフェクト
	if( m_pParent->IsIndicateEnable() && m_Delay <= 0 )
	{
		if( !m_pIndicateEffect && m_pEnergy->GetEnergySectorNum() > 0 )
		{
			m_pIndicateEffect = m_pParent->CreateEffect( m_pParam->EffectName[ m_pEnergy->GetEnergySectorNum()-1 ], m_pParent->GetMatrix() );
		}

		if( m_pIndicateEffect ) m_pIndicateEffect->SetMatrix( m_pParent->GetMatrix() );
	}
	else
	{
		EffectEndAppear( m_pIndicateEffect );
	}
}

//------------------------------------------------------------------------------
//	スピア発動
//------------------------------------------------------------------------------
void Overdrive::_ExecSpear( void )
{
	Spear* pSpear = new Spear( m_pParent );
	MEM_CHECK( pSpear );

	//	発射位置を取得
	Point Pos = m_pParent->GetPos();
	const Matrix3* pBoneMat = m_pParent->GetSprite()->GetBoneMatrix( BONE_SPEAR );
	if( pBoneMat ) Pos = pBoneMat->GetPos();

	//	ターゲットを取得
	Point Target = Pos + Point( 0.0f, -1.0f );
	Enemy* pEnemy = m_pParent->GetWorld()->GetEnemyManager()->GetSpearTarget( m_pParent->GetPos() );
	if( pEnemy ) Target = pEnemy->GetPos();

	//	マトリクス設定
	Matrix3 Mat = Matrix3::TransRotZ( Pos, (Target - Pos).GetAngle() );
	pSpear->Init( &m_pParam->Spear, Mat, m_ReadyEnergy );

	m_SpearList.push_back( pSpear );

	//	ディレイ
	m_Delay = m_pParam->Delay;
}

//------------------------------------------------------------------------------
//	ルーラー発動
//------------------------------------------------------------------------------
void Overdrive::_ExecRuler( void )
{
	Ruler* pRuler = new Ruler( m_pParent );
	MEM_CHECK( pRuler );

	//	ターゲットを取得
	pRuler->Init( &m_pParam->Ruler, m_pParent->GetMatrix(), m_ReadyEnergy );

	m_RulerList.push_back( pRuler );
	
	//	ディレイ
	m_Delay = m_pParam->Delay;
}

//------------------------------------------------------------------------------
//	スピアの更新
//------------------------------------------------------------------------------
void Overdrive::_UpdateSpear( void )
{
	for( SpearList::iterator it = m_SpearList.begin(); it != m_SpearList.end(); )
	{
		(*it)->Update();
		
		if( (*it)->IsDelete() )
		{
			SafeDelete( *it );
			it = m_SpearList.erase( it );
		}
		else
		{
			++it;
		}
	}
}
	
//------------------------------------------------------------------------------
//	ルーラーの更新
//------------------------------------------------------------------------------
void Overdrive::_UpdateRuler( void )
{
	for( RulerList::iterator it = m_RulerList.begin(); it != m_RulerList.end(); )
	{
		(*it)->Update();
		
		if( (*it)->IsDelete() )
		{
			SafeDelete( *it );
			it = m_RulerList.erase( it );
		}
		else
		{
			++it;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
//
//	オーヴァドライヴエネルギー
//
////////////////////////////////////////////////////////////////////////////////
OverdriveEnergy::OverdriveEnergy( const OVERDRIVE_PARAM* pParam )
{
	m_pParam	= pParam;
	m_Energy	= ENERGY_DEFAULT;
}

//******************************************************************************
//	リセット
//******************************************************************************
void OverdriveEnergy::Reset( void )
{
	m_Energy = ENERGY_DEFAULT;
}

//******************************************************************************
//	更新
//******************************************************************************
void OverdriveEnergy::Update( void )
{
}

//******************************************************************************
//	発動
//******************************************************************************
void OverdriveEnergy::Drive( float Use )
{
	if( GetConfig()->GetDebugInfo().bOverdriveInfinite ) return;

	//	消費
	DecreaseZero( m_Energy, Use );
}

//******************************************************************************
//	自然回復
//******************************************************************************
float OverdriveEnergy::Regenerate( float Energy )
{
	if( m_Energy < ENERGY_MAX )
	{
		Increase( m_Energy, ENERGY_MAX, Energy * m_pParam->NormalRegen );
		DecreaseZero( Energy, m_pParam->UseRegenEnergy );
	}

	return Energy;
}

//******************************************************************************
//	アイテム取得
//******************************************************************************
float OverdriveEnergy::OnGetItem( float Energy )
{
	if( m_Energy < ENERGY_MAX )
	{
		Increase( m_Energy, ENERGY_MAX, Energy * m_pParam->ItemRegen );
		DecreaseZero( Energy, m_pParam->UseRegenEnergy );
	}

	return Energy;
}

//******************************************************************************
//	エネルギー残量割合を得る
//******************************************************************************
float OverdriveEnergy::GetEnergyRate( void ) const
{
	return m_Energy / ENERGY_MAX;
}

//******************************************************************************
//	エネルギー区分割合を得る
//******************************************************************************
float OverdriveEnergy::GetEnergySectorRate( void ) const
{
	if( m_Energy >= ENERGY_MAX ) return 1.0f;

	return fmodf( m_Energy, ENERGY_SECTOR ) / ENERGY_SECTOR;
}

//******************************************************************************
//	エネルギー区分の数を得る
//******************************************************************************
int OverdriveEnergy::GetEnergySectorNum( void ) const
{
	return (int)floorf( m_Energy / ENERGY_SECTOR );
}

//******************************************************************************
//	残量が発動可能か
//******************************************************************************
bool OverdriveEnergy::IsDriveEnable( float Use ) const
{
	if( GetConfig()->GetDebugInfo().bOverdriveInfinite ) return true;

	return m_Energy >= Use;
}

//******************************************************************************
//	エネルギーが満タンか
//******************************************************************************
bool OverdriveEnergy::IsEnergyFull( void ) const
{
	return m_Energy >= ENERGY_MAX;
}