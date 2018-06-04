#include "stdafx.h"
#include "StageTelop.h"
#include "Stage.h"

#include "World/World.h"
#include "Config/Config.h"

#include "SpriteFrame/SpriteFrame.h"
#include "Controller/SystemController.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�X�e�[�W�e���b�v
//
////////////////////////////////////////////////////////////////////////////////
StageTelop::StageTelop( Stage* pStage )
{
	m_pStage		= pStage;
	m_pSprite		= NULL;
	m_pParam		= NULL;
	m_FrameCount	= 0;
	m_State			= STATE_OPEN;
}

StageTelop::~StageTelop()
{
	SafeDelete( m_pSprite );
}

//******************************************************************************
//	������
//******************************************************************************
void StageTelop::Init( const SpriteFrameFile* pFile, const STAGE_TELOP* pParam, Label Name, const Matrix3& Mat )
{
	_ASSERT( pFile && pParam );

	m_Name			= Name;
	m_pParam		= pParam;

	//	�X�v���C�g�t���[��
	m_pSprite = new SpriteFrame();
	MEM_CHECK( m_pSprite );
	m_pSprite->Init( pFile, Mat );

	//	�A�j���Đ�
	m_pSprite->ChangeAnime( m_pParam->AnimeName );

	m_State			= STATE_OPEN;
	m_FrameCount	= 0;
}

//******************************************************************************
//	�X�V
//******************************************************************************
void StageTelop::Update( void )
{
	//	�X�v���C�g
	m_pSprite->Update( NULL );

	switch( m_State )
	{
	case STATE_OPEN:
		{
			SystemController Ctrl;

			//	�A�j���I���ŕ���
			if( m_pParam->bAnimeEndClose && 
				m_pSprite->IsAnimePlaying() && m_pSprite->IsAnimeLast() )
			{
				Close();
			}

			//	�{�^���ɂ��X�L�b�v
			if( m_pParam->bSkip && 
				m_FrameCount >= m_pParam->SkipTime &&
				Ctrl.IsAnyInput( INPUT_TRIG ) )
			{
				Close();
			}
			
			//	���ԃ`�F�b�N
			if( m_pParam->Time &&
				m_FrameCount >= m_pParam->Time )
			{
				Close();
			}
		}
		break;
	case STATE_CLOSE:
		{
			//	�A�j���I��
			if( m_pSprite->IsAnimeLast() )
			{
				_Delete();
			}
		}
		break;
	}

	//	�J�E���g
	m_FrameCount++;
}

//******************************************************************************
//	�X�V
//******************************************************************************
void StageTelop::Draw( void )
{
	if( IsDelete() ) return;

	m_pSprite->Draw( m_pStage->GetWorld()->GetDrawOffset() );
}

//******************************************************************************
//	�������ݒ�
//******************************************************************************
void StageTelop::SetText( Label Bone, pstr pText )
{
	//	������̒u��
	String256 Str( pText );
	GetConfig()->ReplaceKeyName( Str );

	SafePtr( m_pSprite )->SetBoneText( Bone, Str.c_str() );
}

//******************************************************************************
//	�A�j���ύX
//******************************************************************************
void StageTelop::ChangeAnime( Label AnimeName, int Intp )
{
	if( m_State != STATE_OPEN ) return;
	
	SafePtr( m_pSprite )->ChangeAnime( AnimeName, Intp );
}

//******************************************************************************
//	����
//******************************************************************************
void StageTelop::Close( void )
{
	if( m_State != STATE_OPEN ) return;

	//	����A�j���Đ�
	if( m_pSprite->HasAnime( m_pParam->CloseAnimeName ) )
	{
		m_pSprite->ChangeAnime( m_pParam->CloseAnimeName );
		m_State = STATE_CLOSE;
	}
	//	�A�j�����Ȃ���ΏI��
	else
	{
		_Delete();
	}
}