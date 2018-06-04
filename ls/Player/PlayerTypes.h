#pragma once


////////////////////////////////////////////////////////////////////////////////
//
//	プレイヤー型定義
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	定数
//------------------------------------------------------------------------------
const int OVERDRIVE_EFFECT_NUM			= 3;

//------------------------------------------------------------------------------
//	ブレード：エネルギーパラメータ
//------------------------------------------------------------------------------
struct BLADE_ENERGY_PARAM
{
	BLADE_ENERGY_PARAM( void );

	float			ExhaustEnergyRate;			//	攻撃ヒット時のエネルギー消耗率
	float			ExhaustLimitRate;			//	攻撃ヒット時の耐久値消耗率

	float			NormalRegen;				//	エネルギー自然回復量
	float			NormalLimitRegen;			//	耐久値自然回復量
	float			ItemRegen;					//	アイテム取得によるエネルギー回復量
	float			ItemLimitRegen;				//	アイテム取得による耐久値回復量
	float			ReloadRegen;				//	リロード時のエネルギー・耐久値回復量
	float			CrashRegen;					//	欠損時のエネルギー・耐久値回復量
	float			CancelEnergyDecay;			//	キャンセルエネルギーの減少量
	
	float			UseRegenEnergy;				//	エネルギー回復時消費量
	float			UseLimitEnergy;				//	耐久値回復時消費量
	float			UseReloadEnergy;			//	リロード状態回復時消費量
};

//------------------------------------------------------------------------------
//	ブレード：スマッシュパラメータ
//------------------------------------------------------------------------------
struct BLADE_SMASH_PARAM
{
	BLADE_SMASH_PARAM( void );

	Label				HitEffectName;				//	ヒットエフェクト

	int					ChargeTime;					//	タメ時間
	int					ChargeStartTime;			//	タメ開始時間
	int					ChargeMaxTime;				//	タメ最大時間
	int					RulerChargeTime;			//	ルーラーのタメ時間
	float				Length;						//	剣の長さ
	float				Radius;						//	剣の太さ（半径）

	float				Attack;						//	攻撃力
	float				AttackVel;					//	攻撃時反動力
	int					Knockback;					//	攻撃時ノックバック時間

	float				ExhaustRate;				//	消耗比率
	float				ChargeExhaustEnergy;		//	タメ時のエネルギー消耗
	float				ChargeExhaustLimit;			//	タメ時の耐久値消耗
};

//------------------------------------------------------------------------------
//	ブレードパラメータ
//------------------------------------------------------------------------------
struct BLADE_PARAM
{
	BLADE_PARAM( void );

	Label				EffectName;					//	エフェクト
	Label				HitEffectName;				//	ヒットエフェクト
	Label				RepulseEffectName;			//	反発エフェクト
	Label				RepulseHitEffectName;		//	反発ヒットエフェクト
	Label				RepulseEndEffectName;		//	反発終了エフェクト
	Label				CrashEffectName;			//	破損エフェクト
	Label				RulerEffectName;			//	ルーラー中のエフェクト
	Label				RulerHitEffectName;			//	ルーラー中のヒットエフェクト

	float				Length;						//	剣の長さ
	float				Radius;						//	剣の太さ（半径）
	int					NormalDelay;				//	通常攻撃時のディレイ
	int					HitDelay;					//	剣を当てた後のディレイ
	int					CancelTime;					//	キャンセル受付時間
	int					NotExhaustTime;				//	消費しない時間
	int					NotReloadTime;				//	リロードできない時間

	float				Attack;						//	攻撃力
	float				RepulseAttack;				//	反発中攻撃力
	float				AttackEnergyRate;			//	エネルギーによる攻撃力減損率
	float				AttackLimitRate;			//	耐久による攻撃力減損率
	float				AttackVel;					//	攻撃時反動力
	int					Knockback;					//	攻撃時ノックバック時間
	int					RepulseInterval;			//	反発の発生間隔
	
	float				RulerAttack;				//	ルーラー中の攻撃力
	int					RulerDelay;					//	ルーラー中のディレイ
	float				RulerLength;				//	ルーラー中の剣長さ
	float				RulerRadius;				//	ルーラー中の剣太さ

	int					HitItemNum;					//	命中時アイテム発生数
	int					RepulseItemNum;				//	反発時アイテム発生数
	int					CrashItemNum;				//	破損時アイテム発生数
	int					ReloadItemNum;				//	リロード時アイテム発生数
	float				ItemSpeed;					//	アイテム発生速度

	BLADE_ENERGY_PARAM	EnergyParam;
	BLADE_SMASH_PARAM	SmashParam;	
};

//------------------------------------------------------------------------------
//	スピアパラメータ
//------------------------------------------------------------------------------
struct SPEAR_PARAM
{
	SPEAR_PARAM( void );

	Label			EffectName;				//	エフェクト
	Label			HitEffectName;			//	ヒットエフェクト
	Label			CrashEffectName;		//	破損エフェクト
	Label			LockEffectName;			//	ロックエフェクト

	float			Length;					//	長さ
	float			Radius;					//	太さ（半径）
	float			Speed;					//	スピード
	int				Time;					//	効果時間
	float			AttackVel;				//	攻撃の反動
	int				Knockback;				//	ノックバック
	float			GroundExhaust;			//	地形ヒットによる消耗

	float			Energy;					//	エネルギー
	float			EnergyMax;				//	エネルギーの最大値
	float			EnergyItemRate;			//	アイテム取得によるエネルギー加算量
	float			EnergyDecayRate;		//	攻撃ヒットによるエネルギー減衰率

	float			Attack;					//	攻撃力
	float			AttackMax;				//	攻撃力の最大値
	float			AttackItemRate;			//	アイテム取得による攻撃力加算量
	float			AttackDecayRate;		//	攻撃ヒットによる攻撃力減衰率
	
	int				HitItemNum;				//	命中時アイテム発生数
	int				CrashItemNum;			//	破損時アイテム発生数
	float			ItemSpeed;				//	アイテム発生速度
};

//------------------------------------------------------------------------------
//	ルーラーパラメータ
//------------------------------------------------------------------------------
struct RULER_PARAM
{
	RULER_PARAM( void );

	Label			EffectName;				//	エフェクト
	Label			CrashEffectName;		//	破損エフェクト
	Label			PlayerEffectName;		//	自機に付くエフェクト

	float			Radius;					//	半径
	float			RadiusMax;				//	半径の最大値
	float			RadiusItemRate;			//	アイテム取得による半径の加算量
	int				Time;					//	効果時間
	int				TimeMax;				//	効果時間の最大値
	float			TimeItemRate;			//	アイテム取得による効果時間の加算量
	
	int				CrashItemNum;			//	破損時アイテム発生数
	float			ItemSpeed;				//	アイテム発生速度
};

//------------------------------------------------------------------------------
//	オーヴァドライヴパラメータ
//------------------------------------------------------------------------------
struct OVERDRIVE_PARAM
{
	OVERDRIVE_PARAM( void );

	float			NormalRegen;						//	通常時エネルギー回復量
	float			ItemRegen;							//	アイテム取得によるエネルギー回復量
	float			UseRegenEnergy;						//	エネルギー回復時消費量

	Label			EffectName[OVERDRIVE_EFFECT_NUM];	//	エフェクトの名前
	Label			ChargeEffectName;					//	チャージエフェクトの名前
	Label			MaxEffectName;						//	最大チャージエフェクトの名前

	int				Delay;								//	使用後のディレイ

	SPEAR_PARAM		Spear;
	RULER_PARAM		Ruler;
};

//------------------------------------------------------------------------------
//	ダッシュ：払い抜けパラメータ
//------------------------------------------------------------------------------
struct DASH_SPLASH_PARAM
{
	DASH_SPLASH_PARAM( void );

	Label			EffectName;					//	エフェクト
	Label			HitEffectName;				//	ヒットエフェクト

	float			CollideRadius;				//	判定レイの太さ
	float			RulerRadius;				//	ルーラー時判定レイの太さ
	float			Attack;						//	攻撃力
	float			AttackVel;					//	攻撃時反動力
	int				Knockback;					//	攻撃時ノックバック時間
	float			ExhaustRate;				//	消耗比率
	int				HitInvincible;				//	ヒット時の無敵時間
};

//------------------------------------------------------------------------------
//	ダッシュパラメータ
//------------------------------------------------------------------------------
struct DASH_PARAM
{
	DASH_PARAM( void );

	Label				EffectName;					//	エフェクト

	float				MoveLength;					//	移動距離
	int					Delay;						//	ディレイ
	int					RulerDelay;					//	ルーラー時のディレイ

	DASH_SPLASH_PARAM	Splash;
};

//------------------------------------------------------------------------------
//	シールドパラメータ
//------------------------------------------------------------------------------
struct SHIELD_PARAM
{
	SHIELD_PARAM( void );
	
	Label			HitEffectName;				//	ヒットエフェクト
	Label			BreakEffectName;			//	破損エフェクト
	Label			RecoveryEffectName;			//	回復エフェクト

	int				DamageInvincible;			//	被弾時無敵時間
	int				BreakInvincible;			//	ブレイク時無敵時間
	int				ReviveInvincible;			//	復活時無敵時間
	int				DamageNoMove;				//	被弾時移動不可時間
	int				BreakTime;					//	ブレイク状態時間
	int				SerialDamageTime;			//	連続被弾判定時間

	float			NormalRegen;				//	通常時シールド回復量
	float			ItemRegen;					//	アイテム取得によるシールド回復量
	float			NormalRecovery;				//	通常時ブレイク部回復量
	float			ItemRecovery;				//	アイテム取得によるブレイク部回復量
	int				BreakNumMax;				//	ブレイク回数の最大値
	float			BreakNumDecayRate;			//	ブレイク回数によるブレイク部回復率
	float			UseRegenEnergy;				//	シールド回復時消費量
	
	int				BreakItemNum;				//	破損時アイテム発生数
	float			ItemSpeed;					//	アイテム発生速度
};

//------------------------------------------------------------------------------
//	プレイヤーパラメータ
//------------------------------------------------------------------------------
struct PLAYER_PARAM
{
	PLAYER_PARAM( void );

	Label			SpriteName;					//	スプライト名

	float			MoveSpeed;					//	移動速度
	float			SlowSpeed;					//	低速状態の移動速度
	float			Decel;						//	減速係数
	float			CrashVel;					//	ブレード欠損時の反動力
	float			BreakVel;					//	ブレイク時の反動力
	int				KnockbackTime;				//	ノックバック時間
	int				ReversalTime;				//	リバーサル可能時間
	int				RepulseInvincible;			//	反発後の無敵時間

	float			RulerMoveSpeed;				//	ルーラー時の移動速度

	float			HitRadius;					//	当たり半径
	float			ItemCollectRadius;			//	アイテム回収半径

	float			RegenEnergy;				//	自然回復エネルギー
	float			ItemEnergy;					//	アイテムのエネルギー

	BLADE_PARAM		Blade;
	OVERDRIVE_PARAM	Overdrive;
	DASH_PARAM		Dash;
	SHIELD_PARAM	Shield;
};