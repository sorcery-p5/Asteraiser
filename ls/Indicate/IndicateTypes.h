#pragma once


////////////////////////////////////////////////////////////////////////////////
//
//	インジケート：型定義
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	インジケートパラメータ
//------------------------------------------------------------------------------
struct INDICATE_PARAM
{
	Label		SpriteName;
	Label		OpenAnime;
	Label		CloseAnime;
	Point		Pos;

	bool		bInitOpen;
};

//------------------------------------------------------------------------------
//	インジケート：マーカー
//------------------------------------------------------------------------------
struct INDICATE_MARKER
{
	Label		BoneName;
	Point		MinPos;
	Point		MaxPos;
};

//------------------------------------------------------------------------------
//	インジケート：回転
//------------------------------------------------------------------------------
struct INDICATE_SPIN
{
	Label		BoneName;
	float		MinRot;
	float		MaxRot;
};

//------------------------------------------------------------------------------
//	インジケート：線グラフ
//------------------------------------------------------------------------------
struct INDICATE_LINE_GRAPH
{
	Label		BoneName;
	Label		PieceName;
	float		Width;
	int			Divide;
	Color		ColMin;
	Color		ColMax;
	uint		Phase;
	BLEND_MODE	Blend;

	float		ValMin;
	float		ValMax;
	int			TimeMax;

	Point		ValAxis;
	Point		TimeAxis;
	
	Label		SeparatePiece;
	Color		SeparateColor;
};