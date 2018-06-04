#include "stdafx.h"
#include "EffectParticle.h"

#include "EffectManager.h"
#include "EffectFile.h"
#include "Effect.h"
#include "EffectDrawObj.h"

#include "Move/EffectMove.h"
#include "Move/EffectMoveDefault.h"
#include "Move/EffectMoveVelocity.h"
#include "Move/EffectMoveFollow.h"
#include "Move/EffectMoveCircular.h"

#include "SpriteFrame/SpriteFrame.h"


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクトパーティクル
//
////////////////////////////////////////////////////////////////////////////////
EffectParticle::EffectParticle( const Effect* pParent, const EFFECT_PARTICLE* pParam )
{
	_ASSERT(pParam && pParent);

	m_pParent		= pParent;
	m_pParam		= pParam;
	m_pAppear		= NULL;
	m_pDrawObj		= NULL;
	m_pMove			= NULL;
	m_pTimeKey		= NULL;

	m_FrameCount	= 0;
	m_bDelete		= false;
	m_bUpdateEnable	= true;

	m_Scale.One();
}
EffectParticle::~EffectParticle()
{
	SafeDelete( m_pTimeKey );
	SafeDelete( m_pDrawObj );
	SafeDelete( m_pMove );
}

//******************************************************************************
//	初期化
//******************************************************************************
void EffectParticle::Init( EffectManager* pManager, const EffectFile* pFile, const Matrix3& Parent, const EFFECT_APPEAR* pAppear )
{
	_ASSERT( pAppear );
	m_pAppear = pAppear;

	//	パラメータ
	m_FrameCount	= 0;
	m_bDelete		= false;
	
	m_Color			= m_pParam->Col;

	//	移動物初期化
	m_pMove = _CreateMove( pAppear->MoveType );
	MEM_CHECK( m_pMove );
	m_pMove->Init( pManager, Parent, pAppear );

	//	描画物の作成
	m_pDrawObj = _CreateDrawObj( m_pParam->Type );
	SafePtr( m_pDrawObj )->Init( pManager, pFile, GetMatrix() );

	//	タイムキーの生成
	m_pTimeKey	= new EffectTimeKey( this );
	MEM_CHECK( m_pTimeKey );

	//	色の反映
	ApplyColor();
}

//******************************************************************************
//	更新
//******************************************************************************
void EffectParticle::Update( EffectManager* pManager )
{
	if( m_bDelete ) return;

	_ASSERT( m_pMove );

	if( !m_pParent->IsPause() )
	{
		//	フレームカウントチェック
		if( m_pParam->EndFrame > 0 && m_FrameCount >= m_pParam->EndFrame )
		{	
			//	ループ処理
			if( m_pParam->bLoop )
			{
				m_FrameCount = 0;
				m_pTimeKey->Reset();
			}
			//	終了
			else
			{
				m_bDelete = true;
			}
		}

		//	タイムキー更新
		m_pTimeKey->Update( m_FrameCount );

		//	移動更新
		if( m_bUpdateEnable )
		{
			m_pMove->Update( pManager );
		}
	}

	//	描画物の更新
	if( m_bUpdateEnable )
	{
		SafePtr( m_pDrawObj )->Update( pManager, GetMatrix() );
	}

	if( !m_pParent->IsPause() )
	{
		//	カウント進める
		m_FrameCount++;
	}
}

//******************************************************************************
//	描画
//******************************************************************************
void EffectParticle::Draw( const Matrix3& OffsetMat )
{
	if( IsDelete() ) return;
	if( IsEndless() && m_pParent->IsEndAppear() ) return;
	
	SafePtr( m_pDrawObj )->Draw( OffsetMat );
}

//******************************************************************************
//	削除
//******************************************************************************
void EffectParticle::Delete( void )
{
	SafePtr( m_pDrawObj )->Delete();
	m_bDelete = true;
}

//******************************************************************************
//	生成終了
//******************************************************************************
void EffectParticle::EndAppear( void )
{
	SafePtr( m_pDrawObj )->EndAppear();
}

//******************************************************************************
//	色の反映
//******************************************************************************
void EffectParticle::ApplyColor( void )
{
	SafePtr( m_pDrawObj )->SetColor( m_pParent->GetColor() * m_Color );
}

//******************************************************************************
//	表示設定の変更
//******************************************************************************
void EffectParticle::SetVisible( bool bVisible )
{
	SafePtr( m_pDrawObj )->SetVisible( bVisible );
}

//******************************************************************************
//	一時停止
//******************************************************************************
void EffectParticle::Pause( bool bPause )
{
	SafePtr( m_pDrawObj )->Pause( bPause );
}

//******************************************************************************
//	表示状態の取得
//******************************************************************************
bool EffectParticle::IsVisible( void ) const
{
	return m_pParent->IsVisible();
}

//******************************************************************************
//	マトリクス取得
//******************************************************************************
const Matrix3& EffectParticle::GetMatrix( void ) const
{
	_ASSERT( m_pMove );

	return m_pMove->GetMatrix();
}

//******************************************************************************
//	タイムキー：キーの個数を取得
//******************************************************************************
uint EffectParticle::GetTimeKeyNum( EFFECT_TIME_KEY_TYPE Type ) const
{
	return (uint)m_pParam->vTimeKey[Type].size();
}

//******************************************************************************
//	タイムキー：キーを取得
//******************************************************************************
const EFFECT_TIME_KEY* EffectParticle::GetTimeKey( EFFECT_TIME_KEY_TYPE Type, int Index ) const
{
	return &(m_pParam->vTimeKey[Type].at(Index));
}

//******************************************************************************
//	タイムキー：キーパラメータの反映
//******************************************************************************
void EffectParticle::ApplyTimeKey( EFFECT_TIME_KEY_TYPE Type, const EFFECT_TIME_KEY_PARAM& Key )
{
	//	移動
	if( Type >= EFFECT_TIME_KEY_MOVE_BEGIN )
	{
		m_pMove->ApplyTimeKey( Type, Point( Key.Val ) );
	}
	//	更新の有無
	else if( Type == EFFECT_TIME_KEY_UPDATE_ENABLE )
	{
		m_bUpdateEnable = Key.bFlag == TRUE;
	}
	//	色
	else if( Type == EFFECT_TIME_KEY_COLOR )
	{
		m_Color.Set( Key.Col );
		ApplyColor();
	}
	//	サウンド
	else if( Type == EFFECT_TIME_KEY_VOL )
	{
		SafePtr( m_pDrawObj )->SetVolume( Key.Val[0] );
	}
	//	インデクス
	else if( Type == EFFECT_TIME_KEY_INDEX )
	{
		SafePtr( m_pDrawObj )->SetIndex( Key.Index.ID, Key.Index.Index );
	}
}

//******************************************************************************
//	タイムキー：キーパラメータの反映
//******************************************************************************
void EffectParticle::ApplyTimeKey( EFFECT_TIME_KEY_TYPE Type, const EFFECT_TIME_KEY_PARAM& Key, const EFFECT_TIME_KEY_PARAM& Next, float f )
{
	//	移動
	if( Type >= EFFECT_TIME_KEY_MOVE_BEGIN )
	{
		Point Val = Lerp( Point(Key.Val), Point(Next.Val), f );
		m_pMove->ApplyTimeKey( Type, Val );
	}
	//	色
	else if( Type == EFFECT_TIME_KEY_COLOR )
	{
		m_Color = Color::Lerp( Key.Col, Next.Col, f );
		ApplyColor();
	}
	//	サウンド
	else if( Type == EFFECT_TIME_KEY_VOL )
	{
		SafePtr( m_pDrawObj )->SetVolume( Lerp( Key.Val[0], Next.Val[0], f ) );
	}
	//	インデクス
	else if( Type == EFFECT_TIME_KEY_INDEX )
	{
		int Index = (int)Lerp( (float)Key.Index.Index, (float)Next.Index.Index, f );
		SafePtr( m_pDrawObj )->SetIndex( Key.Index.ID, Index );
	}
}

//------------------------------------------------------------------------------
//	描画物の生成
//------------------------------------------------------------------------------
EffectDrawObj* EffectParticle::_CreateDrawObj( EFFECT_PARTICLE_TYPE Type )	const
{
	switch( Type )
	{
		case EFFECT_PARTICLE_DUMMY:			return NULL;
		case EFFECT_PARTICLE_SPRITE:		return new EffectDrawObjSprite( m_pParam, this );
		case EFFECT_PARTICLE_SPRITE_FRAME:	return new EffectDrawObjSpriteFrame( m_pParam, this );
		case EFFECT_PARTICLE_LINE:			return new EffectDrawObjLine( m_pParam, this );
		case EFFECT_PARTICLE_FONT:			return new EffectDrawObjFont( m_pParam, this );
		case EFFECT_PARTICLE_SOUND:			return new EffectDrawObjSound( m_pParam, this );
		case EFFECT_PARTICLE_EFFECT:		return new EffectDrawObjEffect( m_pParam, this );
	}

	_ASSERT(false);
	return NULL;
}

//------------------------------------------------------------------------------
//	移動物の生成
//------------------------------------------------------------------------------
EffectMove* EffectParticle::_CreateMove( EFFECT_MOVE_TYPE Type )
{
	switch( Type )
	{
		case EFFECT_MOVE_DEFAULT:		return new EffectMoveDefault( this );
		case EFFECT_MOVE_VELOCITY:		return new EffectMoveVelocity( this, m_pParam->pVelocity );
		case EFFECT_MOVE_FOLLOW:		return new EffectMoveFollow( this );
		case EFFECT_MOVE_CIRCULAR:		return new EffectMoveCircular( this );
	}

	_ASSERT(false);
	return NULL;
}
