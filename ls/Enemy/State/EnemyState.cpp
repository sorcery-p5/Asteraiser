#include "stdafx.h"
#include "EnemyState.h"

#include "EnemyMoveAction.h"
#include "EnemyRotAction.h"
#include "EnemyAttackAction.h"
#include "EnemyTarget.h"

#include "../Enemy.h"
#include "../EnemyData.h"
#include "../EnemyStateData.h"

#include "World/World.h"
#include "Player/Player.h"
#include "Player/PlayerManager.h"
#include "Score/ScoreManager.h"
#include "Stage/StageManager.h"
#include "Stage/Stage.h"

#include "SpriteFrame/SpriteFrame.h"


////////////////////////////////////////////////////////////////////////////////
//
//	敵ステート
//
////////////////////////////////////////////////////////////////////////////////
EnemyState::EnemyState( Enemy* pEnemy )
{
	_ASSERT( pEnemy && pEnemy->GetData() );

	m_pEnemy			= pEnemy;
	m_pData				= pEnemy->GetData();
	m_pCurState			= NULL;
	m_pResidentState	= NULL;
	m_StateCount		= 0;
}
EnemyState::~EnemyState()
{
	DeleteVec( m_ActionList );
}

//******************************************************************************
//	初期化
//******************************************************************************
void EnemyState::Init( void )
{
	//	初期ステート
	_ChangeState( m_pData->GetHandlerStateName( ENEMY_HANDLER_STATE_ROOT ) );

	//	常駐ステート
	m_pResidentState = m_pData->GetStateData( m_pData->GetHandlerStateName( ENEMY_HANDLER_STATE_RESIDENT ) );
	if( m_pResidentState )
	{
		m_vExecResidentAction.clear();
		m_vExecResidentAction.resize( m_pResidentState->GetActionNum(), false );
	}
}

//******************************************************************************
//	更新
//******************************************************************************
void EnemyState::Update( void )
{
	//	ステート
	if( m_pCurState )
	{
		//	アクション実行
		_UpdateExecAction( m_pCurState, m_vExecAction );
		
		m_StateCount++;
	}

	//	常駐ステート
	if( m_pResidentState )
	{
		//	アクション実行
		_UpdateExecAction( m_pResidentState, m_vExecResidentAction );
	}

	//	アクション更新
	for( ActionList::iterator it = m_ActionList.begin(); it != m_ActionList.end(); )
	{
		EnemyAction* pAction = *it;

		//	削除
		if( pAction->IsDelete() )
		{
			SafeDelete( pAction );
			it = m_ActionList.erase( it );
			continue;
		}
		
		//	更新
		pAction->Update();

		++it;
	}
	
	//	ステート変更
	if( !m_NextStateName.IsEmpty() )
	{
		_ChangeState( m_NextStateName );
		m_NextStateName.Clear();
	}
}

//******************************************************************************
//	ハンドラステートを実行する
//******************************************************************************
void EnemyState::ChangeHandlerState( Label Name )
{
	Label StateName = m_pData->GetHandlerStateName( Name );

	if( !StateName.IsEmpty() && m_CurStateName != StateName )
	{
		_SetNextState( StateName );
	}
}

//******************************************************************************
//	移動目標位置を取得
//******************************************************************************
const Point* EnemyState::GetMoveTarget( void ) const
{
	for( ActionList::const_iterator it = m_ActionList.begin(); it != m_ActionList.end(); ++it )
	{
		if( (*it)->GetType() == ENEMY_ACTION_MOVE )
		{
			return (*it)->GetTargetPos();
		}
	}
	return NULL;
}

//******************************************************************************
//	旋回目標位置を取得
//******************************************************************************
const Point* EnemyState::GetRotTarget( void ) const
{
	for( ActionList::const_iterator it = m_ActionList.begin(); it != m_ActionList.end(); ++it )
	{
		if( (*it)->GetType() == ENEMY_ACTION_ROT )
		{
			return (*it)->GetTargetPos();
		}
	}
	return NULL;
}

//------------------------------------------------------------------------------
//	ステート変更
//------------------------------------------------------------------------------
void EnemyState::_ChangeState( Label Name )
{
	m_pCurState = m_pData->GetStateData( Name );

	if( m_pCurState )
	{
		m_CurStateName	= Name;
		m_StateCount	= 0;

		//	アクション停止
		for( ActionList::iterator it = m_ActionList.begin(); it != m_ActionList.end(); ++it )
		{
			EnemyAction* pAction = *it;
			if( !pAction->IsContinue() )
			{
				pAction->Delete();
			}
		}

		m_vExecAction.clear();
		m_vExecAction.resize( m_pCurState->GetActionNum(), false );
	}
}

//------------------------------------------------------------------------------
//	ステート変更
//------------------------------------------------------------------------------
void EnemyState::_SetNextState( Label Name )
{
	const EnemyStateData* pStateData = m_pData->GetStateData( Name );

	if( pStateData && Name != m_CurStateName )
	{
		m_NextStateName = Name;
	}
}

//------------------------------------------------------------------------------
//	アクション実行の更新
//------------------------------------------------------------------------------
void EnemyState::_UpdateExecAction( const EnemyStateData* pStateData, VecBool& vExecAction )
{
	//	アクション取得
	for( int Index = 0; Index < pStateData->GetActionNum(); Index++ )
	{
		const ENEMY_ACTION* pParam = pStateData->GetActionParam( Index );
		if( !vExecAction[Index] || !pParam->bOnce )
		{
			//	条件チェック
			if( _CheckCondition( pParam->ConditionList ) )
			{
				vExecAction[Index] = true;

				//	実行
				_ExecAction( pParam );
			}
		}
	}
}

//------------------------------------------------------------------------------
//	アクション実行
//------------------------------------------------------------------------------
void EnemyState::_ExecAction( const ENEMY_ACTION* pParam )
{
	switch( pParam->Type )
	{
	//	移動
	case ENEMY_ACTION_MOVE:
		{
			EnemyMoveAction* pAction = new EnemyMoveAction( m_pEnemy );
			MEM_CHECK( pAction );
			pAction->Init( pParam );

			m_ActionList.push_back( pAction );
		}
		break;

	//	旋回
	case ENEMY_ACTION_ROT:
		{
			EnemyRotAction* pAction = new EnemyRotAction( m_pEnemy );
			MEM_CHECK( pAction );
			pAction->Init( pParam );

			m_ActionList.push_back( pAction );
		}
		break;

	//	攻撃
	case ENEMY_ACTION_ATTACK:
		{
			EnemyAttackAction* pAction = new EnemyAttackAction( m_pEnemy );
			MEM_CHECK( pAction );
			pAction->Init( pParam );

			m_ActionList.push_back( pAction );
		}
		break;

	//	召喚
	case ENEMY_ACTION_SUMMON:
		{
			Matrix3 Mat = Matrix3::TransRotZ( Point(pParam->Summon.X, pParam->Summon.Y), pParam->Summon.Rot );

			m_pEnemy->AddChildEnemy( pParam->Summon.Name, pParam->Summon.BoneName, Mat, pParam->Summon.bAttach,  pParam->Summon.AppearName );
		}
		break;

	//	ワープ
	case ENEMY_ACTION_WARP:
		{
			EnemyTarget Target( m_pEnemy );
			Target.Init( &pParam->Warp.Target, m_pEnemy->GetPos() );

			m_pEnemy->SetPos( Target.GetPos() );
		}
		break;

	//	移動停止
	case ENEMY_ACTION_END_MOVE:
		{
			_DeleteAction( ENEMY_ACTION_MOVE );
		}
		break;
		
	//	旋回停止
	case ENEMY_ACTION_END_ROT:
		{
			_DeleteAction( ENEMY_ACTION_ROT );
		}
		break;
		
	//	攻撃停止
	case ENEMY_ACTION_END_ATTACK:
		{
			_DeleteAction( ENEMY_ACTION_ATTACK );

			//	登録済みの攻撃を解除
			m_pEnemy->DeleteLinkAttack( pParam->Name );
		}
		break;

	//	全アクション停止
	case ENEMY_ACTION_END_ACTION:
		{
			_DeleteActionAll();
		}
		break;

	//	アニメ変更
	case ENEMY_ACTION_CHANGE_ANIME:
		{
			if( m_pEnemy->GetSprite() )
			{
				m_pEnemy->GetSprite()->ChangeAnime( Label(pParam->Name), pParam->Param );
			}
		}
		break;

	//	アニメ停止
	case ENEMY_ACTION_PAUSE_ANIME:
		{
			m_pEnemy->SetPause( pParam->bFlag );
		}
		break;

	//	ステート変更
	case ENEMY_ACTION_CHANGE_STATE:
		{
			if( pParam->bFlag || m_NextStateName.IsEmpty() )
			{
				_SetNextState( Label(pParam->Name) );
			}
		}
		break;
		
	//	エフェクト
	case ENEMY_ACTION_EFFECT:
		{
			m_pEnemy->CreateEffect( Label(pParam->Name) );
		}
		break;
	
	//	サウンド
	case ENEMY_ACTION_SOUND:
		{
			m_pEnemy->PlaySound( Label(pParam->Name), pParam->Param );
		}
		break;
		
	//	色変更
	case ENEMY_ACTION_COLOR:
		{
			if( m_pEnemy->GetSprite() ) m_pEnemy->GetSprite()->SetColor( pParam->Col );
		}
		break;

	//	耐久力変更
	case ENEMY_ACTION_ADD_LIFE:
		{
			m_pEnemy->AddLife( pParam->Param );
		}
		break;
		
	//	防御力変更
	case ENEMY_ACTION_ADD_DEFENSE:
		{
			m_pEnemy->AddDefense( pParam->Param );
		}
		break;

	//	無敵
	case ENEMY_ACTION_INVINCIBLE:
		{
			m_pEnemy->SetInvincible( pParam->bFlag );
		}
		break;
		
	//	ハイド
	case ENEMY_ACTION_HIDE:
		{
			m_pEnemy->SetHide( pParam->bFlag );
		}
		break;

	//	ダメージ無効
	case ENEMY_ACTION_NO_DAMAGE:
		{
			m_pEnemy->SetNoDamage( pParam->bFlag );
		}
		break;

	//	当たり
	case ENEMY_ACTION_HIT:
		{
			m_pEnemy->SetHitEnable( pParam->bFlag );
		}
		break;
		
	//	地形当たり
	case ENEMY_ACTION_GROUND_HIT:
		{
			m_pEnemy->SetGroundHitEnable( pParam->bFlag );
		}
		break;

	//	画面外への移動可否
	case ENEMY_ACTION_OUT_FIELD:
		{
			m_pEnemy->SetOutField( pParam->bFlag );
		}
		break;

	//	カウント可否の設定
	case ENEMY_ACTION_NO_COUNT:
		{
			m_pEnemy->SetNoCount( pParam->bFlag );
		}
		break;

	//	反発の有無
	case ENEMY_ACTION_REPULSE:
		{
			m_pEnemy->SetRepulse( pParam->bFlag );
		}
		break;

	//	スコアロック
	case ENEMY_ACTION_LOCK_SCORE:
		{
			m_pEnemy->GetWorld()->GetScoreManager()->SetReteLock( pParam->Param );
		}
		break;

	//	親への通知
	case ENEMY_ACTION_PARENT_NOTIFY:
		{
			m_pEnemy->NotifyParent( Label(pParam->Name) );
		}
		break;

	//	子への通知
	case ENEMY_ACTION_CHILD_NOTIFY:
		{
			m_pEnemy->NotifyChild( Label(pParam->ParamStr), Label(pParam->Name) );
		}
		break;

	//	ステージへの通知
	case ENEMY_ACTION_STAGE_NOTIFY:
		{
			Stage* pStage = m_pEnemy->GetWorld()->GetStageManager()->GetCurStage();
			if( pStage )
			{
				pStage->AddNotify( Label(pParam->Name) );
			}
		}
		break;

	//	破壊
	case ENEMY_ACTION_DESTROY:
		{
			m_pEnemy->Destroy();
		}
		break;

	//	消去
	case ENEMY_ACTION_ESCAPE:
		{
			m_pEnemy->Escape();
		}
		break;
	}
}

//------------------------------------------------------------------------------
//	アクションの削除
//------------------------------------------------------------------------------
void EnemyState::_DeleteAction( ENEMY_ACTION_TYPE Type )
{
	for( ActionList::iterator it = m_ActionList.begin(); it != m_ActionList.end(); ++it )
	{
		EnemyAction* pAction = *it;
		if( pAction->GetType() == Type )
		{
			pAction->Delete();
		}
	}
}

//------------------------------------------------------------------------------
//	全アクションの削除
//------------------------------------------------------------------------------
void EnemyState::_DeleteActionAll( void )
{
	for( ActionList::iterator it = m_ActionList.begin(); it != m_ActionList.end(); ++it )
	{
		(*it)->Delete();
	}
}

//------------------------------------------------------------------------------
//	アクションが終了しているか？
//------------------------------------------------------------------------------
bool EnemyState::_IsEndAction( ENEMY_ACTION_TYPE Type ) const
{
	for( ActionList::const_iterator it = m_ActionList.begin(); it != m_ActionList.end(); ++it )
	{
		EnemyAction* pAction = *it;
		if( pAction->GetType() == Type &&
			!pAction->IsDelete() &&
			!pAction->IsActionEnd() )
		{
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------
//	条件のチェック
//------------------------------------------------------------------------------
bool EnemyState::_CheckCondition( const ENEMY_CONDITION_LIST& ConditionList ) const
{
	for( uint i = 0; i < ConditionList.size(); i++ )
	{
		if( !_CheckCondition( ConditionList[i] ) )
		{
			return false;
		}
	}

	return true;
}
	
//------------------------------------------------------------------------------
//	条件のチェック
//------------------------------------------------------------------------------
bool EnemyState::_CheckCondition( const ENEMY_CONDITION& Condition ) const
{
	bool bRet = false;

	switch( Condition.Type )
	{
	case ENEMY_CONDITION_MOVE_END:			bRet = _IsEndAction( ENEMY_ACTION_MOVE );									break;
	case ENEMY_CONDITION_ROT_END:			bRet = _IsEndAction( ENEMY_ACTION_ROT );									break;
	case ENEMY_CONDITION_ATTACK_END:		bRet = _IsEndAction( ENEMY_ACTION_ATTACK );									break;
	case ENEMY_CONDITION_NOTIFY:			bRet = m_pEnemy->IsNotify( Condition.Name );								break;
	case ENEMY_CONDITION_CHILD_DEFEAT:		bRet = m_pEnemy->IsChildDefeat( Condition.Name );							break;
	case ENEMY_CONDITION_CHILD_ALL_DEFEAT:	bRet = m_pEnemy->IsChildDefeatAll();										break;
	case ENEMY_CONDITION_CHILD_EMPTY:		bRet = m_pEnemy->IsChildEmpty();											break;
	case ENEMY_CONDITION_ANIME_END:			bRet = m_pEnemy->GetSprite()->IsAnimeLast();								break;
	case ENEMY_CONDITION_ANIME_NOTIFY:		bRet = m_pEnemy->GetSprite()->GetAnimeNotify( Condition.Name ) != NULL;		break;
	case ENEMY_CONDITION_FRAME:				bRet = m_StateCount >= Condition.Param;										break;
	case ENEMY_CONDITION_RANDOM:			bRet = m_pEnemy->GetRand().GetRandFloat(100.0f) < Condition.Param;			break;
	case ENEMY_CONDITION_STATE_END:			bRet = !m_NextStateName.IsEmpty();											break;
	case ENEMY_CONDITION_LIFE_LESS:			bRet = m_pEnemy->GetLife() <= Condition.Param;								break;
	case ENEMY_CONDITION_DAMAGE:			bRet = m_pEnemy->IsDamage() == Condition.bFlag;								break;
	case ENEMY_CONDITION_KNOCKBACK:			bRet = m_pEnemy->IsKnockback() == Condition.bFlag;							break;
	case ENEMY_CONDITION_REPULSE:			bRet = m_pEnemy->IsRepulseDamage() == Condition.bFlag;						break;
	case ENEMY_CONDITION_DEAD:				bRet = m_pEnemy->IsDead() == Condition.bFlag;								break;

	case ENEMY_CONDITION_SELF_POS_X:
		{
			float Pos = m_pEnemy->GetPos().x - m_pEnemy->GetWorld()->GetFieldOffset().x;
			bRet = Pos >= Condition.Range[0] && Pos <= Condition.Range[1];
		}
		break;
	case ENEMY_CONDITION_SELF_POS_Y:
		{
			float Pos = m_pEnemy->GetPos().y - m_pEnemy->GetWorld()->GetFieldOffset().y;
			bRet = Pos >= Condition.Range[0] && Pos <= Condition.Range[1];
		}
		break;

	case ENEMY_CONDITION_PLAYER_RANGE:
		{
			Player* pPlayer = m_pEnemy->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer )
			{
				float LenSq = (pPlayer->GetPos() - m_pEnemy->GetPos()).LengthSq();
				bRet = LenSq >= Sq( Condition.Range[0] ) && LenSq <= Sq( Condition.Range[1] );
			}
		}
		break;
	case ENEMY_CONDITION_PLAYER_RANGE_X:
		{
			Player* pPlayer = m_pEnemy->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer )
			{
				float Len = pPlayer->GetPos().x - m_pEnemy->GetPos().x;
				bRet = Len >= Condition.Range[0] && Len <= Condition.Range[1];
			}
		}
		break;
	case ENEMY_CONDITION_PLAYER_RANGE_Y:
		{
			Player* pPlayer = m_pEnemy->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer )
			{
				float Len = pPlayer->GetPos().y - m_pEnemy->GetPos().y;
				bRet = Len >= Condition.Range[0] && Len <= Condition.Range[1];
			}
		}
		break;
	case ENEMY_CONDITION_PLAYER_ANGLE:
		{
			Player* pPlayer = m_pEnemy->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer )
			{
				Angle Ang = (pPlayer->GetPos() - m_pEnemy->GetPos()).GetAngle();
				float Diff = Abs( Angle::Diff( Ang, m_pEnemy->GetRot() ) );
				bRet = Diff >= Condition.Range[0] && Diff <= Condition.Range[1];
			}
		}
		break;
	case ENEMY_CONDITION_PLAYER_ANGLE_REL:
		{
			Player* pPlayer = m_pEnemy->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer )
			{
				Angle Ang = (pPlayer->GetPos() - m_pEnemy->GetPos()).GetAngle();
				float Diff = Angle::Diff( Ang, m_pEnemy->GetRot() );
				bRet = Diff >= Condition.Range[0] && Diff <= Condition.Range[1];
			}
		}
		break;
	case ENEMY_CONDITION_PLAYER_POS_X:
		{
			Player* pPlayer = m_pEnemy->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer )
			{
				float Pos = pPlayer->GetPos().x - m_pEnemy->GetWorld()->GetFieldOffset().x;
				bRet = Pos >= Condition.Range[0] && Pos <= Condition.Range[1];
			}
		}
		break;
	case ENEMY_CONDITION_PLAYER_POS_Y:
		{
			Player* pPlayer = m_pEnemy->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer )
			{
				float Pos = pPlayer->GetPos().y - m_pEnemy->GetWorld()->GetFieldOffset().y;
				bRet = Pos >= Condition.Range[0] && Pos <= Condition.Range[1];
			}
		}
		break;
	case ENEMY_CONDITION_PLAYER_STATE:
		{
			Player* pPlayer = m_pEnemy->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer )
			{
				bRet = pPlayer->IsState( Condition.Name );
			}
		}
		break;
	case ENEMY_CONDITION_PLAYER_STATE_TIME:
		{
			Player* pPlayer = m_pEnemy->GetWorld()->GetPlayerManager()->GetPlayer();
			if( pPlayer )
			{
				bRet = pPlayer->GetStateCount() >= Condition.Param;
			}
		}
		break;
	}

	//	反転
	if( Condition.bNot )
	{
		bRet = !bRet;
	}

	return bRet;
}