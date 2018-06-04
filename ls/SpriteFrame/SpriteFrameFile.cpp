#include "stdafx.h"
#include "SpriteFrameFile.h"
#include "SpriteFrameAnimeData.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"
#include "Resource/TextureResource.h"
#include "Resource/FontResource.h"
#include "Resource/SoundResource.h"
#include "Resource/ResourceUtil.h"

#include "Effect/EffectFile.h"
#include "Effect/EffectManager.h"
#include "Util/XMLLoadUtil.h"


namespace
{
	pstr	NODE_FRAME			= "Frame";
	pstr	NODE_TEX			= "Tex";
	pstr	NODE_PIECE			= "Piece";
	pstr	NODE_BONE			= "Bone";
	pstr	NODE_ANIME			= "Anime";
	pstr	NODE_EFFECT			= "Effect";
	pstr	NODE_SOUND			= "Sound";
	pstr	NODE_ERASE			= "Erase";
	pstr	NODE_FONT			= "Font";
	pstr	NODE_NOTIFY			= "Notify";
	pstr	NODE_COLLIDE		= "Collide";
	pstr	NODE_EMPTY			= "Empty";
	pstr	NODE_POINT			= "Point";
	pstr	NODE_LINE			= "Line";
	pstr	NODE_RECT			= "Rect";
	
	pstr	ATTR_BLEND			= "Blend";
	pstr	ATTR_PHASE			= "Phase";
	pstr	ATTR_NAME			= "Name";
	pstr	ATTR_PATH			= "Path";
	pstr	ATTR_PIECE			= "Piece";
	pstr	ATTR_POS			= "Pos";
	pstr	ATTR_ROT			= "Rot";
	pstr	ATTR_SCALE			= "Scale";
	pstr	ATTR_CENTER			= "Center";
	pstr	ATTR_COLOR			= "Color";
	pstr	ATTR_INDEX			= "Index";
	pstr	ATTR_STEP			= "Step";
	pstr	ATTR_INTERVAL		= "Interval";
	pstr	ATTR_LOOP			= "Loop";
	pstr	ATTR_LOOP_FRAME		= "LoopFrame";
	pstr	ATTR_ATTACH			= "Attach";
	pstr	ATTR_END			= "End";
	pstr	ATTR_FRAME			= "Frame";
	pstr	ATTR_INTP			= "Intp";
	pstr	ATTR_PAN			= "Pan";
	pstr	ATTR_VOL			= "Vol";
	pstr	ATTR_TEXT			= "Text";
	pstr	ATTR_FONT			= "Font";
	pstr	ATTR_ALIGN			= "Align";
	pstr	ATTR_KEY			= "Key";
	pstr	ATTR_VALUE			= "Value";
	pstr	ATTR_RADIUS			= "Radius";
	pstr	ATTR_LENGTH			= "Length";
	pstr	ATTR_WIDTH			= "Width";
	pstr	ATTR_RECT			= "Rect";
	pstr	ATTR_BONE			= "Bone";
	pstr	ATTR_DIR_TYPE		= "DirType";
	pstr	ATTR_ANGLE			= "Angle";
	pstr	ATTR_NUM			= "Num";
	pstr	ATTR_DIVIDE			= "Divide";

	pstr	SCALE_DEFAULT		= "1,1";
	pstr	COLOR_DEFAULT		= "255,255,255,255";
	pstr	ROT_LOOP_DEFAULT	= "true";
	pstr	ANGLE_DEFAULT		= "360";
	pstr	NUM_DEFAULT			= "3";
}

////////////////////////////////////////////////////////////////////////////////
//
//	�X�v���C�g�t���[���F�t�@�C��
//
////////////////////////////////////////////////////////////////////////////////
SpriteFrameFile::SpriteFrameFile( void )
{
	m_Param.BlendMode	= BLEND_ALPHA;
	m_Param.DrawPhase	= 0;
}

SpriteFrameFile::~SpriteFrameFile()
{
	DeleteMap( m_PieceMap );
	DeleteMap( m_AnimeDataMap );
	ReleaseMap( m_TexResMap );
	ReleaseMap( m_FontResMap );
	ReleaseMap( m_EffectResMap );
	ReleaseMap( m_SoundResMap );
}

//******************************************************************************
//	�ǂݍ���
//******************************************************************************
bool SpriteFrameFile::Load( pstr pPath )
{
	//	�t�@�C���ǂݍ���
	XML Xml;
	if( !Xml.Load( pPath ) )
	{
		return false;
	}

	//	�t���[���p�����[�^
	const XML::Node* pFrameNode = Xml.GetRoot().FindChild( NODE_FRAME );
	if( pFrameNode ) _LoadFrameParam( pFrameNode );

	//	�e�N�X�`��
	const XML::Node* pTexNode = Xml.GetRoot().FindChild( NODE_TEX );
	if( pTexNode ) _LoadTex( pTexNode );

	//	�t�H���g
	const XML::Node* pFontNode = Xml.GetRoot().FindChild( NODE_FONT );
	if( pFontNode ) _LoadFont( pFontNode );

	//	�s�[�X
	const XML::Node* pPieceNode = Xml.GetRoot().FindChild( NODE_PIECE );
	if( pPieceNode ) _LoadPiece( pPieceNode );

	//	�{�[��
	const XML::Node* pBoneNode = Xml.GetRoot().FindChild( NODE_BONE );
	if( pBoneNode ) _LoadBone( pBoneNode, -1 );

	//	�A�j��
	const XML::Node* pAnimeNode = Xml.GetRoot().FindChild( NODE_ANIME );
	if( pAnimeNode ) _LoadAnime( pAnimeNode );

	//	������
	const XML::Node* pCollideNode = Xml.GetRoot().FindChild( NODE_COLLIDE );
	if( pCollideNode ) _LoadCollide( pCollideNode );

	return true;
}

//******************************************************************************
//	�{�[���p�����[�^�̎擾
//******************************************************************************	
const SPRITE_FRAME_BONE* SpriteFrameFile::GetBoneParam( int BoneID ) const
{
	_ASSERT( BoneID >= 0 && BoneID < GetBoneNum() );
	return &m_vBoneParam[BoneID];
}

//******************************************************************************
//	�e�N�X�`���̎擾
//******************************************************************************
const Texture* SpriteFrameFile::GetTex( Label Name ) const
{
	TexResMap::const_iterator it = m_TexResMap.find( Name );
	if( it != m_TexResMap.end() )
	{
		return (it->second)->GetData();
	}

	return NULL;
}

//******************************************************************************
//	�t�H���g�̎擾
//******************************************************************************
const Font* SpriteFrameFile::GetFont( Label Name ) const
{
	FontResMap::const_iterator it = m_FontResMap.find( Name );
	if( it != m_FontResMap.end() )
	{
		return (it->second)->GetData();
	}

	return NULL;
}

//******************************************************************************
//	�G�t�F�N�g�t�@�C���̎擾
//******************************************************************************
const EffectFile* SpriteFrameFile::GetEffectFile( Label Name ) const
{
	return FindResourceMap<EffectFile, EffectResource>( m_EffectResMap, Name );
}

//******************************************************************************
//	�T�E���h�t�@�C���̎擾
//******************************************************************************
const SoundFile* SpriteFrameFile::GetSoundFile( Label Name ) const
{
	return FindResourceMap<SoundFile, SoundResource>( m_SoundResMap, Name );
}

//******************************************************************************
//	�s�[�X�̎擾
//******************************************************************************
const Piece* SpriteFrameFile::GetPiece( Label Name ) const
{
	PieceMap::const_iterator it = m_PieceMap.find( Name );
	if( it != m_PieceMap.end() ) return it->second;

	return NULL;
}

//******************************************************************************
//	�A�j���f�[�^�̎擾
//******************************************************************************
const SpriteFrameAnimeData* SpriteFrameFile::GetAnimeData( Label Name ) const
{
	AnimeDataMap::const_iterator it = m_AnimeDataMap.find( Name );
	if( it != m_AnimeDataMap.end() ) return it->second;

	return NULL;
}

//******************************************************************************
//	�u�����h���[�h�̎擾
//******************************************************************************
BLEND_MODE SpriteFrameFile::GetBlendMode( int BoneID ) const
{
	_ASSERT( BoneID >= 0 && BoneID < GetBoneNum() );
	
	BlendMap::const_iterator it = m_BlendMap.find( BoneID );
	if( it != m_BlendMap.end() ) return it->second;

	return m_Param.BlendMode;
}

//******************************************************************************
//	�`��t�F�C�Y�̎擾
//******************************************************************************
int SpriteFrameFile::GetDrawPhase( int BoneID ) const
{
	_ASSERT( BoneID >= 0 && BoneID < GetBoneNum() );
	
	PhaseMap::const_iterator it = m_PhaseMap.find( BoneID );
	if( it != m_PhaseMap.end() ) return it->second;

	return m_Param.DrawPhase;
}

//******************************************************************************
//	������f�[�^���擾
//******************************************************************************
pstr SpriteFrameFile::GetText( int BoneID ) const
{
	_ASSERT( BoneID >= 0 && BoneID < GetBoneNum() );
	
	StringMap::const_iterator it = m_TextMap.find( BoneID );
	if( it != m_TextMap.end() ) return it->second.c_str();

	return "";
}

//******************************************************************************
//	�A���C�������g���擾
//******************************************************************************
FONT_ALIGN SpriteFrameFile::GetAlign( int BoneID ) const
{
	_ASSERT( BoneID >= 0 && BoneID < GetBoneNum() );
	
	AlignMap::const_iterator it = m_AlignMap.find( BoneID );
	if( it != m_AlignMap.end() ) return it->second;

	return FONT_ALIGN_LT;
}

//******************************************************************************
//	�����蔻��f�[�^�擾
//******************************************************************************
const SPRITE_FRAME_COLLIDE* SpriteFrameFile::GetCollide( int BoneID ) const
{
	_ASSERT( BoneID >= 0 && BoneID < GetBoneNum() );
	
	CollideMap::const_iterator it = m_CollideMap.find( BoneID );
	if( it != m_CollideMap.end() ) return &(it->second);

	return NULL;
}

//******************************************************************************
//	�֌`�f�[�^�擾
//******************************************************************************
const SPRITE_FRAME_RING* SpriteFrameFile::GetRing( int BoneID ) const
{
	_ASSERT( BoneID >= 0 && BoneID < GetBoneNum() );
	
	RingMap::const_iterator it = m_RingMap.find( BoneID );
	if( it != m_RingMap.end() ) return &(it->second);

	return NULL;
}

//******************************************************************************
//	���O����{�[���̃C���f�N�X������
//******************************************************************************
int SpriteFrameFile::FindBoneIndex( Label Name ) const
{
	for( int i = 0; i < GetBoneNum(); i++ )
	{
		if( m_vBoneParam[i].Name == Name )
		{
			return i;
		}
	}
	return -1;
}

//******************************************************************************
//	�C���f�N�X����A�j�����𓾂�
//******************************************************************************
Label SpriteFrameFile::GetAnimeName( int Index ) const
{
	AnimeDataMap::const_iterator it = m_AnimeDataMap.begin();
	std::advance( it, Index );

	return it->first;
}

//------------------------------------------------------------------------------
//	�t���[���p�����[�^�̓ǂ݂���
//------------------------------------------------------------------------------
void SpriteFrameFile::_LoadFrameParam( const XML::Node* pNode )
{
	_ASSERT( pNode );

	m_Param.BlendMode = Graphix::StrToBlendMode( pNode->GetAttr( ATTR_BLEND ).GetStr() );
	m_Param.DrawPhase = pNode->GetAttr( ATTR_PHASE ).GetInt();
}

//------------------------------------------------------------------------------
//	�e�N�X�`���̓ǂݍ���
//------------------------------------------------------------------------------
void SpriteFrameFile::_LoadTex( const XML::Node* pNode )
{
	LoadTexNode( m_TexResMap, GetApp()->GetResMng(), pNode );
}

//------------------------------------------------------------------------------
//	�t�H���g�̓ǂݍ���
//------------------------------------------------------------------------------
void SpriteFrameFile::_LoadFont( const XML::Node* pNode )
{
	LoadFontNode( m_FontResMap, GetApp()->GetResMng(), pNode );
}

//------------------------------------------------------------------------------
//	�s�[�X�̓ǂݍ���
//------------------------------------------------------------------------------
void SpriteFrameFile::_LoadPiece( const XML::Node* pNode )
{
	LoadPieceNode( m_PieceMap, pNode );
}

//------------------------------------------------------------------------------
//	�{�[���̓ǂݍ���
//------------------------------------------------------------------------------
int SpriteFrameFile::_LoadBone( const XML::Node* pNode, int ParentID )
{
	_ASSERT( pNode );
	
	int ElderID	= -1;
	int RetChildID	= -1;
	int BoneID = 0;

	const XML::Node* pBoneNode = pNode->FindChild( NODE_BONE );
	while( pBoneNode )
	{
		//	�{�[���p�����[�^�̒ǉ�
		int CurrentID = (int)m_vBoneParam.size();
		{
			SPRITE_FRAME_BONE BoneParam;

			//	���O�擾
			BoneParam.Name		= pBoneNode->GetAttr( ATTR_NAME ).GetStr();

			//	��ʐݒ�
			BoneParam.Type		= SPRITE_FRAME_BONE_DUMMY;

			const XML::Value* pFontAttr = pBoneNode->FindAttr( ATTR_FONT );
			const XML::Value* pPieceAttr = pBoneNode->FindAttr( ATTR_PIECE );

			if( pFontAttr )
			{
				//	�t�H���g
				BoneParam.Type			= SPRITE_FRAME_BONE_FONT;
				BoneParam.SourceName	= pFontAttr->GetStr();
			}
			else if( pPieceAttr )
			{
				BoneParam.SourceName	= pPieceAttr->GetStr();

				SPRITE_FRAME_RING RingParam;
				if( _LoadRing( RingParam, pBoneNode ) )
				{
					//	�֌`
					BoneParam.Type		= SPRITE_FRAME_BONE_RING;

					m_RingMap.insert( RingMap::value_type( CurrentID, RingParam ) );
				}
				else
				{
					//	��`
					BoneParam.Type		= SPRITE_FRAME_BONE_RECT;
				}
			}

			BoneParam.ID		= CurrentID;
			BoneParam.ParentID	= ParentID;
			BoneParam.SiblingID	= -1;
			BoneParam.ChildID	= -1;
			
			BoneParam.DirType	= _LabelToBoneDirType( pBoneNode->GetAttr( ATTR_DIR_TYPE ).GetLabel() );
			
			//	�p�����[�^���o��
			BoneParam.Pos		= pBoneNode->GetAttr( ATTR_POS ).GetPoint();
			BoneParam.Rot		= pBoneNode->GetAttr( ATTR_ROT ).GetDegree();
			BoneParam.Scale		= pBoneNode->GetAttr( ATTR_SCALE, SCALE_DEFAULT ).GetPoint();
			BoneParam.Center	= pBoneNode->GetAttr( ATTR_CENTER ).GetPoint();
			BoneParam.Col		= pBoneNode->GetAttr( ATTR_COLOR, COLOR_DEFAULT ).GetColor();

			//	�ǉ��ݒ�
			const XML::Value* pBlendVal = pBoneNode->FindAttr( ATTR_BLEND );
			const XML::Value* pPhaseVal = pBoneNode->FindAttr( ATTR_PHASE );
			const XML::Value* pTextVal = pBoneNode->FindAttr( ATTR_TEXT );
			const XML::Value* pAlignVal = pBoneNode->FindAttr( ATTR_ALIGN );
			if( pBlendVal ) m_BlendMap.insert( BlendMap::value_type( CurrentID, Graphix::StrToBlendMode( pBlendVal->GetStr() ) ) );
			if( pPhaseVal ) m_PhaseMap.insert( PhaseMap::value_type( CurrentID, pPhaseVal->GetInt() ) );
			if( pTextVal )	m_TextMap.insert( StringMap::value_type( CurrentID, pTextVal->GetStr() ) );
			if( pAlignVal )	m_AlignMap.insert( AlignMap::value_type( CurrentID, FontSprite::StrToAlign( pAlignVal->GetStr() ) ) );

			//	�G�t�F�N�g�E�T�E���h
			_LoadBoneEffect( BoneParam, pBoneNode );
			_LoadBoneSound( BoneParam, pBoneNode );

			//	�ǉ�
			m_vBoneParam.push_back( BoneParam );
		}

		//	�Z�������
		if( ElderID >= 0 )
		{
			m_vBoneParam[ElderID].SiblingID = CurrentID;
		}
		ElderID = CurrentID;

		//	�e�ɓn���qID
		if(	RetChildID < 0 )
		{
			RetChildID = CurrentID;
		}

		//	�q���̒T��
		int ChildID = _LoadBone( pBoneNode, CurrentID );
		m_vBoneParam[CurrentID].ChildID = ChildID;

		pBoneNode = pNode->FindChild( NODE_BONE, pBoneNode );
		BoneID++;
	}

	return RetChildID;
}

//------------------------------------------------------------------------------
//	�A�j���̓ǂݍ���
//------------------------------------------------------------------------------
void SpriteFrameFile::_LoadAnime( const XML::Node* pNode )
{
	_ASSERT( pNode );
	
	for( int DataID = 0; DataID < pNode->GetChildNum(); DataID++ )
	{
		const XML::Node* pDataNode = pNode->GetChild( DataID );

		Label Name = pDataNode->GetAttr( ATTR_NAME ).GetStr();
		if( Name.IsEmpty() ) continue;

		//	�A�j���f�[�^����
		SpriteFrameAnimeData* pAnimeData = new SpriteFrameAnimeData;
		MEM_CHECK( pAnimeData );

		bool bLoop		= pDataNode->GetAttr( ATTR_LOOP ).GetBool();
		int  End		= pDataNode->GetAttr( ATTR_END ).GetInt();
		int  LoopFrame	= pDataNode->GetAttr( ATTR_LOOP_FRAME ).GetInt();

		pAnimeData->Init( GetBoneNum(), End, bLoop, LoopFrame );

		//	�{�[���f�[�^�ǂݍ���
		_LoadAnimeBone( pDataNode, pAnimeData );

		m_AnimeDataMap.insert( AnimeDataMap::value_type( Name, pAnimeData ) );
	}
}

//------------------------------------------------------------------------------
//	��������̓ǂݍ���
//------------------------------------------------------------------------------
void SpriteFrameFile::_LoadCollide( const XML::Node* pNode )
{
	_ASSERT( pNode );

	for( int DataID = 0; DataID < pNode->GetChildNum(); DataID++ )
	{
		const XML::Node* pCollideNode = pNode->GetChild( DataID );

		//	�{�[��ID�擾
		int BoneID = FindBoneIndex( pCollideNode->GetAttr( ATTR_BONE ).GetStr() );
		if( BoneID < 0 ) continue;

		int CollideType = _LabelToCollideType( pCollideNode->GetName() );
		if( CollideType < 0 ) continue;

		//	�p�����[�^�擾
		SPRITE_FRAME_COLLIDE Collide;
		Collide.Type = SPRITE_FRAME_COLLIDE_TYPE(CollideType);

		switch( Collide.Type )
		{
		case SPRITE_FRAME_COLLIDE_POINT:
			{
				Collide.Point.Radius	= pCollideNode->GetAttr( ATTR_RADIUS ).GetFloat();
			}
			break;
		case SPRITE_FRAME_COLLIDE_LINE:
			{
				Collide.Line.Length		= pCollideNode->GetAttr( ATTR_LENGTH ).GetFloat();
				Collide.Line.Width		= pCollideNode->GetAttr( ATTR_WIDTH ).GetFloat();
			}
			break;
		case SPRITE_FRAME_COLLIDE_RECT:
			{
				Rect Rc = pCollideNode->GetAttr( ATTR_RECT ).GetRect();
				Collide.Rect.L	= Rc.l;
				Collide.Rect.T	= Rc.t;
				Collide.Rect.R	= Rc.r;
				Collide.Rect.B	= Rc.b;
			}
			break;
		}

		m_CollideMap.insert( CollideMap::value_type( BoneID, Collide ) );
	}
}

//------------------------------------------------------------------------------
//	�A�j���{�[���f�[�^�̓ǂݍ���
//------------------------------------------------------------------------------
void SpriteFrameFile::_LoadAnimeBone( const XML::Node* pDataNode, SpriteFrameAnimeData* pData )
{
	for( int i = 0; i < pDataNode->GetChildNum(); i++ )
	{
		const XML::Node* pBoneNode = pDataNode->GetChild( i );
		
		Label Name = pBoneNode->GetAttr( ATTR_NAME ).GetStr();
		if( Name.IsEmpty() ) continue;

		int BoneID = FindBoneIndex( Name );
		if( BoneID < 0 ) continue;

		_LoadAnimeKey( pBoneNode, pData, BoneID );
	}	
}

//------------------------------------------------------------------------------
//	�A�j���L�[�t���[���̓ǂݍ���
//------------------------------------------------------------------------------
void SpriteFrameFile::_LoadAnimeKey( const XML::Node* pBoneNode, SpriteFrameAnimeData* pData, int BoneID )
{
	for( int KeyID = 0; KeyID < pBoneNode->GetChildNum(); KeyID++ )
	{
		const XML::Node* pKeyNode = pBoneNode->GetChild( KeyID );

		int AppearType = _LabelToAppearType( pKeyNode->GetName() );
		
		//	�ʒm
		if( Label(pKeyNode->GetName()) == Label(NODE_NOTIFY) )
		{
			SPRITE_FRAME_NOTIFY Notify;
			Notify.Frame	= pKeyNode->GetAttr( ATTR_FRAME ).GetInt();
			Notify.Key		= pKeyNode->GetAttr( ATTR_KEY ).GetStr();
			Notify.Value	= pKeyNode->GetAttr( ATTR_VALUE ).GetStr();
			
			pData->AddNotifyParam( Notify );
		}
		//	�����p�����[�^
		else if( AppearType >= 0 )
		{
			SPRITE_FRAME_APPEAR Appear;
			Appear.Frame	= pKeyNode->GetAttr( ATTR_FRAME ).GetInt();
			Appear.Type		= SPRITE_FRAME_APPERAR_TYPE( AppearType );

			switch( AppearType )
			{
			case SPRITE_FRAME_APPERAR_EFFECT:				
				Appear.pEffect = new SPRITE_FRAME_EFFECT;
				MEM_CHECK( Appear.pEffect );
				_LoadEffectParam( *Appear.pEffect, pKeyNode );
				break;
				
			case SPRITE_FRAME_APPERAR_SOUND:				
				Appear.pSound = new SPRITE_FRAME_SOUND;
				MEM_CHECK( Appear.pSound );
				_LoadSoundParam( *Appear.pSound, pKeyNode );
				break;

			case SPRITE_FRAME_APPERAR_ERASE:
				pKeyNode->GetAttr( ATTR_NAME ).GetLabelStr( Appear.Name );
				break;
			};
			pData->AddAppearParam( BoneID, Appear );
		}
		//	�L�[�t���[��
		else
		{
			//	��ʎ擾
			int KeyType = _LabelToKeyType( pKeyNode->GetName() );
			if( KeyType < 0 ) continue;

			SPRITE_FRAME_KEY KeyInfo;
			KeyInfo.Frame	= pKeyNode->GetAttr( ATTR_FRAME ).GetInt();
			KeyInfo.Intp	= _LabelToIntpType( pKeyNode->GetAttr( ATTR_INTP ).GetStr() );

			switch( KeyType )
			{
			case SPRITE_FRAME_KEY_POS:
				{
					Point Pt = pKeyNode->GetAttr( ATTR_POS ).GetPoint();
					KeyInfo.Float2[0] = Pt.x; KeyInfo.Float2[1] = Pt.y;
				}
				break;
			case SPRITE_FRAME_KEY_ROT:
				{
					KeyInfo.Rot.Rot		= pKeyNode->GetAttr( ATTR_ROT ).GetDegree();
					KeyInfo.Rot.bLoop	= pKeyNode->GetAttr( ATTR_LOOP, ROT_LOOP_DEFAULT ).GetBool();
				}
				break;
			case SPRITE_FRAME_KEY_SCALE:
				{
					Point Pt = pKeyNode->GetAttr( ATTR_SCALE, SCALE_DEFAULT ).GetPoint();
					KeyInfo.Float2[0] = Pt.x; KeyInfo.Float2[1] = Pt.y;
				}
				break;
			case SPRITE_FRAME_KEY_CENTER:
				{
					Point Pt = pKeyNode->GetAttr( ATTR_CENTER ).GetPoint();
					KeyInfo.Float2[0] = Pt.x; KeyInfo.Float2[1] = Pt.y;
				}
				break;
			case SPRITE_FRAME_KEY_COLOR:
				{
					Color Col = pKeyNode->GetAttr( ATTR_COLOR, COLOR_DEFAULT ).GetColor();
					Col.GetArray( KeyInfo.Col );
				}
				break;
			case SPRITE_FRAME_KEY_INDEX:
				{
					KeyInfo.Index.Start		= pKeyNode->GetAttr( ATTR_INDEX ).GetInt();
					KeyInfo.Index.Step		= pKeyNode->GetAttr( ATTR_STEP ).GetInt();
					KeyInfo.Index.Interval	= pKeyNode->GetAttr( ATTR_INTERVAL, "1" ).GetInt();

					if( KeyInfo.Index.Interval	<= 0 ) KeyInfo.Index.Interval = 1;
				}
				break;
			case SPRITE_FRAME_KEY_PIECE:
				{
					Label Name( pKeyNode->GetAttr( ATTR_PIECE ).GetStr() );
					Name.GetStr( KeyInfo.Name );
				}
				break;
			case SPRITE_FRAME_KEY_TEXT:
				{
					pKeyNode->GetAttr( ATTR_TEXT ).GetStr( KeyInfo.Str );
				}
				break;
			}

			pData->AddKeyFrame( BoneID, KeyType, KeyInfo );
		}
	}
}

//------------------------------------------------------------------------------
//	�{�[���̃G�t�F�N�g�p�����[�^�ǂݍ���
//------------------------------------------------------------------------------
void SpriteFrameFile::_LoadBoneEffect( SPRITE_FRAME_BONE& Bone, const XML::Node* pNode )
{
	const XML::Node* pEffectNode = pNode->FindChild( NODE_EFFECT );
	while( pEffectNode )
	{
		//	���\�[�X�ǂݍ���
		SPRITE_FRAME_EFFECT Effect;
		_LoadEffectParam( Effect, pEffectNode );

		Bone.vEffect.push_back( Effect );

		pEffectNode = pNode->FindChild( NODE_EFFECT, pEffectNode );
	}
}

//------------------------------------------------------------------------------
//	�{�[���̃T�E���h�p�����[�^�ǂݍ���
//------------------------------------------------------------------------------
void SpriteFrameFile::_LoadBoneSound( SPRITE_FRAME_BONE& Bone, const XML::Node* pNode )
{
	const XML::Node* pSoundNode = pNode->FindChild( NODE_SOUND );
	while( pSoundNode )
	{
		//	�ǂݍ���
		SPRITE_FRAME_SOUND Sound;
		_LoadSoundParam( Sound, pSoundNode );

		Bone.vSound.push_back( Sound );

		pSoundNode = pNode->FindChild( NODE_SOUND, pSoundNode );
	}
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�p�����[�^�ǂݍ���
//------------------------------------------------------------------------------
void SpriteFrameFile::_LoadEffectParam( SPRITE_FRAME_EFFECT& Effect, const XML::Node* pNode )
{
	//	���\�[�X�ǂݍ���
	Effect.Name = AddResourceMap<EffectResource>( m_EffectResMap, pNode->GetAttr( ATTR_PATH ).GetStr(), GetApp()->GetResMng() );

	//	�p�����[�^�ݒ�
	Effect.Pos			= pNode->GetAttr( ATTR_POS ).GetPoint();
	Effect.Rot			= pNode->GetAttr( ATTR_ROT ).GetDegree();
	Effect.Scale		= pNode->GetAttr( ATTR_SCALE, SCALE_DEFAULT ).GetPoint();
	Effect.bAttach		= pNode->GetAttr( ATTR_ATTACH ).GetBool();
	Effect.Col			= pNode->GetAttr( ATTR_COLOR, COLOR_DEFAULT ).GetColor();
}

//------------------------------------------------------------------------------
//	�T�E���h�p�����[�^�ǂݍ���
//------------------------------------------------------------------------------
void SpriteFrameFile::_LoadSoundParam( SPRITE_FRAME_SOUND& Sound, const XML::Node* pNode )
{
	//	���\�[�X�ǂݍ���
	Sound.Name		= AddResourceMap<SoundResource>( m_SoundResMap, pNode->GetAttr( ATTR_PATH ).GetStr(), GetApp()->GetResMng() );

	//	�p�����[�^�ݒ�
	Sound.Vol		= pNode->GetAttr( ATTR_VOL, "1.0" ).GetFloat();
	Sound.bPan		= pNode->GetAttr( ATTR_PAN ).GetBool();
	Sound.bLoop		= pNode->GetAttr( ATTR_LOOP ).GetBool();
}

//------------------------------------------------------------------------------
//	�֌`�X�v���C�g�p�����[�^�ǂݍ���
//------------------------------------------------------------------------------
bool SpriteFrameFile::_LoadRing( SPRITE_FRAME_RING& Param, const XML::Node* pNode )
{
	const XML::Value* pRadiusAttr = pNode->FindAttr( ATTR_RADIUS );
	if( !pRadiusAttr ) return false;

	Point Rad = pRadiusAttr->GetPoint();

	Param.RadiusMin		= Rad.x;
	Param.RadiusMax		= Rad.y;
	Param.Ang			= pNode->GetAttr( ATTR_ANGLE, ANGLE_DEFAULT ).GetDegree();
	Param.Num			= pNode->GetAttr( ATTR_NUM, NUM_DEFAULT ).GetInt();
	Param.Divide		= pNode->GetAttr( ATTR_DIVIDE ).GetInt();

	return true;
}

//------------------------------------------------------------------------------
//	�����񂩂�L�[��ʂ֕ϊ�
//------------------------------------------------------------------------------
int SpriteFrameFile::_LabelToKeyType( Label Type )
{
	const Label aType[] =
	{
		ATTR_POS,
		ATTR_ROT,
		ATTR_SCALE,
		ATTR_CENTER,
		ATTR_COLOR,
		ATTR_INDEX,
		ATTR_PIECE,
		ATTR_TEXT,
	};

	return LabelToInt( Type, aType, ArrayNum(aType), -1 );
}

//------------------------------------------------------------------------------
//	�����񂩂甭����ʂ֕ϊ�
//------------------------------------------------------------------------------
int SpriteFrameFile::_LabelToAppearType( Label Type )
{
	const Label aType[] =
	{
		NODE_EFFECT,
		NODE_SOUND,
		NODE_ERASE,
	};

	return LabelToInt( Type, aType, ArrayNum(aType), -1 );
}

//------------------------------------------------------------------------------
//	�����񂩂��Ԏ�ʂ֕ϊ�
//------------------------------------------------------------------------------
SPRITE_FRAME_INTP SpriteFrameFile::_LabelToIntpType( Label Type )
{
	const Label aType[] =
	{
		"Fix",
		"Lerp",
	};

	return SPRITE_FRAME_INTP( LabelToInt( Type, aType, ArrayNum(aType), SPRITE_FRAME_INTP_LERP ) );
}

//------------------------------------------------------------------------------
//	�����񂩂瓖�����ʂ֕ϊ�
//------------------------------------------------------------------------------
int SpriteFrameFile::_LabelToCollideType( Label Type )
{
	const Label aType[] =
	{
		NODE_EMPTY,
		NODE_POINT,
		NODE_LINE,
		NODE_RECT,
	};

	return LabelToInt( Type, aType, ArrayNum(aType), -1 );
}

//------------------------------------------------------------------------------
//	�����񂩂�{�[��������ʂ֕ϊ�
//------------------------------------------------------------------------------
SPRITE_FRAME_BONE_DIR SpriteFrameFile::_LabelToBoneDirType( Label Type )
{
	const Label aType[] =
	{
		"Rel",
		"Fix",
	};

	return SPRITE_FRAME_BONE_DIR( LabelToInt( Type, aType, ArrayNum(aType), SPRITE_FRAME_BONE_DIR_REL ) );
}