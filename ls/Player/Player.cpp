#include "stdafx.h"
#include "Player.h"
#include "PlayerData.h"
#include "PlayerManager.h"

#include "Equip/Blade.h"
#include "Equip/Overdrive.h"
#include "Equip/Dash.h"
#include "Equip/Shield.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"

#include "World/World.h"
#include "Config/Config.h"
#include "Controller/Controller.h"

#include "Enemy/Enemy.h"

#include "SpriteFrame/SpriteFrame.h"
#include "SpriteFrame/SpriteFrameFile.h"
#include "Collide/CollideSystem.h"
#include "Effect/EffectManager.h"
#include "Effect/Effect.h"
#include "Item/ItemManager.h"
#include "Item/Item.h"
#include "Score/ScoreManager.h"
#include "Indicate/Indicate.h"
#include "Indicate/IndicateManager.h"


namespace
{
	const Label	BONE_NAME_BODY					= "Body";
	const Label	BONE_NAME_BLADE					= "Blade";

	const Player::ANIME_INFO ANIME_WAIT			= { "Wait",		10	};
	const Player::ANIME_INFO ANIME_MOVE_F		= { "MoveF",	10	};
	const Player::ANIME_INFO ANIME_MOVE_S		= { "MoveS",	10	};
	const Player::ANIME_INFO ANIME_MOVE_B		= { "MoveB",	10	};
	const Player::ANIME_INFO ANIME_CHARGE_F		= { "ChargeF",	3	};
	const Player::ANIME_INFO ANIME_CHARGE_S		= { "ChargeS",	3	};
	const Player::ANIME_INFO ANIME_CHARGE_B		= { "ChargeB",	3	};
	const Player::ANIME_INFO ANIME_BLADE_F		= { "BladeF",	3	};
	const Player::ANIME_INFO ANIME_BLADE_S		= { "BladeS",	3	};
	const Player::ANIME_INFO ANIME_BLADE_B		= { "BladeB",	3	};
	const Player::ANIME_INFO ANIME_SMASH_F		= { "SmashF",	3	};
	const Player::ANIME_INFO ANIME_SMASH_S		= { "SmashS",	3	};
	const Player::ANIME_INFO ANIME_SMASH_B		= { "SmashB",	3	};
	const Player::ANIME_INFO ANIME_RELOAD		= { "Reload",	5	};
	const Player::ANIME_INFO ANIME_CANCEL		= { "Cancel",	5	};
	const Player::ANIME_INFO ANIME_DASH_N		= { "DashN",	0	};
	const Player::ANIME_INFO ANIME_DASH_S		= { "DashS",	0	};
	const Player::ANIME_INFO ANIME_SPLASH_N		= { "SplashN",	0	};
	const Player::ANIME_INFO ANIME_SPLASH_S		= { "SplashS",	0	};
	const Player::ANIME_INFO ANIME_SPEAR		= { "Spear",	0	};
	const Player::ANIME_INFO ANIME_RULER		= { "Ruler",	0	};
	const Player::ANIME_INFO ANIME_KNOCK		= { "Knock",	0	};
	const Player::ANIME_INFO ANIME_DEAD			= { "Dead",		0	};
	const Player::ANIME_INFO ANIME_REVIVE		= { "Revive",	0	};

	const float	DAMAGE_COEF_NORMAL	= 1.0f;
	const float	DAMAGE_COEF_BLADE	= 1.25f;
	const float	DAMAGE_COEF_CANCEL	= 1.25f;
	const float	DAMAGE_COEF_RELOAD	= 1.5f;
	const float	DAMAGE_COEF_OVER	= 2.0f;
	const float	DAMAGE_COEF_DASH	= 1.25f;
	const float	DAMAGE_COEF_CLASH	= 1.8f;
	
	const Label STATE_NAME_ARRAY[] =
	{
		"Normal",
		"Charge",
		"Blade",
		"Repulse",	
		"Reload",
		"Cancel",
		"Dash",
		"Overdrive",
		"Knock",
		"Dead",
	};
	
	const Label ACTION_NAME_ARRAY[] =
	{
		"Blade",
		"Charge",
		"Reload",
		"Cancel",
		"Dash",
		"Smash",
		"Splash",
		"Spear",
		"Ruler",
	};

	const int		ACTION_CHARGE_TIME	= 20;

	const Color		COLOR_INVINCIBLE = Color( 160, 255, 220 );
	
	const int		INDICATE_RING_BONE_NUM		= 8;
	const Label		INDICATE_RING_DASH			= "RingDash";
	const Label		INDICATE_BONE_COLLECT		= "Collect";
	pstr			INDICATE_BONE_MARKER_FMT	= "Marker%d";

	const Color		COLOR_INDICATE_BLACK		= Color( 255,255,255,40 );
	const Color		COLOR_INDICATE_NORMAL		= Color( 255,255,255,104 );
	const Color		COLOR_INDICATE_CHARGE		= Color( 128,192,255,255 );
	const Color		COLOR_INDICATE_COLLECT_MIN	= Color( 255,255,255,128 );
	const Color		COLOR_INDICATE_COLLECT_MAX	= Color( 255,255,255,255 );

	const float		INDICATE_COLLECT_SCALE		= 0.6f;
}


///////////////////////////////////////////////////////////////////////////////
//
//	�v���C���[
//
///////////////////////////////////////////////////////////////////////////////
Player::Player( PlayerManager* pManager )
{
	_ASSERT( m_pManager );

	m_pManager			= pManager;
	m_pData				= NULL;
	m_PlayerID			= 0;
	
	m_pSprite			= NULL;
	m_pCollide			= NULL;
	m_pCaster			= NULL;

	m_State				= STATE_NORMAL;
	m_StateCount		= 0;
	m_InvincibleCount	= 0;
	m_Side				= SIDE_R;

	m_pBlade			= NULL;
	m_pOverdrive		= NULL;
	m_pDash				= NULL;
	m_pShield			= NULL;

	m_bMoveEnable		= true;
	m_bAttackEnable		= true;
	m_bInvincible		= false;
	m_bIndicateEnable	= true;
	m_bRulerHit			= false;
	m_bEnemyRulerHit	= false;

	m_bAnimeReplace		= false;
	m_ActionLock		= 0;
}

Player::~Player()
{
	EffectEndAppear( m_pRulerEffect );

	SafeDelete( m_pBlade );
	SafeDelete( m_pOverdrive );
	SafeDelete( m_pDash );
	SafeDelete( m_pShield );

	SafeDelete( m_pCollide );
	SafeDelete( m_pCaster );
	SafeDelete( m_pSprite );
}

//******************************************************************************
//	������
//******************************************************************************
void Player::Init( const PlayerData* pData, int PlayerID, Point Pos )
{
	_ASSERT( pData );

	m_pData		= pData;
	m_PlayerID	= PlayerID;

	m_State				= STATE_NORMAL;
	m_StateCount		= 0;
	m_InvincibleCount	= 0;
	m_Side				= SIDE_R;
	m_Pos				= Pos;

	//	�X�v���C�g������
	const SpriteFrameFile* pSpriteFile = m_pData->GetSpriteFrameFile( m_pData->GetParam().SpriteName );
	m_pSprite = new SpriteFrame;
	MEM_CHECK( m_pSprite && pSpriteFile );

	m_pSprite->Init( pSpriteFile, _GetSpriteMatrix() );
	
	//	�����蔻�萶��
	m_pCollide		= new CollideObject;
	m_pCaster		= new CollideCaster;
	MEM_CHECK( m_pCollide && m_pCaster );

	m_pCollide->Init( new CollideShapePoint( m_pData->GetParam().HitRadius ), COLLIDE_LAYER_PLAYER );
	m_pCollide->SetOwner( COLLIDE_OWNER_PLAYER, this );
	m_pCollide->UpdateMatrix( &GetMatrix() );
	m_pCollide->SetListener( this );

	m_pCaster->Init( new CollideShapePoint( m_pData->GetParam().HitRadius ) );
	m_pCaster->SetFilter( COLLIDE_FILTER_PLAYER );
	m_pCaster->SetOwner( COLLIDE_OWNER_PLAYER, this );
	m_pCaster->SetCalcType( COLLIDE_CALC_ALL );

	//	�����̏�����
	m_pBlade		= new Blade;
	m_pOverdrive	= new Overdrive;
	m_pDash			= new Dash;
	m_pShield		= new Shield;
	MEM_CHECK( m_pBlade && m_pOverdrive && m_pDash && m_pShield );

	m_pBlade->Init( this, &m_pData->GetParam().Blade );
	m_pOverdrive->Init( this, &m_pData->GetParam().Overdrive );
	m_pDash->Init( this, &m_pData->GetParam().Dash );
	m_pShield->Init( this, &m_pData->GetParam().Shield );

	//	�C���W�P�[�g
	GetWorld()->GetIndicateManager()->AddIndicate( INDICATE_RING_DASH, m_pData->GetIndicateData( INDICATE_RING_DASH ) );

	//	�����A�j��
	_ChangeAnime( ANIME_WAIT );
}

//*****************************************************************************
//	�X�V
//*****************************************************************************
void Player::Update( void )
{
	//	�J�E���g
	m_StateCount++;
	DecreaseZero( m_InvincibleCount, 1 );

	//	�ړ��E�ʒu���X�V
	_UpdateMove();
	
	//	���[���[�t���O���Ƃ�
	if( m_bRulerHit ) m_bRulerHit = false;

	//	�����蔻��
	_CastCollide();

	//	���[���[���
	_UpdateRuler();

	//	�X�v���C�g�X�V�E�`��
	m_pSprite->Pause( _IsPauseSprite() );
	m_pSprite->Update( &_GetSpriteMatrix() );

	//	�A�j�������ւ�
	if( m_bAnimeReplace && m_pSprite->IsAnimeLast() )
	{
		m_bAnimeReplace = false;
	}

	//	������ʒu�X�V
	m_pCollide->UpdateMatrix( &GetMatrix() );

	//	���R��
	_Regenerate();

	//	�����̍X�V
	m_pBlade->Update();
	m_pOverdrive->Update();
	m_pDash->Update();
	m_pShield->Update();

	//	�A�N�V�����K�p�̍X�V
	if( _UpdateApplyAction() )
	{
		return;
	}

	//	��ԍX�V
	switch( m_State )
	{
	case STATE_NORMAL:		_UpdateNormal();		break;
	case STATE_CHARGE:		_UpdateCharge();		break;
	case STATE_BLADE:		_UpdateBlade();			break;
	case STATE_REPULSE:		_UpdateRepulse();		break;
	case STATE_RELOAD:		_UpdateReload();		break;
	case STATE_CANCEL:		_UpdateCancel();		break;
	case STATE_DASH:		_UpdateDash();			break;
	case STATE_OVERDRIVE:	_UpdateOverdrive();		break;
	case STATE_KNOCK:		_UpdateKnock();			break;
	case STATE_DEAD:		_UpdateDead();			break;
	case STATE_REVIVE:		_UpdateRevive();		break;
	}

	//	�C���W�P�[�g
	_UpdateIndicate();

	//	�F�̕ύX
	Color Col = Color::White(); 
	if( m_pShield->IsInvincible() )
	{
		Col = COLOR_INVINCIBLE;
	}
	m_pSprite->SetColor( Col );
}

//******************************************************************************
//	�`��
//******************************************************************************
void Player::Draw( void )
{
	m_pSprite->Draw( GetWorld()->GetDrawOffset() );
}

//******************************************************************************
//	�_���[�W
//******************************************************************************
bool Player::Damage( float Attack, Point Dir, bool bThroughInvincible )
{
	if( Attack < 0 ) return true;

	return _Damage( Attack, Dir, bThroughInvincible );
}

//******************************************************************************
//	�A�C�e���擾
//******************************************************************************
void Player::OnGetItem( Item* pItem )
{
	if( IsDead() ) return;

	float Energy = m_pData->GetParam().ItemEnergy;

	if( m_pOverdrive->IsReady() )
	{
		//	�I�[���@�h���C��������
		m_pOverdrive->OnGetItem( Energy );
	}
	else
	{
		//	�V�[���h
		Energy = m_pShield->OnGetItem( Energy );

		//	�u���[�h
		Energy = m_pBlade->OnGetItem( Energy );

		//	�I�[���@�h���C��
		Energy = m_pOverdrive->OnGetItem( Energy );

		//	�X�R�A�ʒm
		GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_GET_ITEM );
		GetWorld()->GetScoreManager()->OnGetItem();
	}

	//	�A�C�e���擾�ʒm
	pItem->OnGet();
}

//******************************************************************************
//	���[���[���j������
//******************************************************************************
void Player::OnCrashRuler( void )
{
	m_pBlade->OnCrashRuler();
}

//******************************************************************************
//	����
//******************************************************************************
void Player::Revive( void )
{
	//	�A�j��
	_ChangeAnime( ANIME_REVIVE );

	//	������������
	SafePtr( m_pBlade )->Reset();
	SafePtr( m_pDash )->Reset();
	SafePtr( m_pOverdrive )->Reset();
	SafePtr( m_pShield )->Reset();
	SafePtr( m_pShield )->OnRevive();

	//	��~
	m_Vel.Zero();

	_ChangeState( STATE_REVIVE );
}

//******************************************************************************
//	�A�j���̍����ւ�
//******************************************************************************
void Player::ReplaceAnime( Label Name, int Intp )
{
	ANIME_INFO Info;
	Info.Name	= Name;
	Info.Intp	= Intp;

	m_ReplaceAnimeQueue.push_back( Info );
}

//******************************************************************************
//	�A�N�V�����̎��s
//******************************************************************************
void Player::ApplyAction( Label Name, Point Intp )
{
	ACTION_INFO Info;
	Info.Action	= _LabelToAction( Name );
	Info.Dir	= Intp;

	m_ApplyActionQueue.push_back( Info );
}

//******************************************************************************
//	�A�N�V�����̐�����ݒ�
//******************************************************************************
void Player::LockAction( Label Name, bool bEnable )
{
	ACTION Act = _LabelToAction( Name );

	//	�����ݒ�
	if( bEnable )
	{
		if( Act == ACTION_ALL )
		{
			m_ActionLock = ACTION_ALL;
		}
		else
		{
			m_ActionLock |= (1 << Act);
		}
	}
	//	��������
	else
	{
		if( Act == ACTION_ALL )
		{
			m_ActionLock = 0;
		}
		else
		{
			m_ActionLock &= ~(1 << Act);
		}
	}
}

//******************************************************************************
//	�ʒu�̈ړ�
//******************************************************************************
void Player::SetPos( Point Pos )
{
	m_Pos = Pos;

	//	�t�B�[���h���Ɏ��߂�
	Rect FieldRect = GetWorld()->GetFieldRect();
	FieldRect.Widen( -m_pData->GetParam().HitRadius * 2 );
	m_Pos = FieldRect.Inside( m_Pos );
}

//*****************************************************************************
//	���[���h�̎擾
//*****************************************************************************
World* Player::GetWorld( void ) const
{
	return m_pManager->GetWorld();
}

//******************************************************************************
//	�������擾
//******************************************************************************
Rand& Player::GetRand( void ) const
{
	return m_pManager->GetRand();
}

//******************************************************************************
//	���@�{�̂̃��F���擾
//******************************************************************************
Color Player::GetBodyAlpha( void ) const
{
	Color Col = Color::White();

	if( m_pSprite ) Col.a = m_pSprite->GetBoneColor( BONE_NAME_BODY ).a;

	return Col;
}

//*****************************************************************************
//	���G��Ԃ��H
//*****************************************************************************
bool Player::IsInvincible( bool bThrough ) const
{
	if( GetConfig()->GetDebugInfo().bPlayerInvincible )
	{
		return true;
	}

	if( IsDead() )
	{
		return true;
	}

	if( m_State == STATE_REVIVE )
	{
		return true;
	}

	if( m_pDash->IsInvincible() )
	{
		return true;
	}

	if( bThrough ) return false;

	if( m_pShield->IsInvincible() ||
		m_pOverdrive->IsInvincible() )
	{
		return true;
	}

	if( m_bRulerHit )
	{
		return true;
	}

	if( m_bEnemyRulerHit )
	{
		return true;
	}

	if( m_InvincibleCount > 0 )
	{
		return true;
	}


	return m_bInvincible;
}

//******************************************************************************
//	�\�������L�������ׂ�
//******************************************************************************
bool Player::IsIndicateEnable( void ) const
{
	if( m_bRulerHit )
	{
		return false;
	}

	if( m_bEnemyRulerHit )
	{
		return false;
	}

	if( IsDead() )
	{
		return false;
	}

	if( m_State == STATE_REVIVE ||
		m_State == STATE_OVERDRIVE )
	{
		return false;
	}

	return m_bIndicateEnable;
}

//******************************************************************************
//	���݂̃X�e�[�g�����v���邩�ǂ������ׂ�
//******************************************************************************
bool Player::IsState( Label Name ) const
{
	STATE State = _LabelToState( Name );

	return m_State == State;
}

//******************************************************************************
//	�`���[�W������
//******************************************************************************
bool Player::IsCharged( void ) const
{
	int ChargeTime = m_bRulerHit?	m_pData->GetParam().Blade.SmashParam.RulerChargeTime :
									m_pData->GetParam().Blade.SmashParam.ChargeTime;
	
	return m_State == STATE_CHARGE && m_StateCount >= ChargeTime;
}

//******************************************************************************
//	�G�t�F�N�g���쐬����
//******************************************************************************
Effect* Player::CreateEffect( Label Name, const Matrix3& Mat ) const
{
	if( Name.IsEmpty() ) return NULL;

	const EffectFile* pEffectFile = m_pData->GetEffectFile( Name );
	if( !pEffectFile ) return NULL;
	
	return GetApp()->GetEffectMng()->CreateEffect( pEffectFile, Mat );
}

//------------------------------------------------------------------------------
//	�X�V�F�ʏ���
//------------------------------------------------------------------------------
void Player::_UpdateNormal( void )
{
	const Controller* pCtrl = _GetController();

	//	�����ւ��A�j���X�V
	_UpdateReplaceAnime();

	//	�A�j���̍X�V
	bool bSideChange = false;
	Point Dir = pCtrl->GetDir();

	ANIME_INFO Anime = ANIME_WAIT;

	if( _IsMoveEnable() )
	{
		//	������
		if( Dir.y > 0.0f )
		{
			Anime = ANIME_MOVE_B;
		}
		//	������
		else if( Dir.y == 0.0f && Dir.x != 0.0f )
		{
			Anime = ANIME_MOVE_S;
		}
		//	�O����
		else if( Dir.y < 0.0f )
		{
			Anime = ANIME_MOVE_F;
		}

		//	����
		if( Dir.x != 0.0f )
		{
			SIDE Side = (Dir.x > 0.0)? SIDE_R : SIDE_L;

			bSideChange = m_Side != Side;
			m_Side = Side;
		}
	}

	//	�����A�j��
	if( bSideChange || m_pSprite->GetAnimeName() != Anime.Name )
	{
		_ChangeAnime( Anime );
	}

	if( _IsAttackEnable() )
	{
		//	�I�[���@�h���C��:�X�s�A
		if( pCtrl->IsInput( CONTROLLER_DASH, INPUT_PRESS ) &&
			pCtrl->IsInput( CONTROLLER_BLADE, INPUT_TRIG ) )
		{
			if( m_pOverdrive->IsSpearEnable() )
			{
				_OverdriveSpear();
				return;
			}
		}

		//	�I�[���@�h���C��:���[���[
		else if( pCtrl->IsInput( CONTROLLER_DASH, INPUT_PRESS ) &&
				 pCtrl->IsInput( CONTROLLER_RELOAD, INPUT_TRIG ) )
		{
			if( m_pOverdrive->IsRulerEnable() )
			{
				_OverdriveRuler();
				return;;
			}
		}

		//	�����[�h
		else if( pCtrl->IsInput( CONTROLLER_RELOAD, INPUT_TRIG ) )
		{
			//	�L�����Z�������[�h
			if( m_pBlade->IsCancelEnable() )
			{
				_CancelReload();
				return;
			}
			//	�ʏ탊���[�h
			else if( m_pBlade->IsReloadEnable() )
			{
				_Reload();
				return;
			}
		}

		//	�u���[�h
		else if( !pCtrl->IsInput( CONTROLLER_DASH, INPUT_PRESS ) &&
				 pCtrl->IsInput( CONTROLLER_BLADE, INPUT_PRESS ) )
		{
			if( m_pBlade->IsSlashEnable() )
			{
				_Charge( Dir );
				return;
			}
			else
			{
				m_pBlade->OnDisableSlash();
				return;
			}
		}

		//	�_�b�V��
		else if( pCtrl->IsInput( CONTROLLER_DASH, INPUT_TRIG ) &&
				 !pCtrl->IsInput( CONTROLLER_BLADE, INPUT_TRIG ) && 
				 !pCtrl->IsInput( CONTROLLER_RELOAD, INPUT_TRIG ) &&
				 !Dir.IsZero() )
		{
				
			if( !m_pDash->IsDelay() )
			{
				//	���[���[���̓_�b�V���a��
				if( m_bRulerHit )
				{
					_Splash( Dir );
					return;
				}
				//	�ʏ�_�b�V��
				else
				{
					_Dash( Dir );
					return;
				}
			}
		}
	}
}

//------------------------------------------------------------------------------
//	�X�V�F�^�����
//------------------------------------------------------------------------------
void Player::_UpdateCharge( void )
{
	const Controller* pCtrl = _GetController();
	Point Dir = pCtrl->GetDir();

	//	���샍�b�N
	if( _IsActionLocked( ACTION_CHARGE ) )
	{
		_Slash();
		return;
	}

	//	�܂ꂽ
	if( m_pBlade->IsCrash() )
	{
		_Knockback( Point( 0.0f, m_pData->GetParam().CrashVel ) );
		return;
	}

	//	������
	if( pCtrl->IsInput( CONTROLLER_RELOAD, INPUT_TRIG ) )
	{
		//	�u���[�h�Ƀf�B���C�t�^
		m_pBlade->Delay();

		_Normal();
		return;
	}
	
	int ChargeTime = m_bRulerHit?	m_pData->GetParam().Blade.SmashParam.RulerChargeTime :
									m_pData->GetParam().Blade.SmashParam.ChargeTime;

	//	�^�������O
	if( m_StateCount < ChargeTime )
	{
		//	������
		if( !pCtrl->IsInput( CONTROLLER_BLADE, INPUT_PRESS ) )
		{
			//	�ʏ�a��
			_Slash();
			return;
		}
	}
	//	�^���ő�
	else if( m_StateCount >= m_pData->GetParam().Blade.SmashParam.ChargeMaxTime )
	{
		//	�X�}�b�V��
		_Smash();
		return;
	}
	//	�^��������
	else
	{
		//	������/���[���[���͍ő�����
		if( !pCtrl->IsInput( CONTROLLER_BLADE, INPUT_PRESS ) || m_bRulerHit )
		{
			//	�X�}�b�V��
			_Smash();
			return;
		}

		//	��������
		if( pCtrl->IsInput( CONTROLLER_DASH, INPUT_TRIG ) &&
			!Dir.IsZero() )
		{
			_Splash( Dir );
			return;
		}
	}
}

//------------------------------------------------------------------------------
//	�X�V�F�u���[�h���
//------------------------------------------------------------------------------
void Player::_UpdateBlade( void )
{
	const Controller* pCtrl = _GetController();

	//	�܂ꂽ
	if( m_pBlade->IsCrash() )
	{
		_Knockback( Point( 0.0f, m_pData->GetParam().CrashVel ) );
		return;
	}

	//	������Ԃֈڍs
	if( m_pBlade->IsRepulse() )
	{
		_Repulse();
		return;
	}

	//	���[���[���E�L�����Z���\�̏ꍇ
	if( m_bRulerHit && m_pBlade->IsHit() && m_pBlade->IsSlashEnable() )
	{
		//	�A���U��
		if( pCtrl->IsInput( CONTROLLER_BLADE, INPUT_PRESS ) )
		{
			//	�Î~��ԂȂ甽�]
			if( m_bRulerHit && pCtrl->GetDir().IsZero() )
			{
				m_Side = (m_StateCount % 2)? SIDE_R : SIDE_L;
			}

			m_pBlade->CancelState();

			_Charge( pCtrl->GetDir() );
			return;
		}
	}

	//	�A�j���I��
	if( m_pSprite->IsAnimeLast() )
	{
		//	�X�R�A�ʒm
		if( !m_pBlade->IsHit() )
		{
			GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_MISS );
		}

		//	���[���[���A�Î~��ԂȂ甽�]
		if( m_bRulerHit && pCtrl->GetDir().IsZero() )
		{
			m_Side = (m_StateCount % 2)? SIDE_R : SIDE_L;
		}

		//	�ʏ��Ԃ�
		_Normal();
	}
}

//------------------------------------------------------------------------------
//	�X�V�F�������
//------------------------------------------------------------------------------
void Player::_UpdateRepulse( void )
{
	const Controller* pCtrl = _GetController();

	//	�܂ꂽ
	if( m_pBlade->IsCrash() )
	{
		_Knockback( Point( 0.0f, m_pData->GetParam().CrashVel ) );
		return;
	}

	if( _IsAttackEnable() )
	{
		Point Dir = pCtrl->GetDir();

		//	�_�b�V��
		if( pCtrl->IsInput( CONTROLLER_DASH, INPUT_TRIG ) &&
			!pCtrl->IsInput( CONTROLLER_BLADE, INPUT_TRIG ) && 
			!pCtrl->IsInput( CONTROLLER_RELOAD, INPUT_TRIG ) &&
			!Dir.IsZero() )
		{
			if( !m_pDash->IsDelay() )
			{
				_Dash( Dir );
				return;
			}
		}
		//	�蓮����
		if( pCtrl->IsInput( CONTROLLER_RELOAD, INPUT_TRIG ) )
		{
			//	�ʏ��Ԃ�
			_Normal();
			return;
		}
	}

	//	�U��؂���
	if( !m_pBlade->IsRepulse() )
	{
		//	���G���Ԑݒ�
		m_InvincibleCount = Max( m_InvincibleCount, m_pData->GetParam().RepulseInvincible );

		_ChangeState( STATE_BLADE );
	}
}

//------------------------------------------------------------------------------
//	�X�V�F�����[�h���
//------------------------------------------------------------------------------
void Player::_UpdateReload( void )
{
	//	�A�j���I��
	if( m_pSprite->IsAnimeLast() )
	{
		//	�ʏ��Ԃ�
		_Normal();
	}
}

//------------------------------------------------------------------------------
//	�X�V�F�L�����Z�������[�h���
//------------------------------------------------------------------------------
void Player::_UpdateCancel( void )
{
	//	�A�j���I��
	if( m_pSprite->IsAnimeLast() )
	{
		//	�ʏ��Ԃ�
		_Normal();
	}
}

//------------------------------------------------------------------------------
//	�X�V�F�_�b�V�����
//------------------------------------------------------------------------------
void Player::_UpdateDash( void )
{
	const Controller* pCtrl = _GetController();

	//	�u���[�h
	if( m_pDash->IsEnd() )
	{
		if( _IsAttackEnable() )
		{
			if( pCtrl->IsInput( CONTROLLER_BLADE, INPUT_PRESS ) )
			{
				if( m_pBlade->IsSlashEnable() )
				{
					_Charge( pCtrl->GetDir() );
					return;
				}
				else
				{
					m_pBlade->OnDisableSlash();
				}
			}
		}
	}

	//	�_�b�V���I��
	if( m_pSprite->IsAnimeLast() )
	{
		//	�ʏ��Ԃ�
		_Normal();
	}
}

//------------------------------------------------------------------------------
//	�X�V�F�I�[���@�h���C���������
//------------------------------------------------------------------------------
void Player::_UpdateOverdrive( void )
{
	//	�A�j���I��
	if( m_pSprite->IsAnimeLast() )
	{
		//	�ʏ��Ԃ�
		_Normal();
	}
}

//------------------------------------------------------------------------------
//	�X�V�F�m�b�N�o�b�N���
//------------------------------------------------------------------------------
void Player::_UpdateKnock( void )
{
	const Controller* pCtrl = _GetController();

	//	���o�[�T���s��
	if( m_StateCount >= m_pData->GetParam().ReversalTime &&
		_IsAttackEnable() )
	{
		bool bAction = false;
		Point Dir = pCtrl->GetDir();

		//	�I�[���@�h���C��:�X�s�A
		if( pCtrl->IsInput( CONTROLLER_DASH, INPUT_PRESS ) &&
			pCtrl->IsInput( CONTROLLER_BLADE, INPUT_TRIG ) )
		{
			if( m_pOverdrive->IsSpearEnable() )
			{
				_OverdriveSpear();
				bAction = true;
			}
		}

		//	�I�[���@�h���C��:���[���[
		else if( pCtrl->IsInput( CONTROLLER_DASH, INPUT_PRESS ) &&
				 pCtrl->IsInput( CONTROLLER_RELOAD, INPUT_TRIG ) )
		{
			if( m_pOverdrive->IsRulerEnable() )
			{
				_OverdriveRuler();
				bAction = true;
			}
		}

		//	�_�b�V��
		else if( pCtrl->IsInput( CONTROLLER_DASH, INPUT_TRIG ) &&
				 !pCtrl->IsInput( CONTROLLER_BLADE, INPUT_TRIG ) && 
				 !pCtrl->IsInput( CONTROLLER_RELOAD, INPUT_TRIG ) &&
				 !Dir.IsZero() )
		{
			if( !m_pDash->IsDelay() )
			{
				_Dash( Dir );
				bAction = true;
			}
		}

		if( bAction )
		{
			//	�u���C�N�L�����Z��
			if( m_pShield->IsBreak() )
			{
				m_pShield->CancelBreak();
			}
			return;
		}
	}

	//	�m�b�N�o�b�N�I��
	if( m_StateCount >= m_pData->GetParam().KnockbackTime )
	{
		//	�ʏ��Ԃ�
		_Normal();
	}
}

//------------------------------------------------------------------------------
//	�X�V�F���S���
//------------------------------------------------------------------------------
void Player::_UpdateDead( void )
{
	//	�A�j���I��
	if( m_pSprite->IsAnimeLast() )
	{
		//	���[���h�֒ʒm
		GetWorld()->OnPlayerDead();
	}
}

//------------------------------------------------------------------------------
//	�X�V�F�������
//------------------------------------------------------------------------------
void Player::_UpdateRevive( void )
{
	//	�A�j���I��
	if( m_pSprite->IsAnimeLast() )
	{
		//	�ʏ��Ԃ�
		_Normal();
	}
}

//------------------------------------------------------------------------------
//	�ʒu�̍X�V
//------------------------------------------------------------------------------
void Player::_UpdateMove( void )
{
	if( m_bEnemyRulerHit ) return;

	if( _IsMoveEnable() )
	{
		const Controller* pCtrl = _GetController();
		
		Point Dir = pCtrl->GetDir();
		
		//	�ړ��̍X�V
		m_Pos += Dir * _GetMoveSpeed();
	}

	//	�ݒ�
	SetPos( m_Pos += m_Vel );

	//	����
	m_Vel *= m_pData->GetParam().Decel;
}

//------------------------------------------------------------------------------
//	�����ւ��A�j���̍X�V
//------------------------------------------------------------------------------
void Player::_UpdateReplaceAnime( void )
{
	if( m_ReplaceAnimeQueue.empty() ) return;

	const ANIME_INFO& Info = m_ReplaceAnimeQueue.front();

	if( m_pSprite->HasAnime( Info.Name ) )
	{
		m_pSprite->ChangeAnime( Info.Name, Info.Intp );
		m_bAnimeReplace = true;

		//	�L���[����P��
		m_ReplaceAnimeQueue.pop_front();
	}
}

//------------------------------------------------------------------------------
//	�A�N�V�����K�p�̍X�V
//------------------------------------------------------------------------------
bool Player::_UpdateApplyAction( void )
{
	if( m_ApplyActionQueue.empty() ) return false;

	const ACTION_INFO& Info = m_ApplyActionQueue.front();
	bool bExec = false;

	switch( Info.Action )
	{
	//	�u���[�h
	case ACTION_BLADE:

		//	�^���J�n
		if( m_State == STATE_NORMAL )
		{
			if( m_pBlade->IsSlashEnable() )
			{
				_Charge( Info.Dir );
				return true;
			}
		}
		//	�U��
		else if( m_State == STATE_CHARGE )
		{
			_Slash();
			bExec = true;
		}
		break;

	//	�^��
	case ACTION_CHARGE:

		//	�^���J�n
		if( m_State == STATE_NORMAL )
		{
			if( m_pBlade->IsSlashEnable() )
			{
				_Charge( Info.Dir );
				bExec = true;
			}
		}
		break;

	//	�����[�h
	case ACTION_RELOAD:

		if( m_State == STATE_NORMAL )
		{
			if( m_pBlade->IsReloadEnable() )
			{
				_Reload();
				bExec = true;
			}
		}
		break;

	//	�L�����Z�������[�h
	case ACTION_CANCEL:

		if( m_State == STATE_NORMAL )
		{
			if( m_pBlade->IsCancelEnable() )
			{
				_CancelReload();
				bExec = true;
			}
		}
		break;

	//	�_�b�V��
	case ACTION_DASH:

		_ASSERT( !Info.Dir.IsZero() );

		if( m_State == STATE_NORMAL )
		{
			if( !m_pDash->IsDelay() )
			{
				_Dash( Info.Dir );
				bExec = true;
			}
		}
		break;

	//	�X�}�b�V��
	case ACTION_SMASH:

		//	�^���J�n
		if( m_State == STATE_NORMAL )
		{
			if( m_pBlade->IsSlashEnable() )
			{
				_Charge( Info.Dir );
				return true;
			}
		}
		//	�U��
		else if( m_State == STATE_CHARGE )
		{
			//	�^������
			if( m_StateCount >= m_pData->GetParam().Blade.SmashParam.ChargeTime + ACTION_CHARGE_TIME )
			{
				_Smash();
				bExec = true;
			}
			else
			{
				return true;
			}
		}
		break;

	//	�X�v���b�V��
	case ACTION_SPLASH:

		_ASSERT( !Info.Dir.IsZero() );

		//	�^���J�n
		if( m_State == STATE_NORMAL )
		{
			if( m_pBlade->IsSlashEnable() )
			{
				_Charge( Info.Dir );
				return true;
			}
		}
		//	�U��
		else if( m_State == STATE_CHARGE )
		{
			//	�^������
			if( m_StateCount >= m_pData->GetParam().Blade.SmashParam.ChargeTime + ACTION_CHARGE_TIME )
			{
				_Splash( Info.Dir );
				bExec = true;
			}
			else
			{
				return true;
			}
		}
		break;

	//	�X�s�A
	case ACTION_SPEAR:

		if( m_State == STATE_NORMAL )
		{
			if( m_pOverdrive->IsSpearEnable() )
			{
				_OverdriveSpear();
				bExec = true;
			}
		}
		break;

	//	���[���[
	case ACTION_RULER:

		if( m_State == STATE_NORMAL )
		{
			if( m_pOverdrive->IsRulerEnable() )
			{
				_OverdriveRuler();
				bExec = true;
			}
		}
		break;

	//	����ȊO
	default:
		bExec = true;
		break;

	}

	//	�L���[����P��
	if( bExec )
	{
		m_ApplyActionQueue.pop_front();
	}

	return bExec;
}

//------------------------------------------------------------------------------
//	���[���[��Ԃ̍X�V
//------------------------------------------------------------------------------
void Player::_UpdateRuler( void )
{
	//	���[���[��
	if( m_bRulerHit )
	{
		//	�G�t�F�N�g�̍X�V
		if( m_pRulerEffect )
		{
			m_pRulerEffect->SetMatrix( GetMatrix() );
		}
		//	�G�t�F�N�g�̐���
		else
		{
			m_pRulerEffect = CreateEffect( m_pData->GetParam().Overdrive.Ruler.PlayerEffectName, GetMatrix() );
		}
	}
	//	���[���[�O
	else
	{
		EffectEndAppear( m_pRulerEffect );
	}
}

//------------------------------------------------------------------------------
//	�C���W�P�[�g�̍X�V
//------------------------------------------------------------------------------
void Player::_UpdateIndicate( void )
{
	//	�����O
	Indicate* pRingIndicate	= GetWorld()->GetIndicateManager()->GetIndicate( INDICATE_RING_DASH );
	if( pRingIndicate )
	{
		pRingIndicate->SetMatrix( GetMatrix() );

		pRingIndicate->SetColor( GetBodyAlpha() );

		//	�i�s�����̊p�x��
		int DirIndex = -1;
		const Controller* pCtrl = _GetController();
		if( !pCtrl->GetDir().IsZero() )
		{
			DirIndex = floorf( (pCtrl->GetDir().GetAngle() + Angle::ToRad(22.5f)) / Angle::ToRad(45.0f) );
		}

		//	�_�b�V��
		for( int i = 0; i < INDICATE_RING_BONE_NUM; i++ )
		{
			String256 Bone;
			Bone.Format( INDICATE_BONE_MARKER_FMT, i );
			pRingIndicate->SetBoneVisible( Label(Bone.c_str()), m_pDash->IsActive() || !m_pDash->IsDelay() );

			pRingIndicate->SetColor( Label(Bone.c_str()), i != DirIndex? COLOR_INDICATE_BLACK :
															IsCharged()? COLOR_INDICATE_CHARGE :
																		 COLOR_INDICATE_NORMAL );
		}

		//	�A�C�e�����W
		float CollectRate = GetWorld()->GetItemManager()->GetCollectTimeRate();
		Color Col = Color::Lerp( COLOR_INDICATE_COLLECT_MIN, COLOR_INDICATE_COLLECT_MAX, CollectRate );
		pRingIndicate->SetColor( INDICATE_BONE_COLLECT, Col );
		
		pRingIndicate->SetBoneVisible( INDICATE_BONE_COLLECT, CollectRate > 0.0f );

		CollectRate = Lerp( INDICATE_COLLECT_SCALE, 1.0f, CollectRate );
		pRingIndicate->SetGauge( INDICATE_BONE_COLLECT, CollectRate, CollectRate );

		if( IsIndicateEnable() )
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
void Player::_Normal( void )
{
	//	�������Z�b�g
	SafePtr( m_pBlade )->CancelState();
	SafePtr( m_pDash )->CancelState();
	SafePtr( m_pOverdrive )->CancelState();

	_ChangeState( STATE_NORMAL );
}

//------------------------------------------------------------------------------
//	�u���[�h�^��
//------------------------------------------------------------------------------
void Player::_Charge( Point MoveDir )
{
	//	�؂�グ
	ANIME_INFO Anime = ANIME_CHARGE_F;
	Point SlashDir( SQ2, -SQ2 );

	//	�؂艺�낵
	if( MoveDir.y > 0.0f )
	{
		Anime = ANIME_CHARGE_B;
		SlashDir.Set( SQ2, SQ2 );
	}
	//	���؂�
	else if( MoveDir.y == 0.0f && MoveDir.x != 0.0f )
	{
		Anime = ANIME_CHARGE_S;
		SlashDir.Set( 1.0f, 0.0f );
	}

	//	�A�j��
	_ChangeAnime( Anime );

	//	�^���J�n
	if( m_Side == SIDE_L ) SlashDir.x = -SlashDir.x;
	m_pBlade->Charge( SlashDir );

	_ChangeState( STATE_CHARGE );
}

//------------------------------------------------------------------------------
//	�u���[�h��U��
//------------------------------------------------------------------------------
void Player::_Slash( void )
{
	//	���샍�b�N
	if( _IsActionLocked( ACTION_BLADE ) )
	{
		_Normal();
		return;
	}

	Label CurAnime = m_pSprite->GetAnimeName();

	//	�؂�グ
	ANIME_INFO Anime = ANIME_BLADE_F;

	//	�؂艺�낵
	if( CurAnime == ANIME_CHARGE_B.Name )
	{
		Anime = ANIME_BLADE_B;
	}
	//	���؂�
	else if( CurAnime == ANIME_CHARGE_S.Name )
	{
		Anime = ANIME_BLADE_S;
	}
	
	//	���[���[���E�Î~�Ȃ�΃A�j���̓����_��
	if( m_bRulerHit && _GetController()->GetDir().IsZero() )
	{
		if( m_StateCount % 3 == 0 )
		{
			Anime = ANIME_BLADE_F;
		}
		else if( m_StateCount % 3 == 1 )
		{
			Anime = ANIME_BLADE_S;
		}
		else
		{
			Anime = ANIME_BLADE_B;
		}
	}

	//	�A�j��
	_ChangeAnime( Anime );

	//	�u���[�h����
	m_pBlade->Slash( false );

	_ChangeState( STATE_BLADE );
}

//------------------------------------------------------------------------------
//	�X�}�b�V������
//------------------------------------------------------------------------------
void Player::_Smash( void )
{
	//	���샍�b�N
	if( _IsActionLocked( ACTION_SMASH ) )
	{
		_Normal();
		return;
	}

	Label CurAnime = m_pSprite->GetAnimeName();

	//	�؂�グ
	ANIME_INFO Anime = ANIME_SMASH_F;

	//	�؂艺�낵
	if( CurAnime == ANIME_CHARGE_B.Name )
	{
		Anime = ANIME_SMASH_B;
	}
	//	���؂�
	else if( CurAnime == ANIME_CHARGE_S.Name )
	{
		Anime = ANIME_SMASH_S;
	}

	//	�A�j��
	_ChangeAnime( Anime );

	//	�u���[�h����
	m_pBlade->Slash( true );

	_ChangeState( STATE_BLADE );
}

//------------------------------------------------------------------------------
//	��������
//------------------------------------------------------------------------------
void Player::_Repulse( void )
{
	_ChangeState( STATE_REPULSE );
}

//------------------------------------------------------------------------------
//	�����[�h����
//------------------------------------------------------------------------------
void Player::_Reload( void )
{
	//	���샍�b�N
	if( _IsActionLocked( ACTION_RELOAD ) )
	{
		_Normal();
		return;
	}

	//	�A�j��
	_ChangeAnime( ANIME_RELOAD );

	//	�����[�h����
	m_pBlade->Reload();

	//	�A�C�e���������
	GetWorld()->GetItemManager()->AutoCollect();

	_ChangeState( STATE_RELOAD );
}

//------------------------------------------------------------------------------
//	�L�����Z�������[�h����
//------------------------------------------------------------------------------
void Player::_CancelReload( void )
{
	//	���샍�b�N
	if( _IsActionLocked( ACTION_CANCEL ) )
	{
		_Normal();
		return;
	}

	//	�A�j��
	_ChangeAnime( ANIME_CANCEL );

	//	�L�����Z�������[�h����
	m_pBlade->CancelReload();

	//	�A�C�e���������
	GetWorld()->GetItemManager()->AutoCollect();

	_ChangeState( STATE_CANCEL );
}

//------------------------------------------------------------------------------
//	�_�b�V������
//------------------------------------------------------------------------------
void Player::_Dash( Point MoveDir )
{
	//	���샍�b�N
	if( _IsActionLocked( ACTION_DASH ) )
	{
		_Normal();
		return;
	}

	//	�A�j��
	ANIME_INFO Anime = MoveDir.x == 0.0f? ANIME_DASH_N : ANIME_DASH_S;
	_ChangeAnime( Anime );

	//	�_�b�V������
	m_pDash->Start( MoveDir );

	_ChangeState( STATE_DASH );
}

//------------------------------------------------------------------------------
//	������������
//------------------------------------------------------------------------------
void Player::_Splash( Point MoveDir )
{
	//	���샍�b�N
	if( _IsActionLocked( ACTION_SPLASH ) )
	{
		_Normal();
		return;
	}

	//	�A�j��
	ANIME_INFO Anime = MoveDir.x == 0.0f? ANIME_SPLASH_N : ANIME_SPLASH_S;
	_ChangeAnime( Anime );

	//	����
	m_pDash->Splash( MoveDir );

	//	�u���[�h�Ƀf�B���C�t�^
	m_pBlade->Delay();

	_ChangeState( STATE_DASH );
}

//------------------------------------------------------------------------------
//	�I�[���@�h���C���F�X�s�A����
//------------------------------------------------------------------------------
void Player::_OverdriveSpear( void )
{
	//	���샍�b�N
	if( _IsActionLocked( ACTION_SPEAR ) )
	{
		_Normal();
		return;
	}

	//	�A�j��
	_ChangeAnime( ANIME_SPEAR );

	//	�X�s�A����
	m_pOverdrive->DriveSpear();

	//	�A�C�e���������
	GetWorld()->GetItemManager()->ForceCollect();

	//	�X�R�A�ʒm
	GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_OVERDRIVE );

	_ChangeState( STATE_OVERDRIVE );
}

//------------------------------------------------------------------------------
//	�I�[���@�h���C���F���[���[����
//------------------------------------------------------------------------------
void Player::_OverdriveRuler( void )
{
	//	���샍�b�N
	if( _IsActionLocked( ACTION_RULER ) )
	{
		_Normal();
		return;
	}

	//	�A�j��
	_ChangeAnime( ANIME_RULER );

	//	���[���[����
	m_pOverdrive->DriveRuler();

	//	�A�C�e���������
	GetWorld()->GetItemManager()->ForceCollect();

	//	�X�R�A�ʒm
	GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_OVERDRIVE );

	_ChangeState( STATE_OVERDRIVE );
}

//------------------------------------------------------------------------------
//	�m�b�N�o�b�N����
//------------------------------------------------------------------------------
void Player::_Knockback( Point Dir )
{
	//	�A�j��
	_ChangeAnime( ANIME_KNOCK );

	//	��������������	
	SafePtr( m_pBlade )->CancelState();
	SafePtr( m_pDash )->CancelState();
	SafePtr( m_pOverdrive )->CancelState();

	//	���x����
	m_Vel += Dir;

	_ChangeState( STATE_KNOCK );
}

//------------------------------------------------------------------------------
//	���S����
//------------------------------------------------------------------------------
void Player::_Dead( Point Dir )
{
	//	�A�j��
	_ChangeAnime( ANIME_DEAD );

	//	��������������	
	SafePtr( m_pBlade )->CancelState();
	SafePtr( m_pDash )->CancelState();
	SafePtr( m_pOverdrive )->CancelState();
	SafePtr( m_pOverdrive )->AllErase();

	//	���x����
	m_Vel += Dir;

	_ChangeState( STATE_DEAD );
}

//------------------------------------------------------------------------------
//	�X�e�[�g�̕ύX
//------------------------------------------------------------------------------
void Player::_ChangeState( STATE State )
{
	m_State			= State;
	m_StateCount	= 0;
}

//------------------------------------------------------------------------------
//	�A�j�����
//------------------------------------------------------------------------------
void Player::_ChangeAnime( const ANIME_INFO& Info )
{
	//	�A�j�������ւ���
	if( m_bAnimeReplace ) return;

	if( m_pSprite )
	{
		m_pSprite->ChangeAnime( Info.Name, Info.Intp );
	}
}

//------------------------------------------------------------------------------
//	���R�񕜏���
//------------------------------------------------------------------------------
void Player::_Regenerate( void )
{
	//	���쒆�͎��R�񕜂Ȃ�
	if( m_State != STATE_NORMAL ) return;
	if( m_bEnemyRulerHit ) return;

	float Energy = m_pData->GetParam().RegenEnergy;

	//	�V�[���h
	Energy = m_pShield->Regenerate( Energy );

	//	�u���[�h
	Energy = m_pBlade->Regenerate( Energy );

	//	�I�[���@�h���C��
	Energy = m_pOverdrive->Regenerate( Energy );
}

//------------------------------------------------------------------------------
//	�_���[�W��������
//------------------------------------------------------------------------------
bool Player::_Damage( float Attack, Point Dir, bool bThroughInvincible )
{
	if( GetConfig()->GetDebugInfo().bEnemyHyperAttack )
	{
		Attack *= 100.0f;
	}

	//	���G��Ԃ��H
	if( IsInvincible( bThroughInvincible ) ) return false;

	//	���S����
	if( m_pShield->IsDeadly() )
	{
		//	���S
		_Dead( Dir * m_pData->GetParam().BreakVel );
		return true;
	}

	//	�_���[�W
	m_pShield->Damage( Attack * _GetDamageCoef() );
	
	//	�V�[���h�u���C�N
	if( m_pShield->IsBreak() )
	{
		_Knockback( Dir * m_pData->GetParam().BreakVel );
	}
	//	������^����
	else
	{
		m_Vel += Dir;
	}
	
	return true;
}

//------------------------------------------------------------------------------
//	������̃L���X�g���s��
//------------------------------------------------------------------------------
void Player::_CastCollide( void )
{
	//	�����蔻����s��
	m_pCaster->UpdateMatrix( &GetMatrix() );

	COLLIDE_CAST_RESULT Result;
	if( m_pCaster->Cast( Result ) )
	{
		for( COLLIDE_RESULT_LIST::iterator it = Result.ResultList.begin(); it != Result.ResultList.end(); ++it )
		{
			switch( it->pOwner->Type )
			{
			case COLLIDE_OWNER_RULER:
				{
					m_bRulerHit = true;
				}
				break;
			case COLLIDE_OWNER_ITEM:
				{
					Item* pItem = (Item*)(it->pOwner->pData);
					OnGetItem( pItem );
				}
				break;
			}
		}

		for( COLLIDE_RESULT_LIST::iterator it = Result.ResultList.begin(); it != Result.ResultList.end(); ++it )
		{
			switch( it->pOwner->Type )
			{
			case COLLIDE_OWNER_ENEMY:
				{
					Enemy* pEnemy = (Enemy*)(it->pOwner->pData);
					_Damage( pEnemy->GetBodyDamage(), it->Normal * pEnemy->GetBodyDamageVel() );
				}
				break;
			}
		}
		
		//	�n�`������̋���
		m_Pos = Result.SolvePos;
	}
}

//------------------------------------------------------------------------------
//	�ړ��\���H
//------------------------------------------------------------------------------
bool Player::_IsMoveEnable( void ) const
{
	if( m_State == STATE_DASH ||
		m_State == STATE_OVERDRIVE ||
		m_State == STATE_KNOCK ||
		m_State == STATE_DEAD || 
		m_State == STATE_REVIVE )
	{
		return false;
	}

	if( m_pShield->IsNoMove() )
	{
		return false;
	}
	
	if( GetWorld()->IsFade() )
	{
		return false;
	}

	if( m_bEnemyRulerHit )
	{
		return false;
	}

	return m_bMoveEnable;
}

//------------------------------------------------------------------------------
//	�U���\���H
//------------------------------------------------------------------------------
bool Player::_IsAttackEnable( void ) const
{
	if( GetWorld()->IsFade() )
	{
		return false;
	}

	if( m_bEnemyRulerHit )
	{
		return false;
	}

	return m_bAttackEnable;
}

//------------------------------------------------------------------------------
//	�A�N�V���������������H
//------------------------------------------------------------------------------
bool Player::_IsActionLocked( ACTION Action ) const
{
	dword Flag = (1 << Action);

	return (m_ActionLock & Flag) != 0;
}

//------------------------------------------------------------------------------
//	�X�v���C�g���ꎞ��~���邩�H
//------------------------------------------------------------------------------
bool Player::_IsPauseSprite( void ) const
{
	if( m_State == STATE_REPULSE ) return true;

	if( m_bEnemyRulerHit ) return true;

	return false;
}

//------------------------------------------------------------------------------
//	�ړ����x�̎擾
//------------------------------------------------------------------------------
float Player::_GetMoveSpeed( void ) const
{
	//	�ᑬ
	if( m_State == STATE_REPULSE ||
		m_State == STATE_CHARGE && m_StateCount >= m_pData->GetParam().Blade.SmashParam.ChargeStartTime )
	{
		return m_pData->GetParam().SlowSpeed;
	}

	//	���[���[��
	if( m_bRulerHit )
	{
		return m_pData->GetParam().RulerMoveSpeed;
	}

	return m_pData->GetParam().MoveSpeed;
}

//------------------------------------------------------------------------------
//	�_���[�W�W���̎擾
//------------------------------------------------------------------------------
float Player::_GetDamageCoef( void ) const
{
	if( m_State == STATE_OVERDRIVE )	return DAMAGE_COEF_OVER;
	if( m_State == STATE_BLADE ||
		m_State == STATE_CHARGE )		return DAMAGE_COEF_BLADE;
	if( m_State == STATE_CANCEL )		return DAMAGE_COEF_CANCEL;
	if( m_State == STATE_RELOAD )		return DAMAGE_COEF_RELOAD;
	if( m_pBlade->IsCrash() )			return DAMAGE_COEF_CLASH;
	if( m_pDash->IsDelay() )			return DAMAGE_COEF_DASH;

	return DAMAGE_COEF_NORMAL;
}

//------------------------------------------------------------------------------
//	�X�v���C�g�p�̍s����擾
//------------------------------------------------------------------------------
Matrix3 Player::_GetSpriteMatrix( void ) const
{
	//	�����ɂ���Ĕ��]
	Matrix3 Mat = Matrix3::Scale( Point( m_Side == SIDE_L? -1.0f : 1.0f, 1.0f ) );

	Mat.SetPos( m_Pos );

	return Mat;
}

//------------------------------------------------------------------------------
//	�R���g���[���[�̎擾
//------------------------------------------------------------------------------
const Controller* Player::_GetController( void ) const
{
	return GetWorld()->GetController();
}

//------------------------------------------------------------------------------
//	�����񂩂��Ԃ𓾂�
//------------------------------------------------------------------------------
Player::STATE Player::_LabelToState( Label Name )
{
	return STATE( LabelToInt( Name, STATE_NAME_ARRAY, ArrayNum(STATE_NAME_ARRAY), STATE_NULL ) );
}

//------------------------------------------------------------------------------
//	�����񂩂�A�N�V�����𓾂�
//------------------------------------------------------------------------------
Player::ACTION Player::_LabelToAction( Label Name )
{
	return ACTION( LabelToInt( Name, ACTION_NAME_ARRAY, ArrayNum(ACTION_NAME_ARRAY), ACTION_ALL ) );
}