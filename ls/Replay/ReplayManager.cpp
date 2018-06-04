#include "stdafx.h"
#include "ReplayManager.h"
#include "ReplayData.h"

#include "World/World.h"

namespace
{
	pstr			PATH_REPLAY_DIR					= "Replay/";
	pstr			PATH_REPLAY_SAVE_DIR_FIND		= "%s/*.rep";
	pstr			PATH_REPLAY_SAVE_DIR_FORMAT		= "%s/%d %s.rep";
}

////////////////////////////////////////////////////////////////////////////////
//
//	リプレイ管理
//
////////////////////////////////////////////////////////////////////////////////
ReplayManager::ReplayManager( World* pWorld )
{
	m_pLoadData		= NULL;
	m_pRecordData	= NULL;

	m_FrameOffset	= 0;
	m_PlayID		= 0;
}
ReplayManager::~ReplayManager()
{
	SafeDelete( m_pLoadData );
	SafeDelete( m_pRecordData );
}

//******************************************************************************
//	記録モードで初期化
//******************************************************************************
void ReplayManager::InitRecord( void )
{
	m_pRecordData = new ReplayData;
	MEM_CHECK( m_pRecordData );
}

//******************************************************************************
//	再生モードで初期化
//******************************************************************************
bool ReplayManager::InitLoad( pstr pPath, int Stage )
{
	m_pLoadData = new ReplayData;
	MEM_CHECK( m_pLoadData );

	if( !m_pLoadData->Load( pPath ) ) return false;

	//	開始オフセットを計算
	m_FrameOffset = 0;
	for( uint i = 0; i < m_pLoadData->GetHeader().EventNum; i++ )
	{
		const REPLAY_EVENT& Event = m_pLoadData->GetEvent(i);
		if( Event.Type == REPLAY_EVENT_CHANGE_STAGE &&
			Event.ParamInt[0] == Stage )
		{
			m_FrameOffset = Event.Frame;
		}
	}

	//	プレイIDを取得
	m_PlayID = m_pLoadData->GetHeader().PlayID;

	return true;
}

//******************************************************************************
//	リプレイデータを保存
//******************************************************************************
bool ReplayManager::Save( const SAVEDATA_SCORE& Score, pstr pDirName, int SaveMax )
{
#if DEMO_VER == DEMO_PLAY_VER
	//	デモモードでは保存しない
	return false;
#endif

	if( !m_pRecordData ) return false;

	//	保存ディレクトリを生成
	::CreateDirectory( PATH_REPLAY_DIR, NULL );
	::CreateDirectory( pDirName, NULL );

	//	旧いデータの削除
	if( SaveMax > 0 )
	{
		//	保存ディレクトリ内のファイルを取得
		String256 FindPath;
		FindPath.Format( PATH_REPLAY_SAVE_DIR_FIND, pDirName );
		Directory Dir( FindPath.c_str(), Directory::FILTER_FILE );
		if( Dir.GetChildNum() >= SaveMax )
		{
			//	保存数以上あった場合、最も古いものを消す
			int DelIndex	= -1;
			int MinPlayID	= INT_MAX;
			for( int i = 0; i < Dir.GetChildNum(); i++ )
			{
				int PlayID = 0;
				sscanf( Dir.GetChildTitle(i).c_str(), "%d ", &PlayID );

				if( PlayID < MinPlayID )
				{
					MinPlayID	= PlayID;
					DelIndex	= i;
				}
			}

			if( DelIndex >= 0 )
			{
				::DeleteFile( Dir.GetChildPath(DelIndex) );
			}
		}
	}

	//	日時の差し替え
	String256 FileDate = Score.DateTime.c_str();
	FileDate.Replace( "/", "_" );
	FileDate.Replace( ":", "_" );

	//	保存パス
	String256 SavePath;
	SavePath.Format( PATH_REPLAY_SAVE_DIR_FORMAT, pDirName, Score.PlayID, FileDate.c_str() );

	return m_pRecordData->Save( SavePath.c_str(), Score );
}

//******************************************************************************
//	操作をマニュアルに切り替える
//******************************************************************************
void ReplayManager::ChangeManual( uint FrameCount )
{
	if( !m_pLoadData || m_pRecordData ) return;

	//	移し変え
	m_pRecordData = m_pLoadData;
	m_pLoadData = NULL;

	//	記録データの反映
	m_pRecordData->ChangeManual( FrameCount );
}

//******************************************************************************
//	再生データの記録
//******************************************************************************
void ReplayManager::RecordData( byte Data )
{
	if( !m_pRecordData ) return;
	
	m_pRecordData->AddData( Data );
}

//******************************************************************************
//	ステージ変更の通知
//******************************************************************************
void ReplayManager::RecordChangeStage( uint FrameCount, int Stage )
{
	if( !m_pRecordData ) return;
	
	//	イベントの登録
	REPLAY_EVENT Event;
	Event.Frame			= FrameCount + m_FrameOffset;
	Event.Type			= REPLAY_EVENT_CHANGE_STAGE;
	Event.ParamInt[0]	= Stage;

	m_pRecordData->AddEvent( Event );
}

//******************************************************************************
//	リプレイが終了しているかどうか
//******************************************************************************
bool ReplayManager::IsReplayOver( uint FrameCount ) const
{
	if( m_pLoadData )
	{
		return FrameCount >= m_pLoadData->GetHeader().FrameCount;
	}
	return true;
}

//******************************************************************************
//	再生データを取得
//******************************************************************************
byte ReplayManager::GetReplayData( uint FrameCount ) const
{
	FrameCount += m_FrameOffset;
	if( m_pLoadData && FrameCount < m_pLoadData->GetHeader().FrameCount )
	{
		return m_pLoadData->GetData( FrameCount );
	}
	return 0;
}

//******************************************************************************
//	コンティニュー回数を得る
//******************************************************************************
int ReplayManager::GetContinueCount( void ) const
{
	if( m_pLoadData )
	{
		return m_pLoadData->GetHeader().ContinueCount;
	}
	return 0;
}