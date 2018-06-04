#include "stdafx.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include "EnemyIndicate.h"

#include "World/World.h"

#include "Score/ScoreManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�Ǘ�
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
//	�X�V
//******************************************************************************
void EnemyManager::Update( void )
{
	//	�G
	for( EnemyList::iterator it = m_EnemyList.begin(); it != m_EnemyList.end(); )
	{
		(*it)->Update();

		//	�폜
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

	//	�C���W�P�[�g
	m_pIndicate->Update();

	//	�X�R�A�ʒm
	if( IsDefeatAll() )
	{
		GetWorld()->GetScoreManager()->StopTimeValue( SCORE_TIME_VALUE_ENEMY_EXIST );
	}
}

//******************************************************************************
//	�`��
//******************************************************************************
void EnemyManager::Draw( void )
{
	for( EnemyList::iterator it = m_EnemyList.begin(); it != m_EnemyList.end(); ++it )
	{
		(*it)->Draw();
	}
}

//******************************************************************************
//	�G�̒ǉ�
//******************************************************************************
Enemy* EnemyManager::AddEnemy( const EnemyData* pData, const Matrix3& Mat, Label Name )
{
	Enemy* pEnemy = new Enemy( this );
	if( pEnemy )
	{
		pEnemy->Init( pData, Mat, Name );

		m_EnemyList.push_back( pEnemy );

		//	�{�X�̏o��
		if( pEnemy->IsBoss() )
		{
			m_pIndicate->AddEnemy( pEnemy );
		}
	}

	return pEnemy;
}

//******************************************************************************
//	�G�̎擾
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
//	�X�s�A�̍U���ڕW�ʒu�𓾂�
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
//	�G���폜����
//******************************************************************************
void EnemyManager::EraseEnemy( Label Name )
{
	Enemy* pEnemy = GetEnemy( Name );

	if( pEnemy ) pEnemy->Delete();
}

//******************************************************************************
//	�G�����ׂč폜����
//******************************************************************************
void EnemyManager::EraseAllEnemy( void )
{
	for( EnemyList::iterator it = m_EnemyList.begin(); it != m_EnemyList.end(); ++it )
	{
		(*it)->Delete();
	}
}

//******************************************************************************
//	�G�����ׂĔj�󂷂�
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
//	�G�����S�ς݂��ǂ���
//******************************************************************************
bool EnemyManager::IsDefeat( Label Name ) const
{
	const Enemy* pEnemy = GetEnemy( Name );
	if( !pEnemy ) return true;

	return pEnemy->IsDead() || pEnemy->IsNoCount();
}


//******************************************************************************
//	���ׂĂ̓G�����S�ς݂��ǂ���
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
//	���ׂĂ̓G�����Ȃ����ǂ���
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