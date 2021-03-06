#include "stdafx.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include "EnemyIndicate.h"

#include "World/World.h"

#include "Score/ScoreManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	敵管理
//
////////////////////////////////////////////////////////////////////////////////
EnemyManager::EnemyManager( World* pWorld ) : m_Rand(0)
{
	_ASSERT( pWorld );

	m_pWorld = pWorld;
	m_pIndicate = new EnemyIndicate( pWorld );
	MEM_CHECK( m_pIndicate );
}
EnemyManager::~EnemyManager()
{
	SafeDelete( m_pIndicate );
	DeleteVec( m_EnemyList );
}

//******************************************************************************
//	更新
//******************************************************************************
void EnemyManager::Update( void )
{
	//	敵
	for( EnemyList::iterator it = m_EnemyList.begin(); it != m_EnemyList.end(); )
	{
		(*it)->Update();

		//	削除
		if( (*it)->IsDelete() )
		{
			SafeDelete( *it );
			it = m_EnemyList.erase( it );
		}
		else
		{
			++it;
		}
	}

	//	インジケート
	m_pIndicate->Update();

	//	スコア通知
	if( IsDefeatAll() )
	{
		GetWorld()->GetScoreManager()->StopTimeValue( SCORE_TIME_VALUE_ENEMY_EXIST );
	}
}

//******************************************************************************
//	描画
//******************************************************************************
void EnemyManager::Draw( void )
{
	for( EnemyList::iterator it = m_EnemyList.begin(); it != m_EnemyList.end(); ++it )
	{
		(*it)->Draw();
	}
}

//******************************************************************************
//	敵の追加
//******************************************************************************
Enemy* EnemyManager::AddEnemy( const EnemyData* pData, const Matrix3& Mat, Label Name )
{
	Enemy* pEnemy = new Enemy( this );
	if( pEnemy )
	{
		pEnemy->Init( pData, Mat, Name );

		m_EnemyList.push_back( pEnemy );

		//	ボスの出現
		if( pEnemy->IsBoss() )
		{
			m_pIndicate->AddEnemy( pEnemy );
		}
	}

	return pEnemy;
}

//******************************************************************************
//	敵の取得
//******************************************************************************
Enemy* EnemyManager::GetEnemy( Label Name ) const
{
	for( EnemyList::const_iterator it = m_EnemyList.begin(); it != m_EnemyList.end(); ++it )
	{
		if( (*it)->GetName() == Name )
		{
			return *it;
		}
	}
	return NULL;
}

//******************************************************************************
//	スピアの攻撃目標位置を得る
//******************************************************************************
Enemy* EnemyManager::GetSpearTarget( Point Pos ) const
{
	Enemy* pTarget = NULL;
	for( EnemyList::const_iterator it = m_EnemyList.begin(); it != m_EnemyList.end(); ++it )
	{
		Enemy* pEnemy = (*it);

		if( pEnemy->IsDead() || pEnemy->IsDelete() || !pEnemy->IsHitEnable() || pEnemy->IsNoCount() ) continue;

		if( !pTarget ||
			pEnemy->GetMaxLife() >= pTarget->GetMaxLife() )
		{
			pTarget = pEnemy;
		}
	}

	return pTarget;
}

//******************************************************************************
//	敵を削除する
//******************************************************************************
void EnemyManager::EraseEnemy( Label Name )
{
	Enemy* pEnemy = GetEnemy( Name );

	if( pEnemy ) pEnemy->Delete();
}

//******************************************************************************
//	敵をすべて削除する
//******************************************************************************
void EnemyManager::EraseAllEnemy( void )
{
	for( EnemyList::iterator it = m_EnemyList.begin(); it != m_EnemyList.end(); ++it )
	{
		(*it)->Delete();
	}
}

//******************************************************************************
//	敵をすべて破壊する
//******************************************************************************
void EnemyManager::DestroyAllEnemy( void )
{
	for( EnemyList::iterator it = m_EnemyList.begin(); it != m_EnemyList.end(); ++it )
	{
		if( !(*it)->IsNoCount() && !(*it)->IsDead() )
		{
			(*it)->Destroy();
		}
	}
}

//******************************************************************************
//	敵が死亡済みかどうか
//******************************************************************************
bool EnemyManager::IsDefeat( Label Name ) const
{
	const Enemy* pEnemy = GetEnemy( Name );
	if( !pEnemy ) return true;

	return pEnemy->IsDead() || pEnemy->IsNoCount();
}


//******************************************************************************
//	すべての敵が死亡済みかどうか
//******************************************************************************
bool EnemyManager::IsDefeatAll( void ) const
{
	for( EnemyList::const_iterator it = m_EnemyList.begin(); it != m_EnemyList.end(); ++it )
	{
		if( !(*it)->IsNoCount() && !(*it)->IsDead() )
		{
			return false;
		}
	}
	return true;
}

//******************************************************************************
//	すべての敵が居ないかどうか
//******************************************************************************
bool EnemyManager::IsEmpty( void ) const
{
	for( EnemyList::const_iterator it = m_EnemyList.begin(); it != m_EnemyList.end(); ++it )
	{
		if( !(*it)->IsNoCount() )
		{
			return false;
		}
	}
	return true;
}