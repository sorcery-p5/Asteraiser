#include "P5.h"
#include "Texture.h"

#include "Graphix/DrawDevice.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	テクスチャ
//
///////////////////////////////////////////////////////////////////////////////
//*****************************************************************************
//	ロード
//*****************************************************************************
bool Texture::Load( pstr Path )
{
	//	ファイル名登録
	m_Path = Path;

	//	透過色を追加
	m_Transparent = GetGraphix()->GetDevice()->GetTransparentColor();

	//	読み込み
	return _Load();
}

//-----------------------------------------------------------------------------
//	読み込み
//-----------------------------------------------------------------------------
bool Texture::_Load( void )
{	
	m_pTex.Release();

	HRESULT hr;

	DrawDevice* pDevice = GetGraphix()->GetDevice();

	//	イメージ読み込み
	D3DXIMAGE_INFO	ImageInfo;
	hr = D3DXCreateTextureFromFileEx( pDevice->GetDevice(), m_Path, 
									  D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, 
									  pDevice->GetTexFormat(), 
									  D3DPOOL_DEFAULT, D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR,
									  m_Transparent, &ImageInfo, NULL,
									  &m_pTex );
	if( FAILED(hr) ) return false;

	//	幅と高さを取得
	m_Size.x = (float)ImageInfo.Width;
	m_Size.y = (float)ImageInfo.Height;

	return true;
}

}