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
//	�I�[���@�h���C��
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
//	������
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
//	���Z�b�g
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
//	�X�V
//******************************************************************************
void Overdrive::Update( void )
{
	//	�G�l���M�[
	m_pEnergy->Update();

	//	����
	_UpdateSpear();
	_UpdateRuler();

	switch( m_State )
	{
	case STATE_NONE:	DecreaseZero( m_Delay, 1 );	break;
	case STATE_READY:	_UpdateReady();				break;
	case STATE_AFTER:	_UpdateAfter();				break;
	}

	//	�C���W�P�[�g�̍X�V
	_UpdateIndicate();
}

//******************************************************************************
//	�X�s�A����
//******************************************************************************
void Overdrive::DriveSpear( void )
{
	m_Type			= TYPE_SPEAR;
	m_State			= STATE_READY;
	m_ReadyEnergy	= 0.0f;

	m_pEnergy->Drive( ENERGY_USE_SPEAR );

	//	�C���W�P�[�g�G�t�F�N�g�X�V
	EffectEndAppear( m_pIndicateEffect );

	//	���b�N�G�t�F�N�g����
	Enemy* pEnemy = m_pParent->GetWorld()->GetEnemyManager()->GetSpearTarget( m_pParent->GetPos() );
	if( pEnemy )
	{
		m_pSpearLockEffect = m_pParent->CreateEffect( m_pParam->Spear.LockEffectName, Matrix3::FromTo( pEnemy->GetPos(), m_pParent->GetPos() ) );
	}
}

//******************************************************************************
//	���[���[����
//******************************************************************************
void Overdrive::DriveRuler( void )
{
	m_Type			= TYPE_RULER;
	m_State			= STATE_READY;
	m_ReadyEnergy	= 0.0f;
	
	m_pEnergy->Drive( ENERGY_USE_RULER );
	
	//	�C���W�P�[�g�G�t�F�N�g�X�V
	EffectEndAppear( m_pIndicateEffect );
	
	//	�����O�C���W�P�[�g����
	m_pParent->GetWorld()->GetIndicateManager()->AddIndicate( INDICATE_RULER, m_pParent->GetData()->GetIndicateData( INDICATE_RULER ) );
	
	Indicate* pRingIndicate	= m_pParent->GetWorld()->GetIndicateManager()->GetIndicate( INDICATE_RULER );
	if( pRingIndicate )
	{
		pRingIndicate->SetMatrix( m_pParent->GetMatrix() );
	}
}

//******************************************************************************
//	��Ԃ��L�����Z��
//******************************************************************************
void Overdrive::CancelState( void )
{
	//	���b�N�G�t�F�N�g����
	EffectEndAppear( m_pSpearLockEffect );

	//	�C���W�P�[�g����
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
//	�X�s�A�^���[���[���폜
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
//	���R��
//******************************************************************************
float Overdrive::Regenerate( float Energy )
{
	if( IsActive() ) return Energy;

	int Sector = m_pEnergy->GetEnergySectorNum();

	float Ret = m_pEnergy->Regenerate( Energy );

	//	�񕜂���
	if( Sector < m_pEnergy->GetEnergySectorNum() )
	{
		//	�C���W�P�[�g�G�t�F�N�g�X�V
		EffectEndAppear( m_pIndicateEffect );

		//	�񕜃G�t�F�N�g����
		if( m_pParent->IsIndicateEnable() )
		{
			m_pParent->CreateEffect( m_pEnergy->IsEnergyFull()? m_pParam->MaxEffectName : m_pParam->ChargeEffectName, m_pParent->GetMatrix() );
		}
	}

	return Ret;
}

//******************************************************************************
//	�A�C�e���擾
//******************************************************************************
float Overdrive::OnGetItem( float Energy )
{
	//	�������̓G�l���M�[�𒙑�
	if( IsReady() )
	{
		m_ReadyEnergy += Energy;
		return 0.0f;
	}

	int Sector = m_pEnergy->GetEnergySectorNum();

	float Ret = m_pEnergy->OnGetItem( Energy );

	//	�񕜂���
	if( Sector < m_pEnergy->GetEnergySectorNum() )
	{
		//	�C���W�P�[�g�G�t�F�N�g�X�V
		EffectEndAppear( m_pIndicateEffect );

		//	�񕜃G�t�F�N�g����
		if( m_pParent->IsIndicateEnable() )
		{
			m_pParent->CreateEffect( m_pEnergy->IsEnergyFull()? m_pParam->MaxEffectName : m_pParam->ChargeEffectName, m_pParent->GetMatrix() );
		}
	}

	return Ret;
}

//******************************************************************************
//	�X�s�A�����\���H
//******************************************************************************
bool Overdrive::IsSpearEnable( void ) const
{
	return	m_State == STATE_NONE &&
			m_Delay <= 0 &&
			m_pEnergy->IsDriveEnable( ENERGY_USE_SPEAR );
}

//******************************************************************************
//	���[���[�����\���H
//******************************************************************************
bool Overdrive::IsRulerEnable( void ) const
{
	return	m_State == STATE_NONE &&
			m_Delay <= 0 &&
			m_pEnergy->IsDriveEnable( ENERGY_USE_RULER ) && m_RulerList.empty();
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
void Overdrive::_UpdateReady( void )
{
	//	���G���
	_UpdateInvincible();

	//	���b�N�G�t�F�N�g
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
		//	����
		if( m_Type == TYPE_SPEAR )	_ExecSpear();
		if( m_Type == TYPE_RULER )	_ExecRuler();
		
		m_ReadyEnergy = 0.0f;
		m_State = STATE_AFTER;
	}
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
void Overdrive::_UpdateAfter( void )
{
	//	���G���
	_UpdateInvincible();
}

//------------------------------------------------------------------------------
//	���G��Ԃ̍X�V
//------------------------------------------------------------------------------
void Overdrive::_UpdateInvincible( void )
{
	Label InvNotify( m_pParent->GetSprite()->GetAnimeNotify( NOTIFY_KEY_INVINCEBLE ) );

	//	���G����
	if( InvNotify == NOTIFY_VAL_TRUE )
	{
		m_bInvincible = true;
	}
	//	���G����
	else if( InvNotify == NOTIFY_VAL_FALSE )
	{
		m_bInvincible = false;
	}
}

//------------------------------------------------------------------------------
//	�C���W�P�[�g�̍X�V
//------------------------------------------------------------------------------
void Overdrive::_UpdateIndicate( void )
{
	//	�C���W�P�[�g
	Indicate* pIndicate	= m_pParent->GetWorld()->GetIndicateManager()->GetIndicate( INDICATE_OVERDRIVE );

	if( pIndicate )
	{
		pIndicate->SetGauge( INDICATE_BONE_GAUGE,		m_pEnergy->GetEnergySectorRate(),		1.0f );
		pIndicate->SetText( INDICATE_BONE_NUM,			"%1d", m_pEnergy->GetEnergySectorNum() );
		pIndicate->SetColor( INDICATE_BONE_NUM,			INDICATE_NUM_COLOR[ m_pEnergy->GetEnergySectorNum() ] );
	}

	//	�G�t�F�N�g
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
//	�X�s�A����
//------------------------------------------------------------------------------
void Overdrive::_ExecSpear( void )
{
	Spear* pSpear = new Spear( m_pParent );
	MEM_CHECK( pSpear );

	//	���ˈʒu���擾
	Point Pos = m_pParent->GetPos();
	const Matrix3* pBoneMat = m_pParent->GetSprite()->GetBoneMatrix( BONE_SPEAR );
	if( pBoneMat ) Pos = pBoneMat->GetPos();

	//	�^�[�Q�b�g���擾
	Point Target = Pos + Point( 0.0f, -1.0f );
	Enemy* pEnemy = m_pParent->GetWorld()->GetEnemyManager()->GetSpearTarget( m_pParent->GetPos() );
	if( pEnemy ) Target = pEnemy->GetPos();

	//	�}�g���N�X�ݒ�
	Matrix3 Mat = Matrix3::TransRotZ( Pos, (Target - Pos).GetAngle() );
	pSpear->Init( &m_pParam->Spear, Mat, m_ReadyEnergy );

	m_SpearList.push_back( pSpear );

	//	�f�B���C
	m_Delay = m_pParam->Delay;
}

//------------------------------------------------------------------------------
//	���[���[����
//------------------------------------------------------------------------------
void Overdrive::_ExecRuler( void )
{
	Ruler* pRuler = new Ruler( m_pParent );
	MEM_CHECK( pRuler );

	//	�^�[�Q�b�g���擾
	pRuler->Init( &m_pParam->Ruler, m_pParent->GetMatrix(), m_ReadyEnergy );

	m_RulerList.push_back( pRuler );
	
	//	�f�B���C
	m_Delay = m_pParam->Delay;
}

//------------------------------------------------------------------------------
//	�X�s�A�̍X�V
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
//	���[���[�̍X�V
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
//	�I�[���@�h���C���G�l���M�[
//
////////////////////////////////////////////////////////////////////////////////
OverdriveEnergy::OverdriveEnergy( const OVERDRIVE_PARAM* pParam )
{
	m_pParam	= pParam;
	m_Energy	= ENERGY_DEFAULT;
}

//******************************************************************************
//	���Z�b�g
//******************************************************************************
void OverdriveEnergy::Reset( void )
{
	m_Energy = ENERGY_DEFAULT;
}

//******************************************************************************
//	�X�V
//******************************************************************************
void OverdriveEnergy::Update( void )
{
}

//******************************************************************************
//	����
//******************************************************************************
void OverdriveEnergy::Drive( float Use )
{
	if( GetConfig()->GetDebugInfo().bOverdriveInfinite ) return;

	//	����
	DecreaseZero( m_Energy, Use );
}

//******************************************************************************
//	���R��
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
//	�A�C�e���擾
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
//	�G�l���M�[�c�ʊ����𓾂�
//******************************************************************************
float OverdriveEnergy::GetEnergyRate( void ) const
{
	return m_Energy / ENERGY_MAX;
}

//******************************************************************************
//	�G�l���M�[�敪�����𓾂�
//******************************************************************************
float OverdriveEnergy::GetEnergySectorRate( void ) const
{
	if( m_Energy >= ENERGY_MAX ) return 1.0f;

	return fmodf( m_Energy, ENERGY_SECTOR ) / ENERGY_SECTOR;
}

//******************************************************************************
//	�G�l���M�[�敪�̐��𓾂�
//******************************************************************************
int OverdriveEnergy::GetEnergySectorNum( void ) const
{
	return (int)floorf( m_Energy / ENERGY_SECTOR );
}

//******************************************************************************
//	�c�ʂ������\��
//******************************************************************************
bool OverdriveEnergy::IsDriveEnable( float Use ) const
{
	if( GetConfig()->GetDebugInfo().bOverdriveInfinite ) return true;

	return m_Energy >= Use;
}

//******************************************************************************
//	�G�l���M�[�����^����
//******************************************************************************
bool OverdriveEnergy::IsEnergyFull( void ) const
{
	return m_Energy >= ENERGY_MAX;
}