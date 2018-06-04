#include "stdafx.h"
#include "EffectFile.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"
#include "Resource/TextureResource.h"
#include "Resource/FontResource.h"
#include "Resource/SoundResource.h"
#include "Resource/ResourceUtil.h"
#include "Util/XMLLoadUtil.h"

#include "SpriteFrame/SpriteFrameFile.h"

#include "TimeKey/TimeKey.h"


namespace
{	
	pstr	NODE_EFFECT				= "Effect";
	pstr	NODE_TEX				= "Tex";
	pstr	NODE_PIECE				= "Piece";
	pstr	NODE_PARTICLE			= "Particle";
	pstr	NODE_APPEAR				= "Appear";

	pstr	NODE_DUMMY				= "Dummy";
	pstr	NODE_SPRITE				= "Sprite";
	pstr	NODE_FRAME				= "Frame";
	pstr	NODE_LINE				= "Line";
	pstr	NODE_FONT				= "Font";
	pstr	NODE_SOUND				= "Sound";
	pstr	NODE_VELOCITY			= "Velocity";
	pstr	NODE_FOLLOW				= "Follow";
	pstr	NODE_CIRCULAR			= "Circular";
	pstr	NODE_LINENODE			= "LineNode";
	pstr	NODE_TIMEKEY			= "TimeKey";

	pstr	ATTR_TYPE				= "Type";
	pstr	ATTR_END				= "End";
	pstr	ATTR_LOOP				= "Loop";
	pstr	ATTR_LOOP_FRAME			= "LoopFrame";
	pstr	ATTR_PHASE				= "Phase";
	pstr	ATTR_DIR_TYPE			= "DirType";
	pstr	ATTR_NAME				= "Name";
	pstr	ATTR_PATH				= "Path";
	pstr	ATTR_PIECE				= "Piece";
	pstr	ATTR_COLOR				= "Color";
	pstr	ATTR_BLEND				= "Blend";
	pstr	ATTR_INDEX				= "Index";
	pstr	ATTR_ID					= "ID";
	pstr	ATTR_CENTER				= "Center";
	pstr	ATTR_INTERVAL			= "Interval";
	pstr	ATTR_STEP				= "Step";
	pstr	ATTR_NUM				= "Num";
	pstr	ATTR_DIVIDE				= "Divide";
	pstr	ATTR_WIDTH				= "Width";
	pstr	ATTR_NODE				= "Node";
	pstr	ATTR_FONT				= "Font";
	pstr	ATTR_TEXT				= "Text";
	pstr	ATTR_ALIGN				= "Align";
	pstr	ATTR_VELOCITY			= "Velocity";
	pstr	ATTR_VOL				= "Vol";
	pstr	ATTR_PAN				= "Pan";
	pstr	ATTR_NO_PAUSE			= "NoPause";

	pstr	ATTR_DECEL				= "Decel";
	pstr	ATTR_GRAVITY			= "Gravity";
	pstr	ATTR_FORCE				= "Force";
	pstr	ATTR_FLEX				= "Flex";
	pstr	ATTR_HIT				= "Hit";

	pstr	ATTR_FRAME				= "Frame";
	pstr	ATTR_PARTICLE			= "Particle";
	pstr	ATTR_ANIME				= "Anime";
	pstr	ATTR_DIR				= "Dir";
	pstr	ATTR_DIST				= "Dist";
	pstr	ATTR_SPEED				= "Speed";
	pstr	ATTR_MOVE_RANDOM		= "MoveRandom";
	pstr	ATTR_POS				= "Pos";
	pstr	ATTR_POS_RANGE			= "PosRange";
	pstr	ATTR_SCALE				= "Scale";
	pstr	ATTR_ROT				= "Rot";
	pstr	ATTR_ROTSPEED			= "RotSpeed";
	pstr	ATTR_ROT_RANDOM			= "RotRandom";
	pstr	ATTR_ANGLE				= "Angle";
	pstr	ATTR_ANGLESPEED			= "AngleSpeed";
	pstr	ATTR_AXIS_SCALE			= "AxisScale";
	pstr	ATTR_AXIS_SCALE_RANGE	= "AxisScaleRange";
	pstr	ATTR_AXIS_ANGLE			= "AxisAngle";
	pstr	ATTR_INTP				= "Intp";
	pstr	ATTR_UPDATE_ENABLE		= "UpdateEnable";
	pstr	ATTR_FLAG				= "Flag";

	pstr	COLOR_DEFAULT			= "255,255,255,255";
	pstr	SCALE_DEFAULT			= "1,1";


	pstr	EFFECT_PARTICLE_LINE_TABLE[] =
	{
		"CENTER",
		"TIP",
	};

};


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクトファイル
//
////////////////////////////////////////////////////////////////////////////////
EffectFile::EffectFile( void )
{
	m_pParent = NULL;
}
EffectFile::~EffectFile()
{
	DeleteMap( m_PieceMap );
	ReleaseMap( m_TexResMap );
	ReleaseMap( m_SpriteFrameResMap );
	ReleaseMap( m_FontResMap );
	ReleaseMap( m_SoundResMap );
	ReleaseMap( m_EffectResMap );
	DeleteMap( m_EffectFileMap );
	DeleteMap( m_ParticleMap );
}

//******************************************************************************
//	読み込み
//******************************************************************************
bool EffectFile::Load( pstr pPath )
{
	//	ファイル読み込む
	XML Xml;
	if( !Xml.Load( pPath ) )
	{
		return false;
	}
	
	//	読み込み
	_Load( &Xml.GetRoot() );

	return true;
}

//******************************************************************************
//	子エフェクトとして読み込み
//******************************************************************************
void EffectFile::LoadChild( const EffectFile* pParent, const XML::Node* pNode )
{
	_ASSERT( pParent && pNode );

	m_pParent = pParent;

	//	読み込み
	_Load( pNode );
}

//******************************************************************************
//	テクスチャの取得
//******************************************************************************
const Texture* EffectFile::GetTex( Label Name ) const
{
	const Texture* pTex = FindResourceMap<Texture, TextureResource>( m_TexResMap, Name );

	//	親のリソース取得
	if( !pTex )
	{
		if( m_pParent ) pTex = m_pParent->GetTex( Name );
	}
	return pTex;
}

//******************************************************************************
//	スプライトフレームの取得
//******************************************************************************
const SpriteFrameFile* EffectFile::GetSpriteFrame( Label Name )	const
{
	const SpriteFrameFile* pSpriteFrame = FindResourceMap<SpriteFrameFile, SpriteFrameResource>( m_SpriteFrameResMap, Name );
	
	//	親のリソース取得
	if( !pSpriteFrame )
	{
		if( m_pParent ) pSpriteFrame = m_pParent->GetSpriteFrame( Name );
	}
	return pSpriteFrame;
}

//******************************************************************************
//	フォントの取得
//******************************************************************************
const Font* EffectFile::GetFont( Label Name ) const
{
	const Font* pFont = FindResourceMap<Font, FontResource>( m_FontResMap, Name );

	//	親のリソース取得
	if( !pFont )
	{
		if( m_pParent ) pFont = m_pParent->GetFont( Name );
	}
	return pFont;
}

//******************************************************************************
//	サウンドファイルの取得
//******************************************************************************
const SoundFile* EffectFile::GetSoundFile( Label Name ) const
{
	const SoundFile* pSoundFile = FindResourceMap<SoundFile, SoundResource>( m_SoundResMap, Name );

	//	親のリソース取得
	if( !pSoundFile )
	{
		if( m_pParent ) pSoundFile = m_pParent->GetSoundFile( Name );
	}
	return pSoundFile;
}

//******************************************************************************
//	エフェクトデータの取得
//******************************************************************************
const EffectFile* EffectFile::GetEffect( Label Name ) const
{
	const EffectFile* pEffect = NULL;

	//	ファイル
	EffectFileMap::const_iterator itFile = m_EffectFileMap.find( Name );
	if( itFile != m_EffectFileMap.end() )
	{
		pEffect = itFile->second;
	}

	//	リソース
	EffectResMap::const_iterator itRes = m_EffectResMap.find( Name );
	if( itRes != m_EffectResMap.end() )
	{
		const EffectResource* pRes = itRes->second;
		if( pRes ) pEffect = pRes->GetData();
	}

	//	親のリソース取得
	if( !pEffect )
	{
		if( m_pParent ) pEffect = m_pParent->GetEffect( Name );
	}
	return pEffect;
}

//******************************************************************************
//	ピースの取得
//******************************************************************************
const Piece* EffectFile::GetPiece( Label Name ) const
{
	const Piece* pPiece = NULL;

	PieceMap::const_iterator it = m_PieceMap.find( Name );
	if( it != m_PieceMap.end() ) pPiece = it->second;

	//	親のリソース取得
	if( !pPiece ) 
	{
		if( m_pParent ) pPiece = m_pParent->GetPiece( Name );
	}

	return pPiece;
}

//******************************************************************************
//	パーティクルパラメータの取得
//******************************************************************************
const EFFECT_PARTICLE* EffectFile::GetParticleParam( Label Name ) const
{
	const EFFECT_PARTICLE* pParticle = NULL;

	ParticleMap::const_iterator it = m_ParticleMap.find( Name );
	if( it != m_ParticleMap.end() ) pParticle = it->second;

	//	親のリソース取得
	if( !pParticle )
	{
		if( m_pParent ) pParticle = m_pParent->GetParticleParam( Name );
	}

	return pParticle;
}

//******************************************************************************
//	発生パラメータの取得
//******************************************************************************
const EFFECT_APPEAR* EffectFile::GetAppearParam( uint Index ) const
{
	if( Index > m_AppearVec.size() ) return NULL;

	return &(m_AppearVec[Index]);
}

//------------------------------------------------------------------------------
//	読み込み（共通部）
//------------------------------------------------------------------------------
void EffectFile::_Load( const XML::Node* pNode )
{
	//	エフェクトパラメータ
	const XML::Node* pEffectNode = pNode->FindChild( NODE_EFFECT );
	if( pEffectNode ) _LoadEffectParam( pEffectNode );

	//	テクスチャ
	const XML::Node* pTexNode = pNode->FindChild( NODE_TEX );
	if( pTexNode ) _LoadTex( pTexNode );
	
	//	フォント
	const XML::Node* pFontNode = pNode->FindChild( NODE_FONT );
	if( pFontNode ) _LoadFont( pFontNode );

	//	ピース
	const XML::Node* pPieceNode = pNode->FindChild( NODE_PIECE );
	if( pPieceNode ) _LoadPiece( pPieceNode );

	//	パーティクル
	const XML::Node* pParticleNode = pNode->FindChild( NODE_PARTICLE );
	if( pParticleNode ) _LoadParticle( pParticleNode );

	//	発生パラメータ
	const XML::Node* pAppearNode = pNode->FindChild( NODE_APPEAR );
	if( pAppearNode ) _LoadAppear( pAppearNode );
}

//------------------------------------------------------------------------------
//	エフェクトパラメータの読み込み
//------------------------------------------------------------------------------
void EffectFile::_LoadEffectParam( const XML::Node* pNode )
{
	_ASSERT( pNode );

	m_Param.EndFrame	= pNode->GetAttr( ATTR_END ).GetInt();
	m_Param.bLoop		= pNode->GetAttr( ATTR_LOOP ).GetBool();
	m_Param.LoopFrame	= pNode->GetAttr( ATTR_LOOP_FRAME ).GetInt();
}

//------------------------------------------------------------------------------
//	テクスチャの読み込み
//------------------------------------------------------------------------------
void EffectFile::_LoadTex( const XML::Node* pNode )
{
	LoadTexNode( m_TexResMap, GetApp()->GetResMng(), pNode );
}

//------------------------------------------------------------------------------
//	フォントの読み込み
//------------------------------------------------------------------------------
void EffectFile::_LoadFont( const XML::Node* pNode )
{
	LoadFontNode( m_FontResMap, GetApp()->GetResMng(), pNode );
}

//------------------------------------------------------------------------------
//	ピースの読み込み
//------------------------------------------------------------------------------
void EffectFile::_LoadPiece( const XML::Node* pNode )
{
	LoadPieceNode( m_PieceMap, pNode );
}

//------------------------------------------------------------------------------
//	パーティクルの読み込み
//------------------------------------------------------------------------------
void EffectFile::_LoadParticle( const XML::Node* pNode )
{
	_ASSERT( pNode );

	for( int ParticleID = 0; ParticleID < pNode->GetChildNum(); ParticleID++ )
	{
		const XML::Node* pParticleNode = pNode->GetChild( ParticleID );
		
		Label Name = pParticleNode->GetAttr( ATTR_NAME ).GetStr();
		if( Name.IsEmpty() ) continue;
		
		EFFECT_PARTICLE* pParticle = new EFFECT_PARTICLE;
		MEM_CHECK( pParticle );

		//	共通のパラメータ
		pParticle->EndFrame	= pParticleNode->GetAttr( ATTR_END ).GetInt();
		pParticle->bLoop	= pParticleNode->GetAttr( ATTR_LOOP ).GetBool();

		pParticle->Col		= pParticleNode->GetAttr( ATTR_COLOR, COLOR_DEFAULT ).GetColor();
		pParticle->Blend	= Graphix::StrToBlendMode( pParticleNode->GetAttr( ATTR_BLEND ).GetStr() );
		pParticle->Phase	= pParticleNode->GetAttr( ATTR_PHASE ).GetInt();

		//	スプライト
		if( stricmp( pParticleNode->GetName(), NODE_DUMMY ) == 0 )
		{
			pParticle->Type = EFFECT_PARTICLE_DUMMY;
		}
		//	スプライト
		else if( stricmp( pParticleNode->GetName(), NODE_SPRITE ) == 0 )
		{
			pParticle->Type = EFFECT_PARTICLE_SPRITE;
			pParticle->pSprite = new EFFECT_PARTICLE_SPRITE_PARAM;
			MEM_CHECK( pParticle->pSprite );

			pParticle->pSprite->PieceName	= pParticleNode->GetAttr( ATTR_PIECE ).GetStr();
			pParticle->pSprite->Center		= pParticleNode->GetAttr( ATTR_CENTER ).GetPoint();
			pParticle->pSprite->Index		= pParticleNode->GetAttr( ATTR_INDEX ).GetInt();
		}
		//	スプライトフレーム
		else if( stricmp( pParticleNode->GetName(), NODE_FRAME ) == 0 )
		{
			pParticle->Type = EFFECT_PARTICLE_SPRITE_FRAME;
			pParticle->pSpriteFrame = new EFFECT_PARTICLE_SPRITE_FRAME_PARAM;
			MEM_CHECK( pParticle->pSpriteFrame );

			//	スプライトフレーム読み込み
			pParticle->pSpriteFrame->Name = AddResourceMap< SpriteFrameResource >( m_SpriteFrameResMap, pParticleNode->GetAttr( ATTR_PATH ).GetStr(), GetApp()->GetResMng() );

			pParticle->pSpriteFrame->AnimeName	= pParticleNode->GetAttr( ATTR_ANIME ).GetStr();
		}
		//	ライン
		else if( stricmp( pParticleNode->GetName(), NODE_LINE ) == 0 )
		{
			pParticle->Type = EFFECT_PARTICLE_LINE;
			pParticle->pLine = new EFFECT_PARTICLE_LINE_PARAM;
			MEM_CHECK( pParticle->pLine );

			pParticle->pLine->Type = EFFECT_PARTICLE_LINE_TYPE( StrToInt( pParticleNode->GetAttr( ATTR_TYPE ).GetStr(), EFFECT_PARTICLE_LINE_TABLE, ArrayNum(EFFECT_PARTICLE_LINE_TABLE), EFFECT_PARTICLE_LINE_CENTER ) );

			pParticle->pLine->PieceName	= pParticleNode->GetAttr( ATTR_PIECE ).GetStr();
			pParticle->pLine->Num		= pParticleNode->GetAttr( ATTR_NUM ).GetInt();
			pParticle->pLine->Divide	= pParticleNode->GetAttr( ATTR_DIVIDE ).GetInt();
			pParticle->pLine->Width		= pParticleNode->GetAttr( ATTR_WIDTH ).GetFloat();
			pParticle->pLine->Interval	= pParticleNode->GetAttr( ATTR_INTERVAL ).GetInt();

			pParticle->pLine->vColor.resize( pParticle->pLine->Num, Color::White() );
			pParticle->pLine->vIndex.resize( pParticle->pLine->Num, pParticleNode->GetAttr( ATTR_INDEX ).GetInt() );

			//	ノード取得
			const XML::Node* pLineNode = pParticleNode->FindChild( NODE_LINENODE );
			while( pLineNode )
			{
				int Node = pLineNode->GetAttr( ATTR_NODE ).GetInt();
				
				const XML::Value* pColorVal = pLineNode->FindAttr( ATTR_COLOR );
				const XML::Value* pIndexVal = pLineNode->FindAttr( ATTR_INDEX );

				if( pColorVal )
				{
					_ASSERT( Node < (int)pParticle->pLine->vColor.size() );
					pParticle->pLine->vColor[Node] = pColorVal->GetColor();
				}
				if( pIndexVal )
				{
					_ASSERT( Node < (int)pParticle->pLine->vIndex.size() );
					pParticle->pLine->vIndex[Node] = pIndexVal->GetInt();
				}

				pLineNode = pParticleNode->FindChild( NODE_LINENODE, pLineNode );
			}
		}
		//	フォント
		else if( stricmp( pParticleNode->GetName(), NODE_FONT ) == 0 )
		{
			pParticle->Type = EFFECT_PARTICLE_FONT;
			pParticle->pFont = new EFFECT_PARTICLE_FONT_PARAM;
			MEM_CHECK( pParticle->pFont );

			pParticle->pFont->Text = pParticleNode->GetAttr( ATTR_TEXT ).GetStr();
			pParticle->pFont->Align = FontSprite::StrToAlign( pParticleNode->GetAttr( ATTR_ALIGN ).GetStr() );

			const XML::Value* pPathAttr = pParticleNode->FindAttr( ATTR_PATH );
			//	パス指定
			if( pPathAttr )
			{
				//	フォント読み込み
				FontResource* pFontRes = GetApp()->GetResMng()->LoadResource<FontResource>( pPathAttr->GetStr() );
				_ASSERT( pFontRes );
				m_FontResMap.insert( FontResMap::value_type(pFontRes->GetName(), pFontRes ) );
				
				pParticle->pFont->Name = pFontRes->GetName();
			}
			//	名前指定
			else
			{
				pParticle->pFont->Name = pParticleNode->GetAttr( ATTR_FONT ).GetStr();
			}
		}
		//	サウンド
		else if( stricmp( pParticleNode->GetName(), NODE_SOUND ) == 0 )
		{
			pParticle->Type = EFFECT_PARTICLE_SOUND;
			pParticle->pSound = new EFFECT_PARTICLE_SOUND_PARAM;
			MEM_CHECK( pParticle->pSound );

			pParticle->pSound->Vol		= pParticleNode->GetAttr( ATTR_VOL, "1.0" ).GetFloat();
			pParticle->pSound->bPan		= pParticleNode->GetAttr( ATTR_PAN, "false" ).GetBool();
			pParticle->pSound->bNoPause	= pParticleNode->GetAttr( ATTR_NO_PAUSE ).GetBool();

			//	サウンドファイル読み込み
			pParticle->pSound->Name = AddResourceMap<SoundResource>( m_SoundResMap, pParticleNode->GetAttr( ATTR_PATH ).GetStr(), GetApp()->GetResMng() );
		}
		//	エフェクト
		else if( stricmp( pParticleNode->GetName(), NODE_EFFECT ) == 0 )
		{
			pParticle->Type = EFFECT_PARTICLE_EFFECT;
			pParticle->pEffect = new EFFECT_PARTICLE_EFFECT_PARAM;
			MEM_CHECK( pParticle->pEffect );
		
			const XML::Value* pNameVal = pParticleNode->FindAttr( ATTR_NAME );
			const XML::Value* pPathVal = pParticleNode->FindAttr( ATTR_PATH );

			//	ファイル参照型
			if( pPathVal )
			{
				//	読み込み
				pParticle->pEffect->Name = AddResourceMap<EffectResource>( m_EffectResMap, pPathVal->GetStr(), GetApp()->GetResMng() );
			}
			//	直記型
			else if( pNameVal )
			{
				//	読み込み
				EffectFile* pFile = new EffectFile;
				MEM_CHECK( pFile );

				pFile->LoadChild( this, pParticleNode );

				pParticle->pEffect->Name = pNameVal->GetStr();
				if( pFile )
				{
					m_EffectFileMap.insert( EffectFileMap::value_type(pParticle->pEffect->Name, pFile) );
				}
			}
		}

		//	移動パラメータ
		const XML::Node* pVelocityNode = pParticleNode->FindChild( NODE_VELOCITY );
		if( pVelocityNode )
		{
			pParticle->pVelocity = new EFFECT_MOVE_VELOCITY_PARAM;
			MEM_CHECK( pParticle->pVelocity );
			
			pParticle->pVelocity->Decel			= pVelocityNode->GetAttr( ATTR_DECEL ).GetFloat();
			pParticle->pVelocity->Gravity		= pVelocityNode->GetAttr( ATTR_GRAVITY ).GetFloat();
			pParticle->pVelocity->Force			= pVelocityNode->GetAttr( ATTR_FORCE ).GetFloat();
			pParticle->pVelocity->Flex			= pVelocityNode->GetAttr( ATTR_FLEX ).GetFloat();
			pParticle->pVelocity->bHitEnable	= pVelocityNode->GetAttr( ATTR_HIT ).GetBool();
		}

		//	タイムキー
		const XML::Node* pTimeKeyNode = pParticleNode->FindChild( NODE_TIMEKEY );
		if( pTimeKeyNode )
		{
			_LoadTimeKey( pTimeKeyNode, pParticle );
		}
		
		//	追加
		m_ParticleMap.insert( ParticleMap::value_type( Name, pParticle ) );
	}
}

//------------------------------------------------------------------------------
//	発生パラメータの読み込み
//------------------------------------------------------------------------------
void EffectFile::_LoadAppear( const XML::Node* pNode )
{
	for( int AppearID = 0; AppearID < pNode->GetChildNum(); AppearID++ )
	{
		const XML::Node* pAppearNode = pNode->GetChild( AppearID );
				
		EFFECT_APPEAR Appear;
		
		Appear.Name			= pAppearNode->GetAttr( ATTR_NAME ).GetStr();
		Appear.ParticleName	= pAppearNode->GetAttr( ATTR_PARTICLE ).GetStr();
		Appear.Interval		= pAppearNode->GetAttr( ATTR_INTERVAL, "1" ).GetInt();
		Appear.AppearNum	= pAppearNode->GetAttr( ATTR_NUM, "1" ).GetInt();

		if( Appear.Interval	<= 0 ) Appear.Interval = 1;
		if( Appear.AppearNum <= 0 ) Appear.AppearNum = 1;

		Point Frame			= pAppearNode->GetAttr( ATTR_FRAME ).GetPoint();
		Appear.StartFrame	= (uint)Frame.x;
		Appear.EndFrame		= Max( (uint)Frame.y, Appear.StartFrame );
		Appear.DirType		= _StrToDirType( pAppearNode->GetAttr( ATTR_DIR_TYPE ).GetStr() );
		
		const XML::Node* pVelocityNode	= pAppearNode->FindChild( NODE_VELOCITY );
		const XML::Node* pFollowNode	= pAppearNode->FindChild( NODE_FOLLOW );
		const XML::Node* pCircularNode	= pAppearNode->FindChild( NODE_CIRCULAR );

		Appear.MoveType = EFFECT_MOVE_DEFAULT;

		//	移動型
		if( pVelocityNode )
		{
			Appear.MoveType = EFFECT_MOVE_VELOCITY;

			Point Pos			= pVelocityNode->GetAttr( ATTR_POS ).GetPoint();
			Point PosRange		= pVelocityNode->GetAttr( ATTR_POS_RANGE ).GetPoint();
			Point Dir			= pVelocityNode->GetAttr( ATTR_DIR ).GetPoint();
			Point Dist			= pVelocityNode->GetAttr( ATTR_DIST ).GetPoint();
			Point Speed			= pVelocityNode->GetAttr( ATTR_SPEED ).GetPoint();
			Point MoveRandom	= pVelocityNode->GetAttr( ATTR_MOVE_RANDOM ).GetPoint();
			Point Rot			= pVelocityNode->GetAttr( ATTR_ROT ).GetPoint();
			Point RotSpeed		= pVelocityNode->GetAttr( ATTR_ROTSPEED ).GetPoint();

			Appear.Velocity.X				= Pos.x;
			Appear.Velocity.Y				= Pos.y;
			Appear.Velocity.XRange			= PosRange.x;
			Appear.Velocity.YRange			= PosRange.y;
			Appear.Velocity.Dir				= Angle::ToRad( Dir.x );
			Appear.Velocity.DirRange		= Angle::ToRad( Dir.y );
			Appear.Velocity.Dist			= Dist.x;
			Appear.Velocity.DistRange		= Dist.y;
			Appear.Velocity.Speed			= Speed.x;
			Appear.Velocity.SpeedRange		= Speed.y;
			Appear.Velocity.MoveRandomX		= MoveRandom.x;
			Appear.Velocity.MoveRandomY		= MoveRandom.y;

			Appear.Velocity.Rot				= Angle::ToRad( Rot.x );
			Appear.Velocity.RotRange		= Angle::ToRad( Rot.y );
			Appear.Velocity.RotSpeed		= Angle::ToRad( RotSpeed.x );
			Appear.Velocity.RotSpeedRange	= Angle::ToRad( RotSpeed.y );
			Appear.Velocity.RotRandom		= pVelocityNode->GetAttr( ATTR_ROT_RANDOM ).GetDegree();
		}
		//	追従型
		else if( pFollowNode )
		{
			Appear.MoveType = EFFECT_MOVE_FOLLOW;
			
			Point Pos			= pFollowNode->GetAttr( ATTR_POS ).GetPoint();
			Point PosRange		= pFollowNode->GetAttr( ATTR_POS_RANGE ).GetPoint();
			Point MoveRandom	= pFollowNode->GetAttr( ATTR_MOVE_RANDOM ).GetPoint();
			Point Rot			= pFollowNode->GetAttr( ATTR_ROT ).GetPoint();
			Point RotSpeed		= pFollowNode->GetAttr( ATTR_ROTSPEED ).GetPoint();

			Appear.Follow.X					= Pos.x;
			Appear.Follow.Y					= Pos.y;
			Appear.Follow.XRange			= PosRange.x;
			Appear.Follow.YRange			= PosRange.y;
			Appear.Follow.MoveRandomX		= MoveRandom.x;
			Appear.Follow.MoveRandomY		= MoveRandom.y;
			Appear.Follow.Rot				= Angle::ToRad( Rot.x );
			Appear.Follow.RotRange			= Angle::ToRad( Rot.y );
			Appear.Follow.RotSpeed			= Angle::ToRad( RotSpeed.x );
			Appear.Follow.RotSpeedRange		= Angle::ToRad( RotSpeed.y );
		}
		//	回転型
		else if( pCircularNode )
		{
			Appear.MoveType = EFFECT_MOVE_CIRCULAR;

			Point Dist				= pCircularNode->GetAttr( ATTR_DIST ).GetPoint();
			Point Rot				= pCircularNode->GetAttr( ATTR_ROT ).GetPoint();
			Point RotSpeed			= pCircularNode->GetAttr( ATTR_ROTSPEED ).GetPoint();
			Point TrackAngle		= pCircularNode->GetAttr( ATTR_ANGLE ).GetPoint();
			Point AngleSpeed		= pCircularNode->GetAttr( ATTR_ANGLESPEED ).GetPoint();
			Point AxisScale			= pCircularNode->GetAttr( ATTR_AXIS_SCALE, SCALE_DEFAULT ).GetPoint();
			Point AxisScaleRange	= pCircularNode->GetAttr( ATTR_AXIS_SCALE_RANGE ).GetPoint();
			Point AxisAngle			= pCircularNode->GetAttr( ATTR_AXIS_ANGLE, SCALE_DEFAULT ).GetPoint();

			Appear.Circular.Dist				= Dist.x;
			Appear.Circular.DistRange			= Dist.y;
			Appear.Circular.Rot					= Angle::ToRad( Rot.x );
			Appear.Circular.RotRange			= Angle::ToRad( Rot.y );
			Appear.Circular.RotSpeed			= Angle::ToRad( RotSpeed.x );
			Appear.Circular.RotSpeedRange		= Angle::ToRad( RotSpeed.y );
			Appear.Circular.TrackAngle			= Angle::ToRad( TrackAngle.x );
			Appear.Circular.TrackAngleRange		= Angle::ToRad( TrackAngle.y );
			Appear.Circular.AngleSpeed			= Angle::ToRad( AngleSpeed.x );
			Appear.Circular.AngleSpeedRange		= Angle::ToRad( AngleSpeed.y );
			Appear.Circular.AxisScale[0]		= AxisScale.x;
			Appear.Circular.AxisScale[1]		= AxisScale.y;
			Appear.Circular.AxisScaleRange[0]	= AxisScaleRange.x;
			Appear.Circular.AxisScaleRange[1]	= AxisScaleRange.y;
			Appear.Circular.AxisAngle			= Angle::ToRad( AxisAngle.x );
			Appear.Circular.AxisAngleRange		= Angle::ToRad( AxisAngle.y );
		}

		m_AppearVec.push_back( Appear );
	}
}

//------------------------------------------------------------------------------
//	タイムキーの読み込み
//------------------------------------------------------------------------------
void EffectFile::_LoadTimeKey( const XML::Node* pNode, EFFECT_PARTICLE* pParticle )
{
	for( int KeyID = 0; KeyID < pNode->GetChildNum(); KeyID++ )
	{
		const XML::Node* pKeyNode = pNode->GetChild( KeyID );

		//	種別取得
		int Type = _StrToKeyType( pKeyNode->GetName() );
		if( Type < 0 ) continue;

		EFFECT_TIME_KEY KeyInfo;
		
		KeyInfo.Frame	= pKeyNode->GetAttr( ATTR_FRAME ).GetInt();
		KeyInfo.Intp	= StrToTimeKeyIntpType( pKeyNode->GetAttr( ATTR_INTP ).GetStr() );

		switch( Type )
		{
		case EFFECT_TIME_KEY_COLOR:
			{
				Color Col = pKeyNode->GetAttr( ATTR_COLOR, COLOR_DEFAULT ).GetColor();
				Col.GetArray( KeyInfo.Param.Col );
			}
			break;

		case EFFECT_TIME_KEY_INDEX:
			{
				KeyInfo.Param.Index.Index		= pKeyNode->GetAttr( ATTR_INDEX ).GetInt();
				KeyInfo.Param.Index.ID			= pKeyNode->GetAttr( ATTR_ID ).GetInt();
			}
			break;
		case EFFECT_TIME_KEY_VOL:
			{	
				KeyInfo.Param.Val[0] = pKeyNode->GetAttr( pKeyNode->GetName(), "1.0" ).GetFloat();
				KeyInfo.Param.Val[1] = 0.0f;
			}
			break;
		case EFFECT_TIME_KEY_SPEED:
		case EFFECT_TIME_KEY_VEL:
		case EFFECT_TIME_KEY_POS:
		case EFFECT_TIME_KEY_DIST:	
			{
				Point Pt = pKeyNode->GetAttr( pKeyNode->GetName() ).GetPoint();
				KeyInfo.Param.Val[0] = Pt.x;
				KeyInfo.Param.Val[1] = Pt.y;
			}
			break;
		case EFFECT_TIME_KEY_ROT:
		case EFFECT_TIME_KEY_ROTSPEED:
		case EFFECT_TIME_KEY_ANGLE:
		case EFFECT_TIME_KEY_ANGLE_SPEED:
		case EFFECT_TIME_KEY_AXIS_ANGLE:
			{	
				KeyInfo.Param.Val[0] = pKeyNode->GetAttr( pKeyNode->GetName() ).GetDegree();
				KeyInfo.Param.Val[1] = 0.0f;
			}
			break;
		case EFFECT_TIME_KEY_SCALE:
		case EFFECT_TIME_KEY_AXIS_SCALE:
			{
				Point Pt = pKeyNode->GetAttr( pKeyNode->GetName(), SCALE_DEFAULT ).GetPoint();
				KeyInfo.Param.Val[0] = Pt.x;
				KeyInfo.Param.Val[1] = Pt.y;
			}
			break;
		case EFFECT_TIME_KEY_UPDATE_ENABLE:
			{
				KeyInfo.Param.bFlag = pKeyNode->GetAttr( ATTR_FLAG, "true" ).GetBOOL();
			}
			break;
		}

		pParticle->vTimeKey[ Type ].push_back( KeyInfo );
	}
}

//------------------------------------------------------------------------------
//	文字列から向きタイプに変換
//------------------------------------------------------------------------------
EFFECT_DIR_TYPE EffectFile::_StrToDirType( pstr pStr )
{
	pstr aType[] =
	{
		"ROT",
		"FIX",
		"FOLLOW",
		"CENTER",
	};
	
	return EFFECT_DIR_TYPE( StrToInt( pStr, aType, ArrayNum(aType), EFFECT_DIR_ROT ) );
}

//------------------------------------------------------------------------------
//	文字列からキー種別へ変換
//------------------------------------------------------------------------------
int EffectFile::_StrToKeyType( pstr pStr )
{
	pstr aType[] =
	{
		ATTR_COLOR,			//	色
		ATTR_INDEX,			//	インデクス
		ATTR_UPDATE_ENABLE,	//	更新
		ATTR_VOL,			//	ボリューム
		ATTR_ROT,			//	角度
		ATTR_ROTSPEED,		//	角速度
		ATTR_SCALE,			//	拡縮
		ATTR_VELOCITY,		//	速度
		ATTR_SPEED,			//	速度
		ATTR_POS,			//	位置
		ATTR_DIST,			//	距離
		ATTR_ANGLE,			//	円周上角度
		ATTR_ANGLESPEED,	//	円周上角速度
		ATTR_AXIS_SCALE,	//	軸拡縮
		ATTR_AXIS_ANGLE,	//	軸角度
	};

	return StrToInt( pStr, aType, ArrayNum(aType) );
}
