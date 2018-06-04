#include "stdafx.h"
#include "Effect.h"

#include "EffectFile.h"
#include "EffectManager.h"
#include "EffectParticle.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g
//
////////////////////////////////////////////////////////////////////////////////
Effect::Effect( void )
{
	m_pFile			= NULL;
	m_pParam		= NULL;

	m_FrameCount	= 0;

	m_bDelete		= false;
	m_bEndAppear	= false;
	m_bVisible		= true;
	m_bPause		= false;

	m_Color.SetWhite();
}
Effect::~Effect()
{
	DeleteVec( m_vParticle );
}

//******************************************************************************
//	������
//******************************************************************************
void Effect::Init( const EffectFile* pFile )
{
	_ASSERT( pFile );

	m_pFile = pFile;
	m_pParam = &pFile->GetParam();

	m_FrameCount = 0;
}

//******************************************************************************
//	�X�V
//******************************************************************************
void Effect::Update( EffectManager* pManager )
{
	if( m_bDelete ) return;

	if( !IsPause() )
	{
		//	�t���[���J�E���g�`�F�b�N
		if( m_pParam->EndFrame > 0 )
		{
			if( m_FrameCount >= m_pParam->EndFrame )
			{
				//	���[�v����
				if( m_pParam->bLoop )
				{
					m_FrameCount = m_pParam->LoopFrame;
				}
				//	�I��
				else
				{
					m_bEndAppear = true;
				}
			}
		}

		if( IsEndAppear() )
		{
			//	�폜�`�F�b�N
			bool bEmpty = true;
			for( VecParticle::const_iterator it = m_vParticle.begin(); it != m_vParticle.end(); ++it )
			{
				if( !(*it)->IsEndless() )
				{
					bEmpty = false;
				}
			}

			if( bEmpty )
			{
				m_bDelete = true;
			}
		}
		else
		{
			//	�V�K����
			_UpdateAppear( pManager );
		}
	}

	//	�p�[�e�B�N���X�V
	_UpdateParticle( pManager );
	
	if( !IsPause() )
	{
		//	�J�E���g�i�߂�
		m_FrameCount++;
	}
}

//******************************************************************************
//	�`��
//******************************************************************************
void Effect::Draw( const Matrix3& OffsetMat ) const
{
 	for( VecParticle::const_iterator it = m_vParticle.begin(); it != m_vParticle.end(); ++it )
	{
		(*it)->Draw( OffsetMat );
	}
}

//******************************************************************************
//	�\���ݒ�̕ύX
//******************************************************************************
void Effect::SetVisible( bool bVisible )
{
	m_bVisible = bVisible;
	
	for( VecParticle::iterator it = m_vParticle.begin(); it != m_vParticle.end(); ++it )
	{
		(*it)->SetVisible( bVisible );
	}
}

//******************************************************************************
//	�F�̐ݒ�
//******************************************************************************
void Effect::SetColor( Color Col )
{
	m_Color = Col;

	for( VecParticle::iterator it = m_vParticle.begin(); it != m_vParticle.end(); ++it )
	{
		(*it)->ApplyColor();
	}
}

//******************************************************************************
//	�ꎞ��~
//******************************************************************************
void Effect::Pause( bool bPause )
{
	m_bPause = bPause;

	for( VecParticle::iterator it = m_vParticle.begin(); it != m_vParticle.end(); ++it )
	{
		(*it)->Pause( bPause );
	}
}

//******************************************************************************
//	�폜
//******************************************************************************
void Effect::Delete( void )
{
	m_bDelete = true;
	
	for( VecParticle::iterator it = m_vParticle.begin(); it != m_vParticle.end(); ++it )
	{
		(*it)->Delete();
	}
}

//******************************************************************************
//	�����I��
//******************************************************************************
void Effect::EndAppear( void )
{
	//	�|�[�Y����
	Pause( false );

	m_bEndAppear = true;

	for( VecParticle::iterator it = m_vParticle.begin(); it != m_vParticle.end(); ++it )
	{
		(*it)->EndAppear();
	}
}

//******************************************************************************
//	�G�t�F�N�g�̃X�P�[���𓾂�
//******************************************************************************
Point Effect::GetParticleScale( Label Name )
{
	EffectParticle* pParticle = _FindParticle( Name );
	if( !pParticle ) return Point( 1.0f, 1.0f );

	return pParticle->GetScale();
}

//******************************************************************************
//	�G�t�F�N�g�̃X�P�[����ݒ�
//******************************************************************************
void Effect::SetParticleScale( Label Name, Point Scale )
{
	EffectParticle* pParticle = _FindParticle( Name );
	if( !pParticle ) return;

	pParticle->SetScale( Scale );
}

//******************************************************************************
//	�폜��Ԃ�
//******************************************************************************
bool Effect::IsDelete( void ) const
{
	if( m_bDelete ) return true;

	return false;
}

//******************************************************************************
//	�폜��Ԃ�
//******************************************************************************
bool Effect::IsEndAppear( void ) const
{
	if( m_bEndAppear ) return true;

	if( (m_pParam->bLoop || m_pParam->EndFrame == 0) && !IS_REFERENCE() )
	{
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
//	�p�[�e�B�N���̍X�V
//------------------------------------------------------------------------------
void Effect::_UpdateParticle( EffectManager* pManager )
{
	//	�X�V
	for( VecParticle::iterator it = m_vParticle.begin(); it != m_vParticle.end(); ++it )
	{
		(*it)->Update( pManager );
	}

	//	�폜�`�F�b�N
	for( VecParticle::iterator it = m_vParticle.begin(); it != m_vParticle.end(); )
	{
		if( (*it)->IsDelete() )
		{
			SafeDelete( *it );
			it = m_vParticle.erase( it );
		}
		else
		{
			++it;
		}
	}
}

//------------------------------------------------------------------------------
//	�p�[�e�B�N�������̍X�V
//------------------------------------------------------------------------------
void Effect::_UpdateAppear( EffectManager* pManager )
{
	for( uint i = 0; i < m_pFile->GetAppearParamNum(); i++ )
	{
		const EFFECT_APPEAR* pParam = m_pFile->GetAppearParam( i );

		if( m_FrameCount < pParam->StartFrame || m_FrameCount > pParam->EndFrame )
		{
			continue;
		}

		if( (m_FrameCount - pParam->StartFrame) % pParam->Interval != 0 )
		{
			continue;
		}

		_AppearParticle( pManager, pParam );
	}
}

//------------------------------------------------------------------------------
//	�p�[�e�B�N���̔���
//------------------------------------------------------------------------------
void Effect::_AppearParticle( EffectManager* pManager, const EFFECT_APPEAR* pParam )
{
	const EFFECT_PARTICLE* pParticleParam = m_pFile->GetParticleParam( pParam->ParticleName );
	if( !pParticleParam ) return;

	for( uint i = 0; i < pParam->AppearNum; i++ )
	{
		//	����
		EffectParticle* pParticle = pParticle = new EffectParticle( this, pParticleParam );
		if( !pParticle ) return;

		//	������
		pParticle->Init( pManager, m_pFile, m_Matrix, pParam );

		//	�o�^
		m_vParticle.push_back( pParticle );
	}
}

//------------------------------------------------------------------------------
//	�p�[�e�B�N��������
//------------------------------------------------------------------------------
EffectParticle* Effect::_FindParticle( Label Name )
{
	//	�X�V
	for( VecParticle::iterator it = m_vParticle.begin(); it != m_vParticle.end(); ++it )
	{
		if( (*it)->GetName() == Name )
		{
			return (*it);
		}
	}

	return NULL;
}