#include "stdafx.h"
#include "ReplayData.h"


////////////////////////////////////////////////////////////////////////////////
//
//	リプレイデータ
//
////////////////////////////////////////////////////////////////////////////////
ReplayData::ReplayData( void )
{
	ZeroStruct( m_Header );
}
ReplayData::~ReplayData()
{
	
}

//******************************************************************************
//	リプレイデータを読み込む
//******************************************************************************
bool ReplayData::Load( pstr pPath )
{
	//	初期化
	Reset();

	//	ファイルオープン
	FileHandle	File;
	if( !File.Open( pPath, "rb" ) ) return false;

	//	ヘッダ読み込む
	if( fread( &m_Header, sizeof(REPLAY_HEADER), 1, File ) != 1 ) return false;

	//	フォーマット・バージョン取得
	if( m_Header.Format != REPLAY_FORMAT || m_Header.Version != REPLAY_VERSION ) return false;

	//	イベント読み込む
	if( m_Header.EventNum > 0 )
	{
		m_vEvent.resize( m_Header.EventNum );
		if( fread( &m_vEvent[0], sizeof(REPLAY_EVENT), m_Header.EventNum, File ) != m_Header.EventNum ) return false;
	}

	//	データ読み込む
	if( m_Header.FrameCount > 0 )
	{
		m_vData.resize( m_Header.FrameCount );
		if( fread( &m_vData[0], sizeof(byte), m_Header.FrameCount, File ) != m_Header.FrameCount ) return false;
	}

	return true;
}

//******************************************************************************
//	ヘッダのみを開く
//******************************************************************************
bool ReplayData::LoadHeader( pstr pPath )
{
	//	初期化
	Reset();

	//	ファイルオープン
	FileHandle	File;
	if( !File.Open( pPath, "rb" ) ) return false;

	//	ヘッダ読み込む
	if( fread( &m_Header, sizeof(REPLAY_HEADER), 1, File ) != 1 ) return false;

	//	フォーマット・バージョン取得
	if( m_Header.Format != REPLAY_FORMAT || m_Header.Version != REPLAY_VERSION ) return false;

	return true;
}

//******************************************************************************
//	リプレイデータを保存
//******************************************************************************
bool ReplayData::Save( pstr pPath, const SAVEDATA_SCORE& Score )
{
	//	ヘッダデータ設定
	m_Header.Format			= REPLAY_FORMAT;
	m_Header.Version		= REPLAY_VERSION;
	strncpy( m_Header.DateTime, Score.DateTime.c_str(), sizeof(m_Header.DateTime) );

	m_Header.FrameCount		= m_vData.size();
	m_Header.EventNum		= m_vEvent.size();

	m_Header.PlayID			= Score.PlayID;
	m_Header.Arrival		= Score.Arrival;
	m_Header.Score			= Score.Score;
	m_Header.ContinueCount	= Score.ContinueCount;

	//	ファイルオープン
	FileHandle	File;
	if( !File.Open( pPath, "wb" ) ) return false;

	//	ヘッダ書き込む
	if( fwrite( &m_Header, sizeof(REPLAY_HEADER), 1, File ) != 1 ) return false;

	//	イベント書き込む
	if( m_Header.EventNum > 0 )
	{
		if( fwrite( &m_vEvent[0], sizeof(REPLAY_EVENT), m_Header.EventNum, File ) != m_Header.EventNum ) return false;
	}

	//	データ書き込む
	if( m_Header.FrameCount > 0 )
	{
		if( fwrite( &m_vData[0], sizeof(byte), m_Header.FrameCount, File ) != m_Header.FrameCount ) return false;
	}

	return true;
}

//******************************************************************************
//	リセット
//******************************************************************************
void ReplayData::Reset( void )
{
	ZeroStruct( m_Header );

	m_vData.clear();
	m_vEvent.clear();
}

//******************************************************************************
//	イベントの追加
//******************************************************************************
void ReplayData::AddEvent( const REPLAY_EVENT& Event )
{
	m_vEvent.push_back( Event );
}

//******************************************************************************
//	データの追加
//******************************************************************************
void ReplayData::AddData( byte Data )
{
	m_vData.push_back( Data );
}

//******************************************************************************
//	マニュアル操作へ切り替え
//******************************************************************************
void ReplayData::ChangeManual( uint FrameCount )
{
	//	追記回数増加
	m_Header.OverwriteCount++;

	//	イベント
	for( VecEvent::iterator it = m_vEvent.begin(); it != m_vEvent.end(); )
	{
		if( it->Frame >= FrameCount )
		{
			it = m_vEvent.erase( it );
		}
		else
		{
			++it;
		}
	}

	//	データ
	if( FrameCount < m_vData.size() )
	{
		m_vData.resize( FrameCount );
	}
}