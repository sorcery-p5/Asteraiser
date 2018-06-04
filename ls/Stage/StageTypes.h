#pragma once


////////////////////////////////////////////////////////////////////////////////
//
//	�X�e�[�W�F�^��`
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	�X�e�[�W�p�����[�^
//------------------------------------------------------------------------------
struct STAGE_PARAM
{
	STAGE_PARAM( void );

	int							Time;
	Label						RootState;
	Label						InitActionState;
};

//------------------------------------------------------------------------------
//	�X�e�[�W�A�N�V���������̎��
//------------------------------------------------------------------------------
enum STAGE_CONDITION_TYPE
{
	STAGE_CONDITION_DEFEAT_ENEMY,			//	�G��|����
	STAGE_CONDITION_DEFEAT_ENEMY_ALL,		//	�G��S�ē|����
	STAGE_CONDITION_EMPTY_ENEMY,			//	�G�����Ȃ�
	STAGE_CONDITION_EMPTY_GROUND,			//	�n�`���Ȃ�
	STAGE_CONDITION_END_TELOP,				//	�e���b�v�I��
	STAGE_CONDITION_END_GROUND_ANIME,		//	�n�`�A�j���I��
	STAGE_CONDITION_STATE_FRAME,			//	�X�e�[�g����
	STAGE_CONDITION_STAGE_FRAME,			//	�X�e�[�W����
	STAGE_CONDITION_PLAYER_STATE,			//	���@�̃X�e�[�g
	STAGE_CONDITION_NOTIFY,					//	�ʒm
	STAGE_CONDITION_END_STATE,				//	�X�e�[�g�̏I��
	
	STAGE_CONDITION_NONE,					//	����
};

//------------------------------------------------------------------------------
//	�X�e�[�W�A�N�V�����̏���
//------------------------------------------------------------------------------
struct STAGE_CONDITION
{
	STAGE_CONDITION_TYPE		Type;
	Label						Name;
	int							Param;
};
typedef std::vector< STAGE_CONDITION >	STAGE_CONDITION_LIST;

//------------------------------------------------------------------------------
//	�X�e�[�W�F�e���b�v
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
//	�X�e�[�W�A�N�V�����̎��
//------------------------------------------------------------------------------
enum STAGE_ACTION_TYPE
{
	STAGE_ACTION_ENEMY,				//	�G�̐���
	STAGE_ACTION_GROUND,			//	�n�`�̐���
	STAGE_ACTION_TELOP,				//	�e���b�v�̐���
	STAGE_ACTION_EFFECT,			//	�G�t�F�N�g�̐���

	STAGE_ACTION_BGM_PLAY,			//	BGM�Đ�
	STAGE_ACTION_BGM_FADE,			//	BGM�t�F�[�h
	STAGE_ACTION_BGM_STOP,			//	BGM�I��

	STAGE_ACTION_TELOP_TEXT,		//	�e���b�v�̕�����ύX
	STAGE_ACTION_TELOP_ANIME,		//	�e���b�v�̃A�j���ύX
	STAGE_ACTION_TELOP_CLOSE,		//	�e���b�v�����

	STAGE_ACTION_GROUND_ANIME,		//	�n�`�̃A�j���ǉ�
	STAGE_ACTION_GROUND_END,		//	�n�`���I����Ԃ�

	STAGE_ACTION_ENEMY_CHILD,		//	�G���q�o�^
	STAGE_ACTION_ENEMY_NOTIFY,		//	�G�ɒʒm
	STAGE_ACTION_DESTROY_ENEMY_ALL,	//	�G��S�Ĕj��

	STAGE_ACTION_ERASE_ENEMY,		//	�G������
	STAGE_ACTION_ERASE_ENEMY_ALL,	//	�G��S�ď���
	STAGE_ACTION_ERASE_GROUND,		//	�n�`������
	STAGE_ACTION_ERASE_GROUND_ALL,	//	�n�`��S�ď���

	STAGE_ACTION_PLAYER_POS,		//	���@�̈ʒu��ݒ�
	STAGE_ACTION_PLAYER_ACTION,		//	���@�̃A�N�V����
	STAGE_ACTION_PLAYER_ANIME,		//	���@�̃A�j��
	STAGE_ACTION_PLAYER_MOVE,		//	���@�̈ړ���
	STAGE_ACTION_PLAYER_ATTACK,		//	���@�̍U����
	STAGE_ACTION_PLAYER_LOCK,		//	���@�̑��쐧��
	STAGE_ACTION_PLAYER_INDICATE,	//	���@�̕\������

	STAGE_ACTION_LOCK_SCORE,		//	�X�R�A���b�N
	STAGE_ACTION_ITEM,				//	�A�C�e������
	STAGE_ACTION_ARRIVE,			//	�X�e�[�W���B�ʒm
	STAGE_ACTION_RESULT,			//	�X�e�[�W���U���g�J��
	STAGE_ACTION_CHANGE_PLAYER,		//	���@�̕ύX

	STAGE_ACTION_LOOP,				//	�t���[���̃��[�v
	STAGE_ACTION_CHANGE_STATE,		//	�X�e�[�g�ύX
	STAGE_ACTION_END_STAGE,			//	�X�e�[�W�I��

	STAGE_ACTION_NONE,				//	����
};

//------------------------------------------------------------------------------
//	�X�e�[�W�A�N�V����
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
//	�X�e�[�W�X�e�[�g
//------------------------------------------------------------------------------
struct STAGE_STATE
{
	Label						Name;
	STAGE_ACTION_LIST			ActionList;
};
