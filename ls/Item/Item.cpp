#include "stdafx.h"
#include "Item.h"
#include "ItemManager.h"
#include "ItemData.h"

#include "App/App.h"
#include "World/World.h"

#include "Effect/EffectManager.h"
#include "Effect/Effect.h"

#include "Collide/CollideSystem.h"
#include "Collide/CollideObject.h"
#include "Collide/Shape/CollideShapePoint.h"

#include "Player/Player.h"
#include "Player/PlayerManager.h"
#include "Score/ScoreManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	アイテム
//
////////////////////////////////////////////////////////////////////////////////
Item::Item( ItemManager* pManager )
{
	_ASSERT( pManager );

	m_pManager		= pManager;
	m_pData			= NULL;
	m_pCollide		= NULL;

	m_FrameCount	= 0;
	m_bDelete		= false;
	m_bAutoCollect	= false;
	m_Gravity		= 0.0f;
	m_CollectSpeed	= 0.0f;
}
Item::~Item()
{
	EffectDelete( m_pEffect );
	SafeDelete( m_pCollide );
}

//******************************************************************************
//	初期化
//******************************************************************************
void Item::Init( const ItemData* pData, Point Pos, Point Vel )
{
	_ASSERT( pData );

	m_pData	= pData;

	m_Pos		= Pos;
	m_Vel		= Vel;
	
	m_FrameCount	= 0;
	m_bDelete		= false;
	m_bAutoCollect	= false;
	m_Gravity		= 0.0f;

	//	エフェクト
	m_pEffect = GetApp()->GetEffectMng()->CreateEffect( m_pData->GetEffectFile( m_pData->GetParam().EffectName ), Matrix3::Trans(m_Pos) );

	//	当たり判定
	m_pCollide = new CollideObject;
	MEM_CHECK( m_pCollide );
	m_pCollide->Init( new CollideShapePoint( m_pData->GetParam().CollideRadius ), COLLIDE_LAYER_ITEM );
	m_pCollide->SetOwner( COLLIDE_OWNER_ITEM, this );
	m_pCollide->SetHitEnable( false );
}

//******************************************************************************
//	更新
//******************************************************************************
void Item::Update( void )
{
	if( IsDelete() ) return;

	Player* pPlayer = m_pManager->GetWorld()->GetPlayerManager()->GetPlayer();

	//	回収中
	if( m_bAutoCollect )
	{
		//	自機に向かって移動
		if( pPlayer && !pPlayer->IsDead() )
		{
			Increase( m_CollectSpeed, m_pData->GetParam().CollectSpeedMax, m_pData->GetParam().CollectAccel );

			m_Vel = (pPlayer->GetPos() - m_Pos).GetNormal() * m_CollectSpeed;
		}
		
		//	位置更新
		m_Pos += m_Vel;
	}
	else
	{
		//	減速
		m_Vel *= m_pData->GetParam().Decel;

		//	重力
		if( m_FrameCount >= m_pData->GetParam().GravityTime )
		{
			Increase( m_Gravity, m_pData->GetParam().GravityMax, m_pData->GetParam().Gravity );
		}

		//	位置更新
		m_Pos += m_Vel;
		m_Pos.y += m_Gravity;

		//	画面内にクリップ
		Rect FieldRect = m_pManager->GetWorld()->GetFieldRect();
		FieldRect.l -= m_pData->GetParam().FieldOutRadius;
		FieldRect.r += m_pData->GetParam().FieldOutRadius;
		m_Pos.x = Clamp( m_Pos.x, FieldRect.l, FieldRect.r );

		//	回収判定
		if( pPlayer && !pPlayer->IsDead() &&
			m_FrameCount > m_pData->GetParam().CollectTime )
		{
			Point Dist = pPlayer->GetPos() - m_Pos;
			if( Dist.LengthSq() < Sq( m_pManager->GetCollectRadius() ) )
			{
				AutoCollect();
			}
		}
	}

	//	マトリクス更新
	Matrix3 Mat = Matrix3::Trans( m_Pos );
	if( m_pEffect ) m_pEffect->SetMatrix( Mat );
	if( m_pCollide ) m_pCollide->UpdateMatrix( &Mat );

	//	カウント
	m_FrameCount++;

	//	当たり有効化
	if( m_FrameCount == m_pData->GetParam().CollectTime )
	{
		m_pCollide->SetHitEnable( true );
	}

	//	消滅チェック
	if( m_FrameCount >= m_pData->GetParam().Time ||
		m_Pos.y >= m_pManager->GetWorld()->GetFieldRect().b + m_pData->GetParam().DeleteHeight )
	{
		Delete();

		//	スコア通知
		m_pManager->GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_LOST_ITEM );
	}
}

//******************************************************************************
//	取得された通知
//******************************************************************************
void Item::OnGet( void )
{
	Delete();

	m_pManager->OnGet();
}

//******************************************************************************
//	自動回収
//******************************************************************************
void Item::AutoCollect( void )
{
	m_bAutoCollect = true;
}