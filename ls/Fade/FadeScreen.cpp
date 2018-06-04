#include "stdafx.h"
#include "FadeScreen.h"


////////////////////////////////////////////////////////////////////////////////
//
//	フェードスクリーン
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
//	更新
//******************************************************************************
void FadeScreen::Update( void )
{
	if( !IsFade() ) return;

	Color Col = m_FadeColor;

	//	フェードイン中
	if( m_State == STATE_IN )
	{
		if( m_FrameCount <= m_TargetCount )
		{
			Col.SetAlpha( Lerp<int>( 255, 0, (float)m_FrameCount / (float)m_TargetCount ) );
		}

		m_FrameCount++;
		if( m_FrameCount > m_TargetCount )
		{
			//	通常へ戻る
			m_FrameCount	= 0;
			m_TargetCount	= 0;
			m_State			= STATE_NONE;
		}
	}
	//	フェードアウト中
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
//	描画
//******************************************************************************
void FadeScreen::Draw( void )
{
	if( IsFade() )
	{
		m_pSprite->Draw( Rect( Point(-1,-1), P5::GetWindow()->GetSize() ) );
	}
}

//******************************************************************************
//	フェードインする
//******************************************************************************
void FadeScreen::FadeIn( int Frame )
{
	if( !IsFade() ) return;

	//	色は変更しない
	m_State			= STATE_IN;
	m_FrameCount	= 0;
	m_TargetCount	= Frame;
}

//******************************************************************************
//	フェードインする（始点の色指定）
//******************************************************************************
void FadeScreen::FadeIn( int Frame, Color Col )
{
	m_FadeColor		= Col;

	m_State			= STATE_IN;
	m_FrameCount	= 0;
	m_TargetCount	= Frame;
}

//******************************************************************************
//	フェードアウトする
//******************************************************************************
void FadeScreen::FadeOut( int Frame, Color Col )
{
	m_FadeColor		= Col;

	m_State			= STATE_OUT;
	m_FrameCount	= 0;
	m_TargetCount	= Frame;
}