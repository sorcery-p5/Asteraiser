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
//	攻撃管理
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
//	更新
//******************************************************************************
void AttackManager::Update( void )
{
	//	攻撃
	for( AttackList::iterator it = m_AttackList.begin(); it != m_AttackList.end(); ++it )
	{
		(*it)->Update();
	}

	//	削除チェック
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
	
	//	スコア通知
	if( m_AttackList.empty() )
	{
		GetWorld()->GetScoreManager()->StopTimeValue( SCORE_TIME_VALUE_ATTACK_EXIST );
	}
}

//******************************************************************************
//	攻撃を新規追加
//******************************************************************************
VecAttack AttackManager::AddAttack( const AttackData* pData, Matrix3 Mat )
{
	VecAttack vAttack;

	_ASSERT( pData );

	//	角度に幅を付ける
	if( pData->GetParam().AngleRange != 0.0f )
	{
		Mat = Matrix3::RotZ( GetRand().GetRandRange(0.0f, pData->GetParam().AngleRange) ) * Mat;
	}

	//	同時発生
	int Num = Max( pData->GetParam().Conncurrent.Num, 1 );
	for( int i = 0; i < Num; i++ )
	{
		Attack* pAttack = _CreateAttack( pData, _GetConcurrentMatrix( pData->GetParam().Conncurrent, i, Mat ) );

		m_AttackList.push_back( pAttack );
		vAttack.push_back( pAttack );
	}

	//	発生エフェクト
	if( !pData->GetParam().AppearEffectName.IsEmpty() )
	{
		const EffectFile* pEffectFile = pData->GetEffectFile( pData->GetParam().AppearEffectName );
		if( pEffectFile ) GetApp()->GetEffectMng()->CreateEffect( pEffectFile, Mat );
	}
	
	//	発生サウンド
	if( !pData->GetParam().AppearSoundName.IsEmpty() )
	{
		const SoundFile* pSoundFile = pData->GetSoundFile( pData->GetParam().AppearSoundName );
		if( pSoundFile ) GetTone()->PlaySound( pSoundFile, SOUND_CHANNEL_SE, false, pData->GetParam().AppearSoundVol );
	}

	return vAttack;
}

//------------------------------------------------------------------------------
//	攻撃の生成
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
//	同時発生用のマトリクスを計算
//------------------------------------------------------------------------------
Matrix3 AttackManager::_GetConcurrentMatrix( const ATTACK_CONCURRENT& Param, int Index, const Matrix3& Mat )
{
	float Rot = Mat.GetRotZ();

	//	最初の一発は中心
	if( Index > 0 )
	{
		//	オフセット角度の算出
		int	Sign = (Index % 2) * 2 - 1;
		int Step = (Index + 1) / 2;

		Rot += (float)(Sign * Step) * Param.OffsetAngle;
	}
	
	//	ランダム角度を付与
	if( Param.AngleRange != 0.0f )
	{
		Rot = GetRand().GetRandRange( Rot, Param.AngleRange );
	}

	return Matrix3::TransRotZ( Mat.GetPos(), Rot );
}