#include "stdafx.h"
#include "Ground.h"
#include "GroundManager.h"
#include "GroundData.h"

#include "App/App.h"
#include "World/World.h"

#include "SpriteFrame/SpriteFrame.h"
#include "Collide/CollideObject.h"
#include "Collide/Shape/CollideShapeFrame.h"

#include "Effect/EffectManager.h"
#include "Effect/Effect.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�n�`
//
////////////////////////////////////////////////////////////////////////////////
Ground::Ground( GroundManager* pManager )
{
	m_pManager	= pManager;

	m_pData			= NULL;
	m_pSprite		= NULL;
	m_pCollide		= NULL;

	m_bDelete		= false;
	m_State			= STATE_MAIN;
	m_FrameCount	= 0;
	m_bEnd			= false;
}

Ground::~Ground()
{
	SafeDelete( m_pSprite );
	SafeDelete( m_pCollide );
}

//******************************************************************************
//	������
//******************************************************************************
void Ground::Init( const GroundData* pData, const Matrix3& Mat, Label Name, bool bNoStart )
{
	_ASSERT( pData );

	m_pData			= pData;
	m_Matrix		= Mat;
	m_Pos			= Mat.GetPos();
	m_Rot			= Mat.GetRotZ();
	m_Name			= Name;
	m_FrameCount	= 0;

	//	�X�v���C�g
	const SpriteFrameFile* pSpriteFile = m_pData->GetSpriteFrameFile( m_pData->GetParam().SpriteName );
	if( pSpriteFile )
	{
		m_pSprite = new SpriteFrame;
		MEM_CHECK( m_pSprite );

		m_pSprite->Init( pSpriteFile, Mat );
	}

	//	�A�j��
	if( !m_pData->GetParam().StartAnimeName.IsEmpty() && !bNoStart )
	{
		SafePtr( m_pSprite )->ChangeAnime( m_pData->GetParam().StartAnimeName );
		m_State = STATE_START;
	}
	else
	{
		SafePtr( m_pSprite )->ChangeAnime( m_pData->GetParam().AnimeName );
		m_State = STATE_MAIN;
	}

	//	�����蔻��
	if( m_pData->GetParam().bHitEnable )
	{
		m_pCollide		= new CollideObject;
		MEM_CHECK( m_pCollide );

		m_pCollide->Init( new CollideShapeFrame( m_pSprite ), COLLIDE_LAYER_GROUND );
		m_pCollide->SetOwner( COLLIDE_OWNER_GROUND, this );
		m_pCollide->SetSolveEnable( true );
		m_pCollide->SetListener( this );
	}
}

//******************************************************************************
//	�X�V
//******************************************************************************
void Ground::Update( void )
{
	//	�ʒu�X�V
	m_Pos	+= m_pData->GetParam().Speed;
	m_Rot	+= m_pData->GetParam().RotSpeed;
	m_Matrix = Matrix3::TransRotZ( m_Pos, m_Rot );
	
	//	�X�v���C�g�X�V
	SafePtr(m_pSprite)->Update( &m_Matrix );

	//	������ʒu�X�V
	SafePtr(m_pCollide)->UpdateMatrix( &m_Matrix );

	//	�X�e�[�g�̍X�V
	_UpdateState();

	//	����
	m_FrameCount++;
	if( m_pData->GetParam().Time > 0 &&
		m_FrameCount >= m_pData->GetParam().Time )
	{
		Delete();
	}
}

//******************************************************************************
//	�`��
//******************************************************************************
void Ground::Draw( void )
{
	SafePtr(m_pSprite)->Draw( GetWorld()->GetDrawOffset() );
}

//******************************************************************************
//	�I��������
//******************************************************************************
void Ground::End( void )
{
	if( !m_pData->GetParam().EndAnimeName.IsEmpty() )
	{
		//	�I���A�j��
		PushAnime( m_pData->GetParam().EndAnimeName );
		m_bEnd = true;
	}
	else
	{
		//	�폜
		Delete();
	}
}

//******************************************************************************
//	�A�j����\��ǉ�����
//******************************************************************************
void Ground::PushAnime( Label Name )
{
	m_PushAnimeList.push_back( Name );
}

//******************************************************************************
//	�q�b�g�G�t�F�N�g�𐶐�
//******************************************************************************
void Ground::CreateHitEffect( const Matrix3& Mat ) const
{
	const EffectFile* pEffectFile = m_pData->GetEffectFile( m_pData->GetParam().HitEffectName );
	if( !pEffectFile ) return;
	
	GetApp()->GetEffectMng()->CreateEffect( pEffectFile, Mat );
}

//******************************************************************************
//	���������U�������������邩
//******************************************************************************
bool Ground::IsAttackErase( void ) const
{
	return m_pData->GetParam().bAttackErase;
}

//******************************************************************************
//	���������U�����̃q�b�g�G�t�F�N�g�𔭐������邩
//******************************************************************************
bool Ground::IsAttackHitEffect( void ) const
{
	return m_pData->GetParam().bAttackHitEffect;
}

//******************************************************************************
//	���[���h�擾
//******************************************************************************
World* Ground::GetWorld( void ) const
{
	return m_pManager->GetWorld();
}

//******************************************************************************
//	������Ƀq�b�g���邩�ǂ������ׂ�
//******************************************************************************
bool Ground::IsHit( const COLLIDE_OWNER& Owner ) const
{
	switch( Owner.Type )
	{
	case COLLIDE_OWNER_PLAYER:
		{
			return !m_pData->GetParam().bNotPlayerHit;
		}
		break;

	case COLLIDE_OWNER_ENEMY:
		{
			return !m_pData->GetParam().bNotEnemyHit;
		}
		break;

	case COLLIDE_OWNER_BLADE:
	case COLLIDE_OWNER_SPEAR:
	case COLLIDE_OWNER_ATTACK:
		{
			return !m_pData->GetParam().bNotAttackHit;
		}
		break;
	}

	return true;
}

//------------------------------------------------------------------------------
//	�X�e�[�g�̍X�V
//------------------------------------------------------------------------------
void Ground::_UpdateState( void )
{
	switch( m_State )
	{
	//	�J�n
	case STATE_START:
		if( m_pSprite )
		{
			//	�A�j���I���Œʏ��Ԃ�
			if( m_pSprite->IsAnimeLast() )
			{
				SafePtr( m_pSprite )->ChangeAnime( m_pData->GetParam().AnimeName );
				m_State = STATE_MAIN;
			}
		}
		break;

	//	�ʏ�
	case STATE_MAIN:
		if( m_pSprite )
		{
			if( !m_PushAnimeList.empty() )
			{
				//	�A�j�����I��������A�\��A�j���𗬂�
				if( m_pSprite->IsAnimeLast() )
				{
					SafePtr( m_pSprite )->ChangeAnime( m_PushAnimeList.front() );

					m_PushAnimeList.pop_front();

					if( m_bEnd )
					{
						m_State = STATE_END;
					}
				}
			}
		}
		break;

	//	�I��
	case STATE_END:
		if( m_pSprite )
		{
			//	�A�j���I���ŏI��
			if( m_pSprite->IsAnimeLast() )
			{
				SafePtr( m_pSprite )->ChangeAnime( m_pData->GetParam().EndAnimeName );
				Delete();
			}
		}
		break;
	}
}