#include "stdafx.h"
#include "AttackManager.h"
#include "AttackData.h"
#include "Attack.h"

#include "Impl/AttackBullet.h"
#include "Impl/AttackHoming.h"
#include "Impl/AttackLaser.h"
#include "Impl/AttackBlade.h"
#include "Impl/AttackShield.h"
#include "Impl/AttackRuler.h"

#include "App/App.h"
#include "Effect/EffectManager.h"

#include "World/World.h"
#include "Score/ScoreManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�U���Ǘ�
//
////////////////////////////////////////////////////////////////////////////////
AttackManager::AttackManager( World* pWorld ) : m_Rand(0)
{
	_ASSERT( pWorld );

	m_pWorld = pWorld;
}
AttackManager::~AttackManager()
{
	DeleteVec( m_AttackList );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void AttackManager::Update( void )
{
	//	�U��
	for( AttackList::iterator it = m_AttackList.begin(); it != m_AttackList.end(); ++it )
	{
		(*it)->Update();
	}

	//	�폜�`�F�b�N
	for( AttackList::iterator it = m_AttackList.begin(); it != m_AttackList.end(); )
	{
		if( (*it)->IsDelete() )
		{
			SafeDelete( *it );
			it = m_AttackList.erase( it );
		}
		else
		{
			++it;
		}
	}
	
	//	�X�R�A�ʒm
	if( m_AttackList.empty() )
	{
		GetWorld()->GetScoreManager()->StopTimeValue( SCORE_TIME_VALUE_ATTACK_EXIST );
	}
}

//******************************************************************************
//	�U����V�K�ǉ�
//******************************************************************************
VecAttack AttackManager::AddAttack( const AttackData* pData, Matrix3 Mat )
{
	VecAttack vAttack;

	_ASSERT( pData );

	//	�p�x�ɕ���t����
	if( pData->GetParam().AngleRange != 0.0f )
	{
		Mat = Matrix3::RotZ( GetRand().GetRandRange(0.0f, pData->GetParam().AngleRange) ) * Mat;
	}

	//	��������
	int Num = Max( pData->GetParam().Conncurrent.Num, 1 );
	for( int i = 0; i < Num; i++ )
	{
		Attack* pAttack = _CreateAttack( pData, _GetConcurrentMatrix( pData->GetParam().Conncurrent, i, Mat ) );

		m_AttackList.push_back( pAttack );
		vAttack.push_back( pAttack );
	}

	//	�����G�t�F�N�g
	if( !pData->GetParam().AppearEffectName.IsEmpty() )
	{
		const EffectFile* pEffectFile = pData->GetEffectFile( pData->GetParam().AppearEffectName );
		if( pEffectFile ) GetApp()->GetEffectMng()->CreateEffect( pEffectFile, Mat );
	}
	
	//	�����T�E���h
	if( !pData->GetParam().AppearSoundName.IsEmpty() )
	{
		const SoundFile* pSoundFile = pData->GetSoundFile( pData->GetParam().AppearSoundName );
		if( pSoundFile ) GetTone()->PlaySound( pSoundFile, SOUND_CHANNEL_SE, false, pData->GetParam().AppearSoundVol );
	}

	return vAttack;
}

//------------------------------------------------------------------------------
//	�U���̐���
//------------------------------------------------------------------------------
Attack* AttackManager::_CreateAttack( const AttackData* pData, const Matrix3& Mat )
{
	Attack* pAttack = NULL;
	
	switch( pData->GetParam().Type )
	{
	case ATTACK_TYPE_BULLET:	pAttack	= new AttackBullet( this );	break;
	case ATTACK_TYPE_HOMING:	pAttack	= new AttackHoming( this );	break;
	case ATTACK_TYPE_LASER:		pAttack	= new AttackLaser( this );	break;
	case ATTACK_TYPE_BLADE:		pAttack	= new AttackBlade( this );	break;
	case ATTACK_TYPE_SHIELD:	pAttack	= new AttackShield( this );	break;
	case ATTACK_TYPE_RULER:		pAttack	= new AttackRuler( this );	break;
	}

	MEM_CHECK( pAttack );

	pAttack->Init( pData, Mat );

	return pAttack;
}

//------------------------------------------------------------------------------
//	���������p�̃}�g���N�X���v�Z
//------------------------------------------------------------------------------
Matrix3 AttackManager::_GetConcurrentMatrix( const ATTACK_CONCURRENT& Param, int Index, const Matrix3& Mat )
{
	float Rot = Mat.GetRotZ();

	//	�ŏ��̈ꔭ�͒��S
	if( Index > 0 )
	{
		//	�I�t�Z�b�g�p�x�̎Z�o
		int	Sign = (Index % 2) * 2 - 1;
		int Step = (Index + 1) / 2;

		Rot += (float)(Sign * Step) * Param.OffsetAngle;
	}
	
	//	�����_���p�x��t�^
	if( Param.AngleRange != 0.0f )
	{
		Rot = GetRand().GetRandRange( Rot, Param.AngleRange );
	}

	return Matrix3::TransRotZ( Mat.GetPos(), Rot );
}