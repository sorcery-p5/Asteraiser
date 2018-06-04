#include "stdafx.h"
#include "AttackBullet.h"

#include "../AttackData.h"
#include "../AttackManager.h"

#include "World/World.h"

#include "Collide/Shape/CollideShapePoint.h"
#include "Collide/Shape/CollideShapeLine.h"
#include "Collide/CollideCaster.h"

#include "Player/Player.h"
#include "Player/Equip/Blade.h"
#include "Player/Equip/Spear.h"
#include "Score/ScoreManager.h"
#include "Ground/Ground.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�̍U���F�e��
//
////////////////////////////////////////////////////////////////////////////////
AttackBullet::AttackBullet( AttackManager* pManager ) : Attack( pManager )
{
	m_pParam		= NULL;

	m_Speed			= 0.0f;
	m_RotSpeed		= 0.0f;
}
AttackBullet::~AttackBullet()
{
	
}

//******************************************************************************
//	�^�C���L�[�p�����[�^�̓K�p�i�P�Ɓj
//******************************************************************************
void AttackBullet::ApplyTimeKey( ATTACK_TIME_KEY_TYPE Type, const ATTACK_TIME_KEY_PARAM& Key )
{
	switch( Type )
	{
	case ATTACK_TIME_KEY_SPEED:
		m_Speed = Key.Val[0];
		return;
	case ATTACK_TIME_KEY_ROTSPEED:
		m_RotSpeed = Key.Val[0];
		return;
	}

	__super::ApplyTimeKey( Type, Key );
}

//******************************************************************************
//	�^�C���L�[�p�����[�^�̓K�p�i��ԁj
//******************************************************************************
void AttackBullet::ApplyTimeKey( ATTACK_TIME_KEY_TYPE Type, const ATTACK_TIME_KEY_PARAM& Key, const ATTACK_TIME_KEY_PARAM& Next, float f )
{
	switch( Type )
	{
	case ATTACK_TIME_KEY_SPEED:
		m_Speed = Lerp( Key.Val[0], Next.Val[0], f );
		return;
	case ATTACK_TIME_KEY_ROTSPEED:
		m_RotSpeed = Lerp( Key.Val[0], Next.Val[0], f );
		return;
	}

	__super::ApplyTimeKey( Type, Key, Next, f );
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
void AttackBullet::_OnInit( void )
{
	//	�p�����[�^
	m_pParam	= &m_pData->GetParam().Bullet;

	m_Speed		= m_pManager->GetRand().GetRandRange( m_pParam->Speed, m_pParam->SpeedRange );
	m_RotSpeed	= m_pManager->GetRand().GetRandRange( m_pParam->RotSpeed, m_pParam->RotSpeedRange );
}

//------------------------------------------------------------------------------
//	�X�V
//------------------------------------------------------------------------------
void AttackBullet::_OnUpdate( void )
{
	Matrix3 Mat = GetMatrix();

	//	��]��������
	if( m_RotSpeed != 0.0f )
	{
		Mat.SetRotZ( Mat.GetRotZ() + m_RotSpeed );
	}

	//	���i
	Mat.SetPos( Mat.GetPos() + Mat.GetAxisY() * m_Speed );

	//	����
	m_Speed *= m_pParam->Decel;

	SetMatrix( Mat );
}

//------------------------------------------------------------------------------
//	�I���t���[���ɓ��B
//------------------------------------------------------------------------------
void AttackBullet::_OnEndFrame( void )
{
	//	�U���̐���
	_AppearAttack( _GetAppearAttack() );

	//	�I���G�t�F�N�g�o��
	Label EndEffect = _GetEndEffect();
	if( !EndEffect.IsEmpty() )
	{
		_AppearEffect( EndEffect );
	}
}

//------------------------------------------------------------------------------
//	��ʊO�ɏo��
//------------------------------------------------------------------------------
void AttackBullet::_OnOutField( void )
{
	//	�I���G�t�F�N�g�o��
	Label EndEffect = _GetEndEffect();
	if( !EndEffect.IsEmpty() )
	{
		_AppearEffect( EndEffect );
	}
}

//------------------------------------------------------------------------------
//	������`��쐬
//------------------------------------------------------------------------------
CollideShape* AttackBullet::_CreateCollideShape( void ) const
{
	if( _GetLength() == 0.0f )
	{
		return new CollideShapePoint( _GetRadius() );
	}
	else
	{
		return new CollideShapeLine( -_GetLength(), _GetFrontLength(), _GetRadius() );
	}
}

//------------------------------------------------------------------------------
//	������L���X�g�ɓ�������
//------------------------------------------------------------------------------
bool AttackBullet::_OnCastHit( const COLLIDE_RESULT& Result )
{
	enum
	{
		RESULT_HIT		= (1 << 0),	//	����
		RESULT_ERASE	= (1 << 1),	//	�폜����
		RESULT_APPEAR	= (1 << 2),	//	�h���U���𐶐�����
	};

	uint	Ret		= 0;

	switch( Result.pOwner->Type )
	{
	case COLLIDE_OWNER_PLAYER:
		{
			Player* pPlayer = (Player*)Result.pOwner->pData;
			if( pPlayer->Damage( m_pData->GetParam().Damage, _GetHitVel(Result.Normal) * m_pData->GetParam().HitVel ) )
			{
				Ret |= (RESULT_HIT | RESULT_APPEAR);

				if( !_IsPierce() )
				{
					Ret |= RESULT_ERASE;
				}
			}
		}
		break;
	case COLLIDE_OWNER_BLADE:
		{
			Blade* pBlade = (Blade*)Result.pOwner->pData;
			if( pBlade->OnHitAttack( m_pData->GetParam().Exhaust, IsRepulse(), Result.HitPos ) )
			{
				//	�X�R�A�ʒm
				m_pManager->GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_BULLET_ERASE, m_pData->GetParam().ScoreRate );

				Ret |= (RESULT_HIT | RESULT_ERASE);
			}
		}
		break;
	case COLLIDE_OWNER_SPEAR:
		{
			Spear* pSpear = (Spear*)Result.pOwner->pData;
			if( pSpear->OnHitAttack( m_pData->GetParam().Exhaust, Result.HitPos ) )
			{
				Ret |= (RESULT_HIT | RESULT_ERASE);
			}
		}
		break;
	case COLLIDE_OWNER_RULER:
		{
			Ret |= RESULT_ERASE;
			
			if( !_IsPierce() )
			{
				Ret |= RESULT_HIT;
			}
		}
		break;
	case COLLIDE_OWNER_GROUND:
		{
			Ground* pGround = (Ground*)Result.pOwner->pData;

			if( !_IsPierce() )
			{
				Ret |= (RESULT_APPEAR);
				
				//	�q�b�g�G�t�F�N�g�o�����ǂ���
				if( pGround->IsAttackHitEffect() )
				{
					Ret |= RESULT_HIT;
				}

				//	�U�����������邩�ǂ���
				if( pGround->IsAttackErase() )
				{
					Ret |= RESULT_ERASE;
				}
				
				//	�n�`���̃G�t�F�N�g
				pGround->CreateHitEffect( Result.GetHitMatrix() );
			}
		}
		break;
	}

	if( Ret & RESULT_HIT )
	{
		//	�U������
		if( Ret & RESULT_APPEAR && 
			!_GetAppearAttack().IsEmpty() )
		{
			_AppearAttack( _GetAppearAttack() );
		}
		else
		{
			//	�q�b�g�G�t�F�N�g�o��
			Label HitEffect( m_pData->GetParam().HitEffectName );
			if( !HitEffect.IsEmpty() )
			{
				_AppearEffect( HitEffect, Result.GetHitMatrix( PI ) );
			}
		}
	}
	
	//	�폜����
	if( Ret & RESULT_ERASE )
	{
		//	�I���G�t�F�N�g�o��
		Label EndEffect = _GetEndEffect();
		if( !EndEffect.IsEmpty() )
		{
			_AppearEffect( EndEffect );
		}

		Erase();
	}

	return (Ret & RESULT_ERASE) == 0;
}

//------------------------------------------------------------------------------
//	�����蔻��̒�����ύX
//------------------------------------------------------------------------------
void AttackBullet::_SetHitLength( float Len )
{
	if( _GetCollideCaster() )
	{
		CollideShape* pShape = _GetCollideCaster()->GetShape();
		if( pShape ) pShape->SetLength( 0.0f, Len );
	}
}