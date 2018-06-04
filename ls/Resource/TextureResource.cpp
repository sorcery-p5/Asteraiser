#include "stdafx.h"
#include "TextureResource.h"


////////////////////////////////////////////////////////////////////////////////
//
//	テクスチャリソース
//
////////////////////////////////////////////////////////////////////////////////
TextureResource::TextureResource( void )
: Resource()
{
	m_pTexture = NULL;
}
TextureResource::~TextureResource()
{
	SafeDelete( m_pTexture );
}

//******************************************************************************
//	読み込み
//******************************************************************************
bool TextureResource::Load( pstr pPath )
{
	SafeDelete( m_pTexture );

	//	生成
	m_pTexture = new Texture;
	if( !m_pTexture ) return false;

	//	読み込む
	return m_pTexture->Load( pPath );
}

//******************************************************************************
//	デバイスのリセット通知
//******************************************************************************
void TextureResource::Reset( void )
{
	SafePtr( m_pTexture )->Reset();
}

//******************************************************************************
//	デバイスのリセット復帰通知
//******************************************************************************
void TextureResource::Restore( void )
{
	SafePtr( m_pTexture )->Restore();
}