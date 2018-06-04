#include "stdafx.h"
#include "SpriteFrameAnime.h"
#include "SpriteFrameAnimeData.h"
#include "SpriteFrameFile.h"
#include "SpriteFrameBone.h"


////////////////////////////////////////////////////////////////////////////////
//
//	スプライトフレームアニメ
//
////////////////////////////////////////////////////////////////////////////////
SpriteFrameAnime::SpriteFrameAnime( void )
{
	m_pFile			= NULL;
	m_pPlayData		= NULL;

	m_FrameCount	= 0.0f;
	m_FrameSpeed	= 1.0f;
	m_IntpCount		= 0;
	m_bPause		= false;
}
SpriteFrameAnime::~SpriteFrameAnime()
{
}

//******************************************************************************
//	初期化
//******************************************************************************
void SpriteFrameAnime::Init( const SpriteFrameFile* pFile )
{
	m_pFile			= pFile;
	m_pPlayData		= NULL;

	m_FrameCount	= 0.0f;
	m_FrameSpeed	= 1.0f;
	m_bPause		= false;

	//	ボーン情報生成
	m_vBoneInfo.clear();
	m_vBoneInfo.resize( m_pFile->GetBoneNum() );

	for( int BoneID = 0; BoneID < m_pFile->GetBoneNum(); BoneID++ )
	{
		BONE_INFO& Info = m_vBoneInfo[BoneID];

		FillArray( Info.aKeyIndex, SPRITE_FRAME_KEY_NUM, -1 );
	}
}

//******************************************************************************
//	更新
//******************************************************************************
void SpriteFrameAnime::Update( void )
{
	if( m_bPause ) return;

	if( m_pPlayData )
	{
		//	終了チェック
		float End = (float)m_pPlayData->GetEndFrame();
		if( m_pPlayData->GetEndFrame() > 0 )
		{
			//	ループ処理
			if( m_pPlayData->IsLoop() && IsLastFrame() )
			{
				m_FrameCount = (float)((m_FrameSpeed < 0.0f)? End : m_pPlayData->GetLoopFrame());

				_ResetBoneKey();
			}
		}

		//	発生物の更新
		_UpdateAppear();

		//	ボーンのキーフレームを更新
		_UpdateBoneKey();

		//	通知の更新
		_UpdateNotify();

		//	フレーム進める
		m_FrameCount = Min( m_FrameCount + m_FrameSpeed, End );
		if( m_IntpCount ) m_IntpCount--;
	}
}

//******************************************************************************
//	再生
//******************************************************************************
void SpriteFrameAnime::Play( Label Name, uint Intp )
{
	bool bPrevPlay = IsPlaying();

	m_pPlayData = m_pFile->GetAnimeData( Name );
	if( m_pPlayData )
	{
		m_FrameCount	= 0.0f;
		m_FrameSpeed	= 1.0f;
		m_IntpCount		= Intp;
		m_bPause		= false;
		m_PlayName		= Name;
		
		//	未再生状態から再生する場合
		if( !bPrevPlay )
		{
			_ResetBoneParam();
		}

		//	ボーンのキーフレームインデクスを初期化
		_ResetBoneKey();

		m_NotifyMap.clear();
	}
}

//******************************************************************************
//	停止
//******************************************************************************
void SpriteFrameAnime::Stop( void )
{
	m_pPlayData		= NULL;
	m_bPause		= false;
	m_FrameSpeed	= 1.0f;

	m_PlayName.Clear();	
	m_NotifyMap.clear();
}

//******************************************************************************
//	最終フレームまで再生したか？
//******************************************************************************
bool SpriteFrameAnime::IsLastFrame( void ) const
{
	if( !m_pPlayData ) return false;

	if( m_FrameSpeed < 0.0f )
	{
		return _GetFrameCount() < 0.0f;
	}
	else
	{
		return _GetFrameCount() >= m_pPlayData->GetEndFrame();
	}
}

//******************************************************************************
//	ループフレーム以降かどうか？
//******************************************************************************
bool SpriteFrameAnime::IsAnimeLoopFrame( void ) const
{
	if( !m_pPlayData ) return false;

	if( m_FrameSpeed < 0.0f )
	{
		return _GetFrameCount() < m_pPlayData->GetLoopFrame();
	}
	else
	{
		return _GetFrameCount() >= m_pPlayData->GetLoopFrame();
	}
}

//******************************************************************************
//	ボーンの行列を取得
//******************************************************************************
void SpriteFrameAnime::GetBoneMatrix( Matrix3& Mat, uint BoneID ) const
{
	_ASSERT( BoneID < m_vBoneInfo.size() );

	const BONE_INFO& Info = m_vBoneInfo[BoneID];
	CalcMatrix( Mat, Info.Pos, Info.Rot, Info.Scale );
}

//******************************************************************************
//	ボーンの中心位置を取得
//******************************************************************************
Point SpriteFrameAnime::GetBoneCenterPos( uint BoneID ) const
{
	_ASSERT( BoneID < m_vBoneInfo.size() );

	const BONE_INFO& Info = m_vBoneInfo[BoneID];
	return Info.Center;
}

//******************************************************************************
//	ボーンの色を取得
//******************************************************************************
Color SpriteFrameAnime::GetBoneColor( uint BoneID ) const
{
	_ASSERT( BoneID < m_vBoneInfo.size() );

	const BONE_INFO& Info = m_vBoneInfo[BoneID];
	int KeyIndex	= Info.aKeyIndex[SPRITE_FRAME_KEY_COLOR];
	if( KeyIndex < 0 ) return m_pFile->GetBoneParam(BoneID)->Col;

	return Info.Col;
}

//******************************************************************************
//	ボーンのスプライトインデクスを取得
//******************************************************************************
uint SpriteFrameAnime::GetBoneSpriteIndex( uint BoneID ) const
{
	_ASSERT( BoneID < m_vBoneInfo.size() );

	const BONE_INFO& Info = m_vBoneInfo[BoneID];
	int KeyIndex	= Info.aKeyIndex[SPRITE_FRAME_KEY_INDEX];
	if( KeyIndex < 0 ) return 0;

	const SPRITE_FRAME_KEY* pKey = m_pPlayData->GetKey( BoneID, SPRITE_FRAME_KEY_INDEX, KeyIndex );

	//	最初のキー以前の場合
	int Frame = _GetFrameCount();
	if( Frame < pKey->Frame ) return 0;

	//	計算して出す
	return (Frame - pKey->Frame) / pKey->Index.Interval * pKey->Index.Step + pKey->Index.Start;
}

//******************************************************************************
//	ボーンのピース名を取得
//******************************************************************************
Label SpriteFrameAnime::GetBonePieceName( uint BoneID ) const
{
	_ASSERT( BoneID < m_vBoneInfo.size() );

	const BONE_INFO& Info = m_vBoneInfo[BoneID];
	int KeyIndex	= Info.aKeyIndex[SPRITE_FRAME_KEY_PIECE];
	if( KeyIndex < 0 ) return Label();

	const SPRITE_FRAME_KEY* pKey = m_pPlayData->GetKey( BoneID, SPRITE_FRAME_KEY_PIECE, KeyIndex );

	//	最初のキー以前の場合
	if( _GetFrameCount() < pKey->Frame ) return Label();

	return Label( pKey->Name );
}

//******************************************************************************
//	ボーンの文字列を取得
//******************************************************************************
pstr SpriteFrameAnime::GetBoneText( uint BoneID ) const
{
	_ASSERT( BoneID < m_vBoneInfo.size() );

	const BONE_INFO& Info = m_vBoneInfo[BoneID];
	int KeyIndex	= Info.aKeyIndex[SPRITE_FRAME_KEY_TEXT];
	if( KeyIndex < 0 ) return NULL;

	const SPRITE_FRAME_KEY* pKey = m_pPlayData->GetKey( BoneID, SPRITE_FRAME_KEY_TEXT, KeyIndex );

	//	最初のキー以前の場合
	if( _GetFrameCount() < pKey->Frame ) return NULL;

	return pKey->Str;
}

//******************************************************************************
//	通知メッセージ取得
//******************************************************************************
pstr SpriteFrameAnime::GetNotify( Label Key ) const
{
	MapNotify::const_iterator it = m_NotifyMap.find( Key );
	if( it != m_NotifyMap.end() ) return (it->second).c_str();

	return NULL;
}

//******************************************************************************
//	ボーンに発生物を適用する
//******************************************************************************
void SpriteFrameAnime::ApplyAppear( SpriteFrameBone* pBone )
{
	_ASSERT( pBone && pBone->GetID() < (int)m_vBoneInfo.size() );

	BONE_INFO& Info = m_vBoneInfo[pBone->GetID()];

	for( uint i = 0; i < Info.vAppear.size(); i++ )
	{
		pBone->AddAppear( Info.vAppear[i] );
	}
	Info.vAppear.clear();
}

//******************************************************************************
//	マトリクスを計算
//******************************************************************************
void SpriteFrameAnime::CalcMatrix( Matrix3& Mat, const Point& Pos, float Rot, const Point& Scale )
{
	//	拡縮
	Mat = Matrix3::Scale( Scale );

	//	回転
	if( Rot != 0.0f ) Mat *= Matrix3::RotZ( Rot );

	//	位置
	if( !Pos.IsZero() ) Mat *= Matrix3::Trans( Pos );
}


//------------------------------------------------------------------------------
//	ボーンのパラメータを初期化する
//------------------------------------------------------------------------------
void SpriteFrameAnime::_ResetBoneParam( void )
{
	for( int BoneID = 0; BoneID < m_pFile->GetBoneNum(); BoneID++ )
	{
		BONE_INFO& Info = m_vBoneInfo[BoneID];

		//	状態を初期化
		const SPRITE_FRAME_BONE* pBone = m_pFile->GetBoneParam(BoneID);
		Info.Pos	= pBone->Pos;
		Info.Rot	= pBone->Rot;
		Info.Center	= pBone->Center;
		Info.Scale	= pBone->Scale;
		Info.Col	= pBone->Col;
	}
}

//------------------------------------------------------------------------------
//	ボーンのキーフレームを初期化する
//------------------------------------------------------------------------------
void SpriteFrameAnime::_ResetBoneKey( void )
{
	for( int BoneID = 0; BoneID < m_pFile->GetBoneNum(); BoneID++ )
	{
		BONE_INFO& Info = m_vBoneInfo[BoneID];

		Info.AppearIndex = (m_FrameSpeed < 0.0f)? m_pPlayData->GetAppearNum(BoneID)-1 : 0;

		//	ボーンインデクスを初期化
		if( m_pPlayData->IsBoneEmpty( BoneID ) )
		{
			//	ボーンがない
			FillArray( Info.aKeyIndex, SPRITE_FRAME_KEY_NUM, -1 );
			continue;
		}

		for( uint KeyType = 0; KeyType < SPRITE_FRAME_KEY_NUM; KeyType++ )
		{
			//	キーがない
			if( m_pPlayData->IsKeyEmpty( BoneID, KeyType ) )
			{
				Info.aKeyIndex[KeyType] = -1;
			}
			else
			{
				int Index = 0;
				while( true )
				{
					const SPRITE_FRAME_KEY* pKey = m_pPlayData->GetKey( BoneID, KeyType, Index + 1 );
					if( !pKey || _GetFrameCount() < pKey->Frame )
					{
						break;
					}
					Index++;
				}

				Info.aKeyIndex[KeyType] = Index;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	ボーンのキーフレームを更新する
//------------------------------------------------------------------------------
void SpriteFrameAnime::_UpdateBoneKey( void )
{
	for( int BoneID = 0; BoneID < m_pFile->GetBoneNum(); BoneID++ )
	{
		BONE_INFO& Info = m_vBoneInfo[BoneID];

		//	ボーンがない
		if( m_pPlayData->IsBoneEmpty( BoneID ) )
		{
			continue;
		}

		for( uint KeyType = 0; KeyType < SPRITE_FRAME_KEY_NUM; KeyType++ )
		{
			//	キーがない
			if( Info.aKeyIndex[KeyType] < 0 )
			{
				continue;
			}

			//	次のキーに移行する
			int NextIndex = Info.aKeyIndex[KeyType] + 1;
			const SPRITE_FRAME_KEY* pKey = m_pPlayData->GetKey( BoneID, KeyType, NextIndex );
			if( pKey && _GetFrameCount() >= pKey->Frame )
			{
				Info.aKeyIndex[KeyType] = NextIndex;
			}
		}

		//	ボーンの状態を更新
		if( m_IntpCount )
		{
			//	補間
			float Coef = 1.0f / (float)m_IntpCount;

			Info.Pos	= Lerp( Info.Pos,		_GetKeyFloat2( BoneID, SPRITE_FRAME_KEY_POS ), Coef );
			Info.Center	= Lerp( Info.Center,	_GetKeyFloat2( BoneID, SPRITE_FRAME_KEY_CENTER ), Coef );
			Info.Scale	= Lerp( Info.Scale,		_GetKeyFloat2( BoneID, SPRITE_FRAME_KEY_SCALE ), Coef );
			Info.Rot	= Angle::Lerp( Info.Rot, _GetKeyRot( BoneID ), Coef );
			Info.Col	= Color::Lerp( Info.Col, _GetKeyColor( BoneID ), Coef );
		}
		else
		{
			//	設定
			Info.Pos	= _GetKeyFloat2( BoneID, SPRITE_FRAME_KEY_POS );
			Info.Center	= _GetKeyFloat2( BoneID, SPRITE_FRAME_KEY_CENTER );
			Info.Scale	= _GetKeyFloat2( BoneID, SPRITE_FRAME_KEY_SCALE );
			Info.Rot	= _GetKeyRot( BoneID );
			Info.Col	= _GetKeyColor( BoneID );
		}
	}
}

//------------------------------------------------------------------------------
//	通知の更新
//------------------------------------------------------------------------------
void SpriteFrameAnime::_UpdateNotify( void )
{
	m_NotifyMap.clear();

	for( uint Index = 0; Index < m_pPlayData->GetNotifyNum(); Index++ )
	{
		const SPRITE_FRAME_NOTIFY* pNotify = m_pPlayData->GetNotify( Index );
		if( pNotify && pNotify->Frame == _GetFrameCount() )
		{
			m_NotifyMap.insert( MapNotify::value_type( pNotify->Key, pNotify->Value ) );
		}
	}
}

//------------------------------------------------------------------------------
//	発生物の更新
//------------------------------------------------------------------------------
void SpriteFrameAnime::_UpdateAppear( void )
{
	for( int BoneID = 0; BoneID < m_pFile->GetBoneNum(); BoneID++ )
	{
		BONE_INFO& Info = m_vBoneInfo[BoneID];

		Info.vAppear.clear();

		//	パラメータ取得
		int Max = (m_FrameSpeed >= 0.0f)? (int)m_pPlayData->GetAppearNum( BoneID ) : -1;
		int Add = (m_FrameSpeed >= 0.0f)? 1 : -1;
		for( int Index = Info.AppearIndex; Index != Max; Index += Add )
		{
			const SPRITE_FRAME_APPEAR* pAppear = m_pPlayData->GetAppear( BoneID, Index );
			if( pAppear && pAppear->Frame == _GetFrameCount() )
			{
				Info.vAppear.push_back( pAppear );
				Info.AppearIndex = Index + Add;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	現在のキーフレームのフロート値を取得する
//------------------------------------------------------------------------------
Point SpriteFrameAnime::_GetKeyFloat2( uint BoneID, uint KeyType ) const
{
	_ASSERT( BoneID < m_vBoneInfo.size() );

	Point Ret;
		
	switch( KeyType )
	{
	case SPRITE_FRAME_KEY_POS:		Ret = m_pFile->GetBoneParam(BoneID)->Pos;		break;
	case SPRITE_FRAME_KEY_SCALE:	Ret = m_pFile->GetBoneParam(BoneID)->Scale;		break;
	case SPRITE_FRAME_KEY_CENTER:	Ret = m_pFile->GetBoneParam(BoneID)->Center;	break;
	}

	const BONE_INFO& Info = m_vBoneInfo[BoneID];
	int KeyIndex	= Info.aKeyIndex[KeyType];
	if( KeyIndex < 0 ) return Ret;

	const SPRITE_FRAME_KEY* pKey = m_pPlayData->GetKey( BoneID, KeyType, KeyIndex );

	//	最初のキー以前の場合
	int Frame = _GetFrameCount();
	if( Frame < pKey->Frame ) return Ret;

	//	固定値の場合
	if( pKey->Intp == SPRITE_FRAME_INTP_FIX ) return Point( pKey->Float2 );

	const SPRITE_FRAME_KEY* pNextKey = m_pPlayData->GetKey( BoneID, KeyType, KeyIndex + 1 );

	//	最終キー以降の場合
	if( !pNextKey ) return Point( pKey->Float2 );

	//	補間する
	return Lerp( Point(pKey->Float2), Point(pNextKey->Float2), Coef(m_FrameCount, (float)pKey->Frame, (float)pNextKey->Frame) );
}

//------------------------------------------------------------------------------
//	現在のキーフレームの角度
//------------------------------------------------------------------------------
Angle SpriteFrameAnime::_GetKeyRot( uint BoneID ) const
{
	_ASSERT( BoneID < m_vBoneInfo.size() );

	Angle Rot = m_pFile->GetBoneParam(BoneID)->Rot;

	const BONE_INFO& Info = m_vBoneInfo[BoneID];
	int KeyIndex	= Info.aKeyIndex[SPRITE_FRAME_KEY_ROT];
	if( KeyIndex < 0 ) return Rot;

	const SPRITE_FRAME_KEY* pKey = m_pPlayData->GetKey( BoneID, SPRITE_FRAME_KEY_ROT, KeyIndex );

	//	最初のキー以前の場合
	int Frame = _GetFrameCount();
	if( Frame < pKey->Frame ) return Rot;

	//	固定キーの場合
	if( pKey->Intp == SPRITE_FRAME_INTP_FIX ) return Angle( pKey->Rot.Rot );

	const SPRITE_FRAME_KEY* pNextKey = m_pPlayData->GetKey( BoneID, SPRITE_FRAME_KEY_ROT, KeyIndex + 1 );

	//	最終キー以降の場合
	if( !pNextKey ) return Angle( pKey->Rot.Rot );

	//	補間する
	if( pNextKey->Rot.bLoop )
	{
		return Angle::Lerp( pKey->Rot.Rot, pNextKey->Rot.Rot, Coef(m_FrameCount, (float)pKey->Frame, (float)pNextKey->Frame) );
	}
	else
	{
		return Angle( Lerp<float>( pKey->Rot.Rot, pNextKey->Rot.Rot, Coef(m_FrameCount, (float)pKey->Frame, (float)pNextKey->Frame) ) );
	}
}

//------------------------------------------------------------------------------
//	現在のキーフレームの色を取得する
//------------------------------------------------------------------------------
Color SpriteFrameAnime::_GetKeyColor( uint BoneID ) const
{
	_ASSERT( BoneID < m_vBoneInfo.size() );

	Color Col = m_pFile->GetBoneParam(BoneID)->Col;

	const BONE_INFO& Info = m_vBoneInfo[BoneID];
	int KeyIndex	= Info.aKeyIndex[SPRITE_FRAME_KEY_COLOR];
	if( KeyIndex < 0 ) return Col;

	const SPRITE_FRAME_KEY* pKey = m_pPlayData->GetKey( BoneID, SPRITE_FRAME_KEY_COLOR, KeyIndex );

	//	最初のキー以前の場合
	int Frame = _GetFrameCount();
	if( Frame < pKey->Frame ) return Col;

	//	固定値の場合
	if( pKey->Intp == SPRITE_FRAME_INTP_FIX ) return Color( pKey->Col );

	const SPRITE_FRAME_KEY* pNextKey = m_pPlayData->GetKey( BoneID, SPRITE_FRAME_KEY_COLOR, KeyIndex + 1 );

	//	最終キー以降の場合
	if( !pNextKey ) return Color( pKey->Col );

	//	補間する
	return Color::Lerp( pKey->Col, pNextKey->Col, Coef(m_FrameCount, (float)pKey->Frame, (float)pNextKey->Frame) );
}