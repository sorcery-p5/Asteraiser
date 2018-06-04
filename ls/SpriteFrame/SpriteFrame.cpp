#include "stdafx.h"
#include "SpriteFrame.h"
#include "SpriteFrameBone.h"
#include "SpriteFrameFile.h"
#include "SpriteFrameAnime.h"


////////////////////////////////////////////////////////////////////////////////
//
//	スプライトフレーム
//
////////////////////////////////////////////////////////////////////////////////
SpriteFrame::SpriteFrame( void )
{
	m_pAnime	= NULL;
	m_pFile		= NULL;

	m_Color.SetWhite();
	m_bVisible	= true;
}

SpriteFrame::~SpriteFrame()
{
	SafeDelete( m_pAnime );
	DeleteVec( m_vBone );
}

//*****************************************************************************
//	初期化
//*****************************************************************************
void SpriteFrame::Init( const SpriteFrameFile* pFile, const Matrix3& BaseMat )
{
	_ASSERT( pFile );
	m_pFile = pFile;

	//	ボーン生成
	for( int i = 0; i < pFile->GetBoneNum(); i++ )
	{
		SpriteFrameBone* pBone = _CreateBone( i );
		m_vBone.push_back( pBone );
	}
	
	//	アニメ生成
	if( pFile->HasAnime() )
	{
		m_pAnime = new SpriteFrameAnime;
		MEM_CHECK( m_pAnime );
		
		m_pAnime->Init( pFile );
	}

	//	ワールド行列の計算
	if( !m_vBone.empty() )
	{
		_CalcWorldMatrix( m_vBone.front(), BaseMat );
	}

	//	常駐エフェクト・サウンドを初期化
	for( VecBone::iterator it = m_vBone.begin(); it != m_vBone.end(); ++it )
	{
		(*it)->InitResidentAppear();
	}

	m_BaseMat = BaseMat;
}

//******************************************************************************
//	更新
//******************************************************************************
void SpriteFrame::Update( const Matrix3* pBaseMat )
{
	bool bAnime = false;

	//	アニメ更新
	if( m_pAnime )
	{
		m_pAnime->Update();

		if( m_pAnime->IsPlaying() )
		{
			//	ボーンのローカル行列を更新
			for( VecBone::iterator it = m_vBone.begin(); it != m_vBone.end(); ++it )
			{
				(*it)->ApplyAnime( m_pAnime );
			}

			bAnime = true;
		}
	}

	//	ワールド行列の計算
	if( bAnime || pBaseMat )
	{
		if( pBaseMat ) m_BaseMat = *pBaseMat;

		if( !m_vBone.empty() )
		{
			_CalcWorldMatrix( m_vBone.front(), m_BaseMat );
		}
	}
	
	//	ボーン更新
	for( VecBone::iterator it = m_vBone.begin(); it != m_vBone.end(); ++it )
	{
		(*it)->Update();
		
		//	発生物
		if( m_pAnime )
		{
			m_pAnime->ApplyAppear( *it );
		}
	}
}

//******************************************************************************
//	描画
//******************************************************************************
void SpriteFrame::Draw( const Matrix3& OffsetMat )
{
	for( VecBone::iterator it = m_vBone.begin(); it != m_vBone.end(); ++it )
	{
		(*it)->Draw( OffsetMat );
	}	
}

//******************************************************************************
//	アニメの変更
//******************************************************************************
void SpriteFrame::ChangeAnime( Label Name, uint Intp )
{
	SafePtr(m_pAnime)->Play( Name, Intp );

	//	発生物を消去
	for( VecBone::iterator it = m_vBone.begin(); it != m_vBone.end(); ++it )
	{
		(*it)->ResetAppear();
	}
}

//******************************************************************************
//	アニメを初期状態に戻す
//******************************************************************************
void SpriteFrame::ResetAnime( void )
{
	if( m_pAnime ) m_pAnime->Stop();

	//	ボーン更新
	for( VecBone::iterator it = m_vBone.begin(); it != m_vBone.end(); ++it )
	{
		(*it)->ResetAnime();
	}
	
	//	ワールド行列再計算
	if( !m_vBone.empty() ) _CalcWorldMatrix( m_vBone.front(), m_BaseMat );
}

//******************************************************************************
//	アニメを一時停止
//******************************************************************************
void SpriteFrame::Pause( bool bPause )
{
	if( m_pAnime ) m_pAnime->Pause( bPause );

	for( VecBone::iterator it = m_vBone.begin(); it != m_vBone.end(); ++it )
	{
		(*it)->Pause( bPause );
	}
}

//******************************************************************************
//	アニメの速度変更
//******************************************************************************
void SpriteFrame::SetAnimeSpeed( float Speed )
{
	if( m_pAnime ) m_pAnime->SetFrameSpeed( Speed );
}

//******************************************************************************
//	アニメ再生中かどうか
//******************************************************************************
bool SpriteFrame::IsAnimePlaying( void ) const
{
	if( !m_pAnime ) return false;

	return m_pAnime->IsPlaying();
}

//******************************************************************************
//	アニメ終了フレームに達したかどうか
//******************************************************************************
bool SpriteFrame::IsAnimeLast( void ) const
{
	if( !m_pAnime ) return true;

	return m_pAnime->IsLastFrame();
}

//******************************************************************************
//	アニメがループフレーム以降かどうか
//******************************************************************************
bool SpriteFrame::IsAnimeLoopFrame( void ) const
{
	if( !m_pAnime ) return false;

	return m_pAnime->IsAnimeLoopFrame();
}

//******************************************************************************
//	再生中のアニメ名を取得
//******************************************************************************
Label SpriteFrame::GetAnimeName( void ) const
{
	if( !m_pAnime ) return Label();

	return m_pAnime->GetAnimeName();
}

//******************************************************************************
//	アニメ通知を取得
//******************************************************************************
pstr SpriteFrame::GetAnimeNotify( Label Key ) const
{
	if( !m_pAnime ) return NULL;

	return m_pAnime->GetNotify( Key );
}

//******************************************************************************
//	指定のアニメがあるか
//******************************************************************************
bool SpriteFrame::HasAnime( Label Name ) const
{
	if( !m_pAnime ) return false;

	return m_pFile->GetAnimeData( Name ) != NULL;
}

//******************************************************************************
//	指定のアニメがあるか
//******************************************************************************
int SpriteFrame::GetAnimeNum( void ) const
{
	if( !m_pAnime ) return 0;

	return m_pFile->GetAnimeNum();
}

//******************************************************************************
//	インデクスからアニメ名を得る
//******************************************************************************
Label SpriteFrame::GetAnimeName( int Index ) const
{
	if( !m_pAnime || Index >= GetAnimeNum() ) return Label();

	return m_pFile->GetAnimeName( Index );
}

//******************************************************************************
//	ボーンを検索
//******************************************************************************
int SpriteFrame::FindBone( Label Name ) const
{
	_ASSERT( m_pFile );

	return m_pFile->FindBoneIndex( Name );
}

//******************************************************************************
//	ボーンのマトリクスを取得(インデクス)
//******************************************************************************
const Matrix3& SpriteFrame::GetBoneMatrix( int Index ) const
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	return m_vBone[Index]->GetWorldMatrix();
}

//******************************************************************************
//	ボーンのマトリクスを取得(名前)
//******************************************************************************
const Matrix3* SpriteFrame::GetBoneMatrix( Label Name ) const
{
	_ASSERT( m_pFile );

	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) return &GetBoneMatrix( Index );

	return NULL;
}

//******************************************************************************
//	ボーンのローカルマトリクスを取得(インデクス)
//******************************************************************************
const Matrix3& SpriteFrame::GetBoneLocalMatrix( int Index ) const
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	return m_vBone[Index]->GetLocalMatrix();
}
	
//******************************************************************************
//	ボーンのローカルマトリクスを取得(名前)
//******************************************************************************
const Matrix3* SpriteFrame::GetBoneLocalMatrix( Label Name ) const
{
	_ASSERT( m_pFile );

	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) return &GetBoneLocalMatrix( Index );

	return NULL;
}

//******************************************************************************
//	ボーンの色を取得(インデクス)
//******************************************************************************
Color SpriteFrame::GetBoneColor( int Index ) const
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	return m_vBone[Index]->GetColor( m_pAnime );
}
	
//******************************************************************************
//	ボーンの色を取得(名前)
//******************************************************************************
Color SpriteFrame::GetBoneColor( Label Name ) const
{
	_ASSERT( m_pFile );

	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) return GetBoneColor( Index );

	return Color();
}

//******************************************************************************
//	ボーンの矩形を取得
//******************************************************************************
Rect SpriteFrame::GetBoneRect( Label Name ) const
{
	_ASSERT( m_pFile );

	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) return m_vBone[Index]->GetRect();

	return Rect(); 
}

//******************************************************************************
//	当たり判定情報を取得(インデクス)
//******************************************************************************
SPRITE_FRAME_COLLIDE SpriteFrame::GetCollide( int Index ) const
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	_ASSERT( m_pFile );

	const SPRITE_FRAME_COLLIDE* pCollide = m_pFile->GetCollide( Index );
	if( pCollide ) return *pCollide;

	//	スプライトからサイズ取得.
	Rect Rc	= m_vBone[Index]->GetRect();
	float Rad	= m_vBone[Index]->GetRadius();

	SPRITE_FRAME_COLLIDE Collide;
	ZeroStruct( Collide );

	Collide.Type		=	!Rc.IsZero()?	SPRITE_FRAME_COLLIDE_RECT : 
							Rad != 0.0f?	SPRITE_FRAME_COLLIDE_POINT :
											SPRITE_FRAME_COLLIDE_EMPTY;

	if( Collide.Type == SPRITE_FRAME_COLLIDE_RECT )
	{
		Collide.Rect.L = Rc.l;
		Collide.Rect.T = Rc.t;
		Collide.Rect.R = Rc.r;
		Collide.Rect.B = Rc.b;
	}
	if( Collide.Type == SPRITE_FRAME_COLLIDE_POINT )
	{
		Collide.Point.Radius = Rad;
	}

	return Collide;
}

//******************************************************************************
//	描画の設定
//******************************************************************************
void SpriteFrame::SetVisible( bool bVisible )
{
	m_bVisible = bVisible;
	
	for( VecBone::iterator it = m_vBone.begin(); it != m_vBone.end(); ++it )
	{
		(*it)->UpdateVisible();
	}
}

//******************************************************************************
//	ボーンの描画設定を変更（インデクス）
//******************************************************************************
void SpriteFrame::SetBoneVisible( int Index, bool bVisible )
{	
	m_vBone[Index]->SetVisible( bVisible );
}

//******************************************************************************
//	ボーンの描画設定を変更（名前）
//******************************************************************************
void SpriteFrame::SetBoneVisible( Label Name, bool bVisible )
{
	_ASSERT( m_pFile );

	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) SetBoneVisible( Index, bVisible );	
}

//******************************************************************************
//	補正色を設定
//******************************************************************************
void SpriteFrame::SetColor( Color Col )
{
	m_Color = Col;
	
	//	ボーン更新
	for( VecBone::iterator it = m_vBone.begin(); it != m_vBone.end(); ++it )
	{
		(*it)->ApplyColor( m_pAnime && m_pAnime->IsPlaying()? m_pAnime : NULL );
	}
}

//******************************************************************************
//	ボーンの補正色を設定（インデクス）
//******************************************************************************
void SpriteFrame::SetBoneColor( int Index, Color Col )
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	_ASSERT( m_pFile );

	m_vBone[Index]->SetColor( Col, m_pAnime && m_pAnime->IsPlaying()? m_pAnime : NULL );
}

//******************************************************************************
//	ボーンの補正色を設定（名前）
//******************************************************************************
void SpriteFrame::SetBoneColor( Label Name, Color Col )
{
	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) SetBoneColor( Index, Col );	
}

//******************************************************************************
//	補正色を設定
//******************************************************************************
void SpriteFrame::SetAlpha( byte Alpha )
{
	m_Color.a = Alpha;
	SetColor( m_Color );
}

//******************************************************************************
//	描画フェイズを設定(全体)
//******************************************************************************
void SpriteFrame::SetDrawPhase( uint Phase )
{
	for( VecBone::iterator it = m_vBone.begin(); it != m_vBone.end(); ++it )
	{
		(*it)->SetDrawPhase( Phase );
	}
}

//******************************************************************************
//	描画フェイズを設定(ボーン指定)
//******************************************************************************
void SpriteFrame::SetDrawPhase( int Index, int Phase )
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	m_vBone[Index]->SetDrawPhase( Phase );
}

//******************************************************************************
//	ボーンの文字列設定（インデクス）
//******************************************************************************
void SpriteFrame::SetBoneText( int Index, pstr pStr )
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	m_vBone[Index]->SetText( pStr );
}

//******************************************************************************
//	ボーンの文字列設定（名前）
//******************************************************************************
void SpriteFrame::SetBoneText( Label Name, pstr pStr )
{
	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) SetBoneText( Index, pStr );	
}

//******************************************************************************
//	ボーンの中心角度設定（インデクス）
//******************************************************************************
void SpriteFrame::SetBoneAngle( int Index, float Ang )
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	m_vBone[Index]->SetAngle( Ang );
}

//******************************************************************************
//	ボーンの中心角度設定（名前）
//******************************************************************************
void SpriteFrame::SetBoneAngle( Label Name, float Ang )
{
	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) SetBoneAngle( Index, Ang );	
}

//******************************************************************************
//	ボーンのピースインデクスを設定（インデクス）
//******************************************************************************
void SpriteFrame::SetBonePieceIndex( int Index, int PieceIndex )
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	m_vBone[Index]->SetPieceIndex( PieceIndex );
}

//******************************************************************************
//	ボーンのピースインデクスを設定（名前）
//******************************************************************************
void SpriteFrame::SetBonePieceIndex( Label Name, int PieceIndex )
{
	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) SetBonePieceIndex( Index, PieceIndex );
}

//******************************************************************************
//	ボーンの位置設定（インデクス）
//******************************************************************************
void SpriteFrame::SetBonePos( int Index, Point Pos )
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	m_vBone[Index]->SetPos( Pos );
	
	//	ワールド行列再計算
	_CalcWorldMatrix( m_vBone.front(), m_BaseMat );
}

//******************************************************************************
//	ボーンの位置設定（名前）
//******************************************************************************
void SpriteFrame::SetBonePos( Label Name, Point Pos )
{
	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) SetBonePos( Index, Pos );	
}

//******************************************************************************
//	ボーンの回転設定（インデクス）
//******************************************************************************
void SpriteFrame::SetBoneRot( int Index, float Rot )
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	m_vBone[Index]->SetRot( Rot );
	
	//	ワールド行列再計算
	_CalcWorldMatrix( m_vBone.front(), m_BaseMat );
}

//******************************************************************************
//	ボーンの回転設定（名前）
//******************************************************************************
void SpriteFrame::SetBoneRot( Label Name, float Rot )
{
	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) SetBoneRot( Index, Rot );	
}

//******************************************************************************
//	ボーンの拡縮設定（インデクス）
//******************************************************************************
void SpriteFrame::SetBoneScale( int Index, Point Scale )
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	m_vBone[Index]->SetScale( Scale );

	//	ワールド行列再計算
	_CalcWorldMatrix( m_vBone.front(), m_BaseMat );
}

//******************************************************************************
//	ボーンの拡縮設定（名前）
//******************************************************************************
void SpriteFrame::SetBoneScale( Label Name, Point Scale )
{
	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) SetBoneScale( Index, Scale );	
}

//------------------------------------------------------------------------------
//	ボーン生成
//------------------------------------------------------------------------------
SpriteFrameBone* SpriteFrame::_CreateBone( int BoneID )
{
	const SPRITE_FRAME_BONE* pParam = m_pFile->GetBoneParam( BoneID );
	if( !pParam ) return NULL;

	SpriteFrameBone* pBone = NULL;
	switch( pParam->Type )
	{
	case SPRITE_FRAME_BONE_RECT:	pBone = new SpriteFrameBoneRect();		break;
	case SPRITE_FRAME_BONE_RING:	pBone = new SpriteFrameBoneRing();		break;
	case SPRITE_FRAME_BONE_FONT:	pBone = new SpriteFrameBoneFont();		break;
	default:						pBone = new SpriteFrameBoneDummy();		break;
	}

	SafePtr( pBone )->Init( pParam, this );

	return pBone;
}

//------------------------------------------------------------------------------
//	ボーンのワールド行列を計算
//------------------------------------------------------------------------------
void SpriteFrame::_CalcWorldMatrix( SpriteFrameBone* pBone, const Matrix3& Mat )
{
	_ASSERT( pBone );

	//	兄弟を辿る
	if( pBone->GetSiblingID() >= 0 )
	{
		_CalcWorldMatrix( m_vBone[ pBone->GetSiblingID() ], Mat );
	}

	pBone->CalcWorldMatrix( Mat );

	//	子を辿る
	if( pBone->GetChildID() >= 0 )
	{
		_CalcWorldMatrix( m_vBone[ pBone->GetChildID() ], pBone->GetWorldMatrix() );
	}
}