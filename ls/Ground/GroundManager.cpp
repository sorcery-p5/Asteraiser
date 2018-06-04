#include "stdafx.h"
#include "GroundManager.h"
#include "Ground.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�n�`�Ǘ�
//
////////////////////////////////////////////////////////////////////////////////
GroundManager::GroundManager( World* pWorld )
{
	m_pWorld = pWorld;
}

GroundManager::~GroundManager()
{
	DeleteVec( m_GroundList );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void GroundManager::Update( void )
{
	//	�G
	for( GroundList::iterator it = m_GroundList.begin(); it != m_GroundList.end(); )
	{
		(*it)->Update();

		//	�폜
		if( (*it)->IsDelete() )
		{
			SafeDelete( *it );
			it = m_GroundList.erase( it );
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
void GroundManager::Draw( void )
{
	for( GroundList::iterator it = m_GroundList.begin(); it != m_GroundList.end(); ++it )
	{
		(*it)->Draw();
	}
}

//******************************************************************************
//	�n�`��ǉ�
//******************************************************************************
void GroundManager::AddGround( const GroundData* pData, const Matrix3& Mat, Label Name, bool bNoStart )
{
	if( !Name.IsEmpty() && GetGround( Name ) )
	{
		return;
	}

	Ground* pGround = new Ground( this );
	if( pGround )
	{
		pGround->Init( pData, Mat, Name, bNoStart );

		m_GroundList.push_back( pGround );
	}
}

//******************************************************************************
//	�n�`���擾
//******************************************************************************
Ground* GroundManager::GetGround( Label Name ) const
{
	for( GroundList::const_iterator it = m_GroundList.begin(); it != m_GroundList.end(); ++it )
	{
		if( (*it)->GetName() == Name )
		{
			return *it;
		}
	}
	return NULL;
}

//******************************************************************************
//	�n�`���I��������
//******************************************************************************
void GroundManager::EndGround( Label Name )
{
	Ground* pGround = GetGround( Name );

	if( pGround ) pGround->End();
}

//******************************************************************************
//	�n�`���폜
//******************************************************************************
void GroundManager::EraseGround( Label Name )
{
	Ground* pGround = GetGround( Name );

	if( pGround ) pGround->Delete();
}

//******************************************************************************
//	�S�n�`���폜
//******************************************************************************
void GroundManager::EraseAllGround( void )
{
	for( GroundList::iterator it = m_GroundList.begin(); it != m_GroundList.end(); ++it )
	{
		(*it)->Delete();
	}
}