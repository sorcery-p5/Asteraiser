#include "stdafx.h"
#include "ResourceManager.h"

#include "TextureResource.h"
#include "SoundResource.h"


////////////////////////////////////////////////////////////////////////////////
//
//	リソース管理
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
//	更新
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
//	デバイスのリセット通知
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
//	デバイスのリセット復帰通知
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
//	リソースの削除
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