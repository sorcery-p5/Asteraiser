#include "stdafx.h"
#include "EffectParticle.h"

#include "EffectManager.h"
#include "EffectFile.h"
#include "Effect.h"
#include "EffectDrawObj.h"

#include "Move/EffectMove.h"
#include "Move/EffectMoveDefault.h"
#include "Move/EffectMoveVelocity.h"
#include "Move/EffectMoveFollow.h"
#include "Move/EffectMoveCircular.h"

#include "SpriteFrame/SpriteFrame.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�p�[�e�B�N��
//
////////////////////////////////////////////////////////////////////////////////
EffectParticle::EffectParticle( const Effect* pParent, const EFFECT_PARTICLE* pParam )
{
	_ASSERT(pParam && pParent);

	m_pParent		= pParent;
	m_pParam		= pParam;
	m_pAppear		= NULL;
	m_pDrawObj		= NULL;
	m_pMove			= NULL;
	m_pTimeKey		= NULL;

	m_FrameCount	= 0;
	m_bDelete		= false;
	m_bUpdateEnable	= true;

	m_Scale.One();
}
EffectParticle::~EffectParticle()
{
	SafeDelete( m_pTimeKey );
	SafeDelete( m_pDrawObj );
	SafeDelete( m_pMove );
}

//******************************************************************************
//	������
//******************************************************************************
void EffectParticle::Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Parent, const EFFECT_APPEAR* pAppear )
{
	_ASSERT( pAppear );
	m_pAppear = pAppear;

	//	�p�����[�^
	m_FrameCount	= 0;
	m_bDelete		= false;
	
	m_Color			= m_pParam->Col;

	//	�ړ���������
	m_pMove = _CreateMove( pAppear->MoveType );
	MEM_CHECK( m_pMove );
	m_pMove->Init( pManager, Parent, pAppear );

	//	�`�敨�̍쐬
	m_pDrawObj = _CreateDrawObj( m_pParam->Type );
	SafePtr( m_pDrawObj )->Init( pManager, pFile, GetMatrix() );

	//	�^�C���L�[�̐���
	m_pTimeKey	= new EffectTimeKey( this );
	MEM_CHECK( m_pTimeKey );

	//	�F�̔��f
	ApplyColor();
}

//******************************************************************************
//	�X�V
//******************************************************************************
void EffectParticle::Update( EffectManager* pManager )
{
	if( m_bDelete ) return;

	_ASSERT( m_pMove );

	if( !m_pParent->IsPause() )
	{
		//	�t���[���J�E���g�`�F�b�N
		if( m_pParam->EndFrame > 0 && m_FrameCount >= m_pParam->EndFrame )
		{	
			//	���[�v����
			if( m_pParam->bLoop )
			{
				m_FrameCount = 0;
				m_pTimeKey->Reset();
			}
			//	�I��
			else
			{
				m_bDelete = true;
			}
		}

		//	�^�C���L�[�X�V
		m_pTimeKey->Update( m_FrameCount );

		//	�ړ��X�V
		if( m_bUpdateEnable )
		{
			m_pMove->Update( pManager );
		}
	}

	//	�`�敨�̍X�V
	if( m_bUpdateEnable )
	{
		SafePtr( m_pDrawObj )->Update( pManager, GetMatrix() );
	}

	if( !m_pParent->IsPause() )
	{
		//	�J�E���g�i�߂�
		m_FrameCount++;
	}
}

//******************************************************************************
//	�`��
//******************************************************************************
void EffectParticle::Draw( const Matrix3& OffsetMat )
{
	if( IsDelete() ) return;
	if( IsEndless() && m_pParent->IsEndAppear() ) return;
	
	SafePtr( m_pDrawObj )->Draw( OffsetMat );
}

//******************************************************************************
//	�폜
//******************************************************************************
void EffectParticle::Delete( void )
{
	SafePtr( m_pDrawObj )->Delete();
	m_bDelete = true;
}

//******************************************************************************
//	�����I��
//******************************************************************************
void EffectParticle::EndAppear( void )
{
	SafePtr( m_pDrawObj )->EndAppear();
}

//******************************************************************************
//	�F�̔��f
//******************************************************************************
void EffectParticle::ApplyColor( void )
{
	SafePtr( m_pDrawObj )->SetColor( m_pParent->GetColor() * m_Color );
}

//******************************************************************************
//	�\���ݒ�̕ύX
//******************************************************************************
void EffectParticle::SetVisible( bool bVisible )
{
	SafePtr( m_pDrawObj )->SetVisible( bVisible );
}

//******************************************************************************
//	�ꎞ��~
//******************************************************************************
void EffectParticle::Pause( bool bPause )
{
	SafePtr( m_pDrawObj )->Pause( bPause );
}

//******************************************************************************
//	�\����Ԃ̎擾
//******************************************************************************
bool EffectParticle::IsVisible( void ) const
{
	return m_pParent->IsVisible();
}

//******************************************************************************
//	�}�g���N�X�擾
//******************************************************************************
const Matrix3& EffectParticle::GetMatrix( void ) const
{
	_ASSERT( m_pMove );

	return m_pMove->GetMatrix();
}

//******************************************************************************
//	�^�C���L�[�F�L�[�̌����擾
//******************************************************************************
uint EffectParticle::GetTimeKeyNum( EFFECT_TIME_KEY_TYPE Type ) const
{
	return (uint)m_pParam->vTimeKey[Type].size();
}

//******************************************************************************
//	�^�C���L�[�F�L�[���擾
//******************************************************************************
const EFFECT_TIME_KEY* EffectParticle::GetTimeKey( EFFECT_TIME_KEY_TYPE Type, int Index ) const
{
	return &(m_pParam->vTimeKey[Type].at(Index));
}

//******************************************************************************
//	�^�C���L�[�F�L�[�p�����[�^�̔��f
//******************************************************************************
void EffectParticle::ApplyTimeKey( EFFECT_TIME_KEY_TYPE Type, const EFFECT_TIME_KEY_PARAM& Key )
{
	//	�ړ�
	if( Type >= EFFECT_TIME_KEY_MOVE_BEGIN )
	{
		m_pMove->ApplyTimeKey( Type, Point( Key.Val ) );
	}
	//	�X�V�̗L��
	else if( Type == EFFECT_TIME_KEY_UPDATE_ENABLE )
	{
		m_bUpdateEnable = Key.bFlag == TRUE;
	}
	//	�F
	else if( Type == EFFECT_TIME_KEY_COLOR )
	{
		m_Color.Set( Key.Col );
		ApplyColor();
	}
	//	�T�E���h
	else if( Type == EFFECT_TIME_KEY_VOL )
	{
		SafePtr( m_pDrawObj )->SetVolume( Key.Val[0] );
	}
	//	�C���f�N�X
	else if( Type == EFFECT_TIME_KEY_INDEX )
	{
		SafePtr( m_pDrawObj )->SetIndex( Key.Index.ID, Key.Index.Index );
	}
}

//******************************************************************************
//	�^�C���L�[�F�L�[�p�����[�^�̔��f
//******************************************************************************
void EffectParticle::ApplyTimeKey( EFFECT_TIME_KEY_TYPE Type, const EFFECT_TIME_KEY_PARAM& Key, const EFFECT_TIME_KEY_PARAM& Next, float f )
{
	//	�ړ�
	if( Type >= EFFECT_TIME_KEY_MOVE_BEGIN )
	{
		Point Val = Lerp( Point(Key.Val), Point(Next.Val), f );
		m_pMove->ApplyTimeKey( Type, Val );
	}
	//	�F
	else if( Type == EFFECT_TIME_KEY_COLOR )
	{
		m_Color = Color::Lerp( Key.Col, Next.Col, f );
		ApplyColor();
	}
	//	�T�E���h
	else if( Type == EFFECT_TIME_KEY_VOL )
	{
		SafePtr( m_pDrawObj )->SetVolume( Lerp( Key.Val[0], Next.Val[0], f ) );
	}
	//	�C���f�N�X
	else if( Type == EFFECT_TIME_KEY_INDEX )
	{
		int Index = (int)Lerp( (float)Key.Index.Index, (float)Next.Index.Index, f );
		SafePtr( m_pDrawObj )->SetIndex( Key.Index.ID, Index );
	}
}

//------------------------------------------------------------------------------
//	�`�敨�̐���
//------------------------------------------------------------------------------
EffectDrawObj* EffectParticle::_CreateDrawObj( EFFECT_PARTICLE_TYPE Type )	const
{
	switch( Type )
	{
		case EFFECT_PARTICLE_DUMMY:			return NULL;
		case EFFECT_PARTICLE_SPRITE:		return new EffectDrawObjSprite( m_pParam, this );
		case EFFECT_PARTICLE_SPRITE_FRAME:	return new EffectDrawObjSpriteFrame( m_pParam, this );
		case EFFECT_PARTICLE_LINE:			return new EffectDrawObjLine( m_pParam, this );
		case EFFECT_PARTICLE_FONT:			return new EffectDrawObjFont( m_pParam, this );
		case EFFECT_PARTICLE_SOUND:			return new EffectDrawObjSound( m_pParam, this );
		case EFFECT_PARTICLE_EFFECT:		return new EffectDrawObjEffect( m_pParam, this );
	}

	_ASSERT(false);
	return NULL;
}

//------------------------------------------------------------------------------
//	�ړ����̐���
//------------------------------------------------------------------------------
EffectMove* EffectParticle::_CreateMove( EFFECT_MOVE_TYPE Type )
{
	switch( Type )
	{
		case EFFECT_MOVE_DEFAULT:		return new EffectMoveDefault( this );
		case EFFECT_MOVE_VELOCITY:		return new EffectMoveVelocity( this, m_pParam->pVelocity );
		case EFFECT_MOVE_FOLLOW:		return new EffectMoveFollow( this );
		case EFFECT_MOVE_CIRCULAR:		return new EffectMoveCircular( this );
	}

	_ASSERT(false);
	return NULL;
}
