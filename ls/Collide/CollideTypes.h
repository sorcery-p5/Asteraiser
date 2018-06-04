#pragma once


///////////////////////////////////////////////////////////////////////////////
//
//	当たり関連型定義
//
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//	当たりレイヤー
//-----------------------------------------------------------------------------
enum COLLIDE_LAYER
{
	COLLIDE_LAYER_PLAYER,							//	自機
	COLLIDE_LAYER_BLADE,							//	ブレード・スピア
	COLLIDE_LAYER_RULER,							//	ルーラー
	COLLIDE_LAYER_ENEMY,							//	敵本体
	COLLIDE_LAYER_ITEM,								//	アイテム
	COLLIDE_LAYER_GROUND,							//	地形

	COLLIDE_LAYER_NUM,
	
	COLLIDE_LAYER_DEFAULT = COLLIDE_LAYER_PLAYER,
};

//-----------------------------------------------------------------------------
//	当たりフィルター
//-----------------------------------------------------------------------------
enum COLLIDE_FILTER
{
	COLLIDE_FILTER_PLAYER		= (1<<COLLIDE_LAYER_ENEMY) | (1<<COLLIDE_LAYER_ITEM) | (1<<COLLIDE_LAYER_RULER) | (1<<COLLIDE_LAYER_GROUND),
	COLLIDE_FILTER_BLADE		= (1<<COLLIDE_LAYER_ENEMY) | (1<<COLLIDE_LAYER_ITEM) | (1<<COLLIDE_LAYER_GROUND),
	COLLIDE_FILTER_SPEAR		= (1<<COLLIDE_LAYER_ENEMY) | (1<<COLLIDE_LAYER_GROUND),
	COLLIDE_FILTER_SPLASH		= (1<<COLLIDE_LAYER_ENEMY) | (1<<COLLIDE_LAYER_ITEM),
	COLLIDE_FILTER_ENEMY		= (1<<COLLIDE_LAYER_GROUND) | (1<<COLLIDE_LAYER_RULER),
	COLLIDE_FILTER_ATTACK		= (1<<COLLIDE_LAYER_PLAYER) | (1<<COLLIDE_LAYER_BLADE) | (1<<COLLIDE_LAYER_RULER) | (1<<COLLIDE_LAYER_GROUND),

	COLLIDE_FILTER_ALL			= 0xFFFFFFFFU,		//	全部
};

//-----------------------------------------------------------------------------
//	当たり形状種別
//-----------------------------------------------------------------------------
enum COLLIDE_SHAPE_TYPE
{
	COLLIDE_SHAPE_POINT,
	COLLIDE_SHAPE_LINE,
	COLLIDE_SHAPE_RECT,
	COLLIDE_SHAPE_FRAME,

	COLLIDE_SHAPE_TYPE_NUM,
};

//------------------------------------------------------------------------------
//	当たり判定の計算種別
//------------------------------------------------------------------------------
enum COLLIDE_CALC_TYPE
{
	COLLIDE_CALC_POS		= (1 << 0),		//	当たり位置の算出を行う
	COLLIDE_CALC_NORMAL		= (1 << 1),		//	法線の算出を行う
	COLLIDE_CALC_SOLVE		= (1 << 2),		//	めり込み解決を行う（点のみ有効）
	
	COLLIDE_CALC_POS_NORMAL	= COLLIDE_CALC_POS | COLLIDE_CALC_NORMAL,		//	位置＋法線
	COLLIDE_CALC_ALL		= 0xFFFFFFFFU,									//	全部
};

//------------------------------------------------------------------------------
//	当たり判定のオーナー種別
//------------------------------------------------------------------------------
enum COLLIDE_OWNER_TYPE
{
	COLLIDE_OWNER_NULL,
	COLLIDE_OWNER_PLAYER,
	COLLIDE_OWNER_BLADE,
	COLLIDE_OWNER_SPEAR,
	COLLIDE_OWNER_RULER,
	COLLIDE_OWNER_ENEMY,
	COLLIDE_OWNER_ATTACK,
	COLLIDE_OWNER_ITEM,
	COLLIDE_OWNER_GROUND,

	COLLIDE_OWNER_NUM,
};

//------------------------------------------------------------------------------
//	当たり判定のオーナーデータ
//------------------------------------------------------------------------------
struct COLLIDE_OWNER
{
	COLLIDE_OWNER( void )
	{
		Type	= COLLIDE_OWNER_NULL;
		pData	= NULL;
	}

	COLLIDE_OWNER_TYPE		Type;
	void*					pData;
};

//------------------------------------------------------------------------------
//	当たり結果
//------------------------------------------------------------------------------
struct COLLIDE_RESULT
{
	COLLIDE_LAYER			Layer;
	const COLLIDE_OWNER*	pOwner;
	Point					HitPos;
	Point					Normal;
	float					Length;
	int						BoneID;

	COLLIDE_RESULT( void )
	{
		Layer	= COLLIDE_LAYER_DEFAULT;
		pOwner	= NULL;
		Length	= FLT_MAX;
		BoneID	= -1;
	}

	Matrix3 GetHitMatrix( void )					const	{ return  Matrix3::TransRotZ( HitPos, Normal.GetAngle() ); }
	Matrix3 GetHitMatrix( float AddAngle )			const	{ return  Matrix3::TransRotZ( HitPos, Normal.GetAngle() + AddAngle ); }

	bool	operator < ( const COLLIDE_RESULT& r )	const	{ return Length < r.Length; }
};

typedef std::set<COLLIDE_RESULT>	COLLIDE_RESULT_LIST;

//------------------------------------------------------------------------------
//	当たりキャストの結果
//------------------------------------------------------------------------------
struct COLLIDE_CAST_RESULT
{
	COLLIDE_RESULT_LIST					ResultList;
	Point								SolvePos;
};