#pragma once


///////////////////////////////////////////////////////////////////////////////
//
//	スプライトフレーム：型定義
//
///////////////////////////////////////////////////////////////////////////////

struct SPRITE_FRAME_EFFECT;
struct SPRITE_FRAME_SOUND;

//------------------------------------------------------------------------------
//	フレーム
//------------------------------------------------------------------------------
struct SPRITE_FRAME
{
	BLEND_MODE	BlendMode;
	int			DrawPhase;
};

//------------------------------------------------------------------------------
//	ボーンの種類
//------------------------------------------------------------------------------
enum SPRITE_FRAME_BONE_TYPE
{
	SPRITE_FRAME_BONE_DUMMY,
	SPRITE_FRAME_BONE_RECT,
	SPRITE_FRAME_BONE_RING,
	SPRITE_FRAME_BONE_FONT,
};

//------------------------------------------------------------------------------
//	ボーンの向き種類
//------------------------------------------------------------------------------
enum SPRITE_FRAME_BONE_DIR
{
	SPRITE_FRAME_BONE_DIR_REL,
	SPRITE_FRAME_BONE_DIR_FIX,
};

//-----------------------------------------------------------------------------
//	ボーン；輪形パラメータ
//-----------------------------------------------------------------------------
struct SPRITE_FRAME_RING
{
	float					RadiusMin;
	float					RadiusMax;
	float					Ang;
	int						Num;
	int						Divide;
};

//-----------------------------------------------------------------------------
//	ボーン
//-----------------------------------------------------------------------------
struct SPRITE_FRAME_BONE
{
	SPRITE_FRAME_BONE_TYPE	Type;
	SPRITE_FRAME_BONE_DIR	DirType;
	Label					Name;
	Label					SourceName;

	int						ID;
	int						ParentID;
	int						SiblingID;
	int						ChildID;

	Point					Pos;
	Angle					Rot;
	Point					Scale;
	Point					Center;
	Color					Col;

	std::vector<SPRITE_FRAME_EFFECT>	vEffect;
	std::vector<SPRITE_FRAME_SOUND>		vSound;
};

//------------------------------------------------------------------------------
//	キーフレームの種類
//------------------------------------------------------------------------------
enum SPRITE_FRAME_KEY_TYPE
{
	SPRITE_FRAME_KEY_POS,
	SPRITE_FRAME_KEY_ROT,
	SPRITE_FRAME_KEY_SCALE,
	SPRITE_FRAME_KEY_CENTER,
	SPRITE_FRAME_KEY_COLOR,
	SPRITE_FRAME_KEY_INDEX,
	SPRITE_FRAME_KEY_PIECE,
	SPRITE_FRAME_KEY_TEXT,
	
	SPRITE_FRAME_KEY_NUM,
};

//------------------------------------------------------------------------------
//	キーフレームの補間種類
//------------------------------------------------------------------------------
enum SPRITE_FRAME_INTP
{
	SPRITE_FRAME_INTP_FIX,
	SPRITE_FRAME_INTP_LERP,

	SPRITE_FRAME_INTP_NUM,
};

//------------------------------------------------------------------------------
//	アニメ：キーフレーム
//------------------------------------------------------------------------------
struct SPRITE_FRAME_KEY
{
	int						Frame;
	SPRITE_FRAME_INTP		Intp;

	union
	{
		//	位置・スケール
		float				Float2[2];

		//	角度
		struct
		{
			float			Rot;
			bool			bLoop;
		}
		Rot;

		//	色
		byte				Col[4];
		
		//	インデクス
		struct
		{
			word			Start;
			char			Step;
			byte			Interval;
		}
		Index;

		//	名前
		LabelStr			Name;

		//	文字列
		char				Str[32];
	};
};

//------------------------------------------------------------------------------
//	エフェクト
//------------------------------------------------------------------------------
struct SPRITE_FRAME_EFFECT
{
	Label		Name;
	Point		Pos;
	Angle		Rot;
	Point		Scale;
	bool		bAttach;
	Color		Col;
};

//------------------------------------------------------------------------------
//	サウンド
//------------------------------------------------------------------------------
struct SPRITE_FRAME_SOUND
{
	Label		Name;
	float		Vol;
	bool		bPan;
	bool		bLoop;
};

//------------------------------------------------------------------------------
//	エフェクト･サウンド発生種別
//------------------------------------------------------------------------------
enum SPRITE_FRAME_APPERAR_TYPE
{
	SPRITE_FRAME_APPERAR_EFFECT,
	SPRITE_FRAME_APPERAR_SOUND,
	SPRITE_FRAME_APPERAR_ERASE,

	SPRITE_FRAME_APPERAR_NUM,
};

//------------------------------------------------------------------------------
//	エフェクト･サウンド発生
//------------------------------------------------------------------------------
struct SPRITE_FRAME_APPEAR
{
	SPRITE_FRAME_APPERAR_TYPE	Type;
	int							Frame;
	
	union
	{
		SPRITE_FRAME_EFFECT*	pEffect;
		SPRITE_FRAME_SOUND*		pSound;
		LabelStr				Name;
	};
};

//------------------------------------------------------------------------------
//	通知
//------------------------------------------------------------------------------
struct SPRITE_FRAME_NOTIFY
{
	int							Frame;
	Label						Key;
	std::string					Value;
};

//------------------------------------------------------------------------------
//	当たり判定タイプ
//------------------------------------------------------------------------------
enum SPRITE_FRAME_COLLIDE_TYPE
{
	SPRITE_FRAME_COLLIDE_EMPTY,
	SPRITE_FRAME_COLLIDE_POINT,
	SPRITE_FRAME_COLLIDE_LINE,
	SPRITE_FRAME_COLLIDE_RECT,

	SPRITE_FRAME_COLLIDE_NUM,
};

//------------------------------------------------------------------------------
//	当たり判定用データ
//------------------------------------------------------------------------------
struct SPRITE_FRAME_COLLIDE
{
	SPRITE_FRAME_COLLIDE_TYPE	Type;
	
	union
	{
		struct
		{
			float				Radius;
		}
		Point;
		
		struct
		{
			float				Length;
			float				Width;
		}
		Line;
		
		struct
		{
			float				L;
			float				T;
			float				R;
			float				B;
		}
		Rect;
	};
};