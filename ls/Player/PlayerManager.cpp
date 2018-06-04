#include "stdafx.h"
#include "PlayerManager.h"
#include "Player.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"
#include "Player/PlayerData.h"


////////////////////////////////////////////////////////////////////////////////
//
//	プレイヤー管理
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
//	更新
//******************************************************************************
void PlayerManager::Update( void )
{
	for( VecPlayer::iterator it = m_vPlayer.begin(); it != m_vPlayer.end(); ++it )
	{
		(*it)->Update();
	}
}

//******************************************************************************
//	描画
//******************************************************************************
void PlayerManager::Draw( void )
{
	for( VecPlayer::iterator it = m_vPlayer.begin(); it != m_vPlayer.end(); ++it )
	{
		(*it)->Draw();
	}
}

//******************************************************************************
//	プレイヤー追加
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
//	プレイヤー変更
//******************************************************************************
void PlayerManager::ChangePlayer( const PlayerData* pData )
{
	Player* pPlayer = GetPlayer();
	if( !pPlayer ) return;

	//	位置を取得
	Point Pos = pPlayer->GetPos();

	//	削除
	DeleteVec( m_vPlayer );

	//	新規
	AddPlayer( pData, Pos );
}

//******************************************************************************
//	コンティニュー通知
//******************************************************************************
void PlayerManager::OnContinue( void )
{
	Player* pPlayer = GetPlayer();
	if( pPlayer )
	{
		pPlayer->Revive();
	}
}