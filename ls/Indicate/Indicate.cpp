#include "stdafx.h"
#include "Indicate.h"
#include "IndicateManager.h"
#include "IndicateData.h"

#include "World/World.h"

#include "SpriteFrame/SpriteFrame.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�C���W�P�[�g
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
//	������
//******************************************************************************
void Indicate::Init( const IndicateData* pData )
{
	_ASSERT( pData );

	m_pData = pData;

	m_Matrix = Matrix3::Trans( m_pData->GetParam().Pos );

	//	�X�v���C�g
	const SpriteFrameFile* pSpriteFile = m_pData->GetSpriteFrameFile( m_pData->GetParam().SpriteName );
	if( pSpriteFile )
	{
		m_pSprite = new SpriteFrame;
		MEM_CHECK( m_pSprite );
		m_pSprite->Init( pSpriteFile, m_Matrix );
	}

	//	�J�n�A�j��
	if( m_pData->GetParam().bInitOpen &&
		!m_pData->GetParam().OpenAnime.IsEmpty() )
	{
		m_pSprite->ChangeAnime( m_pData->GetParam().OpenAnime );
	}

	//	��ԍX�V
	m_bOpen = m_pData->GetParam().bInitOpen;
}

//******************************************************************************
//	�`��
//******************************************************************************
void Indicate::Draw( const Matrix3& OffsetMat )
{
	if( m_pSprite )
	{
		m_pSprite->Draw( OffsetMat );
	}

	//	���
	IndicateBase::Draw( OffsetMat );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void Indicate::Update( void )
{
	if( m_pSprite )
	{
		m_pSprite->Update( &m_Matrix );

		//	����
		if( !m_bOpen && m_bCloseDelete )
		{
			if( m_pSprite->IsAnimeLast() )
			{
				Delete();
			}
		}
	}

	//	���̍X�V
	IndicateBase::Update();
}

//******************************************************************************
//	�J��
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
//	����
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
//	�A�j�����L����
//******************************************************************************
bool Indicate::IsAnimeEnable( void ) const
{
	return !m_pSprite->IsAnimeLast() && m_pSprite->IsAnimePlaying();
}

//------------------------------------------------------------------------------
//	�s�[�X���擾
//------------------------------------------------------------------------------
const Piece* Indicate::_GetPiece( Label Name ) const
{
	return m_pData->GetPiece( Name );
}

//------------------------------------------------------------------------------
//	�e�N�X�`���擾
//------------------------------------------------------------------------------
const Texture* Indicate::_GetTex( Label Name ) const
{
	return m_pData->GetTex( Name );
}

//------------------------------------------------------------------------------
//	�}�[�J�[�p�����[�^�̎擾
//------------------------------------------------------------------------------
const INDICATE_MARKER* Indicate::_GetMarker( Label BoneName ) const
{
	return m_pData->GetMarker( BoneName );
}

//------------------------------------------------------------------------------
//	��]�p�����[�^�̎擾
//------------------------------------------------------------------------------
const INDICATE_SPIN* Indicate::_GetSpin( Label BoneName ) const
{
	return m_pData->GetSpin( BoneName );
}

//------------------------------------------------------------------------------
//	���O���t�p�����[�^�̎擾
//------------------------------------------------------------------------------
const INDICATE_LINE_GRAPH* Indicate::_GetLineGraph( Label BoneName ) const
{
	return m_pData->GetLineGraph( BoneName );
}