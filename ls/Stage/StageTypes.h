#pragma once


////////////////////////////////////////////////////////////////////////////////
//
//	ステージ：型定義
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	ステージパラメータ
//------------------------------------------------------------------------------
struct STAGE_PARAM
{
	STAGE_PARAM( void );

	int							Time;
	Label						RootState;
	Label						InitActionState;
};

//------------------------------------------------------------------------------
//	ステージアクション条件の種別
//------------------------------------------------------------------------------
enum STAGE_CONDITION_TYPE
{
	STAGE_CONDITION_DEFEAT_ENEMY,			//	敵を倒した
	STAGE_CONDITION_DEFEAT_ENEMY_ALL,		//	敵を全て倒した
	STAGE_CONDITION_EMPTY_ENEMY,			//	敵がいない
	STAGE_CONDITION_EMPTY_GROUND,			//	地形がない
	STAGE_CONDITION_END_TELOP,				//	テロップ終了
	STAGE_CONDITION_END_GROUND_ANIME,		//	地形アニメ終了
	STAGE_CONDITION_STATE_FRAME,			//	ステート時間
	STAGE_CONDITION_STAGE_FRAME,			//	ステージ時間
	STAGE_CONDITION_PLAYER_STATE,			//	自機のステート
	STAGE_CONDITION_NOTIFY,					//	通知
	STAGE_CONDITION_END_STATE,				//	ステートの終了
	
	STAGE_CONDITION_NONE,					//	無効
};

//------------------------------------------------------------------------------
//	ステージアクションの条件
//------------------------------------------------------------------------------
struct STAGE_CONDITION
{
	STAGE_CONDITION_TYPE		Type;
	Label						Name;
	int							Param;
};
typedef std::vector< STAGE_CONDITION >	STAGE_CONDITION_LIST;

//------------------------------------------------------------------------------
//	ステージ：テロップ
//------------------------------------------------------------------------------
struct STAGE_TELOP
{
	Label						SpriteName;
	Label						AnimeName;
	Label						CloseAnimeName;
	bool						bSkip;
	int							SkipTime;
	bool						bAnimeEndClose;
	int							Time;
};

//------------------------------------------------------------------------------
//	ステージアクションの種別
//------------------------------------------------------------------------------
enum STAGE_ACTION_TYPE
{
	STAGE_ACTION_ENEMY,				//	敵の生成
	STAGE_ACTION_GROUND,			//	地形の生成
	STAGE_ACTION_TELOP,				//	テロップの生成
	STAGE_ACTION_EFFECT,			//	エフェクトの生成

	STAGE_ACTION_BGM_PLAY,			//	BGM再生
	STAGE_ACTION_BGM_FADE,			//	BGMフェード
	STAGE_ACTION_BGM_STOP,			//	BGM終了

	STAGE_ACTION_TELOP_TEXT,		//	テロップの文字列変更
	STAGE_ACTION_TELOP_ANIME,		//	テロップのアニメ変更
	STAGE_ACTION_TELOP_CLOSE,		//	テロップを閉じる

	STAGE_ACTION_GROUND_ANIME,		//	地形のアニメ追加
	STAGE_ACTION_GROUND_END,		//	地形を終了状態に

	STAGE_ACTION_ENEMY_CHILD,		//	敵を子登録
	STAGE_ACTION_ENEMY_NOTIFY,		//	敵に通知
	STAGE_ACTION_DESTROY_ENEMY_ALL,	//	敵を全て破壊

	STAGE_ACTION_ERASE_ENEMY,		//	敵を消滅
	STAGE_ACTION_ERASE_ENEMY_ALL,	//	敵を全て消滅
	STAGE_ACTION_ERASE_GROUND,		//	地形を消滅
	STAGE_ACTION_ERASE_GROUND_ALL,	//	地形を全て消滅

	STAGE_ACTION_PLAYER_POS,		//	自機の位置を設定
	STAGE_ACTION_PLAYER_ACTION,		//	自機のアクション
	STAGE_ACTION_PLAYER_ANIME,		//	自機のアニメ
	STAGE_ACTION_PLAYER_MOVE,		//	自機の移動可否
	STAGE_ACTION_PLAYER_ATTACK,		//	自機の攻撃可否
	STAGE_ACTION_PLAYER_LOCK,		//	自機の操作制限
	STAGE_ACTION_PLAYER_INDICATE,	//	自機の表示物可否

	STAGE_ACTION_LOCK_SCORE,		//	スコアロック
	STAGE_ACTION_ITEM,				//	アイテム生成
	STAGE_ACTION_ARRIVE,			//	ステージ到達通知
	STAGE_ACTION_RESULT,			//	ステージリザルト開く
	STAGE_ACTION_CHANGE_PLAYER,		//	自機の変更

	STAGE_ACTION_LOOP,				//	フレームのループ
	STAGE_ACTION_CHANGE_STATE,		//	ステート変更
	STAGE_ACTION_END_STAGE,			//	ステージ終了

	STAGE_ACTION_NONE,				//	無効
};

//------------------------------------------------------------------------------
//	ステージアクション
//------------------------------------------------------------------------------
struct STAGE_ACTION
{
	STAGE_ACTION_TYPE			Type;
	int							Frame;
	Label						Name;

	Label						DataName;
	Point						Pos;
	Angle						Rot;

	float						ParamVal;
	Label						ParamStr;

	std::string					Text;

	STAGE_CONDITION_LIST		ConditionList;
};
typedef std::vector< STAGE_ACTION >	STAGE_ACTION_LIST;

//------------------------------------------------------------------------------
//	ステージステート
//------------------------------------------------------------------------------
struct STAGE_STATE
{
	Label						Name;
	STAGE_ACTION_LIST			ActionList;
};
