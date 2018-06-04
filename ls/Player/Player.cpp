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
//	プレイヤー
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
//	初期化
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

	//	スプライト初期化
	const SpriteFrameFile* pSpriteFile = m_pData->GetSpriteFrameFile( m_pData->GetParam().SpriteName );
	m_pSprite = new SpriteFrame;
	MEM_CHECK( m_pSprite && pSpriteFile );

	m_pSprite->Init( pSpriteFile, _GetSpriteMatrix() );
	
	//	当たり判定生成
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

	//	武装の初期化
	m_pBlade		= new Blade;
	m_pOverdrive	= new Overdrive;
	m_pDash			= new Dash;
	m_pShield		= new Shield;
	MEM_CHECK( m_pBlade && m_pOverdrive && m_pDash && m_pShield );

	m_pBlade->Init( this, &m_pData->GetParam().Blade );
	m_pOverdrive->Init( this, &m_pData->GetParam().Overdrive );
	m_pDash->Init( this, &m_pData->GetParam().Dash );
	m_pShield->Init( this, &m_pData->GetParam().Shield );

	//	インジケート
	GetWorld()->GetIndicateManager()->AddIndicate( INDICATE_RING_DASH, m_pData->GetIndicateData( INDICATE_RING_DASH ) );

	//	初期アニメ
	_ChangeAnime( ANIME_WAIT );
}

//*****************************************************************************
//	更新
//*****************************************************************************
void Player::Update( void )
{
	//	カウント
	m_StateCount++;
	DecreaseZero( m_InvincibleCount, 1 );

	//	移動・位置を更新
	_UpdateMove();
	
	//	ルーラーフラグ落とす
	if( m_bRulerHit ) m_bRulerHit = false;

	//	当たり判定
	_CastCollide();

	//	ルーラー状態
	_UpdateRuler();

	//	スプライト更新・描画
	m_pSprite->Pause( _IsPauseSprite() );
	m_pSprite->Update( &_GetSpriteMatrix() );

	//	アニメ差し替え
	if( m_bAnimeReplace && m_pSprite->IsAnimeLast() )
	{
		m_bAnimeReplace = false;
	}

	//	当たり位置更新
	m_pCollide->UpdateMatrix( &GetMatrix() );

	//	自然回復
	_Regenerate();

	//	武装の更新
	m_pBlade->Update();
	m_pOverdrive->Update();
	m_pDash->Update();
	m_pShield->Update();

	//	アクション適用の更新
	if( _UpdateApplyAction() )
	{
		return;
	}

	//	状態更新
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

	//	インジケート
	_UpdateIndicate();

	//	色の変更
	Color Col = Color::White(); 
	if( m_pShield->IsInvincible() )
	{
		Col = COLOR_INVINCIBLE;
	}
	m_pSprite->SetColor( Col );
}

//******************************************************************************
//	描画
//******************************************************************************
void Player::Draw( void )
{
	m_pSprite->Draw( GetWorld()->GetDrawOffset() );
}

//******************************************************************************
//	ダメージ
//******************************************************************************
bool Player::Damage( float Attack, Point Dir, bool bThroughInvincible )
{
	if( Attack < 0 ) return true;

	return _Damage( Attack, Dir, bThroughInvincible );
}

//******************************************************************************
//	アイテム取得
//******************************************************************************
void Player::OnGetItem( Item* pItem )
{
	if( IsDead() ) return;

	float Energy = m_pData->GetParam().ItemEnergy;

	if( m_pOverdrive->IsReady() )
	{
		//	オーヴァドライヴ準備中
		m_pOverdrive->OnGetItem( Energy );
	}
	else
	{
		//	シールド
		Energy = m_pShield->OnGetItem( Energy );

		//	ブレード
		Energy = m_pBlade->OnGetItem( Energy );

		//	オーヴァドライヴ
		Energy = m_pOverdrive->OnGetItem( Energy );

		//	スコア通知
		GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_GET_ITEM );
		GetWorld()->GetScoreManager()->OnGetItem();
	}

	//	アイテム取得通知
	pItem->OnGet();
}

//******************************************************************************
//	ルーラーが破損した
//******************************************************************************
void Player::OnCrashRuler( void )
{
	m_pBlade->OnCrashRuler();
}

//******************************************************************************
//	復活
//******************************************************************************
void Player::Revive( void )
{
	//	アニメ
	_ChangeAnime( ANIME_REVIVE );

	//	武装を初期化
	SafePtr( m_pBlade )->Reset();
	SafePtr( m_pDash )->Reset();
	SafePtr( m_pOverdrive )->Reset();
	SafePtr( m_pShield )->Reset();
	SafePtr( m_pShield )->OnRevive();

	//	停止
	m_Vel.Zero();

	_ChangeState( STATE_REVIVE );
}

//******************************************************************************
//	アニメの差し替え
//******************************************************************************
void Player::ReplaceAnime( Label Name, int Intp )
{
	ANIME_INFO Info;
	Info.Name	= Name;
	Info.Intp	= Intp;

	m_ReplaceAnimeQueue.push_back( Info );
}

//******************************************************************************
//	アクションの実行
//******************************************************************************
void Player::ApplyAction( Label Name, Point Intp )
{
	ACTION_INFO Info;
	Info.Action	= _LabelToAction( Name );
	Info.Dir	= Intp;

	m_ApplyActionQueue.push_back( Info );
}

//******************************************************************************
//	アクションの制限を設定
//******************************************************************************
void Player::LockAction( Label Name, bool bEnable )
{
	ACTION Act = _LabelToAction( Name );

	//	制限設定
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
	//	制限解除
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
//	位置の移動
//******************************************************************************
void Player::SetPos( Point Pos )
{
	m_Pos = Pos;

	//	フィールド内に収める
	Rect FieldRect = GetWorld()->GetFieldRect();
	FieldRect.Widen( -m_pData->GetParam().HitRadius * 2 );
	m_Pos = FieldRect.Inside( m_Pos );
}

//*****************************************************************************
//	ワールドの取得
//*****************************************************************************
World* Player::GetWorld( void ) const
{
	return m_pManager->GetWorld();
}

//******************************************************************************
//	乱数を取得
//******************************************************************************
Rand& Player::GetRand( void ) const
{
	return m_pManager->GetRand();
}

//******************************************************************************
//	自機本体のα色を取得
//******************************************************************************
Color Player::GetBodyAlpha( void ) const
{
	Color Col = Color::White();

	if( m_pSprite ) Col.a = m_pSprite->GetBoneColor( BONE_NAME_BODY ).a;

	return Col;
}

//*****************************************************************************
//	無敵状態か？
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
//	表示物が有効か調べる
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
//	現在のステートが合致するかどうか調べる
//******************************************************************************
bool Player::IsState( Label Name ) const
{
	STATE State = _LabelToState( Name );

	return m_State == State;
}

//******************************************************************************
//	チャージ完了か
//******************************************************************************
bool Player::IsCharged( void ) const
{
	int ChargeTime = m_bRulerHit?	m_pData->GetParam().Blade.SmashParam.RulerChargeTime :
									m_pData->GetParam().Blade.SmashParam.ChargeTime;
	
	return m_State == STATE_CHARGE && m_StateCount >= ChargeTime;
}

//******************************************************************************
//	エフェクトを作成する
//******************************************************************************
Effect* Player::CreateEffect( Label Name, const Matrix3& Mat ) const
{
	if( Name.IsEmpty() ) return NULL;

	const EffectFile* pEffectFile = m_pData->GetEffectFile( Name );
	if( !pEffectFile ) return NULL;
	
	return GetApp()->GetEffectMng()->CreateEffect( pEffectFile, Mat );
}

//------------------------------------------------------------------------------
//	更新：通常状態
//------------------------------------------------------------------------------
void Player::_UpdateNormal( void )
{
	const Controller* pCtrl = _GetController();

	//	差し替えアニメ更新
	_UpdateReplaceAnime();

	//	アニメの更新
	bool bSideChange = false;
	Point Dir = pCtrl->GetDir();

	ANIME_INFO Anime = ANIME_WAIT;

	if( _IsMoveEnable() )
	{
		//	下向き
		if( Dir.y > 0.0f )
		{
			Anime = ANIME_MOVE_B;
		}
		//	横向き
		else if( Dir.y == 0.0f && Dir.x != 0.0f )
		{
			Anime = ANIME_MOVE_S;
		}
		//	前向き
		else if( Dir.y < 0.0f )
		{
			Anime = ANIME_MOVE_F;
		}

		//	向き
		if( Dir.x != 0.0f )
		{
			SIDE Side = (Dir.x > 0.0)? SIDE_R : SIDE_L;

			bSideChange = m_Side != Side;
			m_Side = Side;
		}
	}

	//	方向アニメ
	if( bSideChange || m_pSprite->GetAnimeName() != Anime.Name )
	{
		_ChangeAnime( Anime );
	}

	if( _IsAttackEnable() )
	{
		//	オーヴァドライヴ:スピア
		if( pCtrl->IsInput( CONTROLLER_DASH, INPUT_PRESS ) &&
			pCtrl->IsInput( CONTROLLER_BLADE, INPUT_TRIG ) )
		{
			if( m_pOverdrive->IsSpearEnable() )
			{
				_OverdriveSpear();
				return;
			}
		}

		//	オーヴァドライヴ:ルーラー
		else if( pCtrl->IsInput( CONTROLLER_DASH, INPUT_PRESS ) &&
				 pCtrl->IsInput( CONTROLLER_RELOAD, INPUT_TRIG ) )
		{
			if( m_pOverdrive->IsRulerEnable() )
			{
				_OverdriveRuler();
				return;;
			}
		}

		//	リロード
		else if( pCtrl->IsInput( CONTROLLER_RELOAD, INPUT_TRIG ) )
		{
			//	キャンセルリロード
			if( m_pBlade->IsCancelEnable() )
			{
				_CancelReload();
				return;
			}
			//	通常リロード
			else if( m_pBlade->IsReloadEnable() )
			{
				_Reload();
				return;
			}
		}

		//	ブレード
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

		//	ダッシュ
		else if( pCtrl->IsInput( CONTROLLER_DASH, INPUT_TRIG ) &&
				 !pCtrl->IsInput( CONTROLLER_BLADE, INPUT_TRIG ) && 
				 !pCtrl->IsInput( CONTROLLER_RELOAD, INPUT_TRIG ) &&
				 !Dir.IsZero() )
		{
				
			if( !m_pDash->IsDelay() )
			{
				//	ルーラー中はダッシュ斬り
				if( m_bRulerHit )
				{
					_Splash( Dir );
					return;
				}
				//	通常ダッシュ
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
//	更新：タメ状態
//------------------------------------------------------------------------------
void Player::_UpdateCharge( void )
{
	const Controller* pCtrl = _GetController();
	Point Dir = pCtrl->GetDir();

	//	操作ロック
	if( _IsActionLocked( ACTION_CHARGE ) )
	{
		_Slash();
		return;
	}

	//	折れた
	if( m_pBlade->IsCrash() )
	{
		_Knockback( Point( 0.0f, m_pData->GetParam().CrashVel ) );
		return;
	}

	//	取り消し
	if( pCtrl->IsInput( CONTROLLER_RELOAD, INPUT_TRIG ) )
	{
		//	ブレードにディレイ付与
		m_pBlade->Delay();

		_Normal();
		return;
	}
	
	int ChargeTime = m_bRulerHit?	m_pData->GetParam().Blade.SmashParam.RulerChargeTime :
									m_pData->GetParam().Blade.SmashParam.ChargeTime;

	//	タメ完了前
	if( m_StateCount < ChargeTime )
	{
		//	離した
		if( !pCtrl->IsInput( CONTROLLER_BLADE, INPUT_PRESS ) )
		{
			//	通常斬り
			_Slash();
			return;
		}
	}
	//	タメ最大
	else if( m_StateCount >= m_pData->GetParam().Blade.SmashParam.ChargeMaxTime )
	{
		//	スマッシュ
		_Smash();
		return;
	}
	//	タメ完了後
	else
	{
		//	離した/ルーラー中は最速発動
		if( !pCtrl->IsInput( CONTROLLER_BLADE, INPUT_PRESS ) || m_bRulerHit )
		{
			//	スマッシュ
			_Smash();
			return;
		}

		//	払い抜け
		if( pCtrl->IsInput( CONTROLLER_DASH, INPUT_TRIG ) &&
			!Dir.IsZero() )
		{
			_Splash( Dir );
			return;
		}
	}
}

//------------------------------------------------------------------------------
//	更新：ブレード状態
//------------------------------------------------------------------------------
void Player::_UpdateBlade( void )
{
	const Controller* pCtrl = _GetController();

	//	折れた
	if( m_pBlade->IsCrash() )
	{
		_Knockback( Point( 0.0f, m_pData->GetParam().CrashVel ) );
		return;
	}

	//	反発状態へ移行
	if( m_pBlade->IsRepulse() )
	{
		_Repulse();
		return;
	}

	//	ルーラー時・キャンセル可能の場合
	if( m_bRulerHit && m_pBlade->IsHit() && m_pBlade->IsSlashEnable() )
	{
		//	連続攻撃
		if( pCtrl->IsInput( CONTROLLER_BLADE, INPUT_PRESS ) )
		{
			//	静止状態なら反転
			if( m_bRulerHit && pCtrl->GetDir().IsZero() )
			{
				m_Side = (m_StateCount % 2)? SIDE_R : SIDE_L;
			}

			m_pBlade->CancelState();

			_Charge( pCtrl->GetDir() );
			return;
		}
	}

	//	アニメ終了
	if( m_pSprite->IsAnimeLast() )
	{
		//	スコア通知
		if( !m_pBlade->IsHit() )
		{
			GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_MISS );
		}

		//	ルーラー時、静止状態なら反転
		if( m_bRulerHit && pCtrl->GetDir().IsZero() )
		{
			m_Side = (m_StateCount % 2)? SIDE_R : SIDE_L;
		}

		//	通常状態へ
		_Normal();
	}
}

//------------------------------------------------------------------------------
//	更新：反発状態
//------------------------------------------------------------------------------
void Player::_UpdateRepulse( void )
{
	const Controller* pCtrl = _GetController();

	//	折れた
	if( m_pBlade->IsCrash() )
	{
		_Knockback( Point( 0.0f, m_pData->GetParam().CrashVel ) );
		return;
	}

	if( _IsAttackEnable() )
	{
		Point Dir = pCtrl->GetDir();

		//	ダッシュ
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
		//	手動解除
		if( pCtrl->IsInput( CONTROLLER_RELOAD, INPUT_TRIG ) )
		{
			//	通常状態へ
			_Normal();
			return;
		}
	}

	//	振り切った
	if( !m_pBlade->IsRepulse() )
	{
		//	無敵時間設定
		m_InvincibleCount = Max( m_InvincibleCount, m_pData->GetParam().RepulseInvincible );

		_ChangeState( STATE_BLADE );
	}
}

//------------------------------------------------------------------------------
//	更新：リロード状態
//------------------------------------------------------------------------------
void Player::_UpdateReload( void )
{
	//	アニメ終了
	if( m_pSprite->IsAnimeLast() )
	{
		//	通常状態へ
		_Normal();
	}
}

//------------------------------------------------------------------------------
//	更新：キャンセルリロード状態
//------------------------------------------------------------------------------
void Player::_UpdateCancel( void )
{
	//	アニメ終了
	if( m_pSprite->IsAnimeLast() )
	{
		//	通常状態へ
		_Normal();
	}
}

//------------------------------------------------------------------------------
//	更新：ダッシュ状態
//------------------------------------------------------------------------------
void Player::_UpdateDash( void )
{
	const Controller* pCtrl = _GetController();

	//	ブレード
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

	//	ダッシュ終了
	if( m_pSprite->IsAnimeLast() )
	{
		//	通常状態へ
		_Normal();
	}
}

//------------------------------------------------------------------------------
//	更新：オーヴァドライヴ発動状態
//------------------------------------------------------------------------------
void Player::_UpdateOverdrive( void )
{
	//	アニメ終了
	if( m_pSprite->IsAnimeLast() )
	{
		//	通常状態へ
		_Normal();
	}
}

//------------------------------------------------------------------------------
//	更新：ノックバック状態
//------------------------------------------------------------------------------
void Player::_UpdateKnock( void )
{
	const Controller* pCtrl = _GetController();

	//	リバーサル行動
	if( m_StateCount >= m_pData->GetParam().ReversalTime &&
		_IsAttackEnable() )
	{
		bool bAction = false;
		Point Dir = pCtrl->GetDir();

		//	オーヴァドライヴ:スピア
		if( pCtrl->IsInput( CONTROLLER_DASH, INPUT_PRESS ) &&
			pCtrl->IsInput( CONTROLLER_BLADE, INPUT_TRIG ) )
		{
			if( m_pOverdrive->IsSpearEnable() )
			{
				_OverdriveSpear();
				bAction = true;
			}
		}

		//	オーヴァドライヴ:ルーラー
		else if( pCtrl->IsInput( CONTROLLER_DASH, INPUT_PRESS ) &&
				 pCtrl->IsInput( CONTROLLER_RELOAD, INPUT_TRIG ) )
		{
			if( m_pOverdrive->IsRulerEnable() )
			{
				_OverdriveRuler();
				bAction = true;
			}
		}

		//	ダッシュ
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
			//	ブレイクキャンセル
			if( m_pShield->IsBreak() )
			{
				m_pShield->CancelBreak();
			}
			return;
		}
	}

	//	ノックバック終了
	if( m_StateCount >= m_pData->GetParam().KnockbackTime )
	{
		//	通常状態へ
		_Normal();
	}
}

//------------------------------------------------------------------------------
//	更新：死亡状態
//------------------------------------------------------------------------------
void Player::_UpdateDead( void )
{
	//	アニメ終了
	if( m_pSprite->IsAnimeLast() )
	{
		//	ワールドへ通知
		GetWorld()->OnPlayerDead();
	}
}

//------------------------------------------------------------------------------
//	更新：復活状態
//------------------------------------------------------------------------------
void Player::_UpdateRevive( void )
{
	//	アニメ終了
	if( m_pSprite->IsAnimeLast() )
	{
		//	通常状態へ
		_Normal();
	}
}

//------------------------------------------------------------------------------
//	位置の更新
//------------------------------------------------------------------------------
void Player::_UpdateMove( void )
{
	if( m_bEnemyRulerHit ) return;

	if( _IsMoveEnable() )
	{
		const Controller* pCtrl = _GetController();
		
		Point Dir = pCtrl->GetDir();
		
		//	移動の更新
		m_Pos += Dir * _GetMoveSpeed();
	}

	//	設定
	SetPos( m_Pos += m_Vel );

	//	減速
	m_Vel *= m_pData->GetParam().Decel;
}

//------------------------------------------------------------------------------
//	差し替えアニメの更新
//------------------------------------------------------------------------------
void Player::_UpdateReplaceAnime( void )
{
	if( m_ReplaceAnimeQueue.empty() ) return;

	const ANIME_INFO& Info = m_ReplaceAnimeQueue.front();

	if( m_pSprite->HasAnime( Info.Name ) )
	{
		m_pSprite->ChangeAnime( Info.Name, Info.Intp );
		m_bAnimeReplace = true;

		//	キューから撤去
		m_ReplaceAnimeQueue.pop_front();
	}
}

//------------------------------------------------------------------------------
//	アクション適用の更新
//------------------------------------------------------------------------------
bool Player::_UpdateApplyAction( void )
{
	if( m_ApplyActionQueue.empty() ) return false;

	const ACTION_INFO& Info = m_ApplyActionQueue.front();
	bool bExec = false;

	switch( Info.Action )
	{
	//	ブレード
	case ACTION_BLADE:

		//	タメ開始
		if( m_State == STATE_NORMAL )
		{
			if( m_pBlade->IsSlashEnable() )
			{
				_Charge( Info.Dir );
				return true;
			}
		}
		//	振る
		else if( m_State == STATE_CHARGE )
		{
			_Slash();
			bExec = true;
		}
		break;

	//	タメ
	case ACTION_CHARGE:

		//	タメ開始
		if( m_State == STATE_NORMAL )
		{
			if( m_pBlade->IsSlashEnable() )
			{
				_Charge( Info.Dir );
				bExec = true;
			}
		}
		break;

	//	リロード
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

	//	キャンセルリロード
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

	//	ダッシュ
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

	//	スマッシュ
	case ACTION_SMASH:

		//	タメ開始
		if( m_State == STATE_NORMAL )
		{
			if( m_pBlade->IsSlashEnable() )
			{
				_Charge( Info.Dir );
				return true;
			}
		}
		//	振る
		else if( m_State == STATE_CHARGE )
		{
			//	タメ完了
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

	//	スプラッシュ
	case ACTION_SPLASH:

		_ASSERT( !Info.Dir.IsZero() );

		//	タメ開始
		if( m_State == STATE_NORMAL )
		{
			if( m_pBlade->IsSlashEnable() )
			{
				_Charge( Info.Dir );
				return true;
			}
		}
		//	振る
		else if( m_State == STATE_CHARGE )
		{
			//	タメ完了
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

	//	スピア
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

	//	ルーラー
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

	//	それ以外
	default:
		bExec = true;
		break;

	}

	//	キューから撤去
	if( bExec )
	{
		m_ApplyActionQueue.pop_front();
	}

	return bExec;
}

//------------------------------------------------------------------------------
//	ルーラー状態の更新
//------------------------------------------------------------------------------
void Player::_UpdateRuler( void )
{
	//	ルーラー内
	if( m_bRulerHit )
	{
		//	エフェクトの更新
		if( m_pRulerEffect )
		{
			m_pRulerEffect->SetMatrix( GetMatrix() );
		}
		//	エフェクトの生成
		else
		{
			m_pRulerEffect = CreateEffect( m_pData->GetParam().Overdrive.Ruler.PlayerEffectName, GetMatrix() );
		}
	}
	//	ルーラー外
	else
	{
		EffectEndAppear( m_pRulerEffect );
	}
}

//------------------------------------------------------------------------------
//	インジケートの更新
//------------------------------------------------------------------------------
void Player::_UpdateIndicate( void )
{
	//	リング
	Indicate* pRingIndicate	= GetWorld()->GetIndicateManager()->GetIndicate( INDICATE_RING_DASH );
	if( pRingIndicate )
	{
		pRingIndicate->SetMatrix( GetMatrix() );

		pRingIndicate->SetColor( GetBodyAlpha() );

		//	進行方向の角度化
		int DirIndex = -1;
		const Controller* pCtrl = _GetController();
		if( !pCtrl->GetDir().IsZero() )
		{
			DirIndex = floorf( (pCtrl->GetDir().GetAngle() + Angle::ToRad(22.5f)) / Angle::ToRad(45.0f) );
		}

		//	ダッシュ
		for( int i = 0; i < INDICATE_RING_BONE_NUM; i++ )
		{
			String256 Bone;
			Bone.Format( INDICATE_BONE_MARKER_FMT, i );
			pRingIndicate->SetBoneVisible( Label(Bone.c_str()), m_pDash->IsActive() || !m_pDash->IsDelay() );

			pRingIndicate->SetColor( Label(Bone.c_str()), i != DirIndex? COLOR_INDICATE_BLACK :
															IsCharged()? COLOR_INDICATE_CHARGE :
																		 COLOR_INDICATE_NORMAL );
		}

		//	アイテム収集
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
//	通常状態へ
//------------------------------------------------------------------------------
void Player::_Normal( void )
{
	//	武装リセット
	SafePtr( m_pBlade )->CancelState();
	SafePtr( m_pDash )->CancelState();
	SafePtr( m_pOverdrive )->CancelState();

	_ChangeState( STATE_NORMAL );
}

//------------------------------------------------------------------------------
//	ブレードタメ
//------------------------------------------------------------------------------
void Player::_Charge( Point MoveDir )
{
	//	切り上げ
	ANIME_INFO Anime = ANIME_CHARGE_F;
	Point SlashDir( SQ2, -SQ2 );

	//	切り下ろし
	if( MoveDir.y > 0.0f )
	{
		Anime = ANIME_CHARGE_B;
		SlashDir.Set( SQ2, SQ2 );
	}
	//	横切り
	else if( MoveDir.y == 0.0f && MoveDir.x != 0.0f )
	{
		Anime = ANIME_CHARGE_S;
		SlashDir.Set( 1.0f, 0.0f );
	}

	//	アニメ
	_ChangeAnime( Anime );

	//	タメ開始
	if( m_Side == SIDE_L ) SlashDir.x = -SlashDir.x;
	m_pBlade->Charge( SlashDir );

	_ChangeState( STATE_CHARGE );
}

//------------------------------------------------------------------------------
//	ブレードを振る
//------------------------------------------------------------------------------
void Player::_Slash( void )
{
	//	操作ロック
	if( _IsActionLocked( ACTION_BLADE ) )
	{
		_Normal();
		return;
	}

	Label CurAnime = m_pSprite->GetAnimeName();

	//	切り上げ
	ANIME_INFO Anime = ANIME_BLADE_F;

	//	切り下ろし
	if( CurAnime == ANIME_CHARGE_B.Name )
	{
		Anime = ANIME_BLADE_B;
	}
	//	横切り
	else if( CurAnime == ANIME_CHARGE_S.Name )
	{
		Anime = ANIME_BLADE_S;
	}
	
	//	ルーラー中・静止ならばアニメはランダム
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

	//	アニメ
	_ChangeAnime( Anime );

	//	ブレード発動
	m_pBlade->Slash( false );

	_ChangeState( STATE_BLADE );
}

//------------------------------------------------------------------------------
//	スマッシュ発動
//------------------------------------------------------------------------------
void Player::_Smash( void )
{
	//	操作ロック
	if( _IsActionLocked( ACTION_SMASH ) )
	{
		_Normal();
		return;
	}

	Label CurAnime = m_pSprite->GetAnimeName();

	//	切り上げ
	ANIME_INFO Anime = ANIME_SMASH_F;

	//	切り下ろし
	if( CurAnime == ANIME_CHARGE_B.Name )
	{
		Anime = ANIME_SMASH_B;
	}
	//	横切り
	else if( CurAnime == ANIME_CHARGE_S.Name )
	{
		Anime = ANIME_SMASH_S;
	}

	//	アニメ
	_ChangeAnime( Anime );

	//	ブレード発動
	m_pBlade->Slash( true );

	_ChangeState( STATE_BLADE );
}

//------------------------------------------------------------------------------
//	反発する
//------------------------------------------------------------------------------
void Player::_Repulse( void )
{
	_ChangeState( STATE_REPULSE );
}

//------------------------------------------------------------------------------
//	リロードする
//------------------------------------------------------------------------------
void Player::_Reload( void )
{
	//	操作ロック
	if( _IsActionLocked( ACTION_RELOAD ) )
	{
		_Normal();
		return;
	}

	//	アニメ
	_ChangeAnime( ANIME_RELOAD );

	//	リロード発動
	m_pBlade->Reload();

	//	アイテム自動回収
	GetWorld()->GetItemManager()->AutoCollect();

	_ChangeState( STATE_RELOAD );
}

//------------------------------------------------------------------------------
//	キャンセルリロードする
//------------------------------------------------------------------------------
void Player::_CancelReload( void )
{
	//	操作ロック
	if( _IsActionLocked( ACTION_CANCEL ) )
	{
		_Normal();
		return;
	}

	//	アニメ
	_ChangeAnime( ANIME_CANCEL );

	//	キャンセルリロード発動
	m_pBlade->CancelReload();

	//	アイテム自動回収
	GetWorld()->GetItemManager()->AutoCollect();

	_ChangeState( STATE_CANCEL );
}

//------------------------------------------------------------------------------
//	ダッシュする
//------------------------------------------------------------------------------
void Player::_Dash( Point MoveDir )
{
	//	操作ロック
	if( _IsActionLocked( ACTION_DASH ) )
	{
		_Normal();
		return;
	}

	//	アニメ
	ANIME_INFO Anime = MoveDir.x == 0.0f? ANIME_DASH_N : ANIME_DASH_S;
	_ChangeAnime( Anime );

	//	ダッシュ発動
	m_pDash->Start( MoveDir );

	_ChangeState( STATE_DASH );
}

//------------------------------------------------------------------------------
//	払い抜け発動
//------------------------------------------------------------------------------
void Player::_Splash( Point MoveDir )
{
	//	操作ロック
	if( _IsActionLocked( ACTION_SPLASH ) )
	{
		_Normal();
		return;
	}

	//	アニメ
	ANIME_INFO Anime = MoveDir.x == 0.0f? ANIME_SPLASH_N : ANIME_SPLASH_S;
	_ChangeAnime( Anime );

	//	発動
	m_pDash->Splash( MoveDir );

	//	ブレードにディレイ付与
	m_pBlade->Delay();

	_ChangeState( STATE_DASH );
}

//------------------------------------------------------------------------------
//	オーヴァドライヴ：スピア発動
//------------------------------------------------------------------------------
void Player::_OverdriveSpear( void )
{
	//	操作ロック
	if( _IsActionLocked( ACTION_SPEAR ) )
	{
		_Normal();
		return;
	}

	//	アニメ
	_ChangeAnime( ANIME_SPEAR );

	//	スピア発動
	m_pOverdrive->DriveSpear();

	//	アイテム強制回収
	GetWorld()->GetItemManager()->ForceCollect();

	//	スコア通知
	GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_OVERDRIVE );

	_ChangeState( STATE_OVERDRIVE );
}

//------------------------------------------------------------------------------
//	オーヴァドライヴ：ルーラー発動
//------------------------------------------------------------------------------
void Player::_OverdriveRuler( void )
{
	//	操作ロック
	if( _IsActionLocked( ACTION_RULER ) )
	{
		_Normal();
		return;
	}

	//	アニメ
	_ChangeAnime( ANIME_RULER );

	//	ルーラー発動
	m_pOverdrive->DriveRuler();

	//	アイテム強制回収
	GetWorld()->GetItemManager()->ForceCollect();

	//	スコア通知
	GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_OVERDRIVE );

	_ChangeState( STATE_OVERDRIVE );
}

//------------------------------------------------------------------------------
//	ノックバックする
//------------------------------------------------------------------------------
void Player::_Knockback( Point Dir )
{
	//	アニメ
	_ChangeAnime( ANIME_KNOCK );

	//	武装を解除する	
	SafePtr( m_pBlade )->CancelState();
	SafePtr( m_pDash )->CancelState();
	SafePtr( m_pOverdrive )->CancelState();

	//	速度足す
	m_Vel += Dir;

	_ChangeState( STATE_KNOCK );
}

//------------------------------------------------------------------------------
//	死亡する
//------------------------------------------------------------------------------
void Player::_Dead( Point Dir )
{
	//	アニメ
	_ChangeAnime( ANIME_DEAD );

	//	武装を解除する	
	SafePtr( m_pBlade )->CancelState();
	SafePtr( m_pDash )->CancelState();
	SafePtr( m_pOverdrive )->CancelState();
	SafePtr( m_pOverdrive )->AllErase();

	//	速度足す
	m_Vel += Dir;

	_ChangeState( STATE_DEAD );
}

//------------------------------------------------------------------------------
//	ステートの変更
//------------------------------------------------------------------------------
void Player::_ChangeState( STATE State )
{
	m_State			= State;
	m_StateCount	= 0;
}

//------------------------------------------------------------------------------
//	アニメ情報
//------------------------------------------------------------------------------
void Player::_ChangeAnime( const ANIME_INFO& Info )
{
	//	アニメ差し替え中
	if( m_bAnimeReplace ) return;

	if( m_pSprite )
	{
		m_pSprite->ChangeAnime( Info.Name, Info.Intp );
	}
}

//------------------------------------------------------------------------------
//	自然回復処理
//------------------------------------------------------------------------------
void Player::_Regenerate( void )
{
	//	動作中は自然回復なし
	if( m_State != STATE_NORMAL ) return;
	if( m_bEnemyRulerHit ) return;

	float Energy = m_pData->GetParam().RegenEnergy;

	//	シールド
	Energy = m_pShield->Regenerate( Energy );

	//	ブレード
	Energy = m_pBlade->Regenerate( Energy );

	//	オーヴァドライヴ
	Energy = m_pOverdrive->Regenerate( Energy );
}

//------------------------------------------------------------------------------
//	ダメージを加える
//------------------------------------------------------------------------------
bool Player::_Damage( float Attack, Point Dir, bool bThroughInvincible )
{
	if( GetConfig()->GetDebugInfo().bEnemyHyperAttack )
	{
		Attack *= 100.0f;
	}

	//	無敵状態か？
	if( IsInvincible( bThroughInvincible ) ) return false;

	//	死亡判定
	if( m_pShield->IsDeadly() )
	{
		//	死亡
		_Dead( Dir * m_pData->GetParam().BreakVel );
		return true;
	}

	//	ダメージ
	m_pShield->Damage( Attack * _GetDamageCoef() );
	
	//	シールドブレイク
	if( m_pShield->IsBreak() )
	{
		_Knockback( Dir * m_pData->GetParam().BreakVel );
	}
	//	反動を与える
	else
	{
		m_Vel += Dir;
	}
	
	return true;
}

//------------------------------------------------------------------------------
//	当たりのキャストを行う
//------------------------------------------------------------------------------
void Player::_CastCollide( void )
{
	//	当たり判定を行う
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
		
		//	地形当たりの矯正
		m_Pos = Result.SolvePos;
	}
}

//------------------------------------------------------------------------------
//	移動可能か？
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
//	攻撃可能か？
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
//	アクションが制限中か？
//------------------------------------------------------------------------------
bool Player::_IsActionLocked( ACTION Action ) const
{
	dword Flag = (1 << Action);

	return (m_ActionLock & Flag) != 0;
}

//------------------------------------------------------------------------------
//	スプライトを一時停止するか？
//------------------------------------------------------------------------------
bool Player::_IsPauseSprite( void ) const
{
	if( m_State == STATE_REPULSE ) return true;

	if( m_bEnemyRulerHit ) return true;

	return false;
}

//------------------------------------------------------------------------------
//	移動速度の取得
//------------------------------------------------------------------------------
float Player::_GetMoveSpeed( void ) const
{
	//	低速
	if( m_State == STATE_REPULSE ||
		m_State == STATE_CHARGE && m_StateCount >= m_pData->GetParam().Blade.SmashParam.ChargeStartTime )
	{
		return m_pData->GetParam().SlowSpeed;
	}

	//	ルーラー内
	if( m_bRulerHit )
	{
		return m_pData->GetParam().RulerMoveSpeed;
	}

	return m_pData->GetParam().MoveSpeed;
}

//------------------------------------------------------------------------------
//	ダメージ係数の取得
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
//	スプライト用の行列を取得
//------------------------------------------------------------------------------
Matrix3 Player::_GetSpriteMatrix( void ) const
{
	//	向きによって反転
	Matrix3 Mat = Matrix3::Scale( Point( m_Side == SIDE_L? -1.0f : 1.0f, 1.0f ) );

	Mat.SetPos( m_Pos );

	return Mat;
}

//------------------------------------------------------------------------------
//	コントローラーの取得
//------------------------------------------------------------------------------
const Controller* Player::_GetController( void ) const
{
	return GetWorld()->GetController();
}

//------------------------------------------------------------------------------
//	文字列から状態を得る
//------------------------------------------------------------------------------
Player::STATE Player::_LabelToState( Label Name )
{
	return STATE( LabelToInt( Name, STATE_NAME_ARRAY, ArrayNum(STATE_NAME_ARRAY), STATE_NULL ) );
}

//------------------------------------------------------------------------------
//	文字列からアクションを得る
//------------------------------------------------------------------------------
Player::ACTION Player::_LabelToAction( Label Name )
{
	return ACTION( LabelToInt( Name, ACTION_NAME_ARRAY, ArrayNum(ACTION_NAME_ARRAY), ACTION_ALL ) );
}