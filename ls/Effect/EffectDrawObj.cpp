#include "stdafx.h"
#include "EffectDrawObj.h"

#include "EffectManager.h"
#include "EffectFile.h"
#include "Effect.h"
#include "EffectParticle.h"

#include "SpriteFrame/SpriteFrame.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�`�敨(���)
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	�\���ݒ�̎擾
//------------------------------------------------------------------------------
bool EffectDrawObj::_IsVisible( void ) const
{
	return m_pParent->IsVisible();
}

////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�`�敨�i�X�v���C�g�j
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
//	������
//******************************************************************************
void EffectDrawObjSprite::Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Mat )
{
	m_pSprite = new RectSprite;
	if( m_pSprite )
	{
		//	�s�[�X�ݒ�
		const Piece* pPiece = pFile->GetPiece( m_pParam->pSprite->PieceName );
		_ASSERT( pPiece );

		m_pSprite->SetPiece( pPiece, pFile->GetTex(pPiece->GetTexName()) );

		//	�p�����[�^�ݒ�
		m_pSprite->SetBlend( m_pParam->Blend );
		m_pSprite->SetPhase( m_pParam->Phase );
		m_pSprite->SetIndex( m_pParam->pSprite->Index );
	}
}

//******************************************************************************
//	�X�V
//******************************************************************************
void EffectDrawObjSprite::Update( EffectManager* pManager, const Matrix3& Mat )
{
}

//******************************************************************************
//	�`��
//******************************************************************************
void EffectDrawObjSprite::Draw( const Matrix3& OffsetMat )
{
	if( _IsVisible() )
	{
		if( m_pSprite ) m_pSprite->Draw( Matrix3::Trans(-m_pParam->pSprite->Center) * m_pParent->GetMatrix() * OffsetMat );
	}
}

//******************************************************************************
//	�F�̔��f
//******************************************************************************
void EffectDrawObjSprite::SetColor( Color Col )
{
	if( m_pSprite ) m_pSprite->SetColor( Col );
}

//******************************************************************************
//	�X�v���C�g�C���f�N�X�̔��f
//******************************************************************************
void EffectDrawObjSprite::SetIndex( int ID, int Index )
{
	if( m_pSprite ) m_pSprite->SetIndex( Index );
}


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�`�敨�i�X�v���C�g�t���[���j
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
//	������
//******************************************************************************
void EffectDrawObjSpriteFrame::Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Mat )
{
	m_pSpriteFrame = new SpriteFrame;
	MEM_CHECK( m_pSpriteFrame );

	m_pSpriteFrame->Init( pFile->GetSpriteFrame( m_pParam->pSpriteFrame->Name ), Mat );

	//	�A�j���Đ�
	m_pSpriteFrame->ChangeAnime( m_pParam->pSpriteFrame->AnimeName );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void EffectDrawObjSpriteFrame::Update( EffectManager* pManager, const Matrix3& Mat )
{
	SafePtr( m_pSpriteFrame )->Update( &Mat );
}

//******************************************************************************
//	�`��
//******************************************************************************
void EffectDrawObjSpriteFrame::Draw( const Matrix3& OffsetMat )
{
	if( _IsVisible() )
	{
		SafePtr( m_pSpriteFrame )->Draw( OffsetMat );
	}
}

//******************************************************************************
//	�\���̐ݒ�
//******************************************************************************
void EffectDrawObjSpriteFrame::SetVisible( bool bVisible )
{
	SafePtr( m_pSpriteFrame )->SetVisible( bVisible );
}

//******************************************************************************
//	�ꎞ��~
//******************************************************************************
void EffectDrawObjSpriteFrame::Pause( bool bVisible )
{
	SafePtr( m_pSpriteFrame )->Pause( bVisible );
}

//******************************************************************************
//	�F�̔��f
//******************************************************************************
void EffectDrawObjSpriteFrame::SetColor( Color Col )
{
	SafePtr( m_pSpriteFrame )->SetColor( Col );
}


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�`�敨�i���C���j
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
//	������
//******************************************************************************
void EffectDrawObjLine::Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Mat )
{
	m_pSprite = new LineSprite( m_pParam->pLine->Num, m_pParam->pLine->Divide );
	if( m_pSprite )
	{
		//	�s�[�X�ݒ�
		const Piece* pPiece = pFile->GetPiece( m_pParam->pLine->PieceName );
		m_pSprite->SetPiece( pPiece, pFile->GetTex(pPiece->GetTexName()) );

		if( m_pParam->pLine->Width ) m_pSprite->SetWidth( m_pParam->pLine->Width );

		//	�p�����[�^�ݒ�
		m_pSprite->SetBlend( m_pParam->Blend );
		m_pSprite->SetPhase( m_pParam->Phase );		
		m_pSprite->SetIndex( &m_pParam->pLine->vIndex[0] );
		m_pSprite->SetColor( &m_pParam->pLine->vColor[0] );
	}
	
	//	���S�_�^
	if( m_pParam->pLine->Type == EFFECT_PARTICLE_LINE_CENTER )
	{
		m_vPointA.resize( m_pParam->pLine->Num, Mat.GetPos() );
	}
	//	�[�_�^
	else if( m_pParam->pLine->Type == EFFECT_PARTICLE_LINE_TIP )
	{
		Point Tip = Mat.GetAxisY() * m_pParam->pLine->Width;

		m_vPointA.resize( m_pParam->pLine->Num, Mat.GetPos() );
		m_vPointB.resize( m_pParam->pLine->Num, Mat.GetPos() + Tip );
	}

	m_Count		= 0;
}

//******************************************************************************
//	�X�V
//******************************************************************************
void EffectDrawObjLine::Update( EffectManager* pManager, const Matrix3& Mat )
{
	//	�_��̍X�V
	bool bUpdate = false;
	m_Count++;
	if( m_Count >= m_pParam->pLine->Interval )
	{
		bUpdate = true;
		m_Count = 0;
	}

	//	���S�_�^
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
	//	�[�_�^
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
//	�`��
//******************************************************************************
void EffectDrawObjLine::Draw( const Matrix3& OffsetMat )
{
	//	�`��
	if( _IsVisible() )
	{
		if( m_pSprite )
		{
			//	���S�_�^
			if( m_pParam->pLine->Type == EFFECT_PARTICLE_LINE_CENTER )
			{
				std::vector< Point > vPt;

				for( int i = 0; i < m_pParam->pLine->Num; i++ )
				{
					vPt.push_back( OffsetMat.Transform(m_vPointA[i]) );
				}
				m_pSprite->Draw( &vPt[0] );
			}
			//	�[�_�^
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
//	�F�̔��f
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
//	�X�v���C�g�C���f�N�X�̔��f
//******************************************************************************
void EffectDrawObjLine::SetIndex( int ID, int Index )
{
	if( m_pSprite ) m_pSprite->SetIndex( ID, Index );
}


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�`�敨�i�t�H���g�j
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
//	������
//******************************************************************************
void EffectDrawObjFont::Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Mat )
{
	m_pSprite = new FontSprite( pFile->GetFont( m_pParam->pFont->Name ) );
	if( m_pSprite )
	{
		//	�p�����[�^�ݒ�
		m_pSprite->SetBlend( m_pParam->Blend );
		m_pSprite->SetPhase( m_pParam->Phase );
		
		//	�t�H���g�ݒ�
		m_pSprite->SetAlign( m_pParam->pFont->Align );
		m_pSprite->SetText( m_pParam->pFont->Text.c_str() );
	}
}

//******************************************************************************
//	�X�V
//******************************************************************************
void EffectDrawObjFont::Update( EffectManager* pManager, const Matrix3& Mat )
{
}

//******************************************************************************
//	�`��
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
//	�F�̔��f
//******************************************************************************
void EffectDrawObjFont::SetColor( Color Col )
{
	if( m_pSprite ) m_pSprite->SetColor( Col );
}

//******************************************************************************
//	�e�L�X�g�̐ݒ�
//******************************************************************************
void EffectDrawObjFont::SetText( pstr pText )
{
	if( m_pSprite ) m_pSprite->SetText( pText );
}


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�`�敨�i�T�E���h�j
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
//	������
//******************************************************************************
void EffectDrawObjSound::Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Mat )
{
	m_pSound = GetTone()->PlaySound( pFile->GetSoundFile( m_pParam->pSound->Name ), SOUND_CHANNEL_SE, m_pParam->bLoop, m_pParam->pSound->Vol );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void EffectDrawObjSound::Update( EffectManager* pManager, const Matrix3& Mat )
{
}

//******************************************************************************
//	�`��
//******************************************************************************
void EffectDrawObjSound::Draw( const Matrix3& OffsetMat )
{
	if( !m_pSound ) return;
	
	//	�p���̐�����s�Ȃ�
	if( m_pParam->pSound->bPan )
	{
		m_pSound->SetPan( Sound::CalcPanValue(OffsetMat.Transform(m_pParent->GetMatrix().GetPos()), GetWindow()->GetSize()) );
	}
}

//******************************************************************************
//	�\���̐ݒ�
//******************************************************************************
void EffectDrawObjSound::SetVisible( bool bVisible )
{
	SafePtr( m_pSound )->SetMute( !bVisible );
}

//******************************************************************************
//	�ꎞ��~
//******************************************************************************
void EffectDrawObjSound::Pause( bool bPause )
{
	if( !m_pParam->pSound->bNoPause )
	{
		SafePtr( m_pSound )->SetPause( bPause );
	}
}

//******************************************************************************
//	�{�����[���̐ݒ�
//******************************************************************************
void EffectDrawObjSound::SetVolume( float Vol )
{
	SafePtr( m_pSound )->SetVolume( Vol );
}


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�`�敨�i�G�t�F�N�g�j
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
//	������
//******************************************************************************
void EffectDrawObjEffect::Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Mat )
{
	m_pEffect = pManager->CreateEffect( pFile->GetEffect( m_pParam->pEffect->Name ), Mat );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void EffectDrawObjEffect::Update( EffectManager* pManager, const Matrix3& Mat )
{
	if( m_pEffect )
	{
		m_pEffect->SetMatrix( Mat );
	}
}

//******************************************************************************
//	�`��
//******************************************************************************
void EffectDrawObjEffect::Draw( const Matrix3& OffsetMat )
{
}

//******************************************************************************
//	�폜
//******************************************************************************
void EffectDrawObjEffect::Delete( void )
{
	if( m_pEffect ) m_pEffect->Delete();
}

//******************************************************************************
//	�����I��
//******************************************************************************
void EffectDrawObjEffect::EndAppear( void )
{
	if( m_pEffect ) m_pEffect->EndAppear();
}

//******************************************************************************
//	�F�̔��f
//******************************************************************************
void EffectDrawObjEffect::SetColor( Color Col )
{
	if( m_pEffect ) m_pEffect->SetColor( Col );
}

//******************************************************************************
//	�\���̐ݒ�
//******************************************************************************
void EffectDrawObjEffect::SetVisible( bool bVisible )
{
	if( m_pEffect ) m_pEffect->SetVisible( bVisible );
}

//******************************************************************************
//	�ꎞ��~
//******************************************************************************
void EffectDrawObjEffect::Pause( bool bPause )
{
	if( m_pEffect ) m_pEffect->Pause( bPause );
}