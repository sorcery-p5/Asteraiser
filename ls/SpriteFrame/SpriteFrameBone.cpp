#include "stdafx.h"
#include "SpriteFrameBone.h"
#include "SpriteFrame.h"
#include "SpriteFrameFile.h"
#include "SpriteFrameAnime.h"
#include "SpriteFrameUtil.h"

#include "App/App.h"
#include "Effect/EffectManager.h"
#include "Effect/Effect.h"

namespace
{
	const float DUMMY_BONE_LEN		= 20.0f;
}


///////////////////////////////////////////////////////////////////////////////
//
//	�X�v���C�g�t���[���F�{�[���i���j
//
///////////////////////////////////////////////////////////////////////////////
SpriteFrameBone::SpriteFrameBone( void )
{
	m_pFrame	= NULL;
	m_pParam	= NULL;
	m_bVisible	= true;
	m_Color		= Color::White();
}

SpriteFrameBone::~SpriteFrameBone()
{
	for( EffectList::iterator it = m_ResidentEffectList.begin(); it != m_ResidentEffectList.end(); it++ )
	{
		EffectDelete( it->pEffect );
	}
	for( EffectList::iterator it = m_AppearEffectList.begin(); it != m_AppearEffectList.end(); it++ )
	{
		EffectDelete( it->pEffect );
	}
	for( SoundList::iterator it = m_ResidentSoundList.begin(); it != m_ResidentSoundList.end(); it++ )
	{
		SafeStop( it->pSound );
	}
	for( SoundList::iterator it = m_AppearSoundList.begin(); it != m_AppearSoundList.end(); it++ )
	{
		SafeStop( it->pSound );
	}
}

//*****************************************************************************
//	������
//*****************************************************************************
void SpriteFrameBone::Init( const SPRITE_FRAME_BONE* pParam, const SpriteFrame* pFrame )
{
	_ASSERT( pParam && pFrame ); 

	m_pFrame	= pFrame;
	m_pParam	= pParam;

	//	������
	_OnInit();

	//	�s���������
	_ResetLocalMatrix();
}

//*****************************************************************************
//	�X�V
//*****************************************************************************
void SpriteFrameBone::Update( void )
{
	//	�G�t�F�N�g���X�V
	_UpdateEffect();
}

//*****************************************************************************
//	�`��
//*****************************************************************************
void SpriteFrameBone::Draw( const Matrix3& OffsetMat )
{
	if( m_bVisible && m_pFrame->IsVisible() )
	{
		_OnDraw( OffsetMat );
	}

	//	�T�E���h���X�V
	_UpdateSound( OffsetMat );
}

//*****************************************************************************
//	�A�j����K�p����
//*****************************************************************************
void SpriteFrameBone::ApplyAnime( const SpriteFrameAnime* pAnime )
{
	_ASSERT( pAnime );

	//	�K�p
	_OnApplyAnime( pAnime );

	//	�s����擾
	pAnime->GetBoneMatrix( m_LocalMatrix, m_pParam->ID );
}

//******************************************************************************
//	�A�j���[�V������������Ԃɖ߂�
//******************************************************************************
void SpriteFrameBone::ResetAnime( void )
{
	//	�ݒ�
	_OnResetAnime();

	//	�s���������
	_ResetLocalMatrix();

	//	��������������
	ResetAppear();
}

//******************************************************************************
//	�ꎞ��~����
//******************************************************************************
void SpriteFrameBone::Pause( bool bPause )
{
	//	�G�t�F�N�g
	for( EffectList::iterator it = m_ResidentEffectList.begin(); it != m_ResidentEffectList.end(); it++ )
	{
		SafePtr(it->pEffect)->Pause( bPause );
	}
	for( EffectList::iterator it = m_AppearEffectList.begin(); it != m_AppearEffectList.end(); it++ )
	{
		SafePtr(it->pEffect)->Pause( bPause );
	}

	//	�T�E���h
	for( SoundList::iterator it = m_ResidentSoundList.begin(); it != m_ResidentSoundList.end(); it++ )
	{
		SafePtr(it->pSound)->SetPause( bPause );
	}
	for( SoundList::iterator it = m_AppearSoundList.begin(); it != m_AppearSoundList.end(); it++ )
	{
		SafePtr(it->pSound)->SetPause( bPause );
	}
}

//*****************************************************************************
//	���[���h�s��̌v�Z
//*****************************************************************************
void SpriteFrameBone::CalcWorldMatrix( const Matrix3& ParentMat )
{
	if( m_pParam->DirType == SPRITE_FRAME_BONE_DIR_REL )
	{
		m_WorldMatrix = m_LocalMatrix * ParentMat;
	}
	else if( m_pParam->DirType == SPRITE_FRAME_BONE_DIR_FIX )
	{
		//	�����Œ�F�ʒu�̂ݔ��f
		m_WorldMatrix = m_LocalMatrix * Matrix3::Trans( ParentMat.GetPos() );
	}
}

//******************************************************************************
//	�␳�F�̐ݒ�
//******************************************************************************
void SpriteFrameBone::SetColor( Color Col, const SpriteFrameAnime* pAnime )
{
	m_Color = Col;
	ApplyColor( pAnime );
}

//******************************************************************************
//	�␳�F�̔��f
//******************************************************************************
void SpriteFrameBone::ApplyColor( const SpriteFrameAnime* pAnime )
{
	_OnApplyColor( GetColor( pAnime ) );
}

//******************************************************************************
//	�␳�F�̎擾
//******************************************************************************
Color SpriteFrameBone::GetColor( const SpriteFrameAnime* pAnime ) const
{
	return (m_Color * m_pFrame->GetColor() * (pAnime? pAnime->GetBoneColor( m_pParam->ID ) : m_pParam->Col) );
}

//******************************************************************************
//	�풓�G�t�F�N�g��T�E���h�𐶐�
//******************************************************************************
void SpriteFrameBone::InitResidentAppear( void )
{
	//	�G�t�F�N�g
	for( uint i = 0; i < m_pParam->vEffect.size(); i++ )
	{
		const SPRITE_FRAME_EFFECT& Param = m_pParam->vEffect[i];

		//	�o�^
		EFFECT Effect;
		Effect.pEffect	= _CreateEffect( &Param );
		Effect.pParam	= &Param;

		m_ResidentEffectList.push_back( Effect );
	}
	
	//	�T�E���h
	for( uint i = 0; i < m_pParam->vSound.size(); i++ )
	{
		const SPRITE_FRAME_SOUND& Param = m_pParam->vSound[i];

		//	�o�^
		SOUND Sound;
		Sound.pSound	= _CreateSound( &Param, true );
		Sound.pParam	= &Param;

		m_ResidentSoundList.push_back( Sound );
	}
}

//******************************************************************************
//	�������̃��Z�b�g
//******************************************************************************
void SpriteFrameBone::ResetAppear( void )
{
	for( EffectList::iterator it = m_AppearEffectList.begin(); it != m_AppearEffectList.end(); it++ )
	{
		EffectEndAppear( it->pEffect );
	}
	m_AppearSoundList.clear();
	m_AppearEffectList.clear();
}

//******************************************************************************
//	�������̒ǉ�
//******************************************************************************
void SpriteFrameBone::AddAppear( const SPRITE_FRAME_APPEAR* pParam )
{
	//	�G�t�F�N�g
	if( pParam->Type == SPRITE_FRAME_APPERAR_EFFECT )
	{
		Effect* pEffect  = _CreateEffect( pParam->pEffect );

		//	���[�v
		if( pParam->pEffect->bAttach )
		{
			EFFECT Param;
			Param.pEffect	= pEffect;
			Param.pParam	= pParam->pEffect;

			m_AppearEffectList.push_back( Param );
		}
	}
	//	�T�E���h
	else if( pParam->Type == SPRITE_FRAME_APPERAR_SOUND )
	{
		Sound* pSound  = _CreateSound( pParam->pSound, pParam->pSound->bLoop );

		//	���[�v
		if( pParam->pSound->bLoop )
		{
			SOUND Param;
			Param.pSound	= pSound;
			Param.pParam	= pParam->pSound;

			m_AppearSoundList.push_back( Param );
		}
	}
	//	�폜
	else if( pParam->Type == SPRITE_FRAME_APPERAR_ERASE )
	{
		for( EffectList::iterator it = m_ResidentEffectList.begin(); it != m_ResidentEffectList.end(); it++ )
		{
			if( it->pParam->Name == Label(pParam->Name) )
			{
				EffectEndAppear( it->pEffect );
			}
		}
		for( EffectList::iterator it = m_AppearEffectList.begin(); it != m_AppearEffectList.end(); it++ )
		{
			if( it->pParam->Name == Label(pParam->Name) ) EffectEndAppear( it->pEffect );
		}
		for( SoundList::iterator it = m_ResidentSoundList.begin(); it != m_ResidentSoundList.end(); it++ )
		{
			if( it->pParam->Name == Label(pParam->Name) ) SafeStop( it->pSound );
		}
		for( SoundList::iterator it = m_AppearSoundList.begin(); it != m_AppearSoundList.end(); it++ )
		{
			if( it->pParam->Name == Label(pParam->Name) ) SafeStop( it->pSound );
		}
	}
}

//******************************************************************************
//	�`��̐ݒ�
//******************************************************************************
void SpriteFrameBone::SetVisible( bool bVisible )
{
	m_bVisible = bVisible;

	UpdateVisible();
}

//******************************************************************************
//	�`��ݒ�̍X�V
//******************************************************************************
void SpriteFrameBone::UpdateVisible( void )
{
	bool bVisible = m_bVisible && m_pFrame->IsVisible();

	//	�G�t�F�N�g
	for( EffectList::iterator it = m_ResidentEffectList.begin(); it != m_ResidentEffectList.end(); it++ )
	{
		SafePtr(it->pEffect)->SetVisible( bVisible );
	}
	for( EffectList::iterator it = m_AppearEffectList.begin(); it != m_AppearEffectList.end(); it++ )
	{
		SafePtr(it->pEffect)->SetVisible( bVisible );
	}

	//	�T�E���h
	for( SoundList::iterator it = m_ResidentSoundList.begin(); it != m_ResidentSoundList.end(); it++ )
	{
		SafePtr(it->pSound)->SetMute( !bVisible );
	}
	for( SoundList::iterator it = m_AppearSoundList.begin(); it != m_AppearSoundList.end(); it++ )
	{
		SafePtr(it->pSound)->SetMute( !bVisible );
	}
}

//******************************************************************************
//	�ʒu�̐ݒ�
//******************************************************************************
void SpriteFrameBone::SetPos( Point Pos )
{
	m_LocalMatrix.SetPos( Pos );
}

//******************************************************************************
//	��]�̐ݒ�
//******************************************************************************
void SpriteFrameBone::SetRot( float Rot )
{
	m_LocalMatrix.SetRotZ( Rot );
}

//******************************************************************************
//	�g�k�̐ݒ�
//******************************************************************************
void SpriteFrameBone::SetScale( Point Scale )
{
	m_LocalMatrix.SetScale( Scale );
}

//------------------------------------------------------------------------------
//	���[�J���s�������������
//------------------------------------------------------------------------------
void SpriteFrameBone::_ResetLocalMatrix( void )
{
	SpriteFrameAnime::CalcMatrix( m_LocalMatrix, m_pParam->Pos, m_pParam->Rot, m_pParam->Scale );
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�̍X�V
//------------------------------------------------------------------------------
void SpriteFrameBone::_UpdateEffect( void )
{
	for( EffectList::iterator it = m_ResidentEffectList.begin(); it != m_ResidentEffectList.end(); ++it )
	{
		if( it->pEffect )
		{
			(it->pEffect)->SetMatrix( _CalcEffectMatrix(it->pParam) );
		}
	}
	for( EffectList::iterator it = m_AppearEffectList.begin(); it != m_AppearEffectList.end(); ++it )
	{
		if( it->pEffect )
		{
			(it->pEffect)->SetMatrix( _CalcEffectMatrix(it->pParam) );
		}
	}
}

//------------------------------------------------------------------------------
//	�T�E���h�̍X�V
//------------------------------------------------------------------------------
void SpriteFrameBone::_UpdateSound( const Matrix3& OffsetMat )
{
	for( SoundList::iterator it = m_ResidentSoundList.begin(); it != m_ResidentSoundList.end(); ++it )
	{
		if( it->pSound )
		{
			//	�p���̍X�V
			if(it->pParam->bPan) (it->pSound)->SetPan( Sound::CalcPanValue(OffsetMat.Transform(m_WorldMatrix.GetPos()),GetWindow()->GetSize()) );
		}
	}

	for( SoundList::iterator it = m_AppearSoundList.begin(); it != m_AppearSoundList.end(); ++it )
	{
		if( it->pSound )
		{
			//	�p���̍X�V
			if(it->pParam->bPan) it->pSound->SetPan( Sound::CalcPanValue(OffsetMat.Transform(m_WorldMatrix.GetPos()),GetWindow()->GetSize()) );
		}
	}
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�̐���
//------------------------------------------------------------------------------
Effect* SpriteFrameBone::_CreateEffect( const SPRITE_FRAME_EFFECT* pParam )
{
	//	����
	const EffectFile* pEffectFile = m_pFrame->GetFile()->GetEffectFile( pParam->Name );
	Effect* pEffect = GetApp()->GetEffectMng()->CreateEffect( pEffectFile, _CalcEffectMatrix(pParam) );
	if( !pEffect ) return NULL;

	//	�F�̐ݒ�
	pEffect->SetColor( pParam->Col );

	return pEffect;
}

//------------------------------------------------------------------------------
//	�T�E���h�̐���
//------------------------------------------------------------------------------
Sound* SpriteFrameBone::_CreateSound( const SPRITE_FRAME_SOUND* pParam, bool bLoop )
{
	//	����
	const SoundFile* pSoundFile = m_pFrame->GetFile()->GetSoundFile( pParam->Name );
	if( !pSoundFile ) return NULL;

	Sound* pSound = GetTone()->PlaySound( pSoundFile, SOUND_CHANNEL_SE, bLoop, pParam->Vol );
	if( !pSound ) return NULL;
	
	return pSound;
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�ʒu�̌v�Z
//------------------------------------------------------------------------------
Matrix3 SpriteFrameBone::_CalcEffectMatrix( const SPRITE_FRAME_EFFECT* pParam )
{
	Matrix3 Mat = Matrix3::Scale( pParam->Scale );
	Mat *= Matrix3::TransRotZ( pParam->Pos, pParam->Rot );

	return Mat * m_WorldMatrix;
}


///////////////////////////////////////////////////////////////////////////////
//
//	�X�v���C�g�t���[���F�{�[���i�_�~�[�j
//
///////////////////////////////////////////////////////////////////////////////
SpriteFrameBoneDummy::SpriteFrameBoneDummy( void )
{
	m_pShape	= NULL;
}

SpriteFrameBoneDummy::~SpriteFrameBoneDummy()
{
	SafeDelete( m_pShape );
}

//------------------------------------------------------------------------------
//	���������̏���
//------------------------------------------------------------------------------
void SpriteFrameBoneDummy::_OnInit( void )
{
	//	�`��p�`������
	m_pShape = new ShapeArrowLine;
	MEM_CHECK( m_pShape );

	m_pShape->SetColor( m_pParam->Col );
}

//------------------------------------------------------------------------------
//	�`�掞�̏���
//------------------------------------------------------------------------------
void SpriteFrameBoneDummy::_OnDraw( const Matrix3& OffsetMat )
{
	if( m_pShape && SpriteFrameUtil::IsDrawDummyBone() )
	{
		Matrix3 Mat = m_WorldMatrix * OffsetMat;

		m_pShape->Draw( Mat.GetPos(), Mat.GetPos() + Mat.GetAxisY() * DUMMY_BONE_LEN );
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	�X�v���C�g�t���[���F�{�[���i��`�j
//
///////////////////////////////////////////////////////////////////////////////
SpriteFrameBoneRect::SpriteFrameBoneRect( void )
{
	m_pSprite	= NULL;
	m_Index		= -1;
}

SpriteFrameBoneRect::~SpriteFrameBoneRect()
{
	SafeDelete( m_pSprite );
}

//******************************************************************************
//	�`��t�F�C�Y��ݒ�
//******************************************************************************
void SpriteFrameBoneRect::SetDrawPhase( int Phase )
{
	SafePtr( m_pSprite )->SetPhase( Phase );
}

//******************************************************************************
//	��`���擾
//******************************************************************************
Rect SpriteFrameBoneRect::GetRect( void ) const
{
	return Rect( -m_Center.x, -m_Center.y, m_pSprite->GetSize().x - m_Center.x, m_pSprite->GetSize().y - m_Center.y );
}

//------------------------------------------------------------------------------
//	���������̏���
//------------------------------------------------------------------------------
void SpriteFrameBoneRect::_OnInit( void )
{
	//	�X�v���C�g�쐬
	if( !m_pParam->SourceName.IsEmpty() )
	{
		m_pSprite = new RectSprite;
		if( m_pSprite )
		{
			//	�s�[�X�ݒ�
			m_PieceName = m_pParam->SourceName;
			const Piece* pPiece = m_pFrame->GetFile()->GetPiece( m_PieceName );

			if( pPiece )
			{
				m_pSprite->SetPiece( pPiece, m_pFrame->GetFile()->GetTex(pPiece->GetTexName()) );
			}

			//	�p�����[�^�ݒ�
			m_pSprite->SetColor( GetColor( NULL ) );
			m_pSprite->SetIndex( m_Index >= 0? m_Index : 0 );
			m_pSprite->SetBlend( m_pFrame->GetFile()->GetBlendMode( m_pParam->ID ) );
			m_pSprite->SetPhase( m_pFrame->GetFile()->GetDrawPhase( m_pParam->ID ) );
		}
	}

	//	�����ʒu
	m_Center = m_pParam->Center;
}

//------------------------------------------------------------------------------
//	�`�掞�̏���
//------------------------------------------------------------------------------
void SpriteFrameBoneRect::_OnDraw( const Matrix3& OffsetMat )
{
	Matrix3 CenterMat = Matrix3::Trans( -m_Center );

	SafePtr(m_pSprite)->Draw( CenterMat * m_WorldMatrix * OffsetMat );
}

//------------------------------------------------------------------------------
//	�A�j���K�p���̏���
//------------------------------------------------------------------------------
void SpriteFrameBoneRect::_OnApplyAnime( const SpriteFrameAnime* pAnime )
{
	if( m_pSprite )
	{
		//	�s�[�X�ύX
		Label PieceName = pAnime->GetBonePieceName( m_pParam->ID );
		if( !PieceName.IsEmpty() && m_PieceName != PieceName )
		{
			m_PieceName = PieceName;
			const Piece* pPiece = m_pFrame->GetFile()->GetPiece( m_PieceName );
			m_pSprite->SetPiece( pPiece, m_pFrame->GetFile()->GetTex(pPiece->GetTexName()) );
		}

		//	�����ʒu
		m_Center = pAnime->GetBoneCenterPos( m_pParam->ID );

		//	�F�ƃC���f�N�X
		m_pSprite->SetColor( GetColor( pAnime ) );
		m_pSprite->SetIndex( m_Index >= 0? m_Index : pAnime->GetBoneSpriteIndex( m_pParam->ID ) );
	}
}

//------------------------------------------------------------------------------
//	�A�j�����Z�b�g���̏���
//------------------------------------------------------------------------------
void SpriteFrameBoneRect::_OnResetAnime( void )
{
	if( m_pSprite )
	{
		//	�s�[�X�ύX
		m_PieceName = m_pParam->SourceName;
		const Piece* pPiece = m_pFrame->GetFile()->GetPiece( m_PieceName );
		m_pSprite->SetPiece( pPiece, m_pFrame->GetFile()->GetTex(pPiece->GetTexName()) );

		//	�F�ƃC���f�N�X
		m_pSprite->SetColor( GetColor(NULL) );
		m_pSprite->SetIndex( m_Index >= 0? m_Index : 0 );
	}
}

//------------------------------------------------------------------------------
//	�F�ύX����
//------------------------------------------------------------------------------
void SpriteFrameBoneRect::_OnApplyColor( Color Col )
{
	SafePtr( m_pSprite )->SetColor( Col );
}


///////////////////////////////////////////////////////////////////////////////
//
//	�X�v���C�g�t���[���F�{�[���i��`�j
//
///////////////////////////////////////////////////////////////////////////////
SpriteFrameBoneRing::SpriteFrameBoneRing( void )
{
	m_pSprite	= NULL;
}

SpriteFrameBoneRing::~SpriteFrameBoneRing()
{
	SafeDelete( m_pSprite );
}

//******************************************************************************
//	�`��t�F�C�Y��ݒ�
//******************************************************************************
void SpriteFrameBoneRing::SetDrawPhase( int Phase )
{
	SafePtr( m_pSprite )->SetPhase( Phase );
}

//******************************************************************************
//	���S�p�x��ݒ�
//******************************************************************************
void SpriteFrameBoneRing::SetAngle( float Ang )
{
	SafePtr( m_pSprite )->SetAngle( Ang );
}

//******************************************************************************
//	���a���擾
//******************************************************************************
float SpriteFrameBoneRing::GetRadius( void ) const
{
	return m_pSprite->GetRadiusMax();
}

//------------------------------------------------------------------------------
//	���������̏���
//------------------------------------------------------------------------------
void SpriteFrameBoneRing::_OnInit( void )
{
	const SPRITE_FRAME_RING* pRingParam = m_pFrame->GetFile()->GetRing( m_pParam->ID );
	if( !pRingParam ) return;

	//	�X�v���C�g�쐬
	if( !m_pParam->SourceName.IsEmpty() )
	{
		m_pSprite = new RingSprite( pRingParam->RadiusMin, pRingParam->RadiusMax, pRingParam->Ang, pRingParam->Num, pRingParam->Divide );
		if( m_pSprite )
		{
			//	�s�[�X�ݒ�
			m_PieceName = m_pParam->SourceName;
			const Piece* pPiece = m_pFrame->GetFile()->GetPiece( m_PieceName );

			if( pPiece )
			{
				m_pSprite->SetPiece( pPiece, m_pFrame->GetFile()->GetTex(pPiece->GetTexName()) );
			}

			//	�p�����[�^�ݒ�
			m_pSprite->SetColor( GetColor( NULL )  );
			m_pSprite->SetIndex( 0 );
			m_pSprite->SetBlend( m_pFrame->GetFile()->GetBlendMode( m_pParam->ID ) );
			m_pSprite->SetPhase( m_pFrame->GetFile()->GetDrawPhase( m_pParam->ID ) );
		}
	}
}

//------------------------------------------------------------------------------
//	�`�掞�̏���
//------------------------------------------------------------------------------
void SpriteFrameBoneRing::_OnDraw( const Matrix3& OffsetMat )
{
	SafePtr(m_pSprite)->Draw( m_WorldMatrix * OffsetMat );
}

//------------------------------------------------------------------------------
//	�A�j���K�p���̏���
//------------------------------------------------------------------------------
void SpriteFrameBoneRing::_OnApplyAnime( const SpriteFrameAnime* pAnime )
{
	if( m_pSprite )
	{
		//	�s�[�X�ύX
		Label PieceName = pAnime->GetBonePieceName( m_pParam->ID );
		if( !PieceName.IsEmpty() && m_PieceName != PieceName )
		{
			m_PieceName = PieceName;
			const Piece* pPiece = m_pFrame->GetFile()->GetPiece( m_PieceName );
			m_pSprite->SetPiece( pPiece, m_pFrame->GetFile()->GetTex(pPiece->GetTexName()) );
		}

		//	�F�ƃC���f�N�X
		m_pSprite->SetColor( GetColor( pAnime ) );
		m_pSprite->SetIndex( pAnime->GetBoneSpriteIndex( m_pParam->ID ) );
	}
}

//------------------------------------------------------------------------------
//	�A�j�����Z�b�g���̏���
//------------------------------------------------------------------------------
void SpriteFrameBoneRing::_OnResetAnime( void )
{
	if( m_pSprite )
	{
		//	�s�[�X�ύX
		m_PieceName = m_pParam->SourceName;
		const Piece* pPiece = m_pFrame->GetFile()->GetPiece( m_PieceName );
		m_pSprite->SetPiece( pPiece, m_pFrame->GetFile()->GetTex(pPiece->GetTexName()) );

		//	�F�ƃC���f�N�X
		m_pSprite->SetColor( GetColor( NULL ) );
		m_pSprite->SetIndex( 0 );
	}
}

//------------------------------------------------------------------------------
//	�F�ύX����
//------------------------------------------------------------------------------
void SpriteFrameBoneRing::_OnApplyColor( Color Col )
{
	SafePtr( m_pSprite )->SetColor( Col );
}


///////////////////////////////////////////////////////////////////////////////
//
//	�X�v���C�g�t���[���F�{�[���i�t�H���g�j
//
///////////////////////////////////////////////////////////////////////////////
SpriteFrameBoneFont::SpriteFrameBoneFont( void )
{
	m_pSprite	= NULL;
}

SpriteFrameBoneFont::~SpriteFrameBoneFont()
{
	SafeDelete( m_pSprite );
}

//******************************************************************************
//	�`��t�F�C�Y��ݒ�
//******************************************************************************
void SpriteFrameBoneFont::SetDrawPhase( int Phase )
{
	SafePtr( m_pSprite )->SetPhase( Phase );
}

//******************************************************************************
//	�������ݒ�
//******************************************************************************
void SpriteFrameBoneFont::SetText( pstr pStr )
{
	SafePtr( m_pSprite )->SetText( pStr );
}

//------------------------------------------------------------------------------
//	���������̏���
//------------------------------------------------------------------------------
void SpriteFrameBoneFont::_OnInit( void )
{
	//	�X�v���C�g�쐬
	if( !m_pParam->SourceName.IsEmpty() )
	{
		m_pSprite = new FontSprite( m_pFrame->GetFile()->GetFont( m_pParam->SourceName ) );
		if( m_pSprite )
		{
			//	�p�����[�^�ݒ�
			m_pSprite->SetText( m_pFrame->GetFile()->GetText( m_pParam->ID ) );
			m_pSprite->SetAlign( m_pFrame->GetFile()->GetAlign( m_pParam->ID ) );

			m_pSprite->SetColor(GetColor( NULL ) );
			m_pSprite->SetBlend( m_pFrame->GetFile()->GetBlendMode( m_pParam->ID ) );
			m_pSprite->SetPhase( m_pFrame->GetFile()->GetDrawPhase( m_pParam->ID ) );
		}
	}
}

//------------------------------------------------------------------------------
//	�`�掞�̏���
//------------------------------------------------------------------------------
void SpriteFrameBoneFont::_OnDraw( const Matrix3& OffsetMat )
{
	if( m_pSprite )
	{
		m_pSprite->SetMatrix( m_WorldMatrix * OffsetMat );
		m_pSprite->Draw();
	}
}

//------------------------------------------------------------------------------
//	�A�j���K�p���̏���
//------------------------------------------------------------------------------
void SpriteFrameBoneFont::_OnApplyAnime( const SpriteFrameAnime* pAnime )
{
	if( m_pSprite )
	{
		//	�F
		m_pSprite->SetColor( GetColor( pAnime ) );
		
		//	������
		pstr pStr = pAnime->GetBoneText( m_pParam->ID );
		if( pStr )
		{
			m_pSprite->SetText( pStr );
		}
	}
}

//------------------------------------------------------------------------------
//	�A�j�����Z�b�g���̏���
//------------------------------------------------------------------------------
void SpriteFrameBoneFont::_OnResetAnime( void )
{
	if( m_pSprite )
	{
		//	�F
		m_pSprite->SetColor( GetColor( NULL ) );
	}
}

//------------------------------------------------------------------------------
//	�F�ύX����
//------------------------------------------------------------------------------
void SpriteFrameBoneFont::_OnApplyColor( Color Col )
{
	SafePtr( m_pSprite )->SetColor( Col );
}