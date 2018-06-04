#pragma once

#include "TimeKey/TimeKeyTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト型定義
//
////////////////////////////////////////////////////////////////////////////////

struct EFFECT_APPEAR;


//------------------------------------------------------------------------------
//	エフェクトパラメータ
//------------------------------------------------------------------------------
struct EFFECT_PARAM
{
	EFFECT_PARAM( void );

	uint				EndFrame;		//	終了フレーム
	uint				LoopFrame;		//	終了フレーム
	bool				bLoop;			//	ループ
};

//------------------------------------------------------------------------------
//	パーティクル種別
//------------------------------------------------------------------------------
enum EFFECT_PARTICLE_TYPE
{
	EFFECT_PARTICLE_DUMMY,					//	ダミー
	EFFECT_PARTICLE_SPRITE,					//	スプライト
	EFFECT_PARTICLE_SPRITE_FRAME,			//	スプライトフレーム
	EFFECT_PARTICLE_LINE,					//	ライン
	EFFECT_PARTICLE_FONT,					//	フォント
	EFFECT_PARTICLE_SOUND,					//	サウンド
	EFFECT_PARTICLE_EFFECT,					//	エフェクト
};

//------------------------------------------------------------------------------
//	ラインパーティクルの種別
//------------------------------------------------------------------------------
enum EFFECT_PARTICLE_LINE_TYPE
{
	EFFECT_PARTICLE_LINE_CENTER,			//	中心点型
	EFFECT_PARTICLE_LINE_TIP,				//	端点型
};

//------------------------------------------------------------------------------
//	スプライトパーティクルパラメータ
//------------------------------------------------------------------------------
struct EFFECT_PARTICLE_SPRITE_PARAM
{
	Label						PieceName;	//	ピース名
	Point						Center;		//	中心位置
	int							Index;		//	インデクス
};

//------------------------------------------------------------------------------
//	スプライトフレームパーティクルパラメータ
//------------------------------------------------------------------------------
struct EFFECT_PARTICLE_SPRITE_FRAME_PARAM
{
	Label						Name;		//	スプライトフレーム名
	Label						AnimeName;	//	アニメ名
};

//------------------------------------------------------------------------------
//	ラインパーティクルパラメータ
//------------------------------------------------------------------------------
struct EFFECT_PARTICLE_LINE_PARAM
{
	EFFECT_PARTICLE_LINE_TYPE	Type;		//	種別
	Label						PieceName;	//	ピース名
	float						Width;		//	幅
	int							Num;		//	ライン個数
	int							Divide;		//	ライン分割数
	int							Interval;	//	点列更新頻度
	std::vector<Color>			vColor;		//	色
	IntVec						vIndex;		//	ページインデクス
};

//------------------------------------------------------------------------------
//	フォントパーティクルパラメータ
//------------------------------------------------------------------------------
struct EFFECT_PARTICLE_FONT_PARAM
{
	Label						Name;		//	フォント名
	std::string					Text;		//	テキスト
	FONT_ALIGN					Align;		//	アライン
};

//------------------------------------------------------------------------------
//	サウンドパーティクルパラメータ
//------------------------------------------------------------------------------
struct EFFECT_PARTICLE_SOUND_PARAM
{
	Label						Name;		//	サウンド名
	float						Vol;		//	ボリューム
	bool						bPan;		//	位置にあわせてパンする
	bool						bNoPause;	//	停止しない
};

//------------------------------------------------------------------------------
//	エフェクトパーティクルパラメータ
//------------------------------------------------------------------------------
struct EFFECT_PARTICLE_EFFECT_PARAM
{
	Label						Name;		//	エフェクト名
};

//------------------------------------------------------------------------------
//	移動パラメータ
//------------------------------------------------------------------------------
struct EFFECT_MOVE_VELOCITY_PARAM
{
	float						Decel;		//	減速係数
	float						Gravity;	//	重力影響
	float						Force;		//	力場影響
	float						Flex;		//	反発係数
	bool						bHitEnable;	//	当たり影響
};

//------------------------------------------------------------------------------
//	タイムキーの種類
//------------------------------------------------------------------------------
enum EFFECT_TIME_KEY_TYPE
{
	//	表示
	EFFECT_TIME_KEY_COLOR,			//	色
	EFFECT_TIME_KEY_INDEX,			//	インデクス

	//	フラグ
	EFFECT_TIME_KEY_UPDATE_ENABLE,	//	更新

	//	サウンド
	EFFECT_TIME_KEY_VOL,			//	ボリューム

	//	移動汎用
	EFFECT_TIME_KEY_ROT,			//	角度
	EFFECT_TIME_KEY_ROTSPEED,		//	角速度
	EFFECT_TIME_KEY_SCALE,			//	拡縮

	//	移動型
	EFFECT_TIME_KEY_VEL,			//	移動量
	EFFECT_TIME_KEY_SPEED,			//	速度

	//	追従型
	EFFECT_TIME_KEY_POS,			//	位置
	
	//	回転型
	EFFECT_TIME_KEY_DIST,			//	距離
	EFFECT_TIME_KEY_ANGLE,			//	円周上角度
	EFFECT_TIME_KEY_ANGLE_SPEED,	//	円周上角速度
	EFFECT_TIME_KEY_AXIS_SCALE,		//	軸拡縮
	EFFECT_TIME_KEY_AXIS_ANGLE,		//	軸角度

	EFFECT_TIME_KEY_NUM,

	EFFECT_TIME_KEY_MOVE_BEGIN		= EFFECT_TIME_KEY_ROT,	//	移動型
};

//------------------------------------------------------------------------------
//	タイムキーパラメータ
//------------------------------------------------------------------------------
struct EFFECT_TIME_KEY_PARAM
{
	union
	{
		float				Val[2];
		byte				Col[4];
		byte				bFlag;

		struct
		{
			char			ID;
			char			Index;
		}
		Index;
	};
};
typedef TIME_KEY_PARAM<EFFECT_TIME_KEY_PARAM>	EFFECT_TIME_KEY;
typedef std::vector< EFFECT_TIME_KEY >			VecEffectTimeKey;

//------------------------------------------------------------------------------
//	方向タイプ
//------------------------------------------------------------------------------
enum EFFECT_DIR_TYPE
{
	EFFECT_DIR_ROT,					//	回転パラメータに従う
	EFFECT_DIR_FIX,					//	ワールド座標で固定
	EFFECT_DIR_FOLLOW,				//	移動方向へ向かう
	EFFECT_DIR_CENTER,				//	中央へ向かう

	EFFECT_DIR_TYPE_NUM,
};

//------------------------------------------------------------------------------
//	パーティクルパラメータ
//------------------------------------------------------------------------------
struct EFFECT_PARTICLE
{
	EFFECT_PARTICLE( void );
	~EFFECT_PARTICLE();

	uint						EndFrame;	//	終了フレーム
	bool						bLoop;		//	ループ

	EFFECT_PARTICLE_TYPE		Type;		//	種別
	Color						Col;		//	色
	BLEND_MODE					Blend;		//	ブレンド
	int							Phase;		//	描画フェーズ

	union
	{
		EFFECT_PARTICLE_SPRITE_PARAM*		pSprite;		//	スプライト
		EFFECT_PARTICLE_SPRITE_FRAME_PARAM*	pSpriteFrame;	//	スプライトフレーム
		EFFECT_PARTICLE_LINE_PARAM*			pLine;			//	ライン
		EFFECT_PARTICLE_FONT_PARAM*			pFont;			//	フォント
		EFFECT_PARTICLE_SOUND_PARAM*		pSound;			//	サウンド
		EFFECT_PARTICLE_EFFECT_PARAM*		pEffect;		//	エフェクト
	};

	EFFECT_MOVE_VELOCITY_PARAM*				pVelocity;		//	移動パラメータ

	VecEffectTimeKey						vTimeKey[EFFECT_TIME_KEY_NUM];		//	タイムキー
};

//------------------------------------------------------------------------------
//	エフェクト移動の種類
//------------------------------------------------------------------------------
enum EFFECT_MOVE_TYPE
{
	EFFECT_MOVE_DEFAULT,				//	固定
	EFFECT_MOVE_VELOCITY,				//	移動型
	EFFECT_MOVE_FOLLOW,					//	追従型
	EFFECT_MOVE_CIRCULAR,				//	回転型
};

//------------------------------------------------------------------------------
//	発生パラメータ
//------------------------------------------------------------------------------
struct EFFECT_APPEAR
{
	uint				StartFrame;		//	発生開始フレーム
	uint				EndFrame;		//	発生終了フレーム
	int					Interval;		//	インターバル
	uint				AppearNum;		//	同時発生個数

	Label				Name;			//	発生物の固有名
	Label				ParticleName;	//	パーティクルの名前
	EFFECT_MOVE_TYPE	MoveType;		//	移動タイプ
	EFFECT_DIR_TYPE		DirType;		//	向きタイプ

	union
	{
		//	移動型
		struct VELOCITY
		{
			float		X, Y;			//	位置
			float		XRange, YRange;	//	位置の範囲
			float		Dir;			//	発生角度
			float		DirRange;		//	発生角度の幅
			float		Dist;			//	発生距離
			float		DistRange;		//	発生距離の幅
			float		Speed;			//	発生初速
			float		SpeedRange;		//	発生初速の幅
			float		MoveRandomX;	//	位置のランダム加算X
			float		MoveRandomY;	//	位置のランダム加算Y
			float		Rot;			//	回転
			float		RotRange;		//	回転の幅
			float		RotSpeed;		//	回転速度
			float		RotSpeedRange;	//	回転速度の幅
			float		RotRandom;		//	回転のランダム加算
		}
		Velocity;

		//	追従型
		struct FOLLOW
		{
			float		X, Y;			//	位置
			float		XRange, YRange;	//	位置の範囲
			float		MoveRandomX;	//	位置のランダム加算X
			float		MoveRandomY;	//	位置のランダム加算Y
			float		Rot;			//	回転
			float		RotRange;		//	回転の幅
			float		RotSpeed;		//	回転速度
			float		RotSpeedRange;	//	回転速度の幅
		}
		Follow;

		//	回転型
		struct CIRCULAR
		{
			float		Dist;				//	距離
			float		DistRange;			//	距離の幅
			float		Rot;				//	回転
			float		RotRange;			//	回転の幅
			float		RotSpeed;			//	回転速度
			float		RotSpeedRange;		//	回転速度の幅
			float		TrackAngle;			//	円周上角度
			float		TrackAngleRange;	//	円周上角度の幅
			float		AngleSpeed;			//	円周上角速度
			float		AngleSpeedRange;	//	円周上角速度の幅
			float		AxisScale[2];		//	軸スケール
			float		AxisScaleRange[2];	//	軸スケールの幅
			float		AxisAngle;			//	軸角度
			float		AxisAngleRange;		//	軸角度の幅
		}
		Circular;
	};
};