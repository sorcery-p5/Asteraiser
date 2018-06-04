#include "stdafx.h"
#include "GroundManager.h"
#include "Ground.h"


////////////////////////////////////////////////////////////////////////////////
//
//	地形管理
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
//	更新
//******************************************************************************
void GroundManager::Update( void )
{
	//	敵
	for( GroundList::iterator it = m_GroundList.begin(); it != m_GroundList.end(); )
	{
		(*it)->Update();

		//	削除
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
//	描画
//******************************************************************************
void GroundManager::Draw( void )
{
	for( GroundList::iterator it = m_GroundList.begin(); it != m_GroundList.end(); ++it )
	{
		(*it)->Draw();
	}
}

//******************************************************************************
//	地形を追加
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
//	地形を取得
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
//	地形を終了させる
//******************************************************************************
void GroundManager::EndGround( Label Name )
{
	Ground* pGround = GetGround( Name );

	if( pGround ) pGround->End();
}

//******************************************************************************
//	地形を削除
//******************************************************************************
void GroundManager::EraseGround( Label Name )
{
	Ground* pGround = GetGround( Name );

	if( pGround ) pGround->Delete();
}

//******************************************************************************
//	全地形を削除
//******************************************************************************
void GroundManager::EraseAllGround( void )
{
	for( GroundList::iterator it = m_GroundList.begin(); it != m_GroundList.end(); ++it )
	{
		(*it)->Delete();
	}
}