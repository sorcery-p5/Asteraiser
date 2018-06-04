#include "stdafx.h"
#include "EffectHolder.h"

#include "Effect.h"


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクトをまとめて持つ
//
////////////////////////////////////////////////////////////////////////////////
EffectHolder::EffectHolder( void )
{
}

EffectHolder::~EffectHolder()
{
}

//******************************************************************************
//	更新
//******************************************************************************
void EffectHolder::Update( const Matrix3* pMat )
{
	for( EffectList::iterator it = m_EffectList.begin(); it != m_EffectList.end(); )
	{
		if( it->pEffect )
		{
			//	位置取得
			if( pMat && it->bAttach )
			{
				it->pEffect->SetMatrix( *pMat );
			}

			++it;
		}
		else
		{
			//	削除
			it = m_EffectList.erase( it );
		}
	}
}

//******************************************************************************
//	一時停止の設定
//******************************************************************************
void EffectHolder::SetPause( bool bPause )
{
	for( EffectList::iterator it = m_EffectList.begin(); it != m_EffectList.end(); ++it )
	{
		if( it->pEffect )
		{
			it->pEffect->Pause( bPause );	
		}
	}
}

//******************************************************************************
//	色の設定
//******************************************************************************
void EffectHolder::SetColor( Color Col )
{
	for( EffectList::iterator it = m_EffectList.begin(); it != m_EffectList.end(); ++it )
	{
		if( it->pEffect )
		{
			it->pEffect->SetColor( Col );	
		}
	}
}

//******************************************************************************
//	エフェクトの追加
//******************************************************************************
void EffectHolder::AddEffect( Effect* pEffect, bool bAttach )
{
	EFFECT Info;
	Info.pEffect	= pEffect;
	Info.bAttach	= bAttach;

	m_EffectList.push_back( Info );
}