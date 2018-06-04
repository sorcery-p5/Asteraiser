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
//	�A�C�e��
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
//	������
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

	//	�G�t�F�N�g
	m_pEffect = GetApp()->GetEffectMng()->CreateEffect( m_pData->GetEffectFile( m_pData->GetParam().EffectName ), Matrix3::Trans(m_Pos) );

	//	�����蔻��
	m_pCollide = new CollideObject;
	MEM_CHECK( m_pCollide );
	m_pCollide->Init( new CollideShapePoint( m_pData->GetParam().CollideRadius ), COLLIDE_LAYER_ITEM );
	m_pCollide->SetOwner( COLLIDE_OWNER_ITEM, this );
	m_pCollide->SetHitEnable( false );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void Item::Update( void )
{
	if( IsDelete() ) return;

	Player* pPlayer = m_pManager->GetWorld()->GetPlayerManager()->GetPlayer();

	//	�����
	if( m_bAutoCollect )
	{
		//	���@�Ɍ������Ĉړ�
		if( pPlayer && !pPlayer->IsDead() )
		{
			Increase( m_CollectSpeed, m_pData->GetParam().CollectSpeedMax, m_pData->GetParam().CollectAccel );

			m_Vel = (pPlayer->GetPos() - m_Pos).GetNormal() * m_CollectSpeed;
		}
		
		//	�ʒu�X�V
		m_Pos += m_Vel;
	}
	else
	{
		//	����
		m_Vel *= m_pData->GetParam().Decel;

		//	�d��
		if( m_FrameCount >= m_pData->GetParam().GravityTime )
		{
			Increase( m_Gravity, m_pData->GetParam().GravityMax, m_pData->GetParam().Gravity );
		}

		//	�ʒu�X�V
		m_Pos += m_Vel;
		m_Pos.y += m_Gravity;

		//	��ʓ��ɃN���b�v
		Rect FieldRect = m_pManager->GetWorld()->GetFieldRect();
		FieldRect.l -= m_pData->GetParam().FieldOutRadius;
		FieldRect.r += m_pData->GetParam().FieldOutRadius;
		m_Pos.x = Clamp( m_Pos.x, FieldRect.l, FieldRect.r );

		//	�������
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

	//	�}�g���N�X�X�V
	Matrix3 Mat = Matrix3::Trans( m_Pos );
	if( m_pEffect ) m_pEffect->SetMatrix( Mat );
	if( m_pCollide ) m_pCollide->UpdateMatrix( &Mat );

	//	�J�E���g
	m_FrameCount++;

	//	������L����
	if( m_FrameCount == m_pData->GetParam().CollectTime )
	{
		m_pCollide->SetHitEnable( true );
	}

	//	���Ń`�F�b�N
	if( m_FrameCount >= m_pData->GetParam().Time ||
		m_Pos.y >= m_pManager->GetWorld()->GetFieldRect().b + m_pData->GetParam().DeleteHeight )
	{
		Delete();

		//	�X�R�A�ʒm
		m_pManager->GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_LOST_ITEM );
	}
}

//******************************************************************************
//	�擾���ꂽ�ʒm
//******************************************************************************
void Item::OnGet( void )
{
	Delete();

	m_pManager->OnGet();
}

//******************************************************************************
//	�������
//******************************************************************************
void Item::AutoCollect( void )
{
	m_bAutoCollect = true;
}