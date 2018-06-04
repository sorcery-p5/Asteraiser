#include "stdafx.h"
#include "ItemManager.h"
#include "Item.h"
#include "ItemData.h"

namespace
{
	const float	SPEED_RAND_MIN		=	0.75f;
	const float	SPEED_RAND_MAX		=	1.0f;
}

////////////////////////////////////////////////////////////////////////////////
//
//	�A�C�e���Ǘ�
//
////////////////////////////////////////////////////////////////////////////////
ItemManager::ItemManager( World* pWorld, const ItemData* pData ) : m_Rand(0)
{
	_ASSERT( pWorld );

	m_pWorld	= pWorld;
	m_pData		= pData;

	m_CollectTime	= 0;
	m_CollectRadius	= pData->GetParam().CollectRadius;

	m_bPlaySound	= false;
}
ItemManager::~ItemManager()
{
	DeleteVec( m_ItemList );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void ItemManager::Update( void )
{
	//	�U��
	for( ItemList::iterator it = m_ItemList.begin(); it != m_ItemList.end(); )
	{
		(*it)->Update();
		
		//	�폜�`�F�b�N
		if( (*it)->IsDelete() )
		{
			SafeDelete( *it );
			it = m_ItemList.erase( it );
		}
		else
		{
			++it;
		}
	}

	//	�������
	if( m_CollectTime > 0 )
	{
		DecreaseZero( m_CollectTime, 1 );
		if( m_CollectTime <= 0 )
		{
			m_CollectRadius = m_pData->GetParam().CollectRadius;
		}
	}

	m_bPlaySound = false;
}

//******************************************************************************
//	�V�K����
//******************************************************************************
void ItemManager::AppearItem( int Num, Point Pos, float Speed )
{
	if( Num == 0 ) return;

	float Vel = Speed * m_pData->GetParam().Speed;

	for( int i = 0; i < Num; i++ )
	{
		Angle Dir = GetRand().GetRandFloat( PI2 );

		_AddItem( Pos, Dir.GetDir() * (Vel * GetRand().GetRandFloat(SPEED_RAND_MIN,SPEED_RAND_MAX) ) );
	}
}

//******************************************************************************
//	������ꂽ�ʒm
//******************************************************************************
void ItemManager::OnGet( void )
{
	if( !m_bPlaySound )
	{
		//	�T�E���h�Đ�
		const SoundFile* pSoundFile = m_pData->GetSoundFile();
		if( pSoundFile )
		{
			if( GetTone()->PlaySound( pSoundFile, SOUND_CHANNEL_SE, false, m_pData->GetParam().SoundVolume ) )
			{
				m_bPlaySound = true;
			}
		}
	}
}

//******************************************************************************
//	�������
//******************************************************************************
void ItemManager::ForceCollect( void )
{
	for( ItemList::iterator it = m_ItemList.begin(); it != m_ItemList.end(); ++it )
	{
		(*it)->AutoCollect();
	}	
}

//******************************************************************************
//	�������
//******************************************************************************
void ItemManager::AutoCollect( void )
{
	m_CollectTime	= m_pData->GetParam().CollectTime;
	m_CollectRadius	= m_pData->GetParam().AutoCollectRadius;
}

//******************************************************************************
//	����������Ԃ̊������擾
//******************************************************************************
float ItemManager::GetCollectTimeRate( void ) const
{
	return (float)m_CollectTime / (float)m_pData->GetParam().CollectTime;
}

//------------------------------------------------------------------------------
//	�A�C�e����ǉ�
//------------------------------------------------------------------------------
void ItemManager::_AddItem( Point Pos, Point Vel )
{
	Item* pItem = new Item( this );
	MEM_CHECK( pItem );

	pItem->Init( m_pData, Pos, Vel );

	m_ItemList.push_back( pItem );
}