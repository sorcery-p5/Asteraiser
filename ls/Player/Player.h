#pragma once

#include "Player/PlayerTypes.h"
#include "Collide/CollideObjectListener.h"

class PlayerManager;
class PlayerData;
class SpriteFrame;
class SpriteFrameResource;
class Effect;
class CollideObject;
class CollideCaster;
class World;
class Controller;
class Blade;
class Overdrive;
class Dash;
class Shield;
class Item;


///////////////////////////////////////////////////////////////////////////////
//
//	プレイヤー
//
///////////////////////////////////////////////////////////////////////////////
class Player : public CollideObjectListener
{
	REFERENCE_IMPL( Player );

public:
	//	アニメ情報
	struct	ANIME_INFO
	{
		Label	Name;
		int		Intp;
	};

public:
	Player( PlayerManager* pManager );
	virtual ~Player();

	void				Init( const PlayerData* pData, int PlayerID, Point Pos );
	void				Update( void );
	void				Draw( void );

	bool				Damage( float Attack, Point Dir, bool bThroughInvincible = false );
	void				OnGetItem( Item* pItem );
	void				OnCrashRuler( void );
	void				Revive( void );

	void				ReplaceAnime( Label Name, int Intp );
	void				ApplyAction( Label Name, Point Dir );
	void				LockAction( Label Name, bool bEnable );

	void				SetPos( Point Pos );
	void				SetSide( Label Side )										{ m_Side = Side == Label("L")? SIDE_L : SIDE_R; }
	void				SetMoveEnable( bool bEnable )								{ m_bMoveEnable = bEnable; }
	void				SetAttackEnable( bool bEnable )								{ m_bAttackEnable = bEnable; }
	void				SetIndicateEnable( bool bEnable )							{ m_bIndicateEnable = bEnable; }
	void				SetEnemyRulerHit( bool bHit )								{ m_bEnemyRulerHit = bHit; }

	World*				GetWorld( void )									const;
	Rand&				GetRand( void )										const;
	Blade*				GetBlade( void )									const	{ return m_pBlade; }
	const SpriteFrame*	GetSprite( void )									const	{ return m_pSprite; }
	const PlayerData*	GetData( void )										const	{ return m_pData; }
	Point				GetPos( void )										const	{ return m_Pos; }
	Matrix3				GetMatrix( void )									const	{ return Matrix3::Trans(m_Pos); }
	int					GetStateCount( void )								const	{ return m_StateCount; }
	Color				GetBodyAlpha( void )								const;

	bool				IsInvincible( bool bThrough = false )				const;
	bool				IsIndicateEnable( void )							const;
	bool				IsDead( void )										const	{ return m_State == STATE_DEAD; }
	bool				IsRulerHit( void )									const	{ return m_bRulerHit; }
	bool				IsState( Label Name )								const;
	bool				IsCharged( void )									const;

	Effect*				CreateEffect( Label Name, const Matrix3& Mat )		const;

private:
	//	状態
	enum STATE
	{
		STATE_NORMAL,		//	通常
		STATE_CHARGE,		//	ブレードタメ
		STATE_BLADE,		//	ブレード振る
		STATE_REPULSE,		//	反発
		STATE_RELOAD,		//	リロード
		STATE_CANCEL,		//	キャンセルリロード
		STATE_DASH,			//	ダッシュ
		STATE_OVERDRIVE,	//	オーヴァドライヴ発動
		STATE_KNOCK,		//	ノックバック
		STATE_DEAD,			//	死亡
		STATE_REVIVE,		//	復活

		STATE_NULL	= -1,	//	無効
	};

	//	向き
	enum SIDE
	{
		SIDE_L,
		SIDE_R,
	};
	
	//	アクション
	enum ACTION
	{
		ACTION_BLADE,
		ACTION_CHARGE,
		ACTION_RELOAD,
		ACTION_CANCEL,	
		ACTION_DASH,
		ACTION_SMASH,
		ACTION_SPLASH,
		ACTION_SPEAR,
		ACTION_RULER,

		ACTION_ALL			= -1,
	};

	//	アクション情報
	struct	ACTION_INFO
	{
		ACTION	Action;
		Point	Dir;
	};

	typedef std::deque<ANIME_INFO>	AnimeInfoQueue;
	typedef std::deque<ACTION_INFO>	ActionInfoQueue;

private:
	void					_UpdateNormal( void );
	void					_UpdateCharge( void );
	void					_UpdateBlade( void );
	void					_UpdateRepulse( void );
	void					_UpdateReload( void );
	void					_UpdateCancel( void );
	void					_UpdateDash( void );
	void					_UpdateOverdrive( void );
	void					_UpdateKnock( void );
	void					_UpdateDead( void );
	void					_UpdateRevive( void );

	void					_UpdateMove( void );
	void					_UpdateReplaceAnime( void );
	bool					_UpdateApplyAction( void );
	void					_UpdateRuler( void );
	void					_UpdateIndicate( void );

	void					_Normal( void );
	void					_Charge( Point MoveDir );
	void					_Slash( void );
	void					_Smash( void );
	void					_Repulse( void );
	void					_Reload( void );
	void					_CancelReload( void );
	void					_Dash( Point MoveDir );
	void					_Splash( Point MoveDir );
	void					_OverdriveSpear( void );
	void					_OverdriveRuler( void );
	void					_Knockback( Point Dir );
	void					_Dead( Point Dir );

	void					_ChangeState( STATE State );
	void					_ChangeAnime( const ANIME_INFO& Info );

	void					_Regenerate( void );
	bool					_Damage( float Attack, Point KnockDir, bool bThroughInvincible = false );

	void					_CastCollide( void );
	Matrix3					_GetSpriteMatrix( void )			const;

	bool					_IsMoveEnable( void )				const;
	bool					_IsAttackEnable( void )				const;
	bool					_IsActionLocked( ACTION Action )	const;
	bool					_IsPauseSprite( void )				const;
	float					_GetMoveSpeed( void )				const;
	float					_GetDamageCoef( void )				const;
	const Controller*		_GetController( void )				const;

	static STATE			_LabelToState( Label Name );
	static ACTION			_LabelToAction( Label Name );

private:
	PlayerManager*			m_pManager;
	const PlayerData*		m_pData;
	int						m_PlayerID;

	SpriteFrame*			m_pSprite;
	CollideObject*			m_pCollide;
	CollideCaster*			m_pCaster;

	Blade*					m_pBlade;
	Overdrive*				m_pOverdrive;
	Dash*					m_pDash;
	Shield*					m_pShield;
	
	RefPtr<Effect>			m_pRulerEffect;

	STATE					m_State;
	int						m_StateCount;
	int						m_InvincibleCount;
	SIDE					m_Side;
	Point					m_Pos;
	Point					m_Vel;

	bool					m_bMoveEnable;
	bool					m_bAttackEnable;
	bool					m_bInvincible;
	bool					m_bIndicateEnable;
	bool					m_bRulerHit;
	bool					m_bEnemyRulerHit;

	AnimeInfoQueue			m_ReplaceAnimeQueue;
	bool					m_bAnimeReplace;
	ActionInfoQueue			m_ApplyActionQueue;
	dword					m_ActionLock;
};