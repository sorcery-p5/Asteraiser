#include "stdafx.h"
#include "Ruler.h"

#include "Player/PlayerTypes.h"
#include "Player/Player.h"
#include "Player/PlayerData.h"

#include "World/World.h"

#include "SpriteFrame/SpriteFrame.h"
#include "Collide/CollideSystem.h"
#include "Effect/EffectManager.h"
#include "Effect/Effect.h"

#include "Enemy/Enemy.h"
#include "Attack/Attack.h"
#include "Item/ItemManager.h"
#include "Score/ScoreManager.h"
#include "Indicate/Indicate.h"
#include "Indicate/IndicateManager.h"


namespace
{
	const Label		INDICATE_RULER			= "RingRuler";
	const Label		INDICATE_BONE_SHORT		= "Short";
	const Label		INDICATE_BONE_LONG		= "Long";
}


////////////////////////////////////////////////////////////////////////////////
//
//	�I�[���@�h���C���E���[���[
//
////////////////////////////////////////////////////////////////////////////////
Ruler::Ruler( Player* pParent )
{
	m_pParent		= pParent;
	m_pParam		= NULL;

	m_pCollide		= NULL;

	m_bDelete		= false;
	m_Radius		= 0.0f;
	m_Time			= 0;
	m_FrameCount	= 0;
}

Ruler::~Ruler()
{
	EffectEndAppear( m_pEffect );
	SafeDelete( m_pCollide );
}

//******************************************************************************
//	������
//******************************************************************************
void Ruler::Init( const RULER_PARAM* pParam, const Matrix3& Mat, float AddEnergy )
{
	//	�p�����[�^
	m_pParam		= pParam;
	m_Radius		= Min( pParam->Radius + pParam->RadiusItemRate * AddEnergy,	pParam->RadiusMax );
	m_Time			= (int)Min( (float)pParam->Time + pParam->TimeItemRate * AddEnergy,	(float)pParam->TimeMax );
	m_FrameCount	= 0;

	//	���a�ɂ��킹�Ĕ{���ݒ�
	float RadRate	= m_Radius / pParam->Radius;
	m_Matrix		= Matrix3::Scale( Point(RadRate, RadRate) ) * Mat;
	
	//	�G�t�F�N�g
	m_pEffect = m_pParent->CreateEffect( pParam->EffectName, m_Matrix );

	//	�����蔻��
	m_pCollide = new CollideObject;
	MEM_CHECK( m_pCollide );

	m_pCollide->Init( new CollideShapePoint( m_Radius ), COLLIDE_LAYER_RULER );
	m_pCollide->SetOwner( COLLIDE_OWNER_RULER, this );
	m_pCollide->UpdateMatrix( &m_Matrix );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void Ruler::Update( void )
{
	if( m_bDelete ) return;

	//	�t���[���J�E���g
	m_FrameCount++;

	//	�C���W�P�[�g�X�V
	_UpdateIndicate();

	//	�I��
	if( m_FrameCount >= m_Time )
	{
		//	�j��
		Crash();
		
		//	�ʒm
		m_pParent->OnCrashRuler();
	}
}

//******************************************************************************
//	�j�󂷂�
//******************************************************************************
void Ruler::Crash( void )
{
	if( m_bDelete ) return;

	//	�j���G�t�F�N�g
	m_pParent->CreateEffect( m_pParam->CrashEffectName, m_Matrix );

	//	�A�C�e������
	m_pParent->GetWorld()->GetItemManager()->AppearItem( m_pParam->CrashItemNum, m_Matrix.GetPos(), m_pParam->ItemSpeed );

	//	�X�R�A�ʒm
	m_pParent->GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_RULER );

	//	�C���W�P�[�g����
	Indicate* pRingIndicate	= m_pParent->GetWorld()->GetIndicateManager()->GetIndicate( INDICATE_RULER );
	if( pRingIndicate )
	{
		pRingIndicate->Close( true );
	}

	Delete();
}

//------------------------------------------------------------------------------
//	�C���W�P�[�g�X�V
//------------------------------------------------------------------------------
void Ruler::_UpdateIndicate( void )
{
	//	�����O
	Indicate* pRingIndicate	= m_pParent->GetWorld()->GetIndicateManager()->GetIndicate( INDICATE_RULER );
	if( pRingIndicate )
	{
		pRingIndicate->SetMatrix( m_Matrix );

		float Value = (float)m_FrameCount / (float)m_Time;
		pRingIndicate->SetSpin( INDICATE_BONE_SHORT, Value );
		pRingIndicate->SetSpin( INDICATE_BONE_LONG, Value );
	}
}