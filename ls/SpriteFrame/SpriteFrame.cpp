#include "stdafx.h"
#include "SpriteFrame.h"
#include "SpriteFrameBone.h"
#include "SpriteFrameFile.h"
#include "SpriteFrameAnime.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�X�v���C�g�t���[��
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
//	������
//*****************************************************************************
void SpriteFrame::Init( const SpriteFrameFile* pFile, const Matrix3& BaseMat )
{
	_ASSERT( pFile );
	m_pFile = pFile;

	//	�{�[������
	for( int i = 0; i < pFile->GetBoneNum(); i++ )
	{
		SpriteFrameBone* pBone = _CreateBone( i );
		m_vBone.push_back( pBone );
	}
	
	//	�A�j������
	if( pFile->HasAnime() )
	{
		m_pAnime = new SpriteFrameAnime;
		MEM_CHECK( m_pAnime );
		
		m_pAnime->Init( pFile );
	}

	//	���[���h�s��̌v�Z
	if( !m_vBone.empty() )
	{
		_CalcWorldMatrix( m_vBone.front(), BaseMat );
	}

	//	�풓�G�t�F�N�g�E�T�E���h��������
	for( VecBone::iterator it = m_vBone.begin(); it != m_vBone.end(); ++it )
	{
		(*it)->InitResidentAppear();
	}

	m_BaseMat = BaseMat;
}

//******************************************************************************
//	�X�V
//******************************************************************************
void SpriteFrame::Update( const Matrix3* pBaseMat )
{
	bool bAnime = false;

	//	�A�j���X�V
	if( m_pAnime )
	{
		m_pAnime->Update();

		if( m_pAnime->IsPlaying() )
		{
			//	�{�[���̃��[�J���s����X�V
			for( VecBone::iterator it = m_vBone.begin(); it != m_vBone.end(); ++it )
			{
				(*it)->ApplyAnime( m_pAnime );
			}

			bAnime = true;
		}
	}

	//	���[���h�s��̌v�Z
	if( bAnime || pBaseMat )
	{
		if( pBaseMat ) m_BaseMat = *pBaseMat;

		if( !m_vBone.empty() )
		{
			_CalcWorldMatrix( m_vBone.front(), m_BaseMat );
		}
	}
	
	//	�{�[���X�V
	for( VecBone::iterator it = m_vBone.begin(); it != m_vBone.end(); ++it )
	{
		(*it)->Update();
		
		//	������
		if( m_pAnime )
		{
			m_pAnime->ApplyAppear( *it );
		}
	}
}

//******************************************************************************
//	�`��
//******************************************************************************
void SpriteFrame::Draw( const Matrix3& OffsetMat )
{
	for( VecBone::iterator it = m_vBone.begin(); it != m_vBone.end(); ++it )
	{
		(*it)->Draw( OffsetMat );
	}	
}

//******************************************************************************
//	�A�j���̕ύX
//******************************************************************************
void SpriteFrame::ChangeAnime( Label Name, uint Intp )
{
	SafePtr(m_pAnime)->Play( Name, Intp );

	//	������������
	for( VecBone::iterator it = m_vBone.begin(); it != m_vBone.end(); ++it )
	{
		(*it)->ResetAppear();
	}
}

//******************************************************************************
//	�A�j����������Ԃɖ߂�
//******************************************************************************
void SpriteFrame::ResetAnime( void )
{
	if( m_pAnime ) m_pAnime->Stop();

	//	�{�[���X�V
	for( VecBone::iterator it = m_vBone.begin(); it != m_vBone.end(); ++it )
	{
		(*it)->ResetAnime();
	}
	
	//	���[���h�s��Čv�Z
	if( !m_vBone.empty() ) _CalcWorldMatrix( m_vBone.front(), m_BaseMat );
}

//******************************************************************************
//	�A�j�����ꎞ��~
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
//	�A�j���̑��x�ύX
//******************************************************************************
void SpriteFrame::SetAnimeSpeed( float Speed )
{
	if( m_pAnime ) m_pAnime->SetFrameSpeed( Speed );
}

//******************************************************************************
//	�A�j���Đ������ǂ���
//******************************************************************************
bool SpriteFrame::IsAnimePlaying( void ) const
{
	if( !m_pAnime ) return false;

	return m_pAnime->IsPlaying();
}

//******************************************************************************
//	�A�j���I���t���[���ɒB�������ǂ���
//******************************************************************************
bool SpriteFrame::IsAnimeLast( void ) const
{
	if( !m_pAnime ) return true;

	return m_pAnime->IsLastFrame();
}

//******************************************************************************
//	�A�j�������[�v�t���[���ȍ~���ǂ���
//******************************************************************************
bool SpriteFrame::IsAnimeLoopFrame( void ) const
{
	if( !m_pAnime ) return false;

	return m_pAnime->IsAnimeLoopFrame();
}

//******************************************************************************
//	�Đ����̃A�j�������擾
//******************************************************************************
Label SpriteFrame::GetAnimeName( void ) const
{
	if( !m_pAnime ) return Label();

	return m_pAnime->GetAnimeName();
}

//******************************************************************************
//	�A�j���ʒm���擾
//******************************************************************************
pstr SpriteFrame::GetAnimeNotify( Label Key ) const
{
	if( !m_pAnime ) return NULL;

	return m_pAnime->GetNotify( Key );
}

//******************************************************************************
//	�w��̃A�j�������邩
//******************************************************************************
bool SpriteFrame::HasAnime( Label Name ) const
{
	if( !m_pAnime ) return false;

	return m_pFile->GetAnimeData( Name ) != NULL;
}

//******************************************************************************
//	�w��̃A�j�������邩
//******************************************************************************
int SpriteFrame::GetAnimeNum( void ) const
{
	if( !m_pAnime ) return 0;

	return m_pFile->GetAnimeNum();
}

//******************************************************************************
//	�C���f�N�X����A�j�����𓾂�
//******************************************************************************
Label SpriteFrame::GetAnimeName( int Index ) const
{
	if( !m_pAnime || Index >= GetAnimeNum() ) return Label();

	return m_pFile->GetAnimeName( Index );
}

//******************************************************************************
//	�{�[��������
//******************************************************************************
int SpriteFrame::FindBone( Label Name ) const
{
	_ASSERT( m_pFile );

	return m_pFile->FindBoneIndex( Name );
}

//******************************************************************************
//	�{�[���̃}�g���N�X���擾(�C���f�N�X)
//******************************************************************************
const Matrix3& SpriteFrame::GetBoneMatrix( int Index ) const
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	return m_vBone[Index]->GetWorldMatrix();
}

//******************************************************************************
//	�{�[���̃}�g���N�X���擾(���O)
//******************************************************************************
const Matrix3* SpriteFrame::GetBoneMatrix( Label Name ) const
{
	_ASSERT( m_pFile );

	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) return &GetBoneMatrix( Index );

	return NULL;
}

//******************************************************************************
//	�{�[���̃��[�J���}�g���N�X���擾(�C���f�N�X)
//******************************************************************************
const Matrix3& SpriteFrame::GetBoneLocalMatrix( int Index ) const
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	return m_vBone[Index]->GetLocalMatrix();
}
	
//******************************************************************************
//	�{�[���̃��[�J���}�g���N�X���擾(���O)
//******************************************************************************
const Matrix3* SpriteFrame::GetBoneLocalMatrix( Label Name ) const
{
	_ASSERT( m_pFile );

	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) return &GetBoneLocalMatrix( Index );

	return NULL;
}

//******************************************************************************
//	�{�[���̐F���擾(�C���f�N�X)
//******************************************************************************
Color SpriteFrame::GetBoneColor( int Index ) const
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	return m_vBone[Index]->GetColor( m_pAnime );
}
	
//******************************************************************************
//	�{�[���̐F���擾(���O)
//******************************************************************************
Color SpriteFrame::GetBoneColor( Label Name ) const
{
	_ASSERT( m_pFile );

	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) return GetBoneColor( Index );

	return Color();
}

//******************************************************************************
//	�{�[���̋�`���擾
//******************************************************************************
Rect SpriteFrame::GetBoneRect( Label Name ) const
{
	_ASSERT( m_pFile );

	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) return m_vBone[Index]->GetRect();

	return Rect(); 
}

//******************************************************************************
//	�����蔻������擾(�C���f�N�X)
//******************************************************************************
SPRITE_FRAME_COLLIDE SpriteFrame::GetCollide( int Index ) const
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	_ASSERT( m_pFile );

	const SPRITE_FRAME_COLLIDE* pCollide = m_pFile->GetCollide( Index );
	if( pCollide ) return *pCollide;

	//	�X�v���C�g����T�C�Y�擾.
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
//	�`��̐ݒ�
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
//	�{�[���̕`��ݒ��ύX�i�C���f�N�X�j
//******************************************************************************
void SpriteFrame::SetBoneVisible( int Index, bool bVisible )
{	
	m_vBone[Index]->SetVisible( bVisible );
}

//******************************************************************************
//	�{�[���̕`��ݒ��ύX�i���O�j
//******************************************************************************
void SpriteFrame::SetBoneVisible( Label Name, bool bVisible )
{
	_ASSERT( m_pFile );

	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) SetBoneVisible( Index, bVisible );	
}

//******************************************************************************
//	�␳�F��ݒ�
//******************************************************************************
void SpriteFrame::SetColor( Color Col )
{
	m_Color = Col;
	
	//	�{�[���X�V
	for( VecBone::iterator it = m_vBone.begin(); it != m_vBone.end(); ++it )
	{
		(*it)->ApplyColor( m_pAnime && m_pAnime->IsPlaying()? m_pAnime : NULL );
	}
}

//******************************************************************************
//	�{�[���̕␳�F��ݒ�i�C���f�N�X�j
//******************************************************************************
void SpriteFrame::SetBoneColor( int Index, Color Col )
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	_ASSERT( m_pFile );

	m_vBone[Index]->SetColor( Col, m_pAnime && m_pAnime->IsPlaying()? m_pAnime : NULL );
}

//******************************************************************************
//	�{�[���̕␳�F��ݒ�i���O�j
//******************************************************************************
void SpriteFrame::SetBoneColor( Label Name, Color Col )
{
	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) SetBoneColor( Index, Col );	
}

//******************************************************************************
//	�␳�F��ݒ�
//******************************************************************************
void SpriteFrame::SetAlpha( byte Alpha )
{
	m_Color.a = Alpha;
	SetColor( m_Color );
}

//******************************************************************************
//	�`��t�F�C�Y��ݒ�(�S��)
//******************************************************************************
void SpriteFrame::SetDrawPhase( uint Phase )
{
	for( VecBone::iterator it = m_vBone.begin(); it != m_vBone.end(); ++it )
	{
		(*it)->SetDrawPhase( Phase );
	}
}

//******************************************************************************
//	�`��t�F�C�Y��ݒ�(�{�[���w��)
//******************************************************************************
void SpriteFrame::SetDrawPhase( int Index, int Phase )
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	m_vBone[Index]->SetDrawPhase( Phase );
}

//******************************************************************************
//	�{�[���̕�����ݒ�i�C���f�N�X�j
//******************************************************************************
void SpriteFrame::SetBoneText( int Index, pstr pStr )
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	m_vBone[Index]->SetText( pStr );
}

//******************************************************************************
//	�{�[���̕�����ݒ�i���O�j
//******************************************************************************
void SpriteFrame::SetBoneText( Label Name, pstr pStr )
{
	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) SetBoneText( Index, pStr );	
}

//******************************************************************************
//	�{�[���̒��S�p�x�ݒ�i�C���f�N�X�j
//******************************************************************************
void SpriteFrame::SetBoneAngle( int Index, float Ang )
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	m_vBone[Index]->SetAngle( Ang );
}

//******************************************************************************
//	�{�[���̒��S�p�x�ݒ�i���O�j
//******************************************************************************
void SpriteFrame::SetBoneAngle( Label Name, float Ang )
{
	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) SetBoneAngle( Index, Ang );	
}

//******************************************************************************
//	�{�[���̃s�[�X�C���f�N�X��ݒ�i�C���f�N�X�j
//******************************************************************************
void SpriteFrame::SetBonePieceIndex( int Index, int PieceIndex )
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	m_vBone[Index]->SetPieceIndex( PieceIndex );
}

//******************************************************************************
//	�{�[���̃s�[�X�C���f�N�X��ݒ�i���O�j
//******************************************************************************
void SpriteFrame::SetBonePieceIndex( Label Name, int PieceIndex )
{
	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) SetBonePieceIndex( Index, PieceIndex );
}

//******************************************************************************
//	�{�[���̈ʒu�ݒ�i�C���f�N�X�j
//******************************************************************************
void SpriteFrame::SetBonePos( int Index, Point Pos )
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	m_vBone[Index]->SetPos( Pos );
	
	//	���[���h�s��Čv�Z
	_CalcWorldMatrix( m_vBone.front(), m_BaseMat );
}

//******************************************************************************
//	�{�[���̈ʒu�ݒ�i���O�j
//******************************************************************************
void SpriteFrame::SetBonePos( Label Name, Point Pos )
{
	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) SetBonePos( Index, Pos );	
}

//******************************************************************************
//	�{�[���̉�]�ݒ�i�C���f�N�X�j
//******************************************************************************
void SpriteFrame::SetBoneRot( int Index, float Rot )
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	m_vBone[Index]->SetRot( Rot );
	
	//	���[���h�s��Čv�Z
	_CalcWorldMatrix( m_vBone.front(), m_BaseMat );
}

//******************************************************************************
//	�{�[���̉�]�ݒ�i���O�j
//******************************************************************************
void SpriteFrame::SetBoneRot( Label Name, float Rot )
{
	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) SetBoneRot( Index, Rot );	
}

//******************************************************************************
//	�{�[���̊g�k�ݒ�i�C���f�N�X�j
//******************************************************************************
void SpriteFrame::SetBoneScale( int Index, Point Scale )
{
	_ASSERT( Index >= 0 && Index < GetBoneNum() );
	m_vBone[Index]->SetScale( Scale );

	//	���[���h�s��Čv�Z
	_CalcWorldMatrix( m_vBone.front(), m_BaseMat );
}

//******************************************************************************
//	�{�[���̊g�k�ݒ�i���O�j
//******************************************************************************
void SpriteFrame::SetBoneScale( Label Name, Point Scale )
{
	int Index = m_pFile->FindBoneIndex( Name );
	if( Index >= 0 ) SetBoneScale( Index, Scale );	
}

//------------------------------------------------------------------------------
//	�{�[������
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
//	�{�[���̃��[���h�s����v�Z
//------------------------------------------------------------------------------
void SpriteFrame::_CalcWorldMatrix( SpriteFrameBone* pBone, const Matrix3& Mat )
{
	_ASSERT( pBone );

	//	�Z���H��
	if( pBone->GetSiblingID() >= 0 )
	{
		_CalcWorldMatrix( m_vBone[ pBone->GetSiblingID() ], Mat );
	}

	pBone->CalcWorldMatrix( Mat );

	//	�q��H��
	if( pBone->GetChildID() >= 0 )
	{
		_CalcWorldMatrix( m_vBone[ pBone->GetChildID() ], pBone->GetWorldMatrix() );
	}
}