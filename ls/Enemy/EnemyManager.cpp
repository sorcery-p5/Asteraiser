#include "stdafx.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include "EnemyIndicate.h"

#include "World/World.h"

#include "Score/ScoreManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	“GŠÇ—
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
//	XV
//******************************************************************************
void EnemyManager::Update( void )
{
	//	“G
	for( EnemyList::iterator it = m_EnemyList.begin(); it != m_EnemyList.end(); )
	{
		(*it)->Update();

		//	íœ
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

	//	ƒCƒ“ƒWƒP[ƒg
	m_pIndicate->Update();

	//	ƒXƒRƒA’Ê’m
	if( IsDefeatAll() )
	{
		GetWorld()->GetScoreManager()->StopTimeValue( SCORE_TIME_VALUE_ENEMY_EXIST );
	}
}

//******************************************************************************
//	•`‰æ
//******************************************************************************
void EnemyManager::Draw( void )
{
	for( EnemyList::iterator it = m_EnemyList.begin(); it != m_EnemyList.end(); ++it )
	{
		(*it)->Draw();
	}
}

//******************************************************************************
//	“G‚Ì’Ç‰Á
//******************************************************************************
Enemy* EnemyManager::AddEnemy( const EnemyData* pData, const Matrix3& Mat, Label Name )
{
	Enemy* pEnemy = new Enemy( this );
	if( pEnemy )
	{
		pEnemy->Init( pData, Mat, Name );

		m_EnemyList.push_back( pEnemy );

		//	ƒ{ƒX‚ÌoŒ»
		if( pEnemy->IsBoss() )
		{
			m_pIndicate->AddEnemy( pEnemy );
		}
	}

	return pEnemy;
}

//******************************************************************************
//	“G‚Ìæ“¾
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
//	ƒXƒsƒA‚ÌUŒ‚–Ú•WˆÊ’u‚ğ“¾‚é
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
//	“G‚ğíœ‚·‚é
//******************************************************************************
void EnemyManager::EraseEnemy( Label Name )
{
	Enemy* pEnemy = GetEnemy( Name );

	if( pEnemy ) pEnemy->Delete();
}

//******************************************************************************
//	“G‚ğ‚·‚×‚Äíœ‚·‚é
//******************************************************************************
void EnemyManager::EraseAllEnemy( void )
{
	for( EnemyList::iterator it = m_EnemyList.begin(); it != m_EnemyList.end(); ++it )
	{
		(*it)->Delete();
	}
}

//******************************************************************************
//	“G‚ğ‚·‚×‚Ä”j‰ó‚·‚é
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
//	“G‚ª€–SÏ‚İ‚©‚Ç‚¤‚©
//******************************************************************************
bool EnemyManager::IsDefeat( Label Name ) const
{
	const Enemy* pEnemy = GetEnemy( Name );
	if( !pEnemy ) return true;

	return pEnemy->IsDead() || pEnemy->IsNoCount();
}


//******************************************************************************
//	‚·‚×‚Ä‚Ì“G‚ª€–SÏ‚İ‚©‚Ç‚¤‚©
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
//	‚·‚×‚Ä‚Ì“G‚ª‹‚È‚¢‚©‚Ç‚¤‚©
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