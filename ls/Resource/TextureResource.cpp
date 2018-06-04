#include "stdafx.h"
#include "TextureResource.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�e�N�X�`�����\�[�X
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
//	�ǂݍ���
//******************************************************************************
bool TextureResource::Load( pstr pPath )
{
	SafeDelete( m_pTexture );

	//	����
	m_pTexture = new Texture;
	if( !m_pTexture ) return false;

	//	�ǂݍ���
	return m_pTexture->Load( pPath );
}

//******************************************************************************
//	�f�o�C�X�̃��Z�b�g�ʒm
//******************************************************************************
void TextureResource::Reset( void )
{
	SafePtr( m_pTexture )->Reset();
}

//******************************************************************************
//	�f�o�C�X�̃��Z�b�g���A�ʒm
//******************************************************************************
void TextureResource::Restore( void )
{
	SafePtr( m_pTexture )->Restore();
}