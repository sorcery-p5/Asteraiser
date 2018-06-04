#include "stdafx.h"
#include "SpriteFrameAnimeData.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�X�v���C�g�t���[���F�A�j���f�[�^
//
////////////////////////////////////////////////////////////////////////////////
SpriteFrameAnimeData::SpriteFrameAnimeData( void )
{
	m_EndFrame	= 0;
	m_bLoop		= false;
	m_LoopFrame	= 0;
}

SpriteFrameAnimeData::~SpriteFrameAnimeData()
{
	_Delete();
}

//******************************************************************************
//	������
//******************************************************************************
void SpriteFrameAnimeData::Init( uint BoneNum, int EndFrame, bool bLoop, int LoopFrame )
{
	_Delete();

	m_EndFrame	= EndFrame;
	m_bLoop		= bLoop;
	m_LoopFrame	= LoopFrame;

	//	�̈�m��
	m_vBoneKey.assign( BoneNum, NULL );
	m_vAppear.resize( BoneNum );
}

//******************************************************************************
//	�L�[�t���[���ǉ�
//******************************************************************************
void SpriteFrameAnimeData::AddKeyFrame( uint BoneID, uint KeyType, const SPRITE_FRAME_KEY& Key )
{
	_ASSERT( BoneID < m_vBoneKey.size() );

	BONE_KEY* pBoneKey = m_vBoneKey[ BoneID ];

	//	���o�^�̏ꍇ�A�{�[���o�^
	if( !pBoneKey )
	{
		pBoneKey = new BONE_KEY;
		MEM_CHECK( pBoneKey );

		m_vBoneKey[ BoneID ] = pBoneKey;
	}

	pBoneKey->aKey[KeyType].push_back( Key );

	//	�ŏI�t���[���X�V
 	m_EndFrame = Max( m_EndFrame, Key.Frame );
}

//******************************************************************************
//	�����p�����[�^�ǉ�
//******************************************************************************
void SpriteFrameAnimeData::AddAppearParam( uint BoneID, const SPRITE_FRAME_APPEAR& Param )
{
	_ASSERT( BoneID < m_vAppear.size() );

	VecAppear& vAppear = m_vAppear[ BoneID ];

	vAppear.push_back( Param );

	//	�ŏI�t���[���X�V
 	m_EndFrame = Max( m_EndFrame, Param.Frame );
}

//******************************************************************************
//	�ʒm�p�����[�^�ǉ�
//******************************************************************************
void SpriteFrameAnimeData::AddNotifyParam( const SPRITE_FRAME_NOTIFY& Param )
{
	m_vNotify.push_back( Param );

	//	�ŏI�t���[���X�V
 	m_EndFrame = Max( m_EndFrame, Param.Frame );
}

//******************************************************************************
//	�{�[�����o�^����Ă��邩�H
//******************************************************************************
bool SpriteFrameAnimeData::IsBoneEmpty( uint BoneID ) const
{
	_ASSERT( BoneID < m_vBoneKey.size() );
	return m_vBoneKey[ BoneID ] == NULL;
}

//******************************************************************************
//	�{�[���ɃL�[���o�^����Ă��邩�H
//******************************************************************************
bool SpriteFrameAnimeData::IsKeyEmpty( uint BoneID, uint KeyType ) const
{
	_ASSERT( BoneID < m_vBoneKey.size() );

	BONE_KEY* pBoneKey = m_vBoneKey[ BoneID ];
	if( !pBoneKey ) return false;

	return pBoneKey->aKey[KeyType].empty();
}

//******************************************************************************
//	�L�[�t���[���̎擾
//******************************************************************************
const SPRITE_FRAME_KEY*	SpriteFrameAnimeData::GetKey( uint BoneID, uint KeyType, uint Index ) const
{
	_ASSERT( BoneID < m_vBoneKey.size() );

	const BONE_KEY* pBoneKey = m_vBoneKey[ BoneID ];
	if( !pBoneKey ) return NULL;

	if( Index >= pBoneKey->aKey[KeyType].size() ) return NULL;

	return &pBoneKey->aKey[KeyType].at(Index);
}

//******************************************************************************
//	�����p�����[�^�̐����擾
//******************************************************************************
uint SpriteFrameAnimeData::GetAppearNum( uint BoneID ) const
{
	return m_vAppear[ BoneID ].size();
}

//******************************************************************************
//	�����p�����[�^���擾
//******************************************************************************
const SPRITE_FRAME_APPEAR* SpriteFrameAnimeData::GetAppear( uint BoneID, uint Index ) const
{
	_ASSERT( BoneID < m_vAppear.size() );

	const VecAppear& vAppear = m_vAppear[ BoneID ];

	if( Index >= vAppear.size() ) return NULL;

	return &vAppear.at(Index);
}

//******************************************************************************
//	�ʒm�p�����[�^�̐����擾
//******************************************************************************
uint SpriteFrameAnimeData::GetNotifyNum( void ) const
{
	return m_vNotify.size();
}

//******************************************************************************
//	�ʒm�p�����[�^���擾
//******************************************************************************
const SPRITE_FRAME_NOTIFY* SpriteFrameAnimeData::GetNotify( uint Index ) const
{
	if( Index >= m_vNotify.size() ) return NULL;

	return &m_vNotify.at(Index);
}

//------------------------------------------------------------------------------
//	�폜
//------------------------------------------------------------------------------
void SpriteFrameAnimeData::_Delete( void )
{
	DeleteVec( m_vBoneKey );

	for( VecBoneAppear::iterator it = m_vAppear.begin(); it != m_vAppear.end(); it++ )
	{
		for( uint i = 0; i < it->size(); i++ )
		{
			if( it->at(i).Type == SPRITE_FRAME_APPERAR_EFFECT ) SafeDelete( it->at(i).pEffect );
			if( it->at(i).Type == SPRITE_FRAME_APPERAR_SOUND )  SafeDelete( it->at(i).pSound );
		}
	}
}