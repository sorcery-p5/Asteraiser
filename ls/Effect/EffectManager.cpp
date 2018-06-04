#include "stdafx.h"
#include "EffectManager.h"

#include "Effect.h"
#include "EffectFile.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"
#include "Effect/EffectFile.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�Ǘ�
//
////////////////////////////////////////////////////////////////////////////////
EffectManager::EffectManager( void )
{
	m_bUpdateEnable	= true;
	m_bDrawEnable	= true;
}
EffectManager::~EffectManager()
{
	DeleteVec( m_EffectList );
	DeleteVec( m_AddEffectList );
}

//******************************************************************************
//	������
//******************************************************************************
void EffectManager::Init( void )
{
	
}

//******************************************************************************
//	�X�V
//******************************************************************************
void EffectManager::Update( void )
{
	if( !m_bUpdateEnable ) return;

	//	�o�^�\��G�t�F�N�g���X�V�E�ǉ�
	for( EffectList::iterator it = m_AddEffectList.begin(); it != m_AddEffectList.end(); ++it )
	{
		m_EffectList.push_back( *it );
	}
	m_AddEffectList.clear();

	//	�폜�ς݃G�t�F�N�g�̏���
	for( EffectList::iterator it = m_EffectList.begin(); it != m_EffectList.end(); )
	{
		if( (*it)->IsDelete() )
		{
			SafeDelete( *it );
			it = m_EffectList.erase( it );
		}
		else
		{
			++it;
		}
	}

	//	�X�V
	for( EffectList::iterator it = m_EffectList.begin(); it != m_EffectList.end(); ++it )
	{
		(*it)->Update( this );
	}
	for( EffectList::iterator it = m_AddEffectList.begin(); it != m_AddEffectList.end(); ++it )
	{
		(*it)->Update( this );
	}
}

//******************************************************************************
//	�`��
//******************************************************************************
void EffectManager::Draw( void ) const
{
	if( !m_bDrawEnable ) return;

	for( EffectList::const_iterator it = m_EffectList.begin(); it != m_EffectList.end(); ++it )
	{
		if( !(*it)->IsDelete() )
		{
			(*it)->Draw( m_DrawOffset );
		}
	}

	for( EffectList::const_iterator it = m_AddEffectList.begin(); it != m_AddEffectList.end(); ++it )
	{
		if( !(*it)->IsDelete() )
		{
			(*it)->Draw( m_DrawOffset );
		}
	}
}

//******************************************************************************
//	�G�t�F�N�g���쐬(�ʒu�̂ݎw��)
//******************************************************************************
Effect* EffectManager::CreateEffect( const EffectFile* pFile, Point Pos )
{
	return CreateEffect( pFile, Matrix3::Trans( Pos ) );
}

//******************************************************************************
//	�G�t�F�N�g���쐬�i�}�g���N�X�w��j
//******************************************************************************
Effect* EffectManager::CreateEffect( const EffectFile* pFile, const Matrix3& Mat )
{
	if( !pFile ) return NULL;

	Effect* pEffect = new Effect;
	MEM_CHECK( pEffect );

	pEffect->Init( pFile );
	pEffect->SetMatrix( Mat );

	//	�o�^�\��
	m_AddEffectList.push_back( pEffect );

	return pEffect;
}

//******************************************************************************
//	�S�G�t�F�N�g���폜
//******************************************************************************
void EffectManager::DeleteAll( void )
{
	DeleteVec( m_EffectList );
	DeleteVec( m_AddEffectList );
}