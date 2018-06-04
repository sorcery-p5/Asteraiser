#include "stdafx.h"
#include "SpriteFrameBone.h"
#include "SpriteFrame.h"
#include "SpriteFrameFile.h"
#include "SpriteFrameAnime.h"
#include "SpriteFrameUtil.h"

#include "App/App.h"
#include "Effect/EffectManager.h"
#include "Effect/Effect.h"

namespace
{
	const float DUMMY_BONE_LEN		= 20.0f;
}


///////////////////////////////////////////////////////////////////////////////
//
//	スプライトフレーム：ボーン（基底）
//
///////////////////////////////////////////////////////////////////////////////
SpriteFrameBone::SpriteFrameBone( void )
{
	m_pFrame	= NULL;
	m_pParam	= NULL;
	m_bVisible	= true;
	m_Color		= Color::White();
}

SpriteFrameBone::~SpriteFrameBone()
{
	for( EffectList::iterator it = m_ResidentEffectList.begin(); it != m_ResidentEffectList.end(); it++ )
	{
		EffectDelete( it->pEffect );
	}
	for( EffectList::iterator it = m_AppearEffectList.begin(); it != m_AppearEffectList.end(); it++ )
	{
		EffectDelete( it->pEffect );
	}
	for( SoundList::iterator it = m_ResidentSoundList.begin(); it != m_ResidentSoundList.end(); it++ )
	{
		SafeStop( it->pSound );
	}
	for( SoundList::iterator it = m_AppearSoundList.begin(); it != m_AppearSoundList.end(); it++ )
	{
		SafeStop( it->pSound );
	}
}

//*****************************************************************************
//	初期化
//*****************************************************************************
void SpriteFrameBone::Init( const SPRITE_FRAME_BONE* pParam, const SpriteFrame* pFrame )
{
	_ASSERT( pParam && pFrame ); 

	m_pFrame	= pFrame;
	m_pParam	= pParam;

	//	初期化
	_OnInit();

	//	行列を初期化
	_ResetLocalMatrix();
}

//*****************************************************************************
//	更新
//*****************************************************************************
void SpriteFrameBone::Update( void )
{
	//	エフェクトを更新
	_UpdateEffect();
}

//*****************************************************************************
//	描画
//*****************************************************************************
void SpriteFrameBone::Draw( const Matrix3& OffsetMat )
{
	if( m_bVisible && m_pFrame->IsVisible() )
	{
		_OnDraw( OffsetMat );
	}

	//	サウンドを更新
	_UpdateSound( OffsetMat );
}

//*****************************************************************************
//	アニメを適用する
//*****************************************************************************
void SpriteFrameBone::ApplyAnime( const SpriteFrameAnime* pAnime )
{
	_ASSERT( pAnime );

	//	適用
	_OnApplyAnime( pAnime );

	//	行列を取得
	pAnime->GetBoneMatrix( m_LocalMatrix, m_pParam->ID );
}

//******************************************************************************
//	アニメーションを初期状態に戻す
//******************************************************************************
void SpriteFrameBone::ResetAnime( void )
{
	//	設定
	_OnResetAnime();

	//	行列を初期化
	_ResetLocalMatrix();

	//	発生物を初期化
	ResetAppear();
}

//******************************************************************************
//	一時停止する
//******************************************************************************
void SpriteFrameBone::Pause( bool bPause )
{
	//	エフェクト
	for( EffectList::iterator it = m_ResidentEffectList.begin(); it != m_ResidentEffectList.end(); it++ )
	{
		SafePtr(it->pEffect)->Pause( bPause );
	}
	for( EffectList::iterator it = m_AppearEffectList.begin(); it != m_AppearEffectList.end(); it++ )
	{
		SafePtr(it->pEffect)->Pause( bPause );
	}

	//	サウンド
	for( SoundList::iterator it = m_ResidentSoundList.begin(); it != m_ResidentSoundList.end(); it++ )
	{
		SafePtr(it->pSound)->SetPause( bPause );
	}
	for( SoundList::iterator it = m_AppearSoundList.begin(); it != m_AppearSoundList.end(); it++ )
	{
		SafePtr(it->pSound)->SetPause( bPause );
	}
}

//*****************************************************************************
//	ワールド行列の計算
//*****************************************************************************
void SpriteFrameBone::CalcWorldMatrix( const Matrix3& ParentMat )
{
	if( m_pParam->DirType == SPRITE_FRAME_BONE_DIR_REL )
	{
		m_WorldMatrix = m_LocalMatrix * ParentMat;
	}
	else if( m_pParam->DirType == SPRITE_FRAME_BONE_DIR_FIX )
	{
		//	向き固定：位置のみ反映
		m_WorldMatrix = m_LocalMatrix * Matrix3::Trans( ParentMat.GetPos() );
	}
}

//******************************************************************************
//	補正色の設定
//******************************************************************************
void SpriteFrameBone::SetColor( Color Col, const SpriteFrameAnime* pAnime )
{
	m_Color = Col;
	ApplyColor( pAnime );
}

//******************************************************************************
//	補正色の反映
//******************************************************************************
void SpriteFrameBone::ApplyColor( const SpriteFrameAnime* pAnime )
{
	_OnApplyColor( GetColor( pAnime ) );
}

//******************************************************************************
//	補正色の取得
//******************************************************************************
Color SpriteFrameBone::GetColor( const SpriteFrameAnime* pAnime ) const
{
	return (m_Color * m_pFrame->GetColor() * (pAnime? pAnime->GetBoneColor( m_pParam->ID ) : m_pParam->Col) );
}

//******************************************************************************
//	常駐エフェクト･サウンドを生成
//******************************************************************************
void SpriteFrameBone::InitResidentAppear( void )
{
	//	エフェクト
	for( uint i = 0; i < m_pParam->vEffect.size(); i++ )
	{
		const SPRITE_FRAME_EFFECT& Param = m_pParam->vEffect[i];

		//	登録
		EFFECT Effect;
		Effect.pEffect	= _CreateEffect( &Param );
		Effect.pParam	= &Param;

		m_ResidentEffectList.push_back( Effect );
	}
	
	//	サウンド
	for( uint i = 0; i < m_pParam->vSound.size(); i++ )
	{
		const SPRITE_FRAME_SOUND& Param = m_pParam->vSound[i];

		//	登録
		SOUND Sound;
		Sound.pSound	= _CreateSound( &Param, true );
		Sound.pParam	= &Param;

		m_ResidentSoundList.push_back( Sound );
	}
}

//******************************************************************************
//	発生物のリセット
//******************************************************************************
void SpriteFrameBone::ResetAppear( void )
{
	for( EffectList::iterator it = m_AppearEffectList.begin(); it != m_AppearEffectList.end(); it++ )
	{
		EffectEndAppear( it->pEffect );
	}
	m_AppearSoundList.clear();
	m_AppearEffectList.clear();
}

//******************************************************************************
//	発生物の追加
//******************************************************************************
void SpriteFrameBone::AddAppear( const SPRITE_FRAME_APPEAR* pParam )
{
	//	エフェクト
	if( pParam->Type == SPRITE_FRAME_APPERAR_EFFECT )
	{
		Effect* pEffect  = _CreateEffect( pParam->pEffect );

		//	ループ
		if( pParam->pEffect->bAttach )
		{
			EFFECT Param;
			Param.pEffect	= pEffect;
			Param.pParam	= pParam->pEffect;

			m_AppearEffectList.push_back( Param );
		}
	}
	//	サウンド
	else if( pParam->Type == SPRITE_FRAME_APPERAR_SOUND )
	{
		Sound* pSound  = _CreateSound( pParam->pSound, pParam->pSound->bLoop );

		//	ループ
		if( pParam->pSound->bLoop )
		{
			SOUND Param;
			Param.pSound	= pSound;
			Param.pParam	= pParam->pSound;

			m_AppearSoundList.push_back( Param );
		}
	}
	//	削除
	else if( pParam->Type == SPRITE_FRAME_APPERAR_ERASE )
	{
		for( EffectList::iterator it = m_ResidentEffectList.begin(); it != m_ResidentEffectList.end(); it++ )
		{
			if( it->pParam->Name == Label(pParam->Name) )
			{
				EffectEndAppear( it->pEffect );
			}
		}
		for( EffectList::iterator it = m_AppearEffectList.begin(); it != m_AppearEffectList.end(); it++ )
		{
			if( it->pParam->Name == Label(pParam->Name) ) EffectEndAppear( it->pEffect );
		}
		for( SoundList::iterator it = m_ResidentSoundList.begin(); it != m_ResidentSoundList.end(); it++ )
		{
			if( it->pParam->Name == Label(pParam->Name) ) SafeStop( it->pSound );
		}
		for( SoundList::iterator it = m_AppearSoundList.begin(); it != m_AppearSoundList.end(); it++ )
		{
			if( it->pParam->Name == Label(pParam->Name) ) SafeStop( it->pSound );
		}
	}
}

//******************************************************************************
//	描画の設定
//******************************************************************************
void SpriteFrameBone::SetVisible( bool bVisible )
{
	m_bVisible = bVisible;

	UpdateVisible();
}

//******************************************************************************
//	描画設定の更新
//******************************************************************************
void SpriteFrameBone::UpdateVisible( void )
{
	bool bVisible = m_bVisible && m_pFrame->IsVisible();

	//	エフェクト
	for( EffectList::iterator it = m_ResidentEffectList.begin(); it != m_ResidentEffectList.end(); it++ )
	{
		SafePtr(it->pEffect)->SetVisible( bVisible );
	}
	for( EffectList::iterator it = m_AppearEffectList.begin(); it != m_AppearEffectList.end(); it++ )
	{
		SafePtr(it->pEffect)->SetVisible( bVisible );
	}

	//	サウンド
	for( SoundList::iterator it = m_ResidentSoundList.begin(); it != m_ResidentSoundList.end(); it++ )
	{
		SafePtr(it->pSound)->SetMute( !bVisible );
	}
	for( SoundList::iterator it = m_AppearSoundList.begin(); it != m_AppearSoundList.end(); it++ )
	{
		SafePtr(it->pSound)->SetMute( !bVisible );
	}
}

//******************************************************************************
//	位置の設定
//******************************************************************************
void SpriteFrameBone::SetPos( Point Pos )
{
	m_LocalMatrix.SetPos( Pos );
}

//******************************************************************************
//	回転の設定
//******************************************************************************
void SpriteFrameBone::SetRot( float Rot )
{
	m_LocalMatrix.SetRotZ( Rot );
}

//******************************************************************************
//	拡縮の設定
//******************************************************************************
void SpriteFrameBone::SetScale( Point Scale )
{
	m_LocalMatrix.SetScale( Scale );
}

//------------------------------------------------------------------------------
//	ローカル行列を初期化する
//------------------------------------------------------------------------------
void SpriteFrameBone::_ResetLocalMatrix( void )
{
	SpriteFrameAnime::CalcMatrix( m_LocalMatrix, m_pParam->Pos, m_pParam->Rot, m_pParam->Scale );
}

//------------------------------------------------------------------------------
//	エフェクトの更新
//------------------------------------------------------------------------------
void SpriteFrameBone::_UpdateEffect( void )
{
	for( EffectList::iterator it = m_ResidentEffectList.begin(); it != m_ResidentEffectList.end(); ++it )
	{
		if( it->pEffect )
		{
			(it->pEffect)->SetMatrix( _CalcEffectMatrix(it->pParam) );
		}
	}
	for( EffectList::iterator it = m_AppearEffectList.begin(); it != m_AppearEffectList.end(); ++it )
	{
		if( it->pEffect )
		{
			(it->pEffect)->SetMatrix( _CalcEffectMatrix(it->pParam) );
		}
	}
}

//------------------------------------------------------------------------------
//	サウンドの更新
//------------------------------------------------------------------------------
void SpriteFrameBone::_UpdateSound( const Matrix3& OffsetMat )
{
	for( SoundList::iterator it = m_ResidentSoundList.begin(); it != m_ResidentSoundList.end(); ++it )
	{
		if( it->pSound )
		{
			//	パンの更新
			if(it->pParam->bPan) (it->pSound)->SetPan( Sound::CalcPanValue(OffsetMat.Transform(m_WorldMatrix.GetPos()),GetWindow()->GetSize()) );
		}
	}

	for( SoundList::iterator it = m_AppearSoundList.begin(); it != m_AppearSoundList.end(); ++it )
	{
		if( it->pSound )
		{
			//	パンの更新
			if(it->pParam->bPan) it->pSound->SetPan( Sound::CalcPanValue(OffsetMat.Transform(m_WorldMatrix.GetPos()),GetWindow()->GetSize()) );
		}
	}
}

//------------------------------------------------------------------------------
//	エフェクトの生成
//------------------------------------------------------------------------------
Effect* SpriteFrameBone::_CreateEffect( const SPRITE_FRAME_EFFECT* pParam )
{
	//	生成
	const EffectFile* pEffectFile = m_pFrame->GetFile()->GetEffectFile( pParam->Name );
	Effect* pEffect = GetApp()->GetEffectMng()->CreateEffect( pEffectFile, _CalcEffectMatrix(pParam) );
	if( !pEffect ) return NULL;

	//	色の設定
	pEffect->SetColor( pParam->Col );

	return pEffect;
}

//------------------------------------------------------------------------------
//	サウンドの生成
//------------------------------------------------------------------------------
Sound* SpriteFrameBone::_CreateSound( const SPRITE_FRAME_SOUND* pParam, bool bLoop )
{
	//	生成
	const SoundFile* pSoundFile = m_pFrame->GetFile()->GetSoundFile( pParam->Name );
	if( !pSoundFile ) return NULL;

	Sound* pSound = GetTone()->PlaySound( pSoundFile, SOUND_CHANNEL_SE, bLoop, pParam->Vol );
	if( !pSound ) return NULL;
	
	return pSound;
}

//------------------------------------------------------------------------------
//	エフェクト位置の計算
//------------------------------------------------------------------------------
Matrix3 SpriteFrameBone::_CalcEffectMatrix( const SPRITE_FRAME_EFFECT* pParam )
{
	Matrix3 Mat = Matrix3::Scale( pParam->Scale );
	Mat *= Matrix3::TransRotZ( pParam->Pos, pParam->Rot );

	return Mat * m_WorldMatrix;
}


///////////////////////////////////////////////////////////////////////////////
//
//	スプライトフレーム：ボーン（ダミー）
//
///////////////////////////////////////////////////////////////////////////////
SpriteFrameBoneDummy::SpriteFrameBoneDummy( void )
{
	m_pShape	= NULL;
}

SpriteFrameBoneDummy::~SpriteFrameBoneDummy()
{
	SafeDelete( m_pShape );
}

//------------------------------------------------------------------------------
//	初期化時の処理
//------------------------------------------------------------------------------
void SpriteFrameBoneDummy::_OnInit( void )
{
	//	描画用形状を作る
	m_pShape = new ShapeArrowLine;
	MEM_CHECK( m_pShape );

	m_pShape->SetColor( m_pParam->Col );
}

//------------------------------------------------------------------------------
//	描画時の処理
//------------------------------------------------------------------------------
void SpriteFrameBoneDummy::_OnDraw( const Matrix3& OffsetMat )
{
	if( m_pShape && SpriteFrameUtil::IsDrawDummyBone() )
	{
		Matrix3 Mat = m_WorldMatrix * OffsetMat;

		m_pShape->Draw( Mat.GetPos(), Mat.GetPos() + Mat.GetAxisY() * DUMMY_BONE_LEN );
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	スプライトフレーム：ボーン（矩形）
//
///////////////////////////////////////////////////////////////////////////////
SpriteFrameBoneRect::SpriteFrameBoneRect( void )
{
	m_pSprite	= NULL;
	m_Index		= -1;
}

SpriteFrameBoneRect::~SpriteFrameBoneRect()
{
	SafeDelete( m_pSprite );
}

//******************************************************************************
//	描画フェイズを設定
//******************************************************************************
void SpriteFrameBoneRect::SetDrawPhase( int Phase )
{
	SafePtr( m_pSprite )->SetPhase( Phase );
}

//******************************************************************************
//	矩形を取得
//******************************************************************************
Rect SpriteFrameBoneRect::GetRect( void ) const
{
	return Rect( -m_Center.x, -m_Center.y, m_pSprite->GetSize().x - m_Center.x, m_pSprite->GetSize().y - m_Center.y );
}

//------------------------------------------------------------------------------
//	初期化時の処理
//------------------------------------------------------------------------------
void SpriteFrameBoneRect::_OnInit( void )
{
	//	スプライト作成
	if( !m_pParam->SourceName.IsEmpty() )
	{
		m_pSprite = new RectSprite;
		if( m_pSprite )
		{
			//	ピース設定
			m_PieceName = m_pParam->SourceName;
			const Piece* pPiece = m_pFrame->GetFile()->GetPiece( m_PieceName );

			if( pPiece )
			{
				m_pSprite->SetPiece( pPiece, m_pFrame->GetFile()->GetTex(pPiece->GetTexName()) );
			}

			//	パラメータ設定
			m_pSprite->SetColor( GetColor( NULL ) );
			m_pSprite->SetIndex( m_Index >= 0? m_Index : 0 );
			m_pSprite->SetBlend( m_pFrame->GetFile()->GetBlendMode( m_pParam->ID ) );
			m_pSprite->SetPhase( m_pFrame->GetFile()->GetDrawPhase( m_pParam->ID ) );
		}
	}

	//	中央位置
	m_Center = m_pParam->Center;
}

//------------------------------------------------------------------------------
//	描画時の処理
//------------------------------------------------------------------------------
void SpriteFrameBoneRect::_OnDraw( const Matrix3& OffsetMat )
{
	Matrix3 CenterMat = Matrix3::Trans( -m_Center );

	SafePtr(m_pSprite)->Draw( CenterMat * m_WorldMatrix * OffsetMat );
}

//------------------------------------------------------------------------------
//	アニメ適用時の処理
//------------------------------------------------------------------------------
void SpriteFrameBoneRect::_OnApplyAnime( const SpriteFrameAnime* pAnime )
{
	if( m_pSprite )
	{
		//	ピース変更
		Label PieceName = pAnime->GetBonePieceName( m_pParam->ID );
		if( !PieceName.IsEmpty() && m_PieceName != PieceName )
		{
			m_PieceName = PieceName;
			const Piece* pPiece = m_pFrame->GetFile()->GetPiece( m_PieceName );
			m_pSprite->SetPiece( pPiece, m_pFrame->GetFile()->GetTex(pPiece->GetTexName()) );
		}

		//	中央位置
		m_Center = pAnime->GetBoneCenterPos( m_pParam->ID );

		//	色とインデクス
		m_pSprite->SetColor( GetColor( pAnime ) );
		m_pSprite->SetIndex( m_Index >= 0? m_Index : pAnime->GetBoneSpriteIndex( m_pParam->ID ) );
	}
}

//------------------------------------------------------------------------------
//	アニメリセット時の処理
//------------------------------------------------------------------------------
void SpriteFrameBoneRect::_OnResetAnime( void )
{
	if( m_pSprite )
	{
		//	ピース変更
		m_PieceName = m_pParam->SourceName;
		const Piece* pPiece = m_pFrame->GetFile()->GetPiece( m_PieceName );
		m_pSprite->SetPiece( pPiece, m_pFrame->GetFile()->GetTex(pPiece->GetTexName()) );

		//	色とインデクス
		m_pSprite->SetColor( GetColor(NULL) );
		m_pSprite->SetIndex( m_Index >= 0? m_Index : 0 );
	}
}

//------------------------------------------------------------------------------
//	色変更処理
//------------------------------------------------------------------------------
void SpriteFrameBoneRect::_OnApplyColor( Color Col )
{
	SafePtr( m_pSprite )->SetColor( Col );
}


///////////////////////////////////////////////////////////////////////////////
//
//	スプライトフレーム：ボーン（矩形）
//
///////////////////////////////////////////////////////////////////////////////
SpriteFrameBoneRing::SpriteFrameBoneRing( void )
{
	m_pSprite	= NULL;
}

SpriteFrameBoneRing::~SpriteFrameBoneRing()
{
	SafeDelete( m_pSprite );
}

//******************************************************************************
//	描画フェイズを設定
//******************************************************************************
void SpriteFrameBoneRing::SetDrawPhase( int Phase )
{
	SafePtr( m_pSprite )->SetPhase( Phase );
}

//******************************************************************************
//	中心角度を設定
//******************************************************************************
void SpriteFrameBoneRing::SetAngle( float Ang )
{
	SafePtr( m_pSprite )->SetAngle( Ang );
}

//******************************************************************************
//	半径を取得
//******************************************************************************
float SpriteFrameBoneRing::GetRadius( void ) const
{
	return m_pSprite->GetRadiusMax();
}

//------------------------------------------------------------------------------
//	初期化時の処理
//------------------------------------------------------------------------------
void SpriteFrameBoneRing::_OnInit( void )
{
	const SPRITE_FRAME_RING* pRingParam = m_pFrame->GetFile()->GetRing( m_pParam->ID );
	if( !pRingParam ) return;

	//	スプライト作成
	if( !m_pParam->SourceName.IsEmpty() )
	{
		m_pSprite = new RingSprite( pRingParam->RadiusMin, pRingParam->RadiusMax, pRingParam->Ang, pRingParam->Num, pRingParam->Divide );
		if( m_pSprite )
		{
			//	ピース設定
			m_PieceName = m_pParam->SourceName;
			const Piece* pPiece = m_pFrame->GetFile()->GetPiece( m_PieceName );

			if( pPiece )
			{
				m_pSprite->SetPiece( pPiece, m_pFrame->GetFile()->GetTex(pPiece->GetTexName()) );
			}

			//	パラメータ設定
			m_pSprite->SetColor( GetColor( NULL )  );
			m_pSprite->SetIndex( 0 );
			m_pSprite->SetBlend( m_pFrame->GetFile()->GetBlendMode( m_pParam->ID ) );
			m_pSprite->SetPhase( m_pFrame->GetFile()->GetDrawPhase( m_pParam->ID ) );
		}
	}
}

//------------------------------------------------------------------------------
//	描画時の処理
//------------------------------------------------------------------------------
void SpriteFrameBoneRing::_OnDraw( const Matrix3& OffsetMat )
{
	SafePtr(m_pSprite)->Draw( m_WorldMatrix * OffsetMat );
}

//------------------------------------------------------------------------------
//	アニメ適用時の処理
//------------------------------------------------------------------------------
void SpriteFrameBoneRing::_OnApplyAnime( const SpriteFrameAnime* pAnime )
{
	if( m_pSprite )
	{
		//	ピース変更
		Label PieceName = pAnime->GetBonePieceName( m_pParam->ID );
		if( !PieceName.IsEmpty() && m_PieceName != PieceName )
		{
			m_PieceName = PieceName;
			const Piece* pPiece = m_pFrame->GetFile()->GetPiece( m_PieceName );
			m_pSprite->SetPiece( pPiece, m_pFrame->GetFile()->GetTex(pPiece->GetTexName()) );
		}

		//	色とインデクス
		m_pSprite->SetColor( GetColor( pAnime ) );
		m_pSprite->SetIndex( pAnime->GetBoneSpriteIndex( m_pParam->ID ) );
	}
}

//------------------------------------------------------------------------------
//	アニメリセット時の処理
//------------------------------------------------------------------------------
void SpriteFrameBoneRing::_OnResetAnime( void )
{
	if( m_pSprite )
	{
		//	ピース変更
		m_PieceName = m_pParam->SourceName;
		const Piece* pPiece = m_pFrame->GetFile()->GetPiece( m_PieceName );
		m_pSprite->SetPiece( pPiece, m_pFrame->GetFile()->GetTex(pPiece->GetTexName()) );

		//	色とインデクス
		m_pSprite->SetColor( GetColor( NULL ) );
		m_pSprite->SetIndex( 0 );
	}
}

//------------------------------------------------------------------------------
//	色変更処理
//------------------------------------------------------------------------------
void SpriteFrameBoneRing::_OnApplyColor( Color Col )
{
	SafePtr( m_pSprite )->SetColor( Col );
}


///////////////////////////////////////////////////////////////////////////////
//
//	スプライトフレーム：ボーン（フォント）
//
///////////////////////////////////////////////////////////////////////////////
SpriteFrameBoneFont::SpriteFrameBoneFont( void )
{
	m_pSprite	= NULL;
}

SpriteFrameBoneFont::~SpriteFrameBoneFont()
{
	SafeDelete( m_pSprite );
}

//******************************************************************************
//	描画フェイズを設定
//******************************************************************************
void SpriteFrameBoneFont::SetDrawPhase( int Phase )
{
	SafePtr( m_pSprite )->SetPhase( Phase );
}

//******************************************************************************
//	文字列を設定
//******************************************************************************
void SpriteFrameBoneFont::SetText( pstr pStr )
{
	SafePtr( m_pSprite )->SetText( pStr );
}

//------------------------------------------------------------------------------
//	初期化時の処理
//------------------------------------------------------------------------------
void SpriteFrameBoneFont::_OnInit( void )
{
	//	スプライト作成
	if( !m_pParam->SourceName.IsEmpty() )
	{
		m_pSprite = new FontSprite( m_pFrame->GetFile()->GetFont( m_pParam->SourceName ) );
		if( m_pSprite )
		{
			//	パラメータ設定
			m_pSprite->SetText( m_pFrame->GetFile()->GetText( m_pParam->ID ) );
			m_pSprite->SetAlign( m_pFrame->GetFile()->GetAlign( m_pParam->ID ) );

			m_pSprite->SetColor(GetColor( NULL ) );
			m_pSprite->SetBlend( m_pFrame->GetFile()->GetBlendMode( m_pParam->ID ) );
			m_pSprite->SetPhase( m_pFrame->GetFile()->GetDrawPhase( m_pParam->ID ) );
		}
	}
}

//------------------------------------------------------------------------------
//	描画時の処理
//------------------------------------------------------------------------------
void SpriteFrameBoneFont::_OnDraw( const Matrix3& OffsetMat )
{
	if( m_pSprite )
	{
		m_pSprite->SetMatrix( m_WorldMatrix * OffsetMat );
		m_pSprite->Draw();
	}
}

//------------------------------------------------------------------------------
//	アニメ適用時の処理
//------------------------------------------------------------------------------
void SpriteFrameBoneFont::_OnApplyAnime( const SpriteFrameAnime* pAnime )
{
	if( m_pSprite )
	{
		//	色
		m_pSprite->SetColor( GetColor( pAnime ) );
		
		//	文字列
		pstr pStr = pAnime->GetBoneText( m_pParam->ID );
		if( pStr )
		{
			m_pSprite->SetText( pStr );
		}
	}
}

//------------------------------------------------------------------------------
//	アニメリセット時の処理
//------------------------------------------------------------------------------
void SpriteFrameBoneFont::_OnResetAnime( void )
{
	if( m_pSprite )
	{
		//	色
		m_pSprite->SetColor( GetColor( NULL ) );
	}
}

//------------------------------------------------------------------------------
//	色変更処理
//------------------------------------------------------------------------------
void SpriteFrameBoneFont::_OnApplyColor( Color Col )
{
	SafePtr( m_pSprite )->SetColor( Col );
}