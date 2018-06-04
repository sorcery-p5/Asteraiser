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
//	�u���[�h
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
//	������
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

	//	�}�g���N�X
	const Matrix3* pMat	= m_pParent->GetSprite()->GetBoneMatrix(BONE_NAME_BLADE);
	_ASSERT( pMat );
	m_Matrix		= *pMat;

	//	�G�l���M�[
	m_pEnergy		= new BladeEnergy( this, &pParam->EnergyParam );
	MEM_CHECK( m_pEnergy  );

	//	�����蔻��
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

	//	�C���W�P�[�g
	m_pParent->GetWorld()->GetIndicateManager()->AddIndicate( INDICATE_RING_BLADE, m_pParent->GetData()->GetIndicateData( INDICATE_RING_BLADE ) );
}

//******************************************************************************
//	���Z�b�g
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
	
	//	�������߂�
	m_pCollide->SetHitEnable( false );
	_UpdateHitSize();

	m_pEnergy->Reset();
}

//******************************************************************************
//	�X�V
//******************************************************************************
void Blade::Update( void )
{
	//	���[���[���
	_UpdateRuler();

	//	�G�l���M�[
	m_pEnergy->Update();

	//	�}�g���N�X�X�V
	_UpdateMatrix( m_pParent->GetSprite()->GetBoneMatrix(BONE_NAME_BLADE) );

	switch( m_State )
	{
	case STATE_NORMAL:	_UpdateNormal();	break;
	case STATE_CHARGE:	_UpdateCharge();	break;
	case STATE_SLASH:	_UpdateSlash();		break;
	}

	//	�G�t�F�N�g�̕\��
	_UpdateEffect();

	//	�C���W�P�[�g�̍X�V
	_UpdateIndicate();

	//	�J�E���g
	DecreaseZero( m_DisableSlashCount, 1 );
	m_StateCount++;
}

//******************************************************************************
//	�u���[�h�^��
//******************************************************************************
void Blade::Charge( Point Dir )
{
	//	�J�E���g��ݒ�
	m_CancelCount	= 0;
	m_bHit			= false;
	m_bSmash		= false;
	m_bEndNotify	= false;

	//	�m�b�N�o�b�N�����o�^
	m_KnockbackDir	= Dir;

	m_State = STATE_CHARGE;
	m_StateCount = 0;
}

//******************************************************************************
//	�u���[�h�U��
//******************************************************************************
void Blade::Slash( bool bSmash )
{
	//	�J�E���g��ݒ�
	m_Delay			= m_bRuler? m_pParam->RulerDelay : m_pParam->NormalDelay;
	m_CancelCount	= 0;
	m_bHit			= false;
	m_bSmash		= bSmash;
	m_bEndNotify	= false;

	//	�����[�h�s���Ԃ�ݒ�
	m_NotReloadCount = m_pParam->NotReloadTime;

	//	�����蔻�萶��
	m_pCollide->SetHitEnable( true );

	//	������T�C�Y�ݒ�
	_UpdateHitSize();

	m_State = STATE_SLASH;
	m_StateCount = 0;
}

//******************************************************************************
//	�����[�h���s��
//******************************************************************************
void Blade::Reload( void )
{	
	//	�A�C�e������
	int ItemNum = (int)( m_pParam->ReloadItemNum * ( 1.0f - m_pEnergy->GetLimitRate() ) );
	m_pParent->GetWorld()->GetItemManager()->AppearItem( ItemNum, m_pParent->GetPos(), m_pParam->ItemSpeed );

	m_pEnergy->Reload();
}

//******************************************************************************
//	�L�����Z�������[�h���s��
//******************************************************************************
void Blade::CancelReload( void )
{
	//	�f�B���C����
	m_Delay			= 0;
	m_CancelCount	= 0;

	//	�����[�h�s���Ԃ�ݒ�
	m_NotReloadCount = m_pParam->NotReloadTime;

	//	�K�p
	m_pEnergy->CancelReload();

	//	�X�R�A�ʒm
	m_pParent->GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_CANCEL );
}

//******************************************************************************
//	��Ԃ�߂�
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
	
	//	�������߂�
	m_pCollide->SetHitEnable( false );
	_UpdateHitSize();
}

//******************************************************************************
//	�f�B���C��t�^����
//******************************************************************************
void Blade::Delay( void )
{
	m_Delay = m_bRuler? m_pParam->RulerDelay : m_pParam->NormalDelay;
}

//******************************************************************************
//	�G�ɓ�������
//******************************************************************************
bool Blade::OnHitEnemy( Enemy* pEnemy, float Power, Point Vel, int Knockback, bool bRepulse, bool bAntiHide, const Matrix3& HitMat, Label HitEffect )
{
	bool bCombo = pEnemy->IsKnockback();

	//	�_���[�W��^����
	float Damage = pEnemy->Damage( Power, Vel, Knockback, bRepulse, bAntiHide );
	if( Damage >= 0.0f )
	{
		//	����
		_Exhaust( Damage, bRepulse, HitMat.GetPos() );
		
		if( !m_bRepulse )
		{
			//	�q�b�g�G�t�F�N�g
			Effect* pEffect = m_pParent->CreateEffect( HitEffect, Matrix3::RotZ( GetRand().GetRandRange(0.0f, Angle::ToRad(15.0f)) ) * HitMat );
			if( pEffect ) pEnemy->AddHitEffect( pEffect );
		
			//	�A�C�e������
			int ItemNum = (int)ceilf( m_pParam->HitItemNum * (Damage / Power) );
			m_pParent->GetWorld()->GetItemManager()->AppearItem( ItemNum, HitMat.GetPos(), m_pParam->ItemSpeed );
		}
		else
		{
			if( pEnemy->IsDead() )
			{
				//	�����I���G�t�F�N�g
				Effect* pEffect = m_pParent->CreateEffect( m_pParam->RepulseEndEffectName, HitMat );
				if( pEffect ) pEnemy->AddHitEffect( pEffect );

				m_RepulseCount = 0;
			}
			else if( m_RepulseCount % m_pParam->RepulseInterval == 0 )
			{
				//	�����q�b�g�G�t�F�N�g
				Effect* pEffect = m_pParent->CreateEffect( m_pParam->RepulseHitEffectName, HitMat );
				if( pEffect ) pEnemy->AddHitEffect( pEffect );
			}
		}

		//	�X�R�A�ʒm
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
//	�U���ɓ�������
//******************************************************************************
bool Blade::OnHitAttack( float Exhaust, bool bRepulse, Point HitPos )
{
	bRepulse = bRepulse && !m_pParent->IsRulerHit();

	//	����
	_Exhaust( Exhaust, bRepulse, HitPos );

	if( m_bRepulse )
	{
		//	�������
		m_bAttackRepulse = true;
	}
	else
	{
		//	�q�b�g�G�t�F�N�g
		m_pParent->CreateEffect( m_pParam->HitEffectName, _GetHitMatrix(HitPos) );
	}

	m_bHit			= true;
	m_CancelCount	= m_pParam->CancelTime;

	return true;
}

//******************************************************************************
//	�A�C�e���Ƀq�b�g����
//******************************************************************************
void Blade::OnHitItem( Item* pItem, Point HitPos )
{
	//	�擾��ʒm
	m_pParent->OnGetItem( pItem );
}

//******************************************************************************
//	������������
//******************************************************************************
void Blade::OnSplash( void )
{
	//	�����[�h�s���Ԃ�ݒ�
	m_NotReloadCount = m_pParam->NotReloadTime;
}

//******************************************************************************
//	���[���[�I��
//******************************************************************************
void Blade::OnCrashRuler( void )
{
	//	�������߂�
	m_pCollide->SetHitEnable( false );
	_UpdateHitSize();

	//	�j��������
	m_pEnergy->Crash();

	//	�j���G�t�F�N�g
	m_pParent->CreateEffect( m_pParam->CrashEffectName, m_Matrix );
}

//******************************************************************************
//	�U��Ȃ��Ƃ��ɐU�낤�Ƃ���
//******************************************************************************
void Blade::OnDisableSlash( void )
{
	m_DisableSlashCount = DISABLE_SLASH_TIME;
}

//******************************************************************************
//	���R��
//******************************************************************************
float Blade::Regenerate( float Energy )
{
	return m_pEnergy->Regenerate( Energy );
}

//******************************************************************************
//	�A�C�e���擾
//******************************************************************************
float Blade::OnGetItem( float Energy )
{
	return m_pEnergy->OnGetItem( Energy );
}

//******************************************************************************
//	�܂ꂽ��Ԃ��H
//******************************************************************************
bool Blade::IsCrash( void ) const
{
	return m_pEnergy->IsCrash();
}

//******************************************************************************
//	�u���[�h�U�邱�Ƃ��\��
//******************************************************************************
bool Blade::IsSlashEnable( void ) const
{
	//	���[���[��
	if( m_bRuler )
	{
		//	�ʏ��Ԃ܂��̓L�����Z�����
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
		//	�ʏ��Ԃ̂�
		if( m_State != STATE_NORMAL ) return false;
	}

	//	�G�l���M�[�s��
	if( !m_pEnergy->IsSlashEnable() ) return false;
	
	//	�f�B���C��
	if( m_Delay > 0 ) return false;

	return true;
}

//******************************************************************************
//	�����[�h�\��
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
//	�L�����Z�������[�h�\��
//******************************************************************************
bool Blade::IsCancelEnable( void ) const
{
	if( m_State != STATE_NORMAL ) return false;
	
	if( !m_pEnergy->IsNormalState() ) return false;

	return m_CancelCount > 0;
}

//******************************************************************************
//	�U���͂̊����𓾂�
//******************************************************************************
float Blade::GetAttackRate( void ) const
{
	return	Lerp( m_pParam->AttackEnergyRate, 1.0f, m_pEnergy->GetEnergyRate() ) *
			Lerp( m_pParam->AttackLimitRate, 1.0f, m_pEnergy->GetLimitRate() );
}

//******************************************************************************
//	�U���͂̎擾
//******************************************************************************
float Blade::GetAttackPower( void ) const
{
	float Attack =	m_bSmash?	m_pParam->SmashParam.Attack :
					m_bRuler?	m_pParam->RulerAttack :
								m_pParam->Attack;
		
	return Attack * GetAttackRate();
}

//******************************************************************************
//	�������̍U���͂̎擾
//******************************************************************************
float Blade::GetRepulseAttackPower( void ) const
{
	return m_pParam->RepulseAttack * GetAttackRate();
}

//******************************************************************************
//	�U���̔����͂��擾
//******************************************************************************
float Blade::GetAttackVel( void ) const
{
	return m_bSmash? m_pParam->SmashParam.AttackVel : m_pParam->AttackVel;
}

//******************************************************************************
//	�U���̃m�b�N�o�b�N���Ԃ��擾
//******************************************************************************
int Blade::GetKnockback( void ) const
{
	return m_bSmash? m_pParam->SmashParam.Knockback : m_pParam->Knockback;
}

//******************************************************************************
//	�����̎擾
//******************************************************************************
Rand& Blade::GetRand( void ) const
{
	return m_pParent->GetRand();
}

//------------------------------------------------------------------------------
//	�X�V�F�ҋ@
//------------------------------------------------------------------------------
void Blade::_UpdateNormal( void )
{
	//	�J�E���g�_�E��
	DecreaseZero( m_Delay, 1 );
	DecreaseZero( m_CancelCount, 1 );
	DecreaseZero( m_NotExhaustCount, 1 );
	DecreaseZero( m_NotReloadCount, 1 );

	if( m_NotExhaustCount == 0 ) m_ExhaustMax = 0;

	//	�ʏ��Ԃɂ�������炸�G�l���M�[�؂�
	if( m_pEnergy->IsNormalRedzone() )
	{
		m_pEnergy->Crash();
	}
}

//------------------------------------------------------------------------------
//	�X�V�F�^��
//------------------------------------------------------------------------------
void Blade::_UpdateCharge( void )
{
	if( m_pParent->IsRulerHit() ) return;

	//	�G�l���M�[����
	if( m_pParent->GetStateCount() >= m_pParam->SmashParam.ChargeStartTime &&
		m_pParent->GetStateCount() < m_pParam->SmashParam.ChargeTime )
	{
		m_pEnergy->ChargeExhaust( m_pParam->SmashParam.ChargeExhaustEnergy, m_pParam->SmashParam.ChargeExhaustLimit );
	}
}

//------------------------------------------------------------------------------
//	�X�V�F�u���[�h�U�蒆
//------------------------------------------------------------------------------
void Blade::_UpdateSlash( void )
{
	m_bRepulse = false;

	//	������L���X�g
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

	//	�������
	if( m_bAttackRepulse )
	{
		m_bRepulse = true;
		m_bAttackRepulse = false;
	}

	//	�I���ʒm
	if( m_pParent->GetSprite() )
	{
		if( m_pParent->GetSprite()->GetAnimeNotify( NOTIFY_END ) )
		{
			m_bEndNotify = true;
		}
	}
}

//------------------------------------------------------------------------------
//	�}�g���N�X�X�V
//------------------------------------------------------------------------------
void Blade::_UpdateMatrix( const Matrix3* pMat )
{
	if( !pMat ) return;

	m_Matrix = *pMat;
	
	//	�G�t�F�N�g
	if( m_pEffect ) m_pEffect->SetMatrix( m_Matrix );

	//	�����蔻��
	m_pCaster->UpdateMatrix( &m_Matrix );
	m_pCollide->UpdateMatrix( &m_Matrix );
}

//------------------------------------------------------------------------------
//	�����蔻��̃T�C�Y���X�V
//------------------------------------------------------------------------------
void Blade::_UpdateHitSize( void )
{
	float Rad = m_pParam->Radius;
	float Len = m_pParam->Length;

	//	�X�}�b�V��
	if( m_bSmash )
	{
		Rad = m_pParam->SmashParam.Radius;
		Len = m_pParam->SmashParam.Length;
	}
	//	���[���[
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
//	�G�t�F�N�g�̍X�V
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

	//	���[���[���E�I�[���@�h���C�����͍X�V���Ȃ�
	if( !m_bRuler && !m_pParent->IsState( STATE_OVERDRIVE ) )
	{
		//	�L�����Z���\��
		if( IsCancelEnable() )
		{
			Col = COLOR_EFFECT_CANCEL;
		}
		//	�G�l���M�[�s��
		else if( m_State == STATE_NORMAL && (!m_pEnergy->IsSlashEnable()) )
		{
			Col = COLOR_EFFECT_DISABLE;
		}
		//	�f�B���C��
		else if( m_State == STATE_NORMAL && m_Delay > 0 )
		{
			Col = COLOR_EFFECT_DELAY;
		}
		//	�G�l���M�[�ʂɉ����ĕω�
		else
		{
			Col = Color::Lerp( COLOR_EFFECT_ENERGY_MIN, COLOR_EFFECT_ENERGY_MAX, m_pEnergy->GetLimitRate() );
		}
	}

	m_pEffect->SetColor( Col );

	//	���@�̃A�j��
	Label VisibleNotify( m_pParent->GetSprite()->GetAnimeNotify( NOTIFY_KEY_BLADE ) );
	if( VisibleNotify == NOTIFY_VISIBLE )
	{
		m_bVisible = true;
	}
	else if( VisibleNotify == NOTIFY_INVISIBLE )
	{
		m_bVisible = false;
	}

	//	��\��
	m_pEffect->SetVisible( !m_pParent->IsDead() && m_bVisible );
}

//------------------------------------------------------------------------------
//	�C���W�P�[�g�̍X�V
//------------------------------------------------------------------------------
void Blade::_UpdateIndicate( void )
{
	//	�C���W�P�[�g
	Indicate* pIndicate	= m_pParent->GetWorld()->GetIndicateManager()->GetIndicate( INDICATE_BLADE );
	if( pIndicate )
	{
		pIndicate->SetGauge( INDICATE_BONE_BLUE,		m_pEnergy->GetNormalEnergyRate(),	1.0f );
		pIndicate->SetGauge( INDICATE_BONE_CANCEL,		m_pEnergy->GetCancelEnergyRate(),	1.0f );
		pIndicate->SetGauge( INDICATE_BONE_RED,			m_pEnergy->GetRedEnergyRate(),		1.0f );
		pIndicate->SetGauge( INDICATE_BONE_BASE_LIMIT,	m_pEnergy->GetLimitRate(),			1.0f );
		pIndicate->SetMarker( INDICATE_BONE_LIMIT,		m_pEnergy->GetLimitRate() );
	}
	
	//	�����O
	Indicate* pRingIndicate	= m_pParent->GetWorld()->GetIndicateManager()->GetIndicate( INDICATE_RING_BLADE );
	if( pRingIndicate )
	{
		pRingIndicate->SetMatrix( m_pParent->GetMatrix() );
		pRingIndicate->SetColor( m_pParent->GetBodyAlpha() );

		//	�Q�[�W
		pRingIndicate->SetAngle( INDICATE_BONE_BLUE,		m_pEnergy->GetNormalEnergyRate() );
		pRingIndicate->SetAngle( INDICATE_BONE_RED,			m_pEnergy->GetRedEnergyRate() );
		pRingIndicate->SetAngle( INDICATE_BONE_BASE_LIMIT,	m_pEnergy->GetLimitRate() );

		//	�L�����Z��
		pRingIndicate->SetAngle( INDICATE_BONE_CANCEL,		Max( m_pEnergy->GetCancelEnergyRate() - m_pEnergy->GetNormalEnergyRate(), 0.0f ) );
		pRingIndicate->SetSpin( INDICATE_BONE_CANCEL,		m_pEnergy->GetNormalEnergyRate() );

		//	�U��Ȃ�����
		float Disable = (float)m_DisableSlashCount / (float)DISABLE_SLASH_TIME;
		pRingIndicate->SetColor( INDICATE_BONE_BASE,		Color::Lerp( COLOR_INDICATE_BASE, COLOR_INDICATE_DELAY, Disable ) );

		//	�L�����Z��
		if( IsCancelEnable() )
		{
			float CancelRate = Lerp( INDICATE_DELAY_SCALE, 1.0f, (float)m_CancelCount / (float)m_pParam->CancelTime );
			pRingIndicate->SetGauge( INDICATE_BONE_DELAY,		CancelRate, CancelRate );
			pRingIndicate->SetColor( INDICATE_BONE_DELAY,		COLOR_INDICATE_CANCEL );
			pRingIndicate->SetBoneVisible( INDICATE_BONE_DELAY,	m_State == STATE_NORMAL );
		}
		//	�f�B���C
		else if( m_Delay )
		{
			float DelayRate = Lerp( INDICATE_DELAY_SCALE, 1.0f, (float)m_Delay / (float)m_pParam->NormalDelay );
			pRingIndicate->SetGauge( INDICATE_BONE_DELAY,		DelayRate, DelayRate );
			pRingIndicate->SetColor( INDICATE_BONE_DELAY,		COLOR_INDICATE_DELAY );
			pRingIndicate->SetBoneVisible( INDICATE_BONE_DELAY,	m_State == STATE_NORMAL );
		}
		//	�`���[�W
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

		//	�ϋv
		pRingIndicate->SetSpin( INDICATE_BONE_LIMIT,		m_pEnergy->GetLimitRate() );
		pRingIndicate->SetColor( INDICATE_BONE_MARKER,		m_pEnergy->IsCrash()?	COLOR_INDICATE_CRASH :
																					Color::Lerp( COLOR_INDICATE_ENERGY_MIN, COLOR_INDICATE_ENERGY_MAX, m_pEnergy->GetLimitRate() ) );

		//	�J��
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
//	���[���[��Ԃ̍X�V
//------------------------------------------------------------------------------
void Blade::_UpdateRuler( void )
{
	//	���[���[�ɓ�����
	if( !m_bRuler && m_pParent->IsRulerHit() )
	{
		//	��p�G�t�F�N�g
		EffectEndAppear( m_pEffect );
		m_pEffect = m_pParent->CreateEffect( m_pParam->RulerEffectName, m_Matrix );

		//	�G�l���M�[������
		m_pEnergy->Reset();
	}
	//	���[���[����o��
	else if( m_bRuler && !m_pParent->IsRulerHit() )
	{
		//	�ʏ�G�t�F�N�g
		EffectEndAppear( m_pEffect );
		m_pEffect = m_pParent->CreateEffect( m_pParam->EffectName, m_Matrix );
	}

	//	�t���O�X�V
	m_bRuler = m_pParent->IsRulerHit();
}

//------------------------------------------------------------------------------
//	�G�ɓ�������
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
//	�n�`�Ƀq�b�g����
//------------------------------------------------------------------------------
void Blade::_OnHitGround( Ground* pGround, Point HitPos )
{
	Matrix3 HitMat = _GetHitMatrix(HitPos);

	//	�����G�t�F�N�g
	if( pGround->IsAttackHitEffect() )
	{
		m_pParent->CreateEffect( m_pParam->RepulseEffectName, HitMat );
		m_pParent->CreateEffect( m_pParam->RepulseHitEffectName, HitMat );
	}

	//	�n�`���̃G�t�F�N�g
	pGround->CreateHitEffect( HitMat );
}

//------------------------------------------------------------------------------
//	�u���[�h����
//------------------------------------------------------------------------------
void Blade::_Exhaust( float Damage, bool bRepulse, Point HitPos )
{
	bool bPreCrash = m_pEnergy->IsCrash();

	//	���Z
	if( !GetConfig()->GetDebugInfo().bBladeInfinite )
	{
		if( !m_pParent->IsRulerHit() )
		{
			float Exhaust = Damage * _GetExhaustRate();

			//	�ő����ʂɍ��킹��
			if( !bRepulse )
			{
				float ExhaustMax = m_ExhaustMax;
				m_ExhaustMax = Max( Exhaust, m_ExhaustMax );
				Exhaust = (Exhaust > ExhaustMax)? Exhaust - ExhaustMax : 0.0f;
			}

			//	�G�l���M�[����
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

	//	�j��
	if( !bPreCrash && m_pEnergy->IsCrash() )
	{
		//	�j���G�t�F�N�g
		m_pParent->CreateEffect( m_pParam->CrashEffectName, m_Matrix );
		
		//	�A�C�e������
		m_pParent->GetWorld()->GetItemManager()->AppearItem( m_pParam->CrashItemNum, m_Matrix.GetPos(), m_pParam->ItemSpeed );

		//	�X�R�A�ʒm
		m_pParent->GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_CRASH );
	}
	//	����
	else if( bRepulse )
	{
		m_bRepulse = true;

		if( m_RepulseCount % m_pParam->RepulseInterval == 0 )
		{
			//	�����G�t�F�N�g
			m_pParent->CreateEffect( m_pParam->RepulseEffectName, _GetHitMatrix(HitPos) );
			
			//	�A�C�e������
			m_pParent->GetWorld()->GetItemManager()->AppearItem( m_pParam->RepulseItemNum, HitPos, m_pParam->ItemSpeed );
		}
		m_RepulseCount++;
	}
}

//------------------------------------------------------------------------------
//	������𓾂�
//------------------------------------------------------------------------------
float Blade::_GetExhaustRate( void ) const
{
	return m_bSmash? m_pParam->SmashParam.ExhaustRate : 1.0f;
}

//------------------------------------------------------------------------------
//	�U���q�b�g�p�̃}�g���N�X�𓾂�
//------------------------------------------------------------------------------
Matrix3 Blade::_GetHitMatrix( Point HitPos ) const
{
	Matrix3 Mat = m_Matrix;
	Mat.SetPos( HitPos );

	return Mat;
}


////////////////////////////////////////////////////////////////////////////////
//
//	�u���[�h�G�l���M�[
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
//	���Z�b�g
//******************************************************************************
void BladeEnergy::Reset( void )
{
	m_EnergyLimit	= ENERGY_MAX;
	m_Energy		= ENERGY_MAX;
	m_CancelEnergy	= 0.0f;

	m_State			= STATE_NORMAL;
}

//******************************************************************************
//	�X�V
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
//	�����[�h
//******************************************************************************
void BladeEnergy::Reload( void )
{
	m_State = STATE_RELOAD;

	m_CancelEnergy	= 0.0f;
}

//******************************************************************************
//	�L�����Z�������[�h
//******************************************************************************
void BladeEnergy::CancelReload( void )
{
	m_CancelEnergy = ENERGY_MAX;
}

//******************************************************************************
//	�G�l���M�[����
//******************************************************************************
void BladeEnergy::Exhaust( float Damage )
{
	//	����
	_ExhaustEnergy( Damage * m_pParam->ExhaustEnergyRate, Damage * m_pParam->ExhaustLimitRate );

	//	�j��
	if( m_EnergyLimit <= ENERGY_REDZONE )
	{
		Crash();
	}
}

//******************************************************************************
//	�^�����̏���
//******************************************************************************
void BladeEnergy::ChargeExhaust( float Energy, float Limit )
{
	//	����
	_ExhaustEnergy( Energy, Limit );

	//	���b�h�]�[���ɂ͍s���Ȃ�
	if( m_EnergyLimit <= ENERGY_REDZONE )
	{
		m_EnergyLimit = ENERGY_REDZONE;
	}
}

//******************************************************************************
//	���R��
//******************************************************************************
float BladeEnergy::Regenerate( float Energy )
{
	if( Energy <= 0.0f ) return 0.0f;

	//	�ʏ���
	if( m_State == STATE_NORMAL )
	{
		//	�G�l���M�[�̉�
		if( m_Energy < m_EnergyLimit )
		{
			Increase( m_Energy, m_EnergyLimit, Energy * m_pParam->NormalRegen );
			DecreaseZero( Energy, m_pParam->UseRegenEnergy );
		}

		//	�ϋv�̉�
		if( m_EnergyLimit < ENERGY_MAX )
		{
			Increase( m_EnergyLimit, ENERGY_MAX, Energy * m_pParam->NormalLimitRegen );
			DecreaseZero( Energy, m_pParam->UseLimitEnergy );
		}
	}
	//	�����[�h�E�j��
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
//	�A�C�e���擾
//******************************************************************************
float BladeEnergy::OnGetItem( float Energy )
{
	if( Energy <= 0.0f ) return 0.0f;

	//	�ʏ���
	if( m_State == STATE_NORMAL )
	{
		//	�G�l���M�[�̉�
		if( m_Energy < m_EnergyLimit )
		{
			Increase( m_Energy, m_EnergyLimit, Energy * m_pParam->ItemRegen );
			DecreaseZero( Energy, m_pParam->UseRegenEnergy );
		}

		//	�ϋv�̉�
		if( m_EnergyLimit < ENERGY_MAX )
		{
			Increase( m_EnergyLimit, ENERGY_MAX, Energy * m_pParam->ItemLimitRegen );
			DecreaseZero( Energy, m_pParam->UseLimitEnergy );
		}
	}
	//	�����[�h�E�j��
	else
	{
		Increase( m_EnergyLimit, ENERGY_MAX, Energy * m_pParam->ItemLimitRegen );
		m_Energy = m_EnergyLimit;

		DecreaseZero( Energy, m_pParam->UseReloadEnergy );
	}

	return Energy;
}

//******************************************************************************
//	�j��������
//******************************************************************************
void BladeEnergy::Crash( void )
{
	m_EnergyLimit	= ENERGY_REDZONE;
	m_Energy		= 0.0f;
	m_CancelEnergy	= 0.0f;

	m_State			= STATE_CRASH;
}

//******************************************************************************
//	�G�l���M�[�c�������𓾂�
//******************************************************************************
float BladeEnergy::GetEnergyRate( void ) const
{
	return Max( m_Energy, m_CancelEnergy ) / ENERGY_MAX;
}

//******************************************************************************
//	�G�l���M�[�c�������𓾂�i���c�ʁj
//******************************************************************************
float BladeEnergy::GetNormalEnergyRate( void ) const
{
	return m_Energy / ENERGY_MAX;
}

//******************************************************************************
//	���b�h�]�[���c�������𓾂�
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
//	�L�����Z���G�l���M�[�̊����𓾂�
//******************************************************************************
float BladeEnergy::GetCancelEnergyRate( void ) const
{
	return m_CancelEnergy / ENERGY_MAX;
}

//******************************************************************************
//	�G�l���M�[�ő�̊����𓾂�
//******************************************************************************
float BladeEnergy::GetLimitRate( void ) const
{
	return m_EnergyLimit / ENERGY_MAX;
}

//******************************************************************************
//	�g�p�\��
//******************************************************************************
bool BladeEnergy::IsSlashEnable( void ) const
{
	return IsNormalState() && Max( m_Energy, m_CancelEnergy ) > ENERGY_REDZONE;
}

//******************************************************************************
//	�ʏ��ԂȂ̂ɔj�����
//******************************************************************************
bool BladeEnergy::IsNormalRedzone( void ) const
{
	return IsNormalState() && m_EnergyLimit <= ENERGY_REDZONE;
}

//******************************************************************************
//	�G�l���M�[�����^����
//******************************************************************************
bool BladeEnergy::IsEnergyLimit( void ) const
{
	return m_Energy >= m_EnergyLimit;
}

//******************************************************************************
//	�G�l���M�[�����S��
//******************************************************************************
bool BladeEnergy::IsEnergyFull( void ) const
{
	return m_Energy >= m_EnergyLimit && m_Energy >= ENERGY_MAX;
}

//------------------------------------------------------------------------------
//	�ʏ���
//------------------------------------------------------------------------------
void BladeEnergy::_UpdateNormal( void )
{
	//	�L�����Z���ꎞ�G�l���M�[����
	if( !m_pParent->IsCharge() )
	{
		DecreaseZero( m_CancelEnergy, m_pParam->CancelEnergyDecay );
		if( m_Energy >= m_CancelEnergy ) m_CancelEnergy = 0.0f;
	}
}

//------------------------------------------------------------------------------
//	�����[�h���
//------------------------------------------------------------------------------
void BladeEnergy::_UpdateReload( void )
{
	//	��������
	if( IsEnergyFull() )
	{
		m_State = STATE_NORMAL;
	}
}

//------------------------------------------------------------------------------
//	�j�����
//------------------------------------------------------------------------------
void BladeEnergy::_UpdateCrash( void )
{
	//	��������
	if( IsEnergyFull() )
	{
		m_State = STATE_NORMAL;
	}
}

//------------------------------------------------------------------------------
//	�G�l���M�[�̏���
//------------------------------------------------------------------------------
void BladeEnergy::_ExhaustEnergy( float Energy, float Limit )
{
	//	�L�����Z���G�l���M�[����
	if( m_CancelEnergy > m_Energy )
	{
		DecreaseZero( m_CancelEnergy, Energy );

		//	�L�����Z���G�l���M�[�����Ȃ���
		if( m_CancelEnergy <= m_Energy )
		{
			m_Energy		= m_CancelEnergy;
			m_CancelEnergy	= 0.0f;
		}
	}
	//	�G�l���M�[����
	else
	{
		DecreaseZero( m_Energy, Energy );
	}

	//	�ϋv�̌���
	DecreaseZero( m_EnergyLimit, Limit );
	if( m_Energy > m_EnergyLimit )
	{
		m_Energy = m_EnergyLimit;
	}
}