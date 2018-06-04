#include "stdafx.h"
#include "Effect.h"

#include "EffectFile.h"
#include "EffectManager.h"
#include "EffectParticle.h"


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト
//
////////////////////////////////////////////////////////////////////////////////
Effect::Effect( void )
{
	m_pFile			= NULL;
	m_pParam		= NULL;

	m_FrameCount	= 0;

	m_bDelete		= false;
	m_bEndAppear	= false;
	m_bVisible		= true;
	m_bPause		= false;

	m_Color.SetWhite();
}
Effect::~Effect()
{
	DeleteVec( m_vParticle );
}

//******************************************************************************
//	初期化
//******************************************************************************
void Effect::Init( const EffectFile* pFile )
{
	_ASSERT( pFile );

	m_pFile = pFile;
	m_pParam = &pFile->GetParam();

	m_FrameCount = 0;
}

//******************************************************************************
//	更新
//******************************************************************************
void Effect::Update( EffectManager* pManager )
{
	if( m_bDelete ) return;

	if( !IsPause() )
	{
		//	フレームカウントチェック
		if( m_pParam->EndFrame > 0 )
		{
			if( m_FrameCount >= m_pParam->EndFrame )
			{
				//	ループ処理
				if( m_pParam->bLoop )
				{
					m_FrameCount = m_pParam->LoopFrame;
				}
				//	終了
				else
				{
					m_bEndAppear = true;
				}
			}
		}

		if( IsEndAppear() )
		{
			//	削除チェック
			bool bEmpty = true;
			for( VecParticle::const_iterator it = m_vParticle.begin(); it != m_vParticle.end(); ++it )
			{
				if( !(*it)->IsEndless() )
				{
					bEmpty = false;
				}
			}

			if( bEmpty )
			{
				m_bDelete = true;
			}
		}
		else
		{
			//	新規発生
			_UpdateAppear( pManager );
		}
	}

	//	パーティクル更新
	_UpdateParticle( pManager );
	
	if( !IsPause() )
	{
		//	カウント進める
		m_FrameCount++;
	}
}

//******************************************************************************
//	描画
//******************************************************************************
void Effect::Draw( const Matrix3& OffsetMat ) const
{
 	for( VecParticle::const_iterator it = m_vParticle.begin(); it != m_vParticle.end(); ++it )
	{
		(*it)->Draw( OffsetMat );
	}
}

//******************************************************************************
//	表示設定の変更
//******************************************************************************
void Effect::SetVisible( bool bVisible )
{
	m_bVisible = bVisible;
	
	for( VecParticle::iterator it = m_vParticle.begin(); it != m_vParticle.end(); ++it )
	{
		(*it)->SetVisible( bVisible );
	}
}

//******************************************************************************
//	色の設定
//******************************************************************************
void Effect::SetColor( Color Col )
{
	m_Color = Col;

	for( VecParticle::iterator it = m_vParticle.begin(); it != m_vParticle.end(); ++it )
	{
		(*it)->ApplyColor();
	}
}

//******************************************************************************
//	一時停止
//******************************************************************************
void Effect::Pause( bool bPause )
{
	m_bPause = bPause;

	for( VecParticle::iterator it = m_vParticle.begin(); it != m_vParticle.end(); ++it )
	{
		(*it)->Pause( bPause );
	}
}

//******************************************************************************
//	削除
//******************************************************************************
void Effect::Delete( void )
{
	m_bDelete = true;
	
	for( VecParticle::iterator it = m_vParticle.begin(); it != m_vParticle.end(); ++it )
	{
		(*it)->Delete();
	}
}

//******************************************************************************
//	生成終了
//******************************************************************************
void Effect::EndAppear( void )
{
	//	ポーズ解除
	Pause( false );

	m_bEndAppear = true;

	for( VecParticle::iterator it = m_vParticle.begin(); it != m_vParticle.end(); ++it )
	{
		(*it)->EndAppear();
	}
}

//******************************************************************************
//	エフェクトのスケールを得る
//******************************************************************************
Point Effect::GetParticleScale( Label Name )
{
	EffectParticle* pParticle = _FindParticle( Name );
	if( !pParticle ) return Point( 1.0f, 1.0f );

	return pParticle->GetScale();
}

//******************************************************************************
//	エフェクトのスケールを設定
//******************************************************************************
void Effect::SetParticleScale( Label Name, Point Scale )
{
	EffectParticle* pParticle = _FindParticle( Name );
	if( !pParticle ) return;

	pParticle->SetScale( Scale );
}

//******************************************************************************
//	削除状態か
//******************************************************************************
bool Effect::IsDelete( void ) const
{
	if( m_bDelete ) return true;

	return false;
}

//******************************************************************************
//	削除状態か
//******************************************************************************
bool Effect::IsEndAppear( void ) const
{
	if( m_bEndAppear ) return true;

	if( (m_pParam->bLoop || m_pParam->EndFrame == 0) && !IS_REFERENCE() )
	{
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
//	パーティクルの更新
//------------------------------------------------------------------------------
void Effect::_UpdateParticle( EffectManager* pManager )
{
	//	更新
	for( VecParticle::iterator it = m_vParticle.begin(); it != m_vParticle.end(); ++it )
	{
		(*it)->Update( pManager );
	}

	//	削除チェック
	for( VecParticle::iterator it = m_vParticle.begin(); it != m_vParticle.end(); )
	{
		if( (*it)->IsDelete() )
		{
			SafeDelete( *it );
			it = m_vParticle.erase( it );
		}
		else
		{
			++it;
		}
	}
}

//------------------------------------------------------------------------------
//	パーティクル発生の更新
//------------------------------------------------------------------------------
void Effect::_UpdateAppear( EffectManager* pManager )
{
	for( uint i = 0; i < m_pFile->GetAppearParamNum(); i++ )
	{
		const EFFECT_APPEAR* pParam = m_pFile->GetAppearParam( i );

		if( m_FrameCount < pParam->StartFrame || m_FrameCount > pParam->EndFrame )
		{
			continue;
		}

		if( (m_FrameCount - pParam->StartFrame) % pParam->Interval != 0 )
		{
			continue;
		}

		_AppearParticle( pManager, pParam );
	}
}

//------------------------------------------------------------------------------
//	パーティクルの発生
//------------------------------------------------------------------------------
void Effect::_AppearParticle( EffectManager* pManager, const EFFECT_APPEAR* pParam )
{
	const EFFECT_PARTICLE* pParticleParam = m_pFile->GetParticleParam( pParam->ParticleName );
	if( !pParticleParam ) return;

	for( uint i = 0; i < pParam->AppearNum; i++ )
	{
		//	生成
		EffectParticle* pParticle = pParticle = new EffectParticle( this, pParticleParam );
		if( !pParticle ) return;

		//	初期化
		pParticle->Init( pManager, m_pFile, m_Matrix, pParam );

		//	登録
		m_vParticle.push_back( pParticle );
	}
}

//------------------------------------------------------------------------------
//	パーティクルを検索
//------------------------------------------------------------------------------
EffectParticle* Effect::_FindParticle( Label Name )
{
	//	更新
	for( VecParticle::iterator it = m_vParticle.begin(); it != m_vParticle.end(); ++it )
	{
		if( (*it)->GetName() == Name )
		{
			return (*it);
		}
	}

	return NULL;
}