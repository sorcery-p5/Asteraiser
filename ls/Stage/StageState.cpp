#include "stdafx.h";
#include "StageState.h"
#include "Stage.h"
#include "StageData.h"
#include "StageTelop.h"

#include "App/App.h"
#include "Effect/EffectManager.h"
#include "BGM/BGMManager.h"
#include "SpriteFrame/SpriteFrame.h"

#include "World/World.h"
#include "World/WorldData.h"
#include "Player/PlayerManager.h"
#include "Player/Player.h"
#include "Enemy/EnemyManager.h"
#include "Enemy/Enemy.h"
#include "Ground/GroundManager.h"
#include "Ground/Ground.h"
#include "Score/ScoreManager.h"
#include "Item/ItemManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	ステージステート
//
////////////////////////////////////////////////////////////////////////////////
StageState::StageState( Stage* pStage )
{
	_ASSERT( pStage );

	m_pStage	= pStage;
	m_pData		= NULL;
	
	m_pCurState			= NULL;
	m_FrameCount		= 0;
}

StageState::~StageState()
{
}

//******************************************************************************
//	初期化
//******************************************************************************
void StageState::Init( const StageData* pData, Label State )
{
	_ASSERT( pData );
	
	m_pData	= pData;

	//	初期ステート開始
	_ChangeState( State.IsEmpty()? m_pData->GetParam().RootState : State );

	//	初期化アクションの実行
	_ExecInitAction( m_pData->GetState( m_pData->GetParam().InitActionState ) );

	_ASSERT( m_pCurState );
}

//******************************************************************************
//	更新
//******************************************************************************
void StageState::Update( void )
{
	//	アクション
	for( uint i = 0; i < m_pCurState->ActionList.size(); i++ )
	{
		if( m_vExecAction[i] ) continue;

		const STAGE_ACTION& Action = m_pCurState->ActionList[ i ];

		if( _IsExecAction( Action ) )
		{
			//	実行
			_ExecAction( Action );

			m_vExecAction[i] = true;
		}
	}

	//	ステート切り替え
	if( !m_NextStateName.IsEmpty() )
	{
		_ChangeState( m_NextStateName );
	}

	//	フレームカウント
	m_FrameCount++;
}

//******************************************************************************
//	通知を追加
//******************************************************************************
void StageState::AddNotify( Label Notify )
{
	m_vNotify.push_back( Notify );
}

//------------------------------------------------------------------------------
//	ステート変更
//------------------------------------------------------------------------------
void StageState::_ChangeState( Label Name )
{
	m_pCurState = m_pData->GetState( Name );
	_ASSERT( m_pCurState );

	if( m_pCurState )
	{
		m_FrameCount	= 0;

		m_vExecAction.clear();
		m_vExecAction.resize( m_pCurState->ActionList.size(), false );

		m_NextStateName.Clear();
	}
}

//------------------------------------------------------------------------------
//	フレームをループさせる
//------------------------------------------------------------------------------
void StageState::_LoopFrame( int Frame )
{
	m_vExecAction.assign( m_vExecAction.size(), false );
	m_FrameCount = Frame;
}

//------------------------------------------------------------------------------
//	初期化アクションの実行
//------------------------------------------------------------------------------
void StageState::_ExecInitAction( const STAGE_STATE* pState )
{
	if( !pState ) return;
	
	for( uint i = 0; i < pState->ActionList.size(); i++ )
	{
		const STAGE_ACTION& Action = pState->ActionList[ i ];

		//	実行
		_ExecAction( Action );
	}
}

//------------------------------------------------------------------------------
//	アクション実行可能か
//------------------------------------------------------------------------------
bool StageState::_IsExecAction( const STAGE_ACTION& Action ) const
{
	if( Action.Frame >= 0 && m_FrameCount < Action.Frame )
	{
		return false;
	}

	for( uint i = 0; i < Action.ConditionList.size(); i++ )
	{
		if( !_CheckCondition( Action.ConditionList[i] ) )
		{
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------
//	条件の判定
//------------------------------------------------------------------------------
bool StageState::_CheckCondition( const STAGE_CONDITION& Condition ) const
{
	switch( Condition.Type )
	{
		case STAGE_CONDITION_DEFEAT_ENEMY:		return m_pStage->GetWorld()->GetEnemyManager()->IsDefeat( Condition.Name );
		case STAGE_CONDITION_DEFEAT_ENEMY_ALL:	return m_pStage->GetWorld()->GetEnemyManager()->IsDefeatAll();
		case STAGE_CONDITION_EMPTY_ENEMY:		return m_pStage->GetWorld()->GetEnemyManager()->IsEmpty();
		case STAGE_CONDITION_EMPTY_GROUND:		return m_pStage->GetWorld()->GetGroundManager()->GetGround( Condition.Name ) == NULL;
		case STAGE_CONDITION_END_TELOP:			return m_pStage->IsTelopEnd( Condition.Name );
		case STAGE_CONDITION_STATE_FRAME:		return m_FrameCount >= Condition.Param;
		case STAGE_CONDITION_STAGE_FRAME:		return m_pStage->GetFrameCount() >= Condition.Param;
		case STAGE_CONDITION_NOTIFY:			return _IsNotify( Condition.Name );
		case STAGE_CONDITION_END_STATE:			return !m_NextStateName.IsEmpty();
			
		case STAGE_CONDITION_END_GROUND_ANIME:
			{
				Ground* pGround = m_pStage->GetWorld()->GetGroundManager()->GetGround( Condition.Name );
				if( pGround ) return pGround->GetSprite()->IsAnimeLast();
			}
			break;
		case STAGE_CONDITION_PLAYER_STATE:
			{
				Player* pPlayer = m_pStage->GetWorld()->GetPlayerManager()->GetPlayer();
				if( pPlayer ) return pPlayer->IsState( Condition.Name );
			}
			break;
	}

	return false;
}

//------------------------------------------------------------------------------
//	アクションの実行
//------------------------------------------------------------------------------
void StageState::_ExecAction( const STAGE_ACTION& Action )
{
	Point FieldPos = m_pStage->GetWorld()->GetFieldOffset();

	switch( Action.Type )
	{
	//	敵
	case STAGE_ACTION_ENEMY:
		{
			const EnemyData*	pEnemyData	= m_pData->GetEnemyData( Action.DataName );
			Matrix3				Mat			= Matrix3::TransRotZ( Action.Pos + FieldPos, Action.Rot );

			m_pStage->GetWorld()->GetEnemyManager()->AddEnemy( pEnemyData, Mat, Action.Name );
		}
		break;

	//	地形
	case STAGE_ACTION_GROUND:
		{
			const GroundData*	pGroundData	= m_pData->GetGroundData( Action.DataName );
			Matrix3				Mat			= Matrix3::TransRotZ( Action.Pos + FieldPos, Action.Rot );

			m_pStage->GetWorld()->GetGroundManager()->AddGround( pGroundData, Mat, Action.Name, Action.ParamVal == TRUE );
		}
		break;

	//	テロップ
	case STAGE_ACTION_TELOP:
		{
			Matrix3		Mat			= Matrix3::TransRotZ( Action.Pos + FieldPos, Action.Rot );

			m_pStage->AddTelop( Action.Name, Mat );
		}
		break;

	//	エフェクト
	case STAGE_ACTION_EFFECT:
		{
			const EffectFile*	pEffectFile	= m_pData->GetEffectFile( Action.DataName );
			Matrix3				Mat			= Matrix3::TransRotZ( Action.Pos + FieldPos, Action.Rot );

			GetApp()->GetEffectMng()->CreateEffect( pEffectFile, Mat );
		}
		break;

	//	BGM再生
	case STAGE_ACTION_BGM_PLAY:
		{
			GetApp()->GetBGMMng()->Play( Action.Text.c_str(), Action.ParamVal );
		}
		break;
	//	BGMフェード
	case STAGE_ACTION_BGM_FADE:
		{
			GetApp()->GetBGMMng()->Fade( (int)Action.ParamVal );
		}
		break;
	//	BGM終了
	case STAGE_ACTION_BGM_STOP:
		{
			GetApp()->GetBGMMng()->Stop();
		}
		break;

	//	テロップ文字列
	case STAGE_ACTION_TELOP_TEXT:
		{
			StageTelop* pTelop = m_pStage->GetTelop( Action.Name );
			if( pTelop )
			{
				pTelop->SetText( Action.ParamStr, Action.Text.c_str() );
			}
		}
		break;

	//	テロップアニメ
	case STAGE_ACTION_TELOP_ANIME:
		{
			StageTelop* pTelop = m_pStage->GetTelop( Action.Name );
			if( pTelop )
			{
				pTelop->ChangeAnime( Action.ParamStr, Action.ParamVal );
			}
		}
		break;

	//	テロップ閉じる
	case STAGE_ACTION_TELOP_CLOSE:
		{
			StageTelop* pTelop = m_pStage->GetTelop( Action.Name );
			if( pTelop )
			{
				pTelop->Close();
			}
		}
		break;

	//	地形のアニメ追加
	case STAGE_ACTION_GROUND_ANIME:
		{
			Ground* pGround = m_pStage->GetWorld()->GetGroundManager()->GetGround( Action.Name );
			if( pGround )
			{
				if( !Action.ParamVal )
				{
					pGround->PushAnime( Label( Action.ParamStr ) );
				}
				else
				{
					pGround->GetSprite()->ChangeAnime( Label( Action.ParamStr ) );
				}
			}
		}
		break;

	//	地形を終了させる
	case STAGE_ACTION_GROUND_END:
		{
			m_pStage->GetWorld()->GetGroundManager()->EndGround( Action.Name );
		}
		break;

	//	敵の子を登録
	case STAGE_ACTION_ENEMY_CHILD:
		{
			Enemy* pParent = m_pStage->GetWorld()->GetEnemyManager()->GetEnemy( Action.Name );
			Enemy* pChild = m_pStage->GetWorld()->GetEnemyManager()->GetEnemy( Action.ParamStr );

			if( pParent && pChild )
			{
				pParent->SetChild( pChild );
				pChild->SetParent( pParent );
			}
		}
		break;

	//	敵に通知
	case STAGE_ACTION_ENEMY_NOTIFY:
		{
			Enemy* pEnemy = m_pStage->GetWorld()->GetEnemyManager()->GetEnemy( Action.Name );

			if( pEnemy )
			{
				pEnemy->OnNotify( Action.ParamStr );
			}
		}
		break;

	//	敵を全破壊
	case STAGE_ACTION_DESTROY_ENEMY_ALL:
		{
			m_pStage->GetWorld()->GetEnemyManager()->DestroyAllEnemy();
		}
		break;

	//	敵を消去
	case STAGE_ACTION_ERASE_ENEMY:
		{
			m_pStage->GetWorld()->GetEnemyManager()->EraseEnemy( Action.Name );
		}
		break;

	//	敵を全消去
	case STAGE_ACTION_ERASE_ENEMY_ALL:
		{
			m_pStage->GetWorld()->GetEnemyManager()->EraseAllEnemy();
		}
		break;

	//	地形を消去
	case STAGE_ACTION_ERASE_GROUND:
		{
			m_pStage->GetWorld()->GetGroundManager()->EraseGround( Action.Name );
		}
		break;

	//	地形を全消去
	case STAGE_ACTION_ERASE_GROUND_ALL:
		{
			m_pStage->GetWorld()->GetGroundManager()->EraseAllGround();
		}
		break;

	//	自機の位置を設定
	case STAGE_ACTION_PLAYER_POS:
		{
			if( Action.ParamVal <= 0 )
			{
				//	瞬時
				Player* pPlayer = m_pStage->GetWorld()->GetPlayerManager()->GetPlayer();
				if( pPlayer ) pPlayer->SetPos( Action.Pos + FieldPos );
			}
			else
			{
				//	時間をかける
				m_pStage->AddAction( &Action );
			}
		}
		break;

	//	自機のアニメを設定
	case STAGE_ACTION_PLAYER_ANIME:
		{
			Player* pPlayer = m_pStage->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer ) pPlayer->ReplaceAnime( Action.ParamStr, Action.ParamVal );
		}
		break;

	//	自機のアクション
	case STAGE_ACTION_PLAYER_ACTION:
		{
			Player* pPlayer = m_pStage->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer ) pPlayer->ApplyAction( Action.Name, Action.Pos );
		}
		break;

	//	自機の移動設定
	case STAGE_ACTION_PLAYER_MOVE:
		{
			Player* pPlayer = m_pStage->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer ) pPlayer->SetMoveEnable( Action.ParamVal == TRUE );
		}
		break;

	//	自機の攻撃設定
	case STAGE_ACTION_PLAYER_ATTACK:
		{
			Player* pPlayer = m_pStage->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer ) pPlayer->SetAttackEnable( Action.ParamVal == TRUE );
		}
		break;

	//	自機の動作制限
	case STAGE_ACTION_PLAYER_LOCK:
		{
			Player* pPlayer = m_pStage->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer ) pPlayer->LockAction( Action.Name, Action.ParamVal == TRUE );
		}
		break;

	//	自機の表示物可否
	case STAGE_ACTION_PLAYER_INDICATE:
		{
			Player* pPlayer = m_pStage->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer ) pPlayer->SetIndicateEnable( Action.ParamVal == TRUE );
		}
		break;

	//	スコアロック
	case STAGE_ACTION_LOCK_SCORE:
		{
			m_pStage->GetWorld()->GetScoreManager()->SetReteLock( (int)Action.ParamVal );
		}
		break;

	//	アイテム発生
	case STAGE_ACTION_ITEM:
		{
			m_pStage->GetWorld()->GetItemManager()->AppearItem( (int)Action.ParamVal, Action.Pos + m_pStage->GetWorld()->GetFieldOffset(), 10.0f );
		}
		break;

	//	ステージ到達通知
	case STAGE_ACTION_ARRIVE:
		{
			m_pStage->GetWorld()->GetScoreManager()->Arrive( (int)Action.ParamVal );
		}
		break;

	//	ステージリザルト開く
	case STAGE_ACTION_RESULT:
		{
			m_pStage->GetWorld()->OpenResult();
		}
		break;

	//	自機の変更
	case STAGE_ACTION_CHANGE_PLAYER:
		{
			m_pStage->GetWorld()->GetPlayerManager()->ChangePlayer( m_pStage->GetWorld()->GetData()->GetPlayerData( Action.Name ) );
		}
		break;

	//	フレームをループ
	case STAGE_ACTION_LOOP:
		{
			_LoopFrame( (int)Action.ParamVal );
		}
		break;

	//	ステート変更
	case STAGE_ACTION_CHANGE_STATE:
		{
			m_NextStateName = Action.Name;
		}
		break;

	//	ステージ終了
	case STAGE_ACTION_END_STAGE:
		{
			m_pStage->End();
		}
		break;
	}
}

//------------------------------------------------------------------------------
//	通知の有無を取得
//------------------------------------------------------------------------------
bool StageState::_IsNotify( Label Notify )  const
{
	NotifyVec::const_iterator it = StdFind( m_vNotify, Notify );
	return it != m_vNotify.end();
}