#include "stdafx.h"
#include "ResourceManager.h"

#include "TextureResource.h"
#include "SoundResource.h"


////////////////////////////////////////////////////////////////////////////////
//
//	���\�[�X�Ǘ�
//
////////////////////////////////////////////////////////////////////////////////
ResourceManager::ResourceManager( void )
{
	
}

ResourceManager::~ResourceManager()
{
	for( int i = 0; i < RES_TYPE_NUM; i++ )
	{
		_ASSERT( m_aResMap[i].empty() );
		DeleteMap( m_aResMap[i] );
	}
}

//******************************************************************************
//	�X�V
//******************************************************************************
void ResourceManager::Update( void )
{
	for( int i = 0; i < RES_TYPE_NUM; i++ )
	{
		for( ResMap::iterator it = m_aResMap[i].begin(); it != m_aResMap[i].end(); ++it )
		{
			Resource* pRes = it->second;

			pRes->Update();
		}
	}
}

//******************************************************************************
//	�f�o�C�X�̃��Z�b�g�ʒm
//******************************************************************************
void ResourceManager::OnDrawDeviceReset( void )
{
	for( int i = 0; i < RES_TYPE_NUM; i++ )
	{
		for( ResMap::iterator it = m_aResMap[i].begin(); it != m_aResMap[i].end(); ++it )
		{
			(it->second)->Reset();
		}
	}
}

//******************************************************************************
//	�f�o�C�X�̃��Z�b�g���A�ʒm
//******************************************************************************
void ResourceManager::OnDrawDeviceRestore( void )
{
	for( int i = 0; i < RES_TYPE_NUM; i++ )
	{
		for( ResMap::iterator it = m_aResMap[i].begin(); it != m_aResMap[i].end(); ++it )
		{
			(it->second)->Restore();
		}
	}
}

//******************************************************************************
//	���\�[�X�̍폜
//******************************************************************************
void ResourceManager::DeleteResource( Resource* pRes )
{
	_ASSERT( pRes && pRes->GetRefCount() <= 0 );

	RES_TYPE Type = pRes->GetType();

	ResMap::iterator it = m_aResMap[Type].find( pRes->GetName() );
	if( it != m_aResMap[Type].end() )
	{
		SafeDelete( pRes );
		m_aResMap[Type].erase( it );
	}
}