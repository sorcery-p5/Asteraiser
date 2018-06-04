#pragma once


///////////////////////////////////////////////////////////////////////////////
//
//	�X�v���C�g�t���[���F�^��`
//
///////////////////////////////////////////////////////////////////////////////

struct SPRITE_FRAME_EFFECT;
struct SPRITE_FRAME_SOUND;

//------------------------------------------------------------------------------
//	�t���[��
//------------------------------------------------------------------------------
struct SPRITE_FRAME
{
	BLEND_MODE	BlendMode;
	int			DrawPhase;
};

//------------------------------------------------------------------------------
//	�{�[���̎��
//------------------------------------------------------------------------------
enum SPRITE_FRAME_BONE_TYPE
{
	SPRITE_FRAME_BONE_DUMMY,
	SPRITE_FRAME_BONE_RECT,
	SPRITE_FRAME_BONE_RING,
	SPRITE_FRAME_BONE_FONT,
};

//------------------------------------------------------------------------------
//	�{�[���̌������
//------------------------------------------------------------------------------
enum SPRITE_FRAME_BONE_DIR
{
	SPRITE_FRAME_BONE_DIR_REL,
	SPRITE_FRAME_BONE_DIR_FIX,
};

//-----------------------------------------------------------------------------
//	�{�[���G�֌`�p�����[�^
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
//	�{�[��
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
//	�L�[�t���[���̎��
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
//	�L�[�t���[���̕�Ԏ��
//------------------------------------------------------------------------------
enum SPRITE_FRAME_INTP
{
	SPRITE_FRAME_INTP_FIX,
	SPRITE_FRAME_INTP_LERP,

	SPRITE_FRAME_INTP_NUM,
};

//------------------------------------------------------------------------------
//	�A�j���F�L�[�t���[��
//------------------------------------------------------------------------------
struct SPRITE_FRAME_KEY
{
	int						Frame;
	SPRITE_FRAME_INTP		Intp;

	union
	{
		//	�ʒu�E�X�P�[��
		float				Float2[2];

		//	�p�x
		struct
		{
			float			Rot;
			bool			bLoop;
		}
		Rot;

		//	�F
		byte				Col[4];
		
		//	�C���f�N�X
		struct
		{
			word			Start;
			char			Step;
			byte			Interval;
		}
		Index;

		//	���O
		LabelStr			Name;

		//	������
		char				Str[32];
	};
};

//------------------------------------------------------------------------------
//	�G�t�F�N�g
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
//	�T�E���h
//------------------------------------------------------------------------------
struct SPRITE_FRAME_SOUND
{
	Label		Name;
	float		Vol;
	bool		bPan;
	bool		bLoop;
};

//------------------------------------------------------------------------------
//	�G�t�F�N�g��T�E���h�������
//------------------------------------------------------------------------------
enum SPRITE_FRAME_APPERAR_TYPE
{
	SPRITE_FRAME_APPERAR_EFFECT,
	SPRITE_FRAME_APPERAR_SOUND,
	SPRITE_FRAME_APPERAR_ERASE,

	SPRITE_FRAME_APPERAR_NUM,
};

//------------------------------------------------------------------------------
//	�G�t�F�N�g��T�E���h����
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
//	�ʒm
//------------------------------------------------------------------------------
struct SPRITE_FRAME_NOTIFY
{
	int							Frame;
	Label						Key;
	std::string					Value;
};

//------------------------------------------------------------------------------
//	�����蔻��^�C�v
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
//	�����蔻��p�f�[�^
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