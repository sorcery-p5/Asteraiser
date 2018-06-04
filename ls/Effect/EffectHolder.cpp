#include "stdafx.h"
#include "EffectHolder.h"

#include "Effect.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g���܂Ƃ߂Ď���
//
////////////////////////////////////////////////////////////////////////////////
EffectHolder::EffectHolder( void )
{
}

EffectHolder::~EffectHolder()
{
}

//******************************************************************************
//	�X�V
//******************************************************************************
void EffectHolder::Update( const Matrix3* pMat )
{
	for( EffectList::iterator it = m_EffectList.begin(); it != m_EffectList.end(); )
	{
		if( it->pEffect )
		{
			//	�ʒu�擾
			if( pMat && it->bAttach )
			{
				it->pEffect->SetMatrix( *pMat );
			}

			++it;
		}
		else
		{
			//	�폜
			it = m_EffectList.erase( it );
		}
	}
}

//******************************************************************************
//	�ꎞ��~�̐ݒ�
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
//	�F�̐ݒ�
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
//	�G�t�F�N�g�̒ǉ�
//******************************************************************************
void EffectHolder::AddEffect( Effect* pEffect, bool bAttach )
{
	EFFECT Info;
	Info.pEffect	= pEffect;
	Info.bAttach	= bAttach;

	m_EffectList.push_back( Info );
}