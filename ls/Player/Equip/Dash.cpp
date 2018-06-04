#include "stdafx.h"
#include "Dash.h"

#include "Player/Player.h"
#include "Player/PlayerData.h"
#include "Player/Equip/Blade.h"

#include "SpriteFrame/SpriteFrame.h"
#include "Collide/CollideSystem.h"
#include "Collide/CollideCaster.h"
#include "Collide/Shape/CollideShapeLine.h"

#include "World/World.h"
#include "Score/ScoreManager.h"

namespace
{
	const Label		NOTIFY_KEY_INVINCEBLE( "Invincible" );
	const Label		NOTIFY_VAL_TRUE( "true" );
	const Label		NOTIFY_VAL_FALSE( "false" );

	const Label		NOTIFY_KEY_WARP( "Warp" );
	const Label		NOTIFY_KEY_END( "End" );
}


///////////////////////////////////////////////////////////////////////////////
//
//	�_�b�V��
//
///////////////////////////////////////////////////////////////////////////////
Dash::Dash( void )
{
	m_pParent		= NULL;
	m_pParam		= NULL;
	m_pCaster		= NULL;

	m_State				= STATE_NORMAL;
	m_Delay				= 0;
	m_bInvincible		= false;
	m_bSplash			= false;
	m_InvincibleCount	= 0;

	m_bEndNotify		= false;
}
Dash::~Dash()
{
	SafeDelete( m_pCaster );
}

//******************************************************************************
//	������
//******************************************************************************
void Dash::Init( Player* pPlayer, const DASH_PARAM* pParam )
{
	_ASSERT( pPlayer && pParam );
	m_pParent		= pPlayer;
	m_pParam		= pParam;
	
	m_State				= STATE_NORMAL;
	m_Delay				= 0;
	m_bInvincible		= false;
	m_bSplash			= false;
	m_bEndNotify		= false;
	m_InvincibleCount	= 0;

	//	������L���X�^�[�쐬
	m_pCaster = new CollideCaster;
	MEM_CHECK( m_pCaster );
	m_pCaster->Init( new CollideShapeLine( m_pParam->MoveLength, m_pParam->Splash.CollideRadius ) );
	m_pCaster->SetFilter( COLLIDE_FILTER_SPLASH );
	m_pCaster->SetCalcType( COLLIDE_CALC_POS_NORMAL );
}

//******************************************************************************
//	���Z�b�g
//******************************************************************************
void Dash::Reset( void )
{
	m_State				= STATE_NORMAL;
	m_Delay				= 0;
	m_bInvincible		= false;
	m_bSplash			= false;
	m_InvincibleCount	= 0;
}

//******************************************************************************
//	�X�V
//******************************************************************************
void Dash::Update( void )
{
	//	�J�E���g�_�E��
	DecreaseZero( m_InvincibleCount, 1 );

	switch( m_State )
	{
	case STATE_NORMAL:	_UpdateNormal();	break;
	case STATE_READY:	_UpdateReady();		break;
	case STATE_AFTER:	_UpdateAfter();		break;
	}
}

//******************************************************************************
//	�J�n
//******************************************************************************
void Dash::Start( Point Dir )
{
	_ASSERT( !Dir.IsZero() );

	m_MoveDir		= Dir;
	m_State			= STATE_READY;
	m_Delay			= m_pParent->IsRulerHit()? m_pParam->RulerDelay : m_pParam->Delay;
	m_bInvincible	= false;
	m_bSplash		= false;
	
	//	�G�t�F�N�g����
	m_pParent->CreateEffect( m_pParam->EffectName, Matrix3::TransRotZ( m_pParent->GetPos(), m_MoveDir.GetAngle() ) );
}

//******************************************************************************
//	��������
//******************************************************************************
void Dash::Splash( Point Dir )
{
	_ASSERT( !Dir.IsZero() );

	m_MoveDir		= Dir;
	m_State			= STATE_READY;
	m_Delay			= m_pParent->IsRulerHit()? m_pParam->RulerDelay : m_pParam->Delay;
	m_bInvincible	= false;
	m_bSplash		= true;
	
	//	�G�t�F�N�g����
	m_pParent->CreateEffect( m_pParam->Splash.EffectName, Matrix3::TransRotZ( m_pParent->GetPos(), m_MoveDir.GetAngle() ) );
}

//******************************************************************************
//	��ԃL�����Z��
//******************************************************************************
void Dash::CancelState( void )
{
	m_State			= STATE_NORMAL;
	m_bInvincible	= false;
	m_bSplash		= false;
	m_bEndNotify	= false;
}

//------------------------------------------------------------------------------
//	�ʏ���
//------------------------------------------------------------------------------
void Dash::_UpdateNormal( void )
{
	//	�J�E���g�_�E��
	DecreaseZero( m_Delay, 1 );
}

//------------------------------------------------------------------------------
//	�����O
//------------------------------------------------------------------------------
void Dash::_UpdateReady( void )
{
	const SpriteFrame* pSprite = m_pParent->GetSprite();

	//	���G����
	Label InvNotify( pSprite->GetAnimeNotify( NOTIFY_KEY_INVINCEBLE ) );
	if( InvNotify == NOTIFY_VAL_TRUE )
	{
		m_bInvincible = true;
	}

	//	���[�v����
	if( pSprite->GetAnimeNotify( NOTIFY_KEY_WARP ) )
	{
		_ExecWarp();

		m_State = STATE_AFTER;
	}
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
void Dash::_UpdateAfter( void )
{
	const SpriteFrame* pSprite = m_pParent->GetSprite();

	//	���G����
	Label InvNotify( pSprite->GetAnimeNotify( NOTIFY_KEY_INVINCEBLE ) );
	if( InvNotify == NOTIFY_VAL_FALSE )
	{
		m_bInvincible = false;
	}

	//	�I���ʒm
	if( pSprite->GetAnimeNotify( NOTIFY_KEY_END ) )
	{
		m_bEndNotify = true;
	}
}

//------------------------------------------------------------------------------
//	���[�v����
//------------------------------------------------------------------------------
void Dash::_ExecWarp( void )
{
	Point PrevPos = m_pParent->GetPos();

	//	�ʒu�ړ�
	m_pParent->SetPos( PrevPos + m_MoveDir * m_pParam->MoveLength );

	//	������������
	if( m_bSplash )
	{
		//	�u���[�h�֒ʒm
		m_pParent->GetBlade()->OnSplash();

		_CastSplash( PrevPos );
	}
}

//------------------------------------------------------------------------------
//	����������������s
//------------------------------------------------------------------------------
void Dash::_CastSplash( Point Pos )
{
	Blade* pBlade = m_pParent->GetBlade();
	_ASSERT( pBlade );

	//	�傫���ݒ�
	float Rad = m_pParent->IsRulerHit()? m_pParam->Splash.RulerRadius : m_pParam->Splash.CollideRadius;
	m_pCaster->GetShape()->SetRadius( Rad );

	//	�ʒu�ݒ�
	m_pCaster->UpdateMatrix( &Matrix3::TransRotZ( Pos, m_MoveDir.GetAngle() ) );

	//	����
	bool bHit = false;
	COLLIDE_CAST_RESULT Result;
	if( m_pCaster->Cast( Result ) )
	{
		for( COLLIDE_RESULT_LIST::iterator it = Result.ResultList.begin(); it != Result.ResultList.end(); ++it )
		{
			const COLLIDE_RESULT& Ret = *it;

			switch( Ret.pOwner->Type )
			{
			case COLLIDE_OWNER_ENEMY:
				{
					if( pBlade->OnHitEnemy( (Enemy*)(Ret.pOwner->pData), 
											m_pParam->Splash.Attack * pBlade->GetAttackRate(), m_MoveDir * m_pParam->Splash.AttackVel,
											m_pParam->Splash.Knockback, false, true,
											Ret.GetHitMatrix(), m_pParam->Splash.HitEffectName ) )
					{
						//	����
						m_InvincibleCount = Max( m_InvincibleCount, m_pParam->Splash.HitInvincible );
						bHit = true;
					}
				}
				break;

			case COLLIDE_OWNER_ITEM:
				{
					pBlade->OnHitItem( (Item*)(Ret.pOwner->pData), Ret.HitPos );
				}
				break;
			}
		}
	}

	//	��U��
	if( !bHit )
	{
		//	�X�R�A�ʒm
		m_pParent->GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_MISS );
	}
}