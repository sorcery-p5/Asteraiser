#include "stdafx.h"
#include "SpriteFrameAnime.h"
#include "SpriteFrameAnimeData.h"
#include "SpriteFrameFile.h"
#include "SpriteFrameBone.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�X�v���C�g�t���[���A�j��
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
//	������
//******************************************************************************
void SpriteFrameAnime::Init( const SpriteFrameFile* pFile )
{
	m_pFile			= pFile;
	m_pPlayData		= NULL;

	m_FrameCount	= 0.0f;
	m_FrameSpeed	= 1.0f;
	m_bPause		= false;

	//	�{�[����񐶐�
	m_vBoneInfo.clear();
	m_vBoneInfo.resize( m_pFile->GetBoneNum() );

	for( int BoneID = 0; BoneID < m_pFile->GetBoneNum(); BoneID++ )
	{
		BONE_INFO& Info = m_vBoneInfo[BoneID];

		FillArray( Info.aKeyIndex, SPRITE_FRAME_KEY_NUM, -1 );
	}
}

//******************************************************************************
//	�X�V
//******************************************************************************
void SpriteFrameAnime::Update( void )
{
	if( m_bPause ) return;

	if( m_pPlayData )
	{
		//	�I���`�F�b�N
		float End = (float)m_pPlayData->GetEndFrame();
		if( m_pPlayData->GetEndFrame() > 0 )
		{
			//	���[�v����
			if( m_pPlayData->IsLoop() && IsLastFrame() )
			{
				m_FrameCount = (float)((m_FrameSpeed < 0.0f)? End : m_pPlayData->GetLoopFrame());

				_ResetBoneKey();
			}
		}

		//	�������̍X�V
		_UpdateAppear();

		//	�{�[���̃L�[�t���[�����X�V
		_UpdateBoneKey();

		//	�ʒm�̍X�V
		_UpdateNotify();

		//	�t���[���i�߂�
		m_FrameCount = Min( m_FrameCount + m_FrameSpeed, End );
		if( m_IntpCount ) m_IntpCount--;
	}
}

//******************************************************************************
//	�Đ�
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
		
		//	���Đ���Ԃ���Đ�����ꍇ
		if( !bPrevPlay )
		{
			_ResetBoneParam();
		}

		//	�{�[���̃L�[�t���[���C���f�N�X��������
		_ResetBoneKey();

		m_NotifyMap.clear();
	}
}

//******************************************************************************
//	��~
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
//	�ŏI�t���[���܂ōĐ��������H
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
//	���[�v�t���[���ȍ~���ǂ����H
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
//	�{�[���̍s����擾
//******************************************************************************
void SpriteFrameAnime::GetBoneMatrix( Matrix3& Mat, uint BoneID ) const
{
	_ASSERT( BoneID < m_vBoneInfo.size() );

	const BONE_INFO& Info = m_vBoneInfo[BoneID];
	CalcMatrix( Mat, Info.Pos, Info.Rot, Info.Scale );
}

//******************************************************************************
//	�{�[���̒��S�ʒu���擾
//******************************************************************************
Point SpriteFrameAnime::GetBoneCenterPos( uint BoneID ) const
{
	_ASSERT( BoneID < m_vBoneInfo.size() );

	const BONE_INFO& Info = m_vBoneInfo[BoneID];
	return Info.Center;
}

//******************************************************************************
//	�{�[���̐F���擾
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
//	�{�[���̃X�v���C�g�C���f�N�X���擾
//******************************************************************************
uint SpriteFrameAnime::GetBoneSpriteIndex( uint BoneID ) const
{
	_ASSERT( BoneID < m_vBoneInfo.size() );

	const BONE_INFO& Info = m_vBoneInfo[BoneID];
	int KeyIndex	= Info.aKeyIndex[SPRITE_FRAME_KEY_INDEX];
	if( KeyIndex < 0 ) return 0;

	const SPRITE_FRAME_KEY* pKey = m_pPlayData->GetKey( BoneID, SPRITE_FRAME_KEY_INDEX, KeyIndex );

	//	�ŏ��̃L�[�ȑO�̏ꍇ
	int Frame = _GetFrameCount();
	if( Frame < pKey->Frame ) return 0;

	//	�v�Z���ďo��
	return (Frame - pKey->Frame) / pKey->Index.Interval * pKey->Index.Step + pKey->Index.Start;
}

//******************************************************************************
//	�{�[���̃s�[�X�����擾
//******************************************************************************
Label SpriteFrameAnime::GetBonePieceName( uint BoneID ) const
{
	_ASSERT( BoneID < m_vBoneInfo.size() );

	const BONE_INFO& Info = m_vBoneInfo[BoneID];
	int KeyIndex	= Info.aKeyIndex[SPRITE_FRAME_KEY_PIECE];
	if( KeyIndex < 0 ) return Label();

	const SPRITE_FRAME_KEY* pKey = m_pPlayData->GetKey( BoneID, SPRITE_FRAME_KEY_PIECE, KeyIndex );

	//	�ŏ��̃L�[�ȑO�̏ꍇ
	if( _GetFrameCount() < pKey->Frame ) return Label();

	return Label( pKey->Name );
}

//******************************************************************************
//	�{�[���̕�������擾
//******************************************************************************
pstr SpriteFrameAnime::GetBoneText( uint BoneID ) const
{
	_ASSERT( BoneID < m_vBoneInfo.size() );

	const BONE_INFO& Info = m_vBoneInfo[BoneID];
	int KeyIndex	= Info.aKeyIndex[SPRITE_FRAME_KEY_TEXT];
	if( KeyIndex < 0 ) return NULL;

	const SPRITE_FRAME_KEY* pKey = m_pPlayData->GetKey( BoneID, SPRITE_FRAME_KEY_TEXT, KeyIndex );

	//	�ŏ��̃L�[�ȑO�̏ꍇ
	if( _GetFrameCount() < pKey->Frame ) return NULL;

	return pKey->Str;
}

//******************************************************************************
//	�ʒm���b�Z�[�W�擾
//******************************************************************************
pstr SpriteFrameAnime::GetNotify( Label Key ) const
{
	MapNotify::const_iterator it = m_NotifyMap.find( Key );
	if( it != m_NotifyMap.end() ) return (it->second).c_str();

	return NULL;
}

//******************************************************************************
//	�{�[���ɔ�������K�p����
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
//	�}�g���N�X���v�Z
//******************************************************************************
void SpriteFrameAnime::CalcMatrix( Matrix3& Mat, const Point& Pos, float Rot, const Point& Scale )
{
	//	�g�k
	Mat = Matrix3::Scale( Scale );

	//	��]
	if( Rot != 0.0f ) Mat *= Matrix3::RotZ( Rot );

	//	�ʒu
	if( !Pos.IsZero() ) Mat *= Matrix3::Trans( Pos );
}


//------------------------------------------------------------------------------
//	�{�[���̃p�����[�^������������
//------------------------------------------------------------------------------
void SpriteFrameAnime::_ResetBoneParam( void )
{
	for( int BoneID = 0; BoneID < m_pFile->GetBoneNum(); BoneID++ )
	{
		BONE_INFO& Info = m_vBoneInfo[BoneID];

		//	��Ԃ�������
		const SPRITE_FRAME_BONE* pBone = m_pFile->GetBoneParam(BoneID);
		Info.Pos	= pBone->Pos;
		Info.Rot	= pBone->Rot;
		Info.Center	= pBone->Center;
		Info.Scale	= pBone->Scale;
		Info.Col	= pBone->Col;
	}
}

//------------------------------------------------------------------------------
//	�{�[���̃L�[�t���[��������������
//------------------------------------------------------------------------------
void SpriteFrameAnime::_ResetBoneKey( void )
{
	for( int BoneID = 0; BoneID < m_pFile->GetBoneNum(); BoneID++ )
	{
		BONE_INFO& Info = m_vBoneInfo[BoneID];

		Info.AppearIndex = (m_FrameSpeed < 0.0f)? m_pPlayData->GetAppearNum(BoneID)-1 : 0;

		//	�{�[���C���f�N�X��������
		if( m_pPlayData->IsBoneEmpty( BoneID ) )
		{
			//	�{�[�����Ȃ�
			FillArray( Info.aKeyIndex, SPRITE_FRAME_KEY_NUM, -1 );
			continue;
		}

		for( uint KeyType = 0; KeyType < SPRITE_FRAME_KEY_NUM; KeyType++ )
		{
			//	�L�[���Ȃ�
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
//	�{�[���̃L�[�t���[�����X�V����
//------------------------------------------------------------------------------
void SpriteFrameAnime::_UpdateBoneKey( void )
{
	for( int BoneID = 0; BoneID < m_pFile->GetBoneNum(); BoneID++ )
	{
		BONE_INFO& Info = m_vBoneInfo[BoneID];

		//	�{�[�����Ȃ�
		if( m_pPlayData->IsBoneEmpty( BoneID ) )
		{
			continue;
		}

		for( uint KeyType = 0; KeyType < SPRITE_FRAME_KEY_NUM; KeyType++ )
		{
			//	�L�[���Ȃ�
			if( Info.aKeyIndex[KeyType] < 0 )
			{
				continue;
			}

			//	���̃L�[�Ɉڍs����
			int NextIndex = Info.aKeyIndex[KeyType] + 1;
			const SPRITE_FRAME_KEY* pKey = m_pPlayData->GetKey( BoneID, KeyType, NextIndex );
			if( pKey && _GetFrameCount() >= pKey->Frame )
			{
				Info.aKeyIndex[KeyType] = NextIndex;
			}
		}

		//	�{�[���̏�Ԃ��X�V
		if( m_IntpCount )
		{
			//	���
			float Coef = 1.0f / (float)m_IntpCount;

			Info.Pos	= Lerp( Info.Pos,		_GetKeyFloat2( BoneID, SPRITE_FRAME_KEY_POS ), Coef );
			Info.Center	= Lerp( Info.Center,	_GetKeyFloat2( BoneID, SPRITE_FRAME_KEY_CENTER ), Coef );
			Info.Scale	= Lerp( Info.Scale,		_GetKeyFloat2( BoneID, SPRITE_FRAME_KEY_SCALE ), Coef );
			Info.Rot	= Angle::Lerp( Info.Rot, _GetKeyRot( BoneID ), Coef );
			Info.Col	= Color::Lerp( Info.Col, _GetKeyColor( BoneID ), Coef );
		}
		else
		{
			//	�ݒ�
			Info.Pos	= _GetKeyFloat2( BoneID, SPRITE_FRAME_KEY_POS );
			Info.Center	= _GetKeyFloat2( BoneID, SPRITE_FRAME_KEY_CENTER );
			Info.Scale	= _GetKeyFloat2( BoneID, SPRITE_FRAME_KEY_SCALE );
			Info.Rot	= _GetKeyRot( BoneID );
			Info.Col	= _GetKeyColor( BoneID );
		}
	}
}

//------------------------------------------------------------------------------
//	�ʒm�̍X�V
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
//	�������̍X�V
//------------------------------------------------------------------------------
void SpriteFrameAnime::_UpdateAppear( void )
{
	for( int BoneID = 0; BoneID < m_pFile->GetBoneNum(); BoneID++ )
	{
		BONE_INFO& Info = m_vBoneInfo[BoneID];

		Info.vAppear.clear();

		//	�p�����[�^�擾
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
//	���݂̃L�[�t���[���̃t���[�g�l���擾����
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

	//	�ŏ��̃L�[�ȑO�̏ꍇ
	int Frame = _GetFrameCount();
	if( Frame < pKey->Frame ) return Ret;

	//	�Œ�l�̏ꍇ
	if( pKey->Intp == SPRITE_FRAME_INTP_FIX ) return Point( pKey->Float2 );

	const SPRITE_FRAME_KEY* pNextKey = m_pPlayData->GetKey( BoneID, KeyType, KeyIndex + 1 );

	//	�ŏI�L�[�ȍ~�̏ꍇ
	if( !pNextKey ) return Point( pKey->Float2 );

	//	��Ԃ���
	return Lerp( Point(pKey->Float2), Point(pNextKey->Float2), Coef(m_FrameCount, (float)pKey->Frame, (float)pNextKey->Frame) );
}

//------------------------------------------------------------------------------
//	���݂̃L�[�t���[���̊p�x
//------------------------------------------------------------------------------
Angle SpriteFrameAnime::_GetKeyRot( uint BoneID ) const
{
	_ASSERT( BoneID < m_vBoneInfo.size() );

	Angle Rot = m_pFile->GetBoneParam(BoneID)->Rot;

	const BONE_INFO& Info = m_vBoneInfo[BoneID];
	int KeyIndex	= Info.aKeyIndex[SPRITE_FRAME_KEY_ROT];
	if( KeyIndex < 0 ) return Rot;

	const SPRITE_FRAME_KEY* pKey = m_pPlayData->GetKey( BoneID, SPRITE_FRAME_KEY_ROT, KeyIndex );

	//	�ŏ��̃L�[�ȑO�̏ꍇ
	int Frame = _GetFrameCount();
	if( Frame < pKey->Frame ) return Rot;

	//	�Œ�L�[�̏ꍇ
	if( pKey->Intp == SPRITE_FRAME_INTP_FIX ) return Angle( pKey->Rot.Rot );

	const SPRITE_FRAME_KEY* pNextKey = m_pPlayData->GetKey( BoneID, SPRITE_FRAME_KEY_ROT, KeyIndex + 1 );

	//	�ŏI�L�[�ȍ~�̏ꍇ
	if( !pNextKey ) return Angle( pKey->Rot.Rot );

	//	��Ԃ���
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
//	���݂̃L�[�t���[���̐F���擾����
//------------------------------------------------------------------------------
Color SpriteFrameAnime::_GetKeyColor( uint BoneID ) const
{
	_ASSERT( BoneID < m_vBoneInfo.size() );

	Color Col = m_pFile->GetBoneParam(BoneID)->Col;

	const BONE_INFO& Info = m_vBoneInfo[BoneID];
	int KeyIndex	= Info.aKeyIndex[SPRITE_FRAME_KEY_COLOR];
	if( KeyIndex < 0 ) return Col;

	const SPRITE_FRAME_KEY* pKey = m_pPlayData->GetKey( BoneID, SPRITE_FRAME_KEY_COLOR, KeyIndex );

	//	�ŏ��̃L�[�ȑO�̏ꍇ
	int Frame = _GetFrameCount();
	if( Frame < pKey->Frame ) return Col;

	//	�Œ�l�̏ꍇ
	if( pKey->Intp == SPRITE_FRAME_INTP_FIX ) return Color( pKey->Col );

	const SPRITE_FRAME_KEY* pNextKey = m_pPlayData->GetKey( BoneID, SPRITE_FRAME_KEY_COLOR, KeyIndex + 1 );

	//	�ŏI�L�[�ȍ~�̏ꍇ
	if( !pNextKey ) return Color( pKey->Col );

	//	��Ԃ���
	return Color::Lerp( pKey->Col, pNextKey->Col, Coef(m_FrameCount, (float)pKey->Frame, (float)pNextKey->Frame) );
}