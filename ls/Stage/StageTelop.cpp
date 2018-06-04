#include "stdafx.h"
#include "StageTelop.h"
#include "Stage.h"

#include "World/World.h"
#include "Config/Config.h"

#include "SpriteFrame/SpriteFrame.h"
#include "Controller/SystemController.h"


////////////////////////////////////////////////////////////////////////////////
//
//	ステージテロップ
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
//	初期化
//******************************************************************************
void StageTelop::Init( const SpriteFrameFile* pFile, const STAGE_TELOP* pParam, Label Name, const Matrix3& Mat )
{
	_ASSERT( pFile && pParam );

	m_Name			= Name;
	m_pParam		= pParam;

	//	スプライトフレーム
	m_pSprite = new SpriteFrame();
	MEM_CHECK( m_pSprite );
	m_pSprite->Init( pFile, Mat );

	//	アニメ再生
	m_pSprite->ChangeAnime( m_pParam->AnimeName );

	m_State			= STATE_OPEN;
	m_FrameCount	= 0;
}

//******************************************************************************
//	更新
//******************************************************************************
void StageTelop::Update( void )
{
	//	スプライト
	m_pSprite->Update( NULL );

	switch( m_State )
	{
	case STATE_OPEN:
		{
			SystemController Ctrl;

			//	アニメ終了で閉じる
			if( m_pParam->bAnimeEndClose && 
				m_pSprite->IsAnimePlaying() && m_pSprite->IsAnimeLast() )
			{
				Close();
			}

			//	ボタンによるスキップ
			if( m_pParam->bSkip && 
				m_FrameCount >= m_pParam->SkipTime &&
				Ctrl.IsAnyInput( INPUT_TRIG ) )
			{
				Close();
			}
			
			//	時間チェック
			if( m_pParam->Time &&
				m_FrameCount >= m_pParam->Time )
			{
				Close();
			}
		}
		break;
	case STATE_CLOSE:
		{
			//	アニメ終了
			if( m_pSprite->IsAnimeLast() )
			{
				_Delete();
			}
		}
		break;
	}

	//	カウント
	m_FrameCount++;
}

//******************************************************************************
//	更新
//******************************************************************************
void StageTelop::Draw( void )
{
	if( IsDelete() ) return;

	m_pSprite->Draw( m_pStage->GetWorld()->GetDrawOffset() );
}

//******************************************************************************
//	文字列を設定
//******************************************************************************
void StageTelop::SetText( Label Bone, pstr pText )
{
	//	文字列の置換
	String256 Str( pText );
	GetConfig()->ReplaceKeyName( Str );

	SafePtr( m_pSprite )->SetBoneText( Bone, Str.c_str() );
}

//******************************************************************************
//	アニメ変更
//******************************************************************************
void StageTelop::ChangeAnime( Label AnimeName, int Intp )
{
	if( m_State != STATE_OPEN ) return;
	
	SafePtr( m_pSprite )->ChangeAnime( AnimeName, Intp );
}

//******************************************************************************
//	閉じる
//******************************************************************************
void StageTelop::Close( void )
{
	if( m_State != STATE_OPEN ) return;

	//	閉じるアニメ再生
	if( m_pSprite->HasAnime( m_pParam->CloseAnimeName ) )
	{
		m_pSprite->ChangeAnime( m_pParam->CloseAnimeName );
		m_State = STATE_CLOSE;
	}
	//	アニメがなければ終了
	else
	{
		_Delete();
	}
}