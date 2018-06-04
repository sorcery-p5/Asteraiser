#include "stdafx.h"
#include "Indicate.h"
#include "IndicateManager.h"
#include "IndicateData.h"

#include "World/World.h"

#include "SpriteFrame/SpriteFrame.h"


////////////////////////////////////////////////////////////////////////////////
//
//	インジケート
//
////////////////////////////////////////////////////////////////////////////////
Indicate::Indicate( IndicateManager* pManager ) : IndicateBase()
{
	_ASSERT( pManager );

	m_pManager		= pManager;
	m_pData			= NULL;
	m_pSprite		= NULL;
	m_bDelete		= false;
	m_bOpen			= false;
	m_bCloseDelete	= false;
}
Indicate::~Indicate()
{
	SafeDelete( m_pSprite );
}

//******************************************************************************
//	初期化
//******************************************************************************
void Indicate::Init( const IndicateData* pData )
{
	_ASSERT( pData );

	m_pData = pData;

	m_Matrix = Matrix3::Trans( m_pData->GetParam().Pos );

	//	スプライト
	const SpriteFrameFile* pSpriteFile = m_pData->GetSpriteFrameFile( m_pData->GetParam().SpriteName );
	if( pSpriteFile )
	{
		m_pSprite = new SpriteFrame;
		MEM_CHECK( m_pSprite );
		m_pSprite->Init( pSpriteFile, m_Matrix );
	}

	//	開始アニメ
	if( m_pData->GetParam().bInitOpen &&
		!m_pData->GetParam().OpenAnime.IsEmpty() )
	{
		m_pSprite->ChangeAnime( m_pData->GetParam().OpenAnime );
	}

	//	状態更新
	m_bOpen = m_pData->GetParam().bInitOpen;
}

//******************************************************************************
//	描画
//******************************************************************************
void Indicate::Draw( const Matrix3& OffsetMat )
{
	if( m_pSprite )
	{
		m_pSprite->Draw( OffsetMat );
	}

	//	基底
	IndicateBase::Draw( OffsetMat );
}

//******************************************************************************
//	更新
//******************************************************************************
void Indicate::Update( void )
{
	if( m_pSprite )
	{
		m_pSprite->Update( &m_Matrix );

		//	閉じる
		if( !m_bOpen && m_bCloseDelete )
		{
			if( m_pSprite->IsAnimeLast() )
			{
				Delete();
			}
		}
	}

	//	基底の更新
	IndicateBase::Update();
}

//******************************************************************************
//	開く
//******************************************************************************
void Indicate::Open( void )
{
	if( m_bOpen ) return;

	if( !m_pData->GetParam().OpenAnime.IsEmpty() )
	{
		m_pSprite->ChangeAnime( m_pData->GetParam().OpenAnime );
	}

	m_bOpen = true;
	m_bCloseDelete = false;
}

//******************************************************************************
//	閉じる
//******************************************************************************
void Indicate::Close( bool bDelete )
{
	if( !m_bOpen ) return;

	if( !m_pData->GetParam().CloseAnime.IsEmpty() )
	{
		m_pSprite->ChangeAnime( m_pData->GetParam().CloseAnime );	
	}

	m_bOpen = false;
	m_bCloseDelete = bDelete;
}

//******************************************************************************
//	アニメが有効か
//******************************************************************************
bool Indicate::IsAnimeEnable( void ) const
{
	return !m_pSprite->IsAnimeLast() && m_pSprite->IsAnimePlaying();
}

//------------------------------------------------------------------------------
//	ピースを取得
//------------------------------------------------------------------------------
const Piece* Indicate::_GetPiece( Label Name ) const
{
	return m_pData->GetPiece( Name );
}

//------------------------------------------------------------------------------
//	テクスチャ取得
//------------------------------------------------------------------------------
const Texture* Indicate::_GetTex( Label Name ) const
{
	return m_pData->GetTex( Name );
}

//------------------------------------------------------------------------------
//	マーカーパラメータの取得
//------------------------------------------------------------------------------
const INDICATE_MARKER* Indicate::_GetMarker( Label BoneName ) const
{
	return m_pData->GetMarker( BoneName );
}

//------------------------------------------------------------------------------
//	回転パラメータの取得
//------------------------------------------------------------------------------
const INDICATE_SPIN* Indicate::_GetSpin( Label BoneName ) const
{
	return m_pData->GetSpin( BoneName );
}

//------------------------------------------------------------------------------
//	線グラフパラメータの取得
//------------------------------------------------------------------------------
const INDICATE_LINE_GRAPH* Indicate::_GetLineGraph( Label BoneName ) const
{
	return m_pData->GetLineGraph( BoneName );
}