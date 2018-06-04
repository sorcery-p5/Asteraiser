#pragma once

#include "TimeKeyTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�^�C���L�[
//
////////////////////////////////////////////////////////////////////////////////
template< typename PARAM, typename TYPE, int TYPE_MAX >
class TimeKey
{
public:
	typedef TIME_KEY_PARAM<PARAM> KEY;

	class Owner
	{
	public:
		virtual uint			GetTimeKeyNum( TYPE Type )												const	= 0;
		virtual const KEY*		GetTimeKey( TYPE Type, int Index )										const	= 0;
		virtual void			ApplyTimeKey( TYPE Type, const PARAM& Key )										= 0;
		virtual void			ApplyTimeKey( TYPE Type, const PARAM& Key, const PARAM& Next, float f )			= 0;
	};

public:
	TimeKey( Owner* pOwner )
	{
		_ASSERT( pOwner );

		m_pOwner = pOwner;
		FillArray( m_aKeyIndex, TYPE_MAX, -1 ); 
	}

	void							Update( uint FrameCount );
	void							Reset( void )				{ FillArray( m_aKeyIndex, TYPE_MAX, -1 ); }

private:
	Owner*							m_pOwner;
	int								m_aKeyIndex[ TYPE_MAX ];
};


TIME_KEY_INTP				StrToTimeKeyIntpType( pstr pStr );


//******************************************************************************
//	�X�V
//******************************************************************************
template< typename PARAM, typename TYPE, int TYPE_MAX >
void TimeKey< PARAM, TYPE, TYPE_MAX>::Update( uint FrameCount )
{
	for( uint i = 0; i < TYPE_MAX; i++ )
	{
		TYPE KeyType = TYPE(i);

		//	�L�[���Ȃ�
		int KeyNum		= (int)m_pOwner->GetTimeKeyNum( KeyType );

		int CurIndex	= m_aKeyIndex[KeyType];
		int NextIndex	= CurIndex + 1;

		//	���̃L�[�擾
		const KEY* pNextKey = NULL;
		if( NextIndex < KeyNum )
		{
			pNextKey = m_pOwner->GetTimeKey( KeyType, NextIndex );

			//	�L�[�̈ڍs�`�F�b�N
			if( FrameCount >= pNextKey->Frame )
			{
				//	�Œ�L�[�̏ꍇ�A�P�Ɣ��s
				if( pNextKey->Intp == TIME_KEY_INTP_FIX )
				{
					m_pOwner->ApplyTimeKey( TYPE(KeyType), pNextKey->Param );
				}

				//	�ڍs����
				m_aKeyIndex[KeyType] = NextIndex;
				CurIndex			 = NextIndex;

				if( NextIndex + 1 < KeyNum )
				{
					NextIndex = NextIndex+1;
					pNextKey = m_pOwner->GetTimeKey( KeyType, NextIndex );
				}
				else
				{
					pNextKey = NULL;
				}
			}
		}

		if( CurIndex >= 0 )
		{
			const KEY* pKey = m_pOwner->GetTimeKey( KeyType, CurIndex );

			//	��ԃL�[�̏ꍇ
			if( pNextKey && pKey->Intp == TIME_KEY_INTP_LERP )
			{
				float f = Coef( (float)FrameCount, (float)pKey->Frame, (float)pNextKey->Frame );
				m_pOwner->ApplyTimeKey( TYPE(KeyType), pKey->Param, pNextKey->Param, f );
			}
			else if( !pNextKey )
			{
				m_pOwner->ApplyTimeKey( TYPE(KeyType), pKey->Param );
			}
		}
	}
}
