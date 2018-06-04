#include "stdafx.h"
#include "IndicateManager.h"
#include "IndicateData.h"
#include "Indicate.h"

#include "World/World.h"


////////////////////////////////////////////////////////////////////////////////
//
//	インジケート管理
//
////////////////////////////////////////////////////////////////////////////////
IndicateManager::IndicateManager( World* pWorld )
{
	_ASSERT( pWorld );
	
	m_pWorld = pWorld;
}

IndicateManager::~IndicateManager()
{
	DeleteMap( m_mapIndicate );
}

//******************************************************************************
//	更新
//******************************************************************************
void IndicateManager::Update( void )
{
	for( MapIndicate::iterator it = m_mapIndicate.begin(); it != m_mapIndicate.end(); )
	{
		(it->second)->Update();

		if( (it->second)->IsDelete() )
		{
			SafeDelete( it->second );
			it = m_mapIndicate.erase( it );
		}
		else
		{
			++it;
		}
	}
}

//******************************************************************************
//	描画
//******************************************************************************
void IndicateManager::Draw( void )
{
	for( MapIndicate::iterator it = m_mapIndicate.begin(); it != m_mapIndicate.end(); ++it )
	{
		(it->second)->Draw( m_pWorld->GetDrawOffset() );
	}
}

//******************************************************************************
//	インジケート追加
//******************************************************************************
void IndicateManager::AddIndicate( Label Name, const IndicateData* pData )
{
	if( !pData ) return;

	//	既に出現済み
	MapIndicate::iterator it = m_mapIndicate.find( Name );
	if( it != m_mapIndicate.end() )
	{
		//	消す
		SafeDelete( it->second );
		m_mapIndicate.erase( it );
	}

	Indicate* pIndicate = new Indicate( this );
	MEM_CHECK( pIndicate );

	pIndicate->Init( pData );

	m_mapIndicate.insert( MapIndicate::value_type( Name, pIndicate ) );
}

//******************************************************************************
//	インジケート追加
//******************************************************************************
void IndicateManager::DeleteIndicate( Label Name )
{
	//	既に出現済み
	MapIndicate::iterator it = m_mapIndicate.find( Name );
	if( it != m_mapIndicate.end() )
	{
		//	消す
		(it->second)->Delete();
	}
}

//******************************************************************************
//	インジケート取得
//******************************************************************************
Indicate* IndicateManager::GetIndicate( Label Name ) const
{
	MapIndicate::const_iterator it = m_mapIndicate.find( Name );
	if( it != m_mapIndicate.end() )
	{
		return it->second;
	}
	return NULL;
}