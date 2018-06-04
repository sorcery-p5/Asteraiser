#include "stdafx.h"
#include "FadeScreen.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�t�F�[�h�X�N���[��
//
////////////////////////////////////////////////////////////////////////////////
FadeScreen::FadeScreen( int Phase )
{
	m_pSprite = new RectSprite;
	MEM_CHECK( m_pSprite );

	m_pSprite->SetPhase( Phase );

	m_State		= STATE_NONE;

	m_FrameCount	= 0;
	m_TargetCount	= 0;
}
FadeScreen::~FadeScreen()
{
	SafeDelete( m_pSprite );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void FadeScreen::Update( void )
{
	if( !IsFade() ) return;

	Color Col = m_FadeColor;

	//	�t�F�[�h�C����
	if( m_State == STATE_IN )
	{
		if( m_FrameCount <= m_TargetCount )
		{
			Col.SetAlpha( Lerp<int>( 255, 0, (float)m_FrameCount / (float)m_TargetCount ) );
		}

		m_FrameCount++;
		if( m_FrameCount > m_TargetCount )
		{
			//	�ʏ�֖߂�
			m_FrameCount	= 0;
			m_TargetCount	= 0;
			m_State			= STATE_NONE;
		}
	}
	//	�t�F�[�h�A�E�g��
	else if( m_State == STATE_OUT )
	{
		if( m_FrameCount <= m_TargetCount )
		{
			Col.SetAlpha( Lerp<int>( 0, 255, (float)m_FrameCount / (float)m_TargetCount ) );
			
			m_FrameCount++;
		}
	}
	m_pSprite->SetColor( Col );
}

//******************************************************************************
//	�`��
//******************************************************************************
void FadeScreen::Draw( void )
{
	if( IsFade() )
	{
		m_pSprite->Draw( Rect( Point(-1,-1), P5::GetWindow()->GetSize() ) );
	}
}

//******************************************************************************
//	�t�F�[�h�C������
//******************************************************************************
void FadeScreen::FadeIn( int Frame )
{
	if( !IsFade() ) return;

	//	�F�͕ύX���Ȃ�
	m_State			= STATE_IN;
	m_FrameCount	= 0;
	m_TargetCount	= Frame;
}

//******************************************************************************
//	�t�F�[�h�C������i�n�_�̐F�w��j
//******************************************************************************
void FadeScreen::FadeIn( int Frame, Color Col )
{
	m_FadeColor		= Col;

	m_State			= STATE_IN;
	m_FrameCount	= 0;
	m_TargetCount	= Frame;
}

//******************************************************************************
//	�t�F�[�h�A�E�g����
//******************************************************************************
void FadeScreen::FadeOut( int Frame, Color Col )
{
	m_FadeColor		= Col;

	m_State			= STATE_OUT;
	m_FrameCount	= 0;
	m_TargetCount	= Frame;
}