#include "P5.h"
#include "Texture.h"

#include "Graphix/DrawDevice.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	�e�N�X�`��
//
///////////////////////////////////////////////////////////////////////////////
//*****************************************************************************
//	���[�h
//*****************************************************************************
bool Texture::Load( pstr Path )
{
	//	�t�@�C�����o�^
	m_Path = Path;

	//	���ߐF��ǉ�
	m_Transparent = GetGraphix()->GetDevice()->GetTransparentColor();

	//	�ǂݍ���
	return _Load();
}

//-----------------------------------------------------------------------------
//	�ǂݍ���
//-----------------------------------------------------------------------------
bool Texture::_Load( void )
{	
	m_pTex.Release();

	HRESULT hr;

	DrawDevice* pDevice = GetGraphix()->GetDevice();

	//	�C���[�W�ǂݍ���
	D3DXIMAGE_INFO	ImageInfo;
	hr = D3DXCreateTextureFromFileEx( pDevice->GetDevice(), m_Path, 
									  D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, 
									  pDevice->GetTexFormat(), 
									  D3DPOOL_DEFAULT, D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR,
									  m_Transparent, &ImageInfo, NULL,
									  &m_pTex );
	if( FAILED(hr) ) return false;

	//	���ƍ������擾
	m_Size.x = (float)ImageInfo.Width;
	m_Size.y = (float)ImageInfo.Height;

	return true;
}

}