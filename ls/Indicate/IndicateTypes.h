#pragma once


////////////////////////////////////////////////////////////////////////////////
//
//	�C���W�P�[�g�F�^��`
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	�C���W�P�[�g�p�����[�^
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
//	�C���W�P�[�g�F�}�[�J�[
//------------------------------------------------------------------------------
struct INDICATE_MARKER
{
	Label		BoneName;
	Point		MinPos;
	Point		MaxPos;
};

//------------------------------------------------------------------------------
//	�C���W�P�[�g�F��]
//------------------------------------------------------------------------------
struct INDICATE_SPIN
{
	Label		BoneName;
	float		MinRot;
	float		MaxRot;
};

//------------------------------------------------------------------------------
//	�C���W�P�[�g�F���O���t
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