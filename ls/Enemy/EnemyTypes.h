#pragma once


////////////////////////////////////////////////////////////////////////////////
//
//	敵型定義
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	敵パラメータ
//------------------------------------------------------------------------------
struct ENEMY_PARAM
{
	ENEMY_PARAM( void );

	Label			SpriteName;
	std::string		PreviewName;
	int				Life;
	float			Decel;
	int				DamageInvincible;
	float			Defense;
	float			ExhaustRate;
	float			ReduceVel;
	float			ReduceKnockback;
	float			BodyDamage;
	float			BodyDamageVel;
	int				ItemNum;
	float			ItemSpeed;
	float			ScoreRate;
	int				CrashKnockback;
	float			CrashVel;
	float			CollideRadius;
	bool			bCollideFrame;
	bool			bHitEnable;
	bool			bGroundHit;
	bool			bPlayerHit;
	bool			bRepulse;
	bool			bNotRepulseNotify;
	bool			bNoCount;
	bool			bBoss;
	bool			bOutField;
	bool			bNoIndicate;
	Color			DeadColor;
};

//------------------------------------------------------------------------------
//	敵：目標タイプ
//------------------------------------------------------------------------------
enum ENEMY_TARGET_TYPE
{
	ENEMY_TARGET_FIX,			//	固定座標
	ENEMY_TARGET_PLAYER,		//	自機
	ENEMY_TARGET_SELF,			//	自分
	ENEMY_TARGET_ENEMY,			//	特定の敵

	ENEMY_TARGET_NONE	= -1,	//	無効
};

//------------------------------------------------------------------------------
//	敵：目標位置タイプ
//------------------------------------------------------------------------------
enum ENEMY_TARGET_POS_TYPE
{
	ENEMY_TARGET_POS_NONE,		//	なし
	ENEMY_TARGET_POS_RECT,		//	直交
	ENEMY_TARGET_POS_POLER,		//	極
};

//------------------------------------------------------------------------------
//	敵：目標軸タイプ
//------------------------------------------------------------------------------
enum ENEMY_TARGET_AXIS_TYPE
{
	ENEMY_TARGET_AXIS_ABS,		//	絶対
	ENEMY_TARGET_AXIS_REL,		//	相対
};


//------------------------------------------------------------------------------
//	敵：目標パラメータ
//------------------------------------------------------------------------------
struct ENEMY_TARGET
{
	ENEMY_TARGET_TYPE		Type;
	ENEMY_TARGET_POS_TYPE	PosType;
	ENEMY_TARGET_AXIS_TYPE	AxisType;
	union
	{
		struct
		{
			float			X, Y;
		};
		
		struct
		{
			float			Ang, Len;
		};
		float				Pos[2];
	};
	float					RandRange[2];
	LabelStr				Name;
	int						Interval;
	bool					bOutField;
};

//------------------------------------------------------------------------------
//	敵：移動タイプ
//------------------------------------------------------------------------------
enum ENEMY_MOVE_TYPE
{
	ENEMY_MOVE_TARGET,				//	目標
	ENEMY_MOVE_TARGET_X,			//	目標：横のみ移動
	ENEMY_MOVE_TARGET_Y,			//	目標：縦のみ移動

	ENEMY_MOVE_NONE			= -1,	//	無効
};

//------------------------------------------------------------------------------
//	敵：移動アクションパラメータ
//------------------------------------------------------------------------------
struct ENEMY_MOVE_ACTION
{
	ENEMY_MOVE_TYPE		Type;
	ENEMY_TARGET		Target;
	float				Accel;
	float				Decel;
	float				MaxSpeed;
};

//------------------------------------------------------------------------------
//	敵：回転タイプ
//------------------------------------------------------------------------------
enum ENEMY_ROT_TYPE
{
	ENEMY_ROT_FIX,				//	固定
	ENEMY_ROT_TARGET,			//	ターゲット
	ENEMY_ROT_MOVE,				//	移動目標
	
	ENEMY_ROT_NONE		= -1,	//	無効
};

//------------------------------------------------------------------------------
//	敵：回転アクションパラメータ
//------------------------------------------------------------------------------
struct ENEMY_ROT_ACTION
{
	ENEMY_ROT_TYPE		Type;
	float				Speed;

	union
	{
		ENEMY_TARGET		Target;
		float				Fix;
	};
};

//------------------------------------------------------------------------------
//	敵：攻撃発生位置タイプ
//------------------------------------------------------------------------------
enum ENEMY_ATTACK_POS_TYPE
{
	ENEMY_ATTACK_POS_CENTER,			//	中心
	ENEMY_ATTACK_POS_BONE,				//	ボーンから発生
	ENEMY_ATTACK_POS_TARGET,			//	目標位置
	
	ENEMY_ATTACK_POS_NONE		= -1,	//	無効
};

//------------------------------------------------------------------------------
//	敵：攻撃方向タイプ
//------------------------------------------------------------------------------
enum ENEMY_ATTACK_DIR_TYPE
{
	ENEMY_ATTACK_DIR_FIX,			//	固定
	ENEMY_ATTACK_DIR_STRAIGHT,		//	直進
	ENEMY_ATTACK_DIR_TARGET,		//	目標指定
	ENEMY_ATTACK_DIR_MOVE,			//	移動目標
	ENEMY_ATTACK_DIR_ROT,			//	旋回目標
	
	ENEMY_ATTACK_DIR_NONE	= -1,	//	無効
};

//------------------------------------------------------------------------------
//	敵：攻撃位置
//------------------------------------------------------------------------------
struct ENEMY_ATTACK_POS 
{
	ENEMY_ATTACK_POS_TYPE		Type;
	union
	{
		ENEMY_TARGET			Target;
		LabelStr				Bone;
	};
};

//------------------------------------------------------------------------------
//	敵：攻撃方向
//------------------------------------------------------------------------------
struct ENEMY_ATTACK_DIR
{
	ENEMY_ATTACK_DIR_TYPE		Type;
	union
	{
		ENEMY_TARGET			Target;
		float					Fix;
	};
};

//------------------------------------------------------------------------------
//	敵：攻撃アクションパラメータ
//------------------------------------------------------------------------------
struct ENEMY_ATTACK_ACTION
{
	int							Interval;
	int							AppearNum;

	ENEMY_ATTACK_POS			Pos;
	ENEMY_ATTACK_DIR			Dir;

	LabelStr					Name;
};

//------------------------------------------------------------------------------
//	敵：召喚アクションパラメータ
//------------------------------------------------------------------------------
struct ENEMY_SUMMON_ACTION
{
	LabelStr					Name;
	LabelStr					AppearName;
	LabelStr					BoneName;
	float						X, Y;
	float						Rot;
	bool						bAttach;
};

//------------------------------------------------------------------------------
//	敵：ワープアクションパラメータ
//------------------------------------------------------------------------------
struct ENEMY_WARP_ACTION
{
	ENEMY_TARGET		Target;
};

//------------------------------------------------------------------------------
//	敵：条件種別
//------------------------------------------------------------------------------
enum ENEMY_CONDITION_TYPE
{
	ENEMY_CONDITION_MOVE_END,			//	移動完了
	ENEMY_CONDITION_ROT_END,			//	旋回完了
	ENEMY_CONDITION_ATTACK_END,			//	攻撃完了
	ENEMY_CONDITION_ANIME_END,			//	アニメ終了
	ENEMY_CONDITION_ANIME_NOTIFY,		//	アニメ通知
	ENEMY_CONDITION_NOTIFY,				//	通知

	ENEMY_CONDITION_CHILD_DEFEAT,		//	子が死亡
	ENEMY_CONDITION_CHILD_ALL_DEFEAT,	//	子が全滅
	ENEMY_CONDITION_CHILD_EMPTY,		//	子がいない

	ENEMY_CONDITION_SELF_POS_X,			//	自身の位置(X軸)
	ENEMY_CONDITION_SELF_POS_Y,			//	自身の位置(Y軸)

	ENEMY_CONDITION_PLAYER_RANGE,		//	自機への距離
	ENEMY_CONDITION_PLAYER_RANGE_X,		//	自機への距離(X軸)
	ENEMY_CONDITION_PLAYER_RANGE_Y,		//	自機への距離(Y軸)
	ENEMY_CONDITION_PLAYER_ANGLE,		//	自機への角度
	ENEMY_CONDITION_PLAYER_ANGLE_REL,	//	自機への角度(相対)
	ENEMY_CONDITION_PLAYER_POS_X,		//	自機の位置(X軸)
	ENEMY_CONDITION_PLAYER_POS_Y,		//	自機の位置(Y軸)
	ENEMY_CONDITION_PLAYER_STATE,		//	自機の状態
	ENEMY_CONDITION_PLAYER_STATE_TIME,	//	自機の状態時間

	ENEMY_CONDITION_FRAME,				//	フレーム経過
	ENEMY_CONDITION_RANDOM,				//	ランダム
	ENEMY_CONDITION_STATE_END,			//	ステート終了
	ENEMY_CONDITION_LIFE_LESS,			//	ライフ減少

	ENEMY_CONDITION_DAMAGE,				//	ダメージ
	ENEMY_CONDITION_KNOCKBACK,			//	ノックバック
	ENEMY_CONDITION_REPULSE,			//	反発
	ENEMY_CONDITION_DEAD,				//	死亡

	ENEMY_CONDITION_NONE		= -1,	//	無効
};

//------------------------------------------------------------------------------
//	敵：条件
//------------------------------------------------------------------------------
struct ENEMY_CONDITION
{
	ENEMY_CONDITION_TYPE	Type;
	bool					bNot;

	union
	{
		float				Param;
		float				Range[2];
		LabelStr			Name;
		bool				bFlag;
	};
};
typedef std::vector< ENEMY_CONDITION >	ENEMY_CONDITION_LIST;

//------------------------------------------------------------------------------
//	敵：アクションタイプ
//------------------------------------------------------------------------------
enum ENEMY_ACTION_TYPE
{
	ENEMY_ACTION_MOVE,					//	移動
	ENEMY_ACTION_ROT,					//	旋回
	ENEMY_ACTION_ATTACK,				//	攻撃
	ENEMY_ACTION_SUMMON,				//	召喚
	ENEMY_ACTION_WARP,					//	ワープ

	ENEMY_ACTION_END_MOVE,				//	移動キャンセル
	ENEMY_ACTION_END_ROT,				//	旋回停止
	ENEMY_ACTION_END_ATTACK,			//	攻撃停止
	ENEMY_ACTION_END_ACTION,			//	全アクション停止

	ENEMY_ACTION_CHANGE_ANIME,			//	アニメ変更
	ENEMY_ACTION_PAUSE_ANIME,			//	アニメの停止
	ENEMY_ACTION_CHANGE_STATE,			//	ステート変更
	ENEMY_ACTION_EFFECT,				//	エフェクト
	ENEMY_ACTION_SOUND,					//	サウンド
	ENEMY_ACTION_COLOR,					//	色変更

	ENEMY_ACTION_ADD_LIFE,				//	耐久力加算
	ENEMY_ACTION_ADD_DEFENSE,			//	防御力加算

	ENEMY_ACTION_INVINCIBLE,			//	無敵
	ENEMY_ACTION_HIDE,					//	ハイド状態
	ENEMY_ACTION_NO_DAMAGE,				//	ダメージ無効
	ENEMY_ACTION_HIT,					//	当たり判定
	ENEMY_ACTION_GROUND_HIT,			//	地形との当たり判定
	ENEMY_ACTION_OUT_FIELD,				//	画面外への移動可否
	ENEMY_ACTION_NO_COUNT,				//	カウント可否
	ENEMY_ACTION_REPULSE,				//	反発の有無
	
	ENEMY_ACTION_PARENT_NOTIFY,			//	親へ通知
	ENEMY_ACTION_CHILD_NOTIFY,			//	子へ通知

	ENEMY_ACTION_STAGE_NOTIFY,			//	ステージへ通知
	ENEMY_ACTION_LOCK_SCORE,			//	スコアロック

	ENEMY_ACTION_DESTROY,				//	破壊
	ENEMY_ACTION_ESCAPE,				//	逃走
	
	ENEMY_ACTION_NONE			= -1,	//	無効
};

//------------------------------------------------------------------------------
//	敵：アクションパラメータ
//------------------------------------------------------------------------------
struct ENEMY_ACTION
{
	ENEMY_ACTION_TYPE		Type;
	ENEMY_CONDITION_LIST	ConditionList;
	int						Time;
	bool					bContinue;
	bool					bOnce;

	union
	{
		ENEMY_MOVE_ACTION	Move;
		ENEMY_ROT_ACTION	Rot;
		ENEMY_ATTACK_ACTION	Attack;
		ENEMY_SUMMON_ACTION	Summon;
		ENEMY_WARP_ACTION	Warp;
	};

	LabelStr				Name;
	LabelStr				ParamStr;
	float					Param;
	bool					bFlag;
	Color					Col;
};

//------------------------------------------------------------------------------
//	攻撃からの通知
//------------------------------------------------------------------------------
const Label		ENEMY_ATTACK_NOTIFY_CRASH		= Label( "Crash" );		//	ブレードクラッシュ
const Label		ENEMY_ATTACK_NOTIFY_BREAK		= Label( "Break" );		//	シールドブレイク
const Label		ENEMY_ATTACK_NOTIFY_REPULSE		= Label( "Repulse" );	//	反動

//------------------------------------------------------------------------------
//	敵：ハンドラステート種別
//------------------------------------------------------------------------------
const Label		ENEMY_HANDLER_STATE_ROOT		= Label( "Root" );
const Label		ENEMY_HANDLER_STATE_RESIDENT	= Label( "Resident" );
const Label		ENEMY_HANDLER_STATE_DAMAGE		= Label( "Damage" );
const Label		ENEMY_HANDLER_STATE_DEAD		= Label( "Dead" );
const Label		ENEMY_HANDLER_STATE_REPULSE		= Label( "Repulse" );
const Label		ENEMY_HANDLER_STATE_CRASH		= Label( "Crash" );
const Label		ENEMY_HANDLER_STATE_BREAK		= Label( "Break" );