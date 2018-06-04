#include "stdafx.h"
#include "IndicateManager.h"
#include "IndicateData.h"
#include "Indicate.h"

#include "World/World.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�C���W�P�[�g�Ǘ�
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
//	�X�V
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
//	�`��
//******************************************************************************
void IndicateManager::Draw( void )
{
	for( MapIndicate::iterator it = m_mapIndicate.begin(); it != m_mapIndicate.end(); ++it )
	{
		(it->second)->Draw( m_pWorld->GetDrawOffset() );
	}
}

//******************************************************************************
//	�C���W�P�[�g�ǉ�
//******************************************************************************
void IndicateManager::AddIndicate( Label Name, const IndicateData* pData )
{
	if( !pData ) return;

	//	���ɏo���ς�
	MapIndicate::iterator it = m_mapIndicate.find( Name );
	if( it != m_mapIndicate.end() )
	{
		//	����
		SafeDelete( it->second );
		m_mapIndicate.erase( it );
	}

	Indicate* pIndicate = new Indicate( this );
	MEM_CHECK( pIndicate );

	pIndicate->Init( pData );

	m_mapIndicate.insert( MapIndicate::value_type( Name, pIndicate ) );
}

//******************************************************************************
//	�C���W�P�[�g�ǉ�
//******************************************************************************
void IndicateManager::DeleteIndicate( Label Name )
{
	//	���ɏo���ς�
	MapIndicate::iterator it = m_mapIndicate.find( Name );
	if( it != m_mapIndicate.end() )
	{
		//	����
		(it->second)->Delete();
	}
}

//******************************************************************************
//	�C���W�P�[�g�擾
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