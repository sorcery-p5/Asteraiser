#include "stdafx.h"
#include "EffectDrawObj.h"

#include "EffectManager.h"
#include "EffectFile.h"
#include "Effect.h"
#include "EffectParticle.h"

#include "SpriteFrame/SpriteFrame.h"


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト描画物(基底)
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	表示設定の取得
//------------------------------------------------------------------------------
bool EffectDrawObj::_IsVisible( void ) const
{
	return m_pParent->IsVisible();
}

////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト描画物（スプライト）
//
////////////////////////////////////////////////////////////////////////////////
EffectDrawObjSprite::EffectDrawObjSprite( const EFFECT_PARTICLE* pParam, const EffectParticle* pParent )
: EffectDrawObj( pParam, pParent )
{
	m_pSprite = NULL;
}

EffectDrawObjSprite::~EffectDrawObjSprite()
{
	SafeDelete( m_pSprite );
}

//******************************************************************************
//	初期化
//******************************************************************************
void EffectDrawObjSprite::Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Mat )
{
	m_pSprite = new RectSprite;
	if( m_pSprite )
	{
		//	ピース設定
		const Piece* pPiece = pFile->GetPiece( m_pParam->pSprite->PieceName );
		_ASSERT( pPiece );

		m_pSprite->SetPiece( pPiece, pFile->GetTex(pPiece->GetTexName()) );

		//	パラメータ設定
		m_pSprite->SetBlend( m_pParam->Blend );
		m_pSprite->SetPhase( m_pParam->Phase );
		m_pSprite->SetIndex( m_pParam->pSprite->Index );
	}
}

//******************************************************************************
//	更新
//******************************************************************************
void EffectDrawObjSprite::Update( EffectManager* pManager, const Matrix3& Mat )
{
}

//******************************************************************************
//	描画
//******************************************************************************
void EffectDrawObjSprite::Draw( const Matrix3& OffsetMat )
{
	if( _IsVisible() )
	{
		if( m_pSprite ) m_pSprite->Draw( Matrix3::Trans(-m_pParam->pSprite->Center) * m_pParent->GetMatrix() * OffsetMat );
	}
}

//******************************************************************************
//	色の反映
//******************************************************************************
void EffectDrawObjSprite::SetColor( Color Col )
{
	if( m_pSprite ) m_pSprite->SetColor( Col );
}

//******************************************************************************
//	スプライトインデクスの反映
//******************************************************************************
void EffectDrawObjSprite::SetIndex( int ID, int Index )
{
	if( m_pSprite ) m_pSprite->SetIndex( Index );
}


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト描画物（スプライトフレーム）
//
////////////////////////////////////////////////////////////////////////////////
EffectDrawObjSpriteFrame::EffectDrawObjSpriteFrame( const EFFECT_PARTICLE* pParam, const EffectParticle* pParent )
: EffectDrawObj( pParam, pParent )
{
	m_pSpriteFrame = NULL;
}

EffectDrawObjSpriteFrame::~EffectDrawObjSpriteFrame()
{
	SafeDelete( m_pSpriteFrame );
}

//******************************************************************************
//	初期化
//******************************************************************************
void EffectDrawObjSpriteFrame::Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Mat )
{
	m_pSpriteFrame = new SpriteFrame;
	MEM_CHECK( m_pSpriteFrame );

	m_pSpriteFrame->Init( pFile->GetSpriteFrame( m_pParam->pSpriteFrame->Name ), Mat );

	//	アニメ再生
	m_pSpriteFrame->ChangeAnime( m_pParam->pSpriteFrame->AnimeName );
}

//******************************************************************************
//	更新
//******************************************************************************
void EffectDrawObjSpriteFrame::Update( EffectManager* pManager, const Matrix3& Mat )
{
	SafePtr( m_pSpriteFrame )->Update( &Mat );
}

//******************************************************************************
//	描画
//******************************************************************************
void EffectDrawObjSpriteFrame::Draw( const Matrix3& OffsetMat )
{
	if( _IsVisible() )
	{
		SafePtr( m_pSpriteFrame )->Draw( OffsetMat );
	}
}

//******************************************************************************
//	表示の設定
//******************************************************************************
void EffectDrawObjSpriteFrame::SetVisible( bool bVisible )
{
	SafePtr( m_pSpriteFrame )->SetVisible( bVisible );
}

//******************************************************************************
//	一時停止
//******************************************************************************
void EffectDrawObjSpriteFrame::Pause( bool bVisible )
{
	SafePtr( m_pSpriteFrame )->Pause( bVisible );
}

//******************************************************************************
//	色の反映
//******************************************************************************
void EffectDrawObjSpriteFrame::SetColor( Color Col )
{
	SafePtr( m_pSpriteFrame )->SetColor( Col );
}


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト描画物（ライン）
//
////////////////////////////////////////////////////////////////////////////////
EffectDrawObjLine::EffectDrawObjLine( const EFFECT_PARTICLE* pParam, const EffectParticle* pParent )
: EffectDrawObj( pParam, pParent )
{
	m_pSprite = NULL;
}

EffectDrawObjLine::~EffectDrawObjLine()
{
	SafeDelete( m_pSprite );
}

//******************************************************************************
//	初期化
//******************************************************************************
void EffectDrawObjLine::Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Mat )
{
	m_pSprite = new LineSprite( m_pParam->pLine->Num, m_pParam->pLine->Divide );
	if( m_pSprite )
	{
		//	ピース設定
		const Piece* pPiece = pFile->GetPiece( m_pParam->pLine->PieceName );
		m_pSprite->SetPiece( pPiece, pFile->GetTex(pPiece->GetTexName()) );

		if( m_pParam->pLine->Width ) m_pSprite->SetWidth( m_pParam->pLine->Width );

		//	パラメータ設定
		m_pSprite->SetBlend( m_pParam->Blend );
		m_pSprite->SetPhase( m_pParam->Phase );		
		m_pSprite->SetIndex( &m_pParam->pLine->vIndex[0] );
		m_pSprite->SetColor( &m_pParam->pLine->vColor[0] );
	}
	
	//	中心点型
	if( m_pParam->pLine->Type == EFFECT_PARTICLE_LINE_CENTER )
	{
		m_vPointA.resize( m_pParam->pLine->Num, Mat.GetPos() );
	}
	//	端点型
	else if( m_pParam->pLine->Type == EFFECT_PARTICLE_LINE_TIP )
	{
		Point Tip = Mat.GetAxisY() * m_pParam->pLine->Width;

		m_vPointA.resize( m_pParam->pLine->Num, Mat.GetPos() );
		m_vPointB.resize( m_pParam->pLine->Num, Mat.GetPos() + Tip );
	}

	m_Count		= 0;
}

//******************************************************************************
//	更新
//******************************************************************************
void EffectDrawObjLine::Update( EffectManager* pManager, const Matrix3& Mat )
{
	//	点列の更新
	bool bUpdate = false;
	m_Count++;
	if( m_Count >= m_pParam->pLine->Interval )
	{
		bUpdate = true;
		m_Count = 0;
	}

	//	中心点型
	if( m_pParam->pLine->Type == EFFECT_PARTICLE_LINE_CENTER )
	{
		Point Pos = Mat.GetPos();

		if( bUpdate )
		{
			for( int i = m_pParam->pLine->Num - 1; i > 0; i-- )
			{
				m_vPointA[i] = m_vPointA[i-1];
			}
		}
		m_vPointA[0] = Pos;
	}
	//	端点型
	else if( m_pParam->pLine->Type == EFFECT_PARTICLE_LINE_TIP )
	{
		Point Pos = Mat.GetPos();
		Point Tip = Mat.GetAxisY() * m_pParam->pLine->Width;

		if( bUpdate )
		{
			for( int i = m_pParam->pLine->Num - 1; i > 0; i-- )
			{
				m_vPointA[i] = m_vPointA[i-1];
				m_vPointB[i] = m_vPointB[i-1];
			}
		}
		m_vPointA[0] = Pos;
		m_vPointB[0] = Pos + Tip;
	}
}

//******************************************************************************
//	描画
//******************************************************************************
void EffectDrawObjLine::Draw( const Matrix3& OffsetMat )
{
	//	描画
	if( _IsVisible() )
	{
		if( m_pSprite )
		{
			//	中心点型
			if( m_pParam->pLine->Type == EFFECT_PARTICLE_LINE_CENTER )
			{
				std::vector< Point > vPt;

				for( int i = 0; i < m_pParam->pLine->Num; i++ )
				{
					vPt.push_back( OffsetMat.Transform(m_vPointA[i]) );
				}
				m_pSprite->Draw( &vPt[0] );
			}
			//	端点型
			else if( m_pParam->pLine->Type == EFFECT_PARTICLE_LINE_TIP )
			{
				std::vector< Point > vPtA, vPtB;

				for( int i = 0; i < m_pParam->pLine->Num; i++ )
				{
					vPtA.push_back( OffsetMat.Transform(m_vPointA[i]) );
					vPtB.push_back( OffsetMat.Transform(m_vPointB[i]) );
				}
				m_pSprite->Draw( &vPtA[0], &vPtB[0] );
			}
		}
	}
}

//******************************************************************************
//	色の反映
//******************************************************************************
void EffectDrawObjLine::SetColor( Color Col )
{
	if( m_pSprite )
	{
		for( uint i = 0; i < m_pParam->pLine->vColor.size(); i++ )
		{
			m_pSprite->SetColor( i, m_pParam->pLine->vColor[i] * Col );
		}
	}
}

//******************************************************************************
//	スプライトインデクスの反映
//******************************************************************************
void EffectDrawObjLine::SetIndex( int ID, int Index )
{
	if( m_pSprite ) m_pSprite->SetIndex( ID, Index );
}


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト描画物（フォント）
//
////////////////////////////////////////////////////////////////////////////////
EffectDrawObjFont::EffectDrawObjFont( const EFFECT_PARTICLE* pParam, const EffectParticle* pParent )
: EffectDrawObj( pParam, pParent )
{
	m_pSprite = NULL;
}

EffectDrawObjFont::~EffectDrawObjFont()
{
	SafeDelete( m_pSprite );
}

//******************************************************************************
//	初期化
//******************************************************************************
void EffectDrawObjFont::Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Mat )
{
	m_pSprite = new FontSprite( pFile->GetFont( m_pParam->pFont->Name ) );
	if( m_pSprite )
	{
		//	パラメータ設定
		m_pSprite->SetBlend( m_pParam->Blend );
		m_pSprite->SetPhase( m_pParam->Phase );
		
		//	フォント設定
		m_pSprite->SetAlign( m_pParam->pFont->Align );
		m_pSprite->SetText( m_pParam->pFont->Text.c_str() );
	}
}

//******************************************************************************
//	更新
//******************************************************************************
void EffectDrawObjFont::Update( EffectManager* pManager, const Matrix3& Mat )
{
}

//******************************************************************************
//	描画
//******************************************************************************
void EffectDrawObjFont::Draw( const Matrix3& OffsetMat )
{
	if( _IsVisible() )
	{
		if( m_pSprite )
		{
			m_pSprite->SetMatrix( m_pParent->GetMatrix() * OffsetMat );
			m_pSprite->Draw();
		}
	}
}

//******************************************************************************
//	色の反映
//******************************************************************************
void EffectDrawObjFont::SetColor( Color Col )
{
	if( m_pSprite ) m_pSprite->SetColor( Col );
}

//******************************************************************************
//	テキストの設定
//******************************************************************************
void EffectDrawObjFont::SetText( pstr pText )
{
	if( m_pSprite ) m_pSprite->SetText( pText );
}


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト描画物（サウンド）
//
////////////////////////////////////////////////////////////////////////////////
EffectDrawObjSound::EffectDrawObjSound( const EFFECT_PARTICLE* pParam, const EffectParticle* pParent )
: EffectDrawObj( pParam, pParent )
{
}

EffectDrawObjSound::~EffectDrawObjSound()
{
	SafeStop( m_pSound );
}

//******************************************************************************
//	初期化
//******************************************************************************
void EffectDrawObjSound::Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Mat )
{
	m_pSound = GetTone()->PlaySound( pFile->GetSoundFile( m_pParam->pSound->Name ), SOUND_CHANNEL_SE, m_pParam->bLoop, m_pParam->pSound->Vol );
}

//******************************************************************************
//	更新
//******************************************************************************
void EffectDrawObjSound::Update( EffectManager* pManager, const Matrix3& Mat )
{
}

//******************************************************************************
//	描画
//******************************************************************************
void EffectDrawObjSound::Draw( const Matrix3& OffsetMat )
{
	if( !m_pSound ) return;
	
	//	パンの制御を行なう
	if( m_pParam->pSound->bPan )
	{
		m_pSound->SetPan( Sound::CalcPanValue(OffsetMat.Transform(m_pParent->GetMatrix().GetPos()), GetWindow()->GetSize()) );
	}
}

//******************************************************************************
//	表示の設定
//******************************************************************************
void EffectDrawObjSound::SetVisible( bool bVisible )
{
	SafePtr( m_pSound )->SetMute( !bVisible );
}

//******************************************************************************
//	一時停止
//******************************************************************************
void EffectDrawObjSound::Pause( bool bPause )
{
	if( !m_pParam->pSound->bNoPause )
	{
		SafePtr( m_pSound )->SetPause( bPause );
	}
}

//******************************************************************************
//	ボリュームの設定
//******************************************************************************
void EffectDrawObjSound::SetVolume( float Vol )
{
	SafePtr( m_pSound )->SetVolume( Vol );
}


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト描画物（エフェクト）
//
////////////////////////////////////////////////////////////////////////////////
EffectDrawObjEffect::EffectDrawObjEffect( const EFFECT_PARTICLE* pParam, const EffectParticle* pParent )
: EffectDrawObj( pParam, pParent )
{
}

EffectDrawObjEffect::~EffectDrawObjEffect()
{
	if( m_pEffect ) m_pEffect->EndAppear();
}

//******************************************************************************
//	初期化
//******************************************************************************
void EffectDrawObjEffect::Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Mat )
{
	m_pEffect = pManager->CreateEffect( pFile->GetEffect( m_pParam->pEffect->Name ), Mat );
}

//******************************************************************************
//	更新
//******************************************************************************
void EffectDrawObjEffect::Update( EffectManager* pManager, const Matrix3& Mat )
{
	if( m_pEffect )
	{
		m_pEffect->SetMatrix( Mat );
	}
}

//******************************************************************************
//	描画
//******************************************************************************
void EffectDrawObjEffect::Draw( const Matrix3& OffsetMat )
{
}

//******************************************************************************
//	削除
//******************************************************************************
void EffectDrawObjEffect::Delete( void )
{
	if( m_pEffect ) m_pEffect->Delete();
}

//******************************************************************************
//	生成終了
//******************************************************************************
void EffectDrawObjEffect::EndAppear( void )
{
	if( m_pEffect ) m_pEffect->EndAppear();
}

//******************************************************************************
//	色の反映
//******************************************************************************
void EffectDrawObjEffect::SetColor( Color Col )
{
	if( m_pEffect ) m_pEffect->SetColor( Col );
}

//******************************************************************************
//	表示の設定
//******************************************************************************
void EffectDrawObjEffect::SetVisible( bool bVisible )
{
	if( m_pEffect ) m_pEffect->SetVisible( bVisible );
}

//******************************************************************************
//	一時停止
//******************************************************************************
void EffectDrawObjEffect::Pause( bool bPause )
{
	if( m_pEffect ) m_pEffect->Pause( bPause );
}