#include "stdafx.h"
#include "Shield.h"

#include "../Player.h"
#include "../PlayerData.h"

#include "World/World.h"
#include "Config/Config.h"

#include "SpriteFrame/SpriteFrame.h"
#include "Effect/Effect.h"

#include "Item/ItemManager.h"
#include "Score/ScoreManager.h"
#include "Indicate/Indicate.h"
#include "Indicate/IndicateManager.h"

namespace
{
	const float		SHIELD_MAX		= 100.0f;
	const int		SECTOR_MAX		= 3;

	const Label		INDICATE_SHIELD				= "Shield";
	const Label		INDICATE_RING_SHIELD		= "RingShield";
	const Label		INDICATE_BONE_GAUGE			= "Gauge";
	const Label		INDICATE_BONE_RECOVERY		= "Recovery";
	const Label		INDICATE_BONE_DEADLY		= "Deadly";
	const Label		INDICATE_BONE_NUM			= "Num";
	const Label		INDICATE_BONE_MARKER		= "Marker";
	
	pstr			INDICATE_BONE_MARKER_FMT	= "Marker%d";
	const Label		INDICATE_ANIME_DEADLY		= "Deadly";
	const Label		INDICATE_ANIME_WAIT			= "Wait";

	const Color		COLOR_INDICATE_DEADLY		= Color( 255,0,0 );
};


///////////////////////////////////////////////////////////////////////////////
//
//	�V�[���h
//
///////////////////////////////////////////////////////////////////////////////
Shield::Shield( void )
{
	m_pParent	= NULL;
	m_pParam	= NULL;
	m_State		= STATE_NORMAL;

	m_Shield	= SHIELD_MAX;
	m_Sector	= SECTOR_MAX;
	m_Recovery	= 0.0f;

	m_InvincibleCount	= 0;
	m_NoMoveCount		= 0;
	m_BreakCount		= 0;
	m_BreakNum			= 0;
	m_SerialDamageCount	= 0;
}
Shield::~Shield()
{
	EffectDelete( m_pEffect );
}

//******************************************************************************
//	������
//******************************************************************************
void Shield::Init( Player* pParent, const SHIELD_PARAM* pParam )
{
	_ASSERT( pParent && pParam );

	m_pParent	= pParent;
	m_pParam	= pParam;

	m_State		= STATE_NORMAL;

	m_Shield	= SHIELD_MAX;
	m_Sector	= SECTOR_MAX;
	m_Recovery	= 0.0f;

	m_InvincibleCount	= 0;
	m_NoMoveCount		= 0;
	m_BreakCount		= 0;
	m_BreakNum			= 0;
	m_SerialDamageCount	= 0;

	//	�C���W�P�[�g
	m_pParent->GetWorld()->GetIndicateManager()->AddIndicate( INDICATE_RING_SHIELD, m_pParent->GetData()->GetIndicateData( INDICATE_RING_SHIELD ) );
}

//******************************************************************************
//	���Z�b�g
//******************************************************************************
void Shield::Reset( void )
{
	m_State		= STATE_NORMAL;

	m_Shield	= SHIELD_MAX;
	m_Sector	= SECTOR_MAX;
	m_Recovery	= 0.0f;

	m_InvincibleCount	= 0;
	m_NoMoveCount		= 0;
	m_BreakCount		= 0;
	m_BreakNum			= 0;
}

//******************************************************************************
//	�X�V
//******************************************************************************
void Shield::Update( void )
{
	//	�G�t�F�N�g�̈ʒu
	if( m_pEffect ) m_pEffect->SetMatrix( m_pParent->GetMatrix() );

	DecreaseZero( m_InvincibleCount, 1 );
	DecreaseZero( m_NoMoveCount, 1 );
	DecreaseZero( m_SerialDamageCount, 1 );

	switch( m_State )
	{
	case STATE_NORMAL:	_UpdateNormal();	break;
	case STATE_BREAK:	_UpdateBreak();		break;
	};

	//	�C���W�P�[�g�̍X�V
	_UpdateIndicate();
}

//******************************************************************************
//	�_���[�W
//******************************************************************************
void Shield::Damage( float Damage )
{
	//	���Z
	if( !GetConfig()->GetDebugInfo().bPlayerNodamage )
	{
		DecreaseZero( m_Shield, Damage );
	}

	//	�X�R�A�ʒm
	m_pParent->GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_DAMAGE );
	m_pParent->GetWorld()->GetScoreManager()->StopTimeValue( SCORE_TIME_VALUE_NO_DAMAGE );

	//	�u���C�N
	if( m_Shield <= 0.0f )
	{
		_Break();
	}
	//	�ʏ�_���[�W
	else
	{
		m_InvincibleCount	= m_pParam->DamageInvincible;
		m_NoMoveCount		= m_pParam->DamageNoMove;
		
		//	�G�t�F�N�g����
		EffectDelete( m_pEffect );
		m_pEffect = m_pParent->CreateEffect( m_pParam->HitEffectName, m_pParent->GetMatrix() );

		//	�A���_���[�W����
		if( m_SerialDamageCount > 0 )
		{
			m_pParent->GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_SERIAL_DAMAGE );
		}
		m_SerialDamageCount = m_pParam->SerialDamageTime;
	}
}

//******************************************************************************
//	���R��
//******************************************************************************
float Shield::Regenerate( float Energy )
{
	//	�V�[���h��
	if( !IsDeadly() && m_Shield < SHIELD_MAX )
	{
		Increase( m_Shield, SHIELD_MAX, Energy * m_pParam->NormalRegen );
		DecreaseZero( Energy, m_pParam->UseRegenEnergy );
	}

	//	�ϋv��
	else if( m_Shield >= SHIELD_MAX && m_Sector < SECTOR_MAX )
	{
		Increase( m_Recovery, SHIELD_MAX, (Energy * m_pParam->NormalRecovery) - (m_BreakNum * m_pParam->BreakNumDecayRate) );
		DecreaseZero( Energy, m_pParam->UseRegenEnergy );

		//	�񕜍X�V
		_UpdateRecovery();
	}

	return Energy;
}

//******************************************************************************
//	�A�C�e���擾
//******************************************************************************
float Shield::OnGetItem( float Energy )
{
	//	�V�[���h��
	if( !IsDeadly() && m_Shield < SHIELD_MAX )
	{
		Increase( m_Shield, SHIELD_MAX, Energy * m_pParam->ItemRegen );
		DecreaseZero( Energy, m_pParam->UseRegenEnergy );
	}

	//	�ϋv��
	else if( ( m_Shield == SHIELD_MAX || IsDeadly() ) && m_Sector < SECTOR_MAX )
	{
		Increase( m_Recovery, SHIELD_MAX, Energy * m_pParam->ItemRecovery );
		DecreaseZero( Energy, m_pParam->UseRegenEnergy );

		//	�񕜍X�V
		_UpdateRecovery();
	}

	return Energy;
}

//******************************************************************************
//	�u���C�N��Ԃ���̕��A
//******************************************************************************
void Shield::CancelBreak( void )
{
	m_BreakCount = 0;

	//	�A�C�e���������
	m_pParent->GetWorld()->GetItemManager()->ForceCollect();

	_Normal();	
}

//******************************************************************************
//	����
//******************************************************************************
void Shield::OnRevive( void )
{
	//	�A�C�e���������
	m_pParent->GetWorld()->GetItemManager()->ForceCollect();

	//	�������G
	m_InvincibleCount	= m_pParam->ReviveInvincible;
}

//------------------------------------------------------------------------------
//	�ʏ���
//------------------------------------------------------------------------------
void Shield::_UpdateNormal( void )
{
}

//------------------------------------------------------------------------------
//	�j�����
//------------------------------------------------------------------------------
void Shield::_UpdateBreak( void )
{
	DecreaseZero( m_BreakCount, 1 );

	if( m_BreakCount <= 0 )
	{
		CancelBreak();
	}
}

//------------------------------------------------------------------------------
//	�Z�N�^�[�񕜂̍X�V
//------------------------------------------------------------------------------
void Shield::_UpdateRecovery( void )
{
	if( m_Recovery >= SHIELD_MAX )
	{
		//	�m����Ԃ����
		if( IsDeadly() )
		{
			m_Shield = SHIELD_MAX;
		}

		m_Recovery = 0.0f;
		Increase( m_Sector, SECTOR_MAX, 1 );
		
		//	�A�C�e���������
		m_pParent->GetWorld()->GetItemManager()->AutoCollect();

		//	�񕜃G�t�F�N�g
		EffectDelete( m_pEffect );
		m_pEffect = m_pParent->CreateEffect( m_pParam->RecoveryEffectName, m_pParent->GetMatrix() );
	}
}

//------------------------------------------------------------------------------
//	�C���W�P�[�g�̍X�V
//------------------------------------------------------------------------------
void Shield::_UpdateIndicate( void )
{
	//	�C���W�P�[�g
	Indicate* pIndicate	= m_pParent->GetWorld()->GetIndicateManager()->GetIndicate( INDICATE_SHIELD );
	if( pIndicate )
	{
		pIndicate->SetGauge( INDICATE_BONE_GAUGE,		_GetShieldRate(),		1.0f );
		pIndicate->SetGauge( INDICATE_BONE_RECOVERY,	_GetRecoveryRate(),		1.0f );
		pIndicate->SetText( INDICATE_BONE_NUM,			"%1d", m_Sector		 );
		pIndicate->SetColor( INDICATE_BONE_NUM,			IsDeadly()? COLOR_INDICATE_DEADLY : Color::White() );
		pIndicate->SetBoneVisible( INDICATE_BONE_DEADLY,IsDeadly() );
	}
	
	//	�����O
	Indicate* pRingIndicate	= m_pParent->GetWorld()->GetIndicateManager()->GetIndicate( INDICATE_RING_SHIELD );
	if( pRingIndicate )
	{
		pRingIndicate->SetMatrix( m_pParent->GetMatrix() );

		//	�F
		Color Col = IsDeadly()? COLOR_INDICATE_DEADLY : Color::White();
		pRingIndicate->SetColor( Col * m_pParent->GetBodyAlpha() );

		//	�Q�[�W
		pRingIndicate->SetAngle( INDICATE_BONE_GAUGE,		_GetShieldRate() );
		pRingIndicate->SetAngle( INDICATE_BONE_RECOVERY,	_GetRecoveryRate() );

		//	�c��
		for( int i = 0; i < SECTOR_MAX; i++ )
		{
			String256 Bone;
			Bone.Format( INDICATE_BONE_MARKER_FMT, i );
			pRingIndicate->SetBoneVisible( Label(Bone.c_str()), i < m_Sector || IsDeadly() );
		}

		//	�A�j��
		if( pRingIndicate->IsOpen() &&
			!pRingIndicate->IsAnimeEnable() &&
			m_pParent->IsIndicateEnable() )
		{
			if( IsDeadly() )
			{
				pRingIndicate->ChangeAnime( INDICATE_ANIME_DEADLY, 0 );
			}
			else
			{
				pRingIndicate->ChangeAnime( INDICATE_ANIME_WAIT, 5 );
			}
		}

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
//	�ʏ��Ԃ�
//------------------------------------------------------------------------------
void Shield::_Normal( void )
{
	m_State			= STATE_NORMAL;
}

//------------------------------------------------------------------------------
//	�j����Ԃ�
//------------------------------------------------------------------------------
void Shield::_Break( void )
{
	m_State				= STATE_BREAK;
	m_BreakCount		= m_pParam->BreakTime;
	m_InvincibleCount	= m_pParam->BreakInvincible;
	m_NoMoveCount		= 0;
	m_SerialDamageCount	= 0;
	
	//	�Z�N�^�[����
	if( !GetConfig()->GetDebugInfo().bShieldInfinite )
	{
		DecreaseZero( m_Sector, 1 );
		Increase( m_BreakNum, m_pParam->BreakNumMax, 1 );
	}

	m_Shield = IsDeadly()? 0.0f : SHIELD_MAX;

	//	�G�t�F�N�g����
	EffectDelete( m_pEffect );
	m_pEffect = m_pParent->CreateEffect( m_pParam->BreakEffectName, m_pParent->GetMatrix() );

	//	�A�C�e������
	m_pParent->GetWorld()->GetItemManager()->AppearItem( m_pParam->BreakItemNum, m_pParent->GetPos(), m_pParam->ItemSpeed );
	
	//	�X�R�A�ʒm	
	m_pParent->GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_BREAK );
}

//------------------------------------------------------------------------------
//	�V�[���h�̎c�ʊ����𓾂�
//------------------------------------------------------------------------------
float Shield::_GetShieldRate( void ) const
{
	return m_Shield / SHIELD_MAX;
}
	
//------------------------------------------------------------------------------
//	�V�[���h�񕜂̎c�ʊ����𓾂�
//------------------------------------------------------------------------------
float Shield::_GetRecoveryRate( void ) const
{
	return m_Recovery / SHIELD_MAX;
}