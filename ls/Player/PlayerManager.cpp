#include "stdafx.h"
#include "PlayerManager.h"
#include "Player.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"
#include "Player/PlayerData.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�v���C���[�Ǘ�
//
////////////////////////////////////////////////////////////////////////////////
PlayerManager::PlayerManager( World* pWorld )
{
	_ASSERT( pWorld );

	m_pWorld = pWorld;
}

PlayerManager::~PlayerManager()
{
	DeleteVec( m_vPlayer );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void PlayerManager::Update( void )
{
	for( VecPlayer::iterator it = m_vPlayer.begin(); it != m_vPlayer.end(); ++it )
	{
		(*it)->Update();
	}
}

//******************************************************************************
//	�`��
//******************************************************************************
void PlayerManager::Draw( void )
{
	for( VecPlayer::iterator it = m_vPlayer.begin(); it != m_vPlayer.end(); ++it )
	{
		(*it)->Draw();
	}
}

//******************************************************************************
//	�v���C���[�ǉ�
//******************************************************************************
void PlayerManager::AddPlayer( const PlayerData* pData, Point Pos )
{
	Player* pPlayer = new Player( this );
	if( pPlayer )
	{
		int PlayerID = GetPlayerNum();

		pPlayer->Init( pData, PlayerID, Pos );
	}
	m_vPlayer.push_back( pPlayer );
}

//******************************************************************************
//	�v���C���[�ύX
//******************************************************************************
void PlayerManager::ChangePlayer( const PlayerData* pData )
{
	Player* pPlayer = GetPlayer();
	if( !pPlayer ) return;

	//	�ʒu���擾
	Point Pos = pPlayer->GetPos();

	//	�폜
	DeleteVec( m_vPlayer );

	//	�V�K
	AddPlayer( pData, Pos );
}

//******************************************************************************
//	�R���e�B�j���[�ʒm
//******************************************************************************
void PlayerManager::OnContinue( void )
{
	Player* pPlayer = GetPlayer();
	if( pPlayer )
	{
		pPlayer->Revive();
	}
}