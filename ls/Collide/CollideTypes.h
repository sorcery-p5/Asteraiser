#pragma once


///////////////////////////////////////////////////////////////////////////////
//
//	������֘A�^��`
//
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//	�����背�C���[
//-----------------------------------------------------------------------------
enum COLLIDE_LAYER
{
	COLLIDE_LAYER_PLAYER,							//	���@
	COLLIDE_LAYER_BLADE,							//	�u���[�h�E�X�s�A
	COLLIDE_LAYER_RULER,							//	���[���[
	COLLIDE_LAYER_ENEMY,							//	�G�{��
	COLLIDE_LAYER_ITEM,								//	�A�C�e��
	COLLIDE_LAYER_GROUND,							//	�n�`

	COLLIDE_LAYER_NUM,
	
	COLLIDE_LAYER_DEFAULT = COLLIDE_LAYER_PLAYER,
};

//-----------------------------------------------------------------------------
//	������t�B���^�[
//-----------------------------------------------------------------------------
enum COLLIDE_FILTER
{
	COLLIDE_FILTER_PLAYER		= (1<<COLLIDE_LAYER_ENEMY) | (1<<COLLIDE_LAYER_ITEM) | (1<<COLLIDE_LAYER_RULER) | (1<<COLLIDE_LAYER_GROUND),
	COLLIDE_FILTER_BLADE		= (1<<COLLIDE_LAYER_ENEMY) | (1<<COLLIDE_LAYER_ITEM) | (1<<COLLIDE_LAYER_GROUND),
	COLLIDE_FILTER_SPEAR		= (1<<COLLIDE_LAYER_ENEMY) | (1<<COLLIDE_LAYER_GROUND),
	COLLIDE_FILTER_SPLASH		= (1<<COLLIDE_LAYER_ENEMY) | (1<<COLLIDE_LAYER_ITEM),
	COLLIDE_FILTER_ENEMY		= (1<<COLLIDE_LAYER_GROUND) | (1<<COLLIDE_LAYER_RULER),
	COLLIDE_FILTER_ATTACK		= (1<<COLLIDE_LAYER_PLAYER) | (1<<COLLIDE_LAYER_BLADE) | (1<<COLLIDE_LAYER_RULER) | (1<<COLLIDE_LAYER_GROUND),

	COLLIDE_FILTER_ALL			= 0xFFFFFFFFU,		//	�S��
};

//-----------------------------------------------------------------------------
//	������`����
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
//	�����蔻��̌v�Z���
//------------------------------------------------------------------------------
enum COLLIDE_CALC_TYPE
{
	COLLIDE_CALC_POS		= (1 << 0),		//	������ʒu�̎Z�o���s��
	COLLIDE_CALC_NORMAL		= (1 << 1),		//	�@���̎Z�o���s��
	COLLIDE_CALC_SOLVE		= (1 << 2),		//	�߂荞�݉������s���i�_�̂ݗL���j
	
	COLLIDE_CALC_POS_NORMAL	= COLLIDE_CALC_POS | COLLIDE_CALC_NORMAL,		//	�ʒu�{�@��
	COLLIDE_CALC_ALL		= 0xFFFFFFFFU,									//	�S��
};

//------------------------------------------------------------------------------
//	�����蔻��̃I�[�i�[���
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
//	�����蔻��̃I�[�i�[�f�[�^
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
//	�����茋��
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
//	������L���X�g�̌���
//------------------------------------------------------------------------------
struct COLLIDE_CAST_RESULT
{
	COLLIDE_RESULT_LIST					ResultList;
	Point								SolvePos;
};