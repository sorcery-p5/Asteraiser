#include "stdafx.h"
#include "EffectManager.h"

#include "Effect.h"
#include "EffectFile.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"
#include "Effect/EffectFile.h"


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト管理
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
//	初期化
//******************************************************************************
void EffectManager::Init( void )
{
	
}

//******************************************************************************
//	更新
//******************************************************************************
void EffectManager::Update( void )
{
	if( !m_bUpdateEnable ) return;

	//	登録予約エフェクトを更新・追加
	for( EffectList::iterator it = m_AddEffectList.begin(); it != m_AddEffectList.end(); ++it )
	{
		m_EffectList.push_back( *it );
	}
	m_AddEffectList.clear();

	//	削除済みエフェクトの消去
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

	//	更新
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
//	描画
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
//	エフェクトを作成(位置のみ指定)
//******************************************************************************
Effect* EffectManager::CreateEffect( const EffectFile* pFile, Point Pos )
{
	return CreateEffect( pFile, Matrix3::Trans( Pos ) );
}

//******************************************************************************
//	エフェクトを作成（マトリクス指定）
//******************************************************************************
Effect* EffectManager::CreateEffect( const EffectFile* pFile, const Matrix3& Mat )
{
	if( !pFile ) return NULL;

	Effect* pEffect = new Effect;
	MEM_CHECK( pEffect );

	pEffect->Init( pFile );
	pEffect->SetMatrix( Mat );

	//	登録予約
	m_AddEffectList.push_back( pEffect );

	return pEffect;
}

//******************************************************************************
//	全エフェクトを削除
//******************************************************************************
void EffectManager::DeleteAll( void )
{
	DeleteVec( m_EffectList );
	DeleteVec( m_AddEffectList );
}