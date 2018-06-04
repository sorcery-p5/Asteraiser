#include "stdafx.h"
#include "IndicateData.h"

#include "App/App.h"

#include "Resource/ResourceManager.h"
#include "Resource/ResourceUtil.h"
#include "Resource/TextureResource.h"

#include "SpriteFrame/SpriteFrameFile.h"
#include "Util/XMLLoadUtil.h"

namespace
{
	pstr	NODE_INDICATE		= "Indicate";
	pstr	NODE_MARKER			= "Marker";
	pstr	NODE_SPIN			= "Spin";
	pstr	NODE_LINE_GRAPH		= "LineGraph";
	pstr	NODE_TEX			= "Tex";
	pstr	NODE_PIECE			= "Piece";

	pstr	ATTR_SPRITE			= "Sprite";
	pstr	ATTR_OPEN_ANIME		= "OpenAnime";
	pstr	ATTR_CLOSE_ANIME	= "CloseAnime";
	pstr	ATTR_POS			= "Pos";
	pstr	ATTR_INIT_OPEN		= "InitOpen";

	pstr	ATTR_BONE			= "Bone";
	pstr	ATTR_MIN			= "Min";
	pstr	ATTR_MAX			= "Max";

	pstr	ATTR_PIECE			= "Piece";
	pstr	ATTR_WIDTH			= "Width";
	pstr	ATTR_DIVIDE			= "Divide";
	pstr	ATTR_COLOR_MIN		= "ColorMin";
	pstr	ATTR_COLOR_MAX		= "ColorMax";
	pstr	ATTR_PHASE			= "Phase";
	pstr	ATTR_BLEND			= "Blend";
	pstr	ATTR_VAL_MIN		= "ValMin";
	pstr	ATTR_VAL_MAX		= "ValMax";
	pstr	ATTR_TIME_MAX		= "TimeMax";
	pstr	ATTR_VAL_AXIS		= "ValAxis";
	pstr	ATTR_TIME_AXIS		= "TimeAxis";
	pstr	ATTR_SEPARATE_PIECE	= "SeparatePiece";
	pstr	ATTR_SEPARATE_COLOR	= "SeparateColor";

	pstr	COLOR_DEFAULT		= "255,255,255,255";
}


////////////////////////////////////////////////////////////////////////////////
//
//	インジケートデータ
//
////////////////////////////////////////////////////////////////////////////////
IndicateData::IndicateData( void )
{

}

IndicateData::~IndicateData()
{
	DeleteMap( m_mapPiece );
	ReleaseMap( m_mapTexRes );
	ReleaseMap( m_mapSpriteRes );
}

//******************************************************************************
//	読み込み
//******************************************************************************
bool IndicateData::Load( pstr pPath )
{
	//	ファイル読み込む
	XML Xml;
	if( !Xml.Load( pPath ) )
	{
		return false;
	}

	//	パラメータ
	const XML::Node* pParamNode = Xml.GetRoot().FindChild( NODE_INDICATE );
	if( pParamNode ) _LoadParam( pParamNode );

	//	マーカー
	const XML::Node* pMarkerNode = Xml.GetRoot().FindChild( NODE_MARKER );
	if( pMarkerNode ) _LoadMarker( pMarkerNode );

	//	マーカー
	const XML::Node* pSpinNode = Xml.GetRoot().FindChild( NODE_SPIN );
	if( pSpinNode ) _LoadSpin( pSpinNode );

	//	線グラフ
	const XML::Node* pLineGraphNode = Xml.GetRoot().FindChild( NODE_LINE_GRAPH );
	if( pLineGraphNode ) _LoadLineGraph( pLineGraphNode );

	//	テクスチャ
	const XML::Node* pTexNode = Xml.GetRoot().FindChild( NODE_TEX );
	if( pTexNode ) LoadTexNode( m_mapTexRes, GetApp()->GetResMng(), pTexNode );

	//	ピース
	const XML::Node* pPieceNode = Xml.GetRoot().FindChild( NODE_PIECE );
	if( pPieceNode ) LoadPieceNode( m_mapPiece, pPieceNode );

	return true;
}

//******************************************************************************
//	スプライトファイルの取得
//******************************************************************************
const SpriteFrameFile* IndicateData::GetSpriteFrameFile( Label Name ) const
{
	return FindResourceMap<SpriteFrameFile, SpriteFrameResource>( m_mapSpriteRes, Name );
}

//******************************************************************************
//	ピースの取得
//******************************************************************************
const Piece* IndicateData::GetPiece( Label Name ) const
{
	PieceMap::const_iterator it = m_mapPiece.find( Name );
	if( it != m_mapPiece.end() ) return it->second;

	return NULL;
}

//******************************************************************************
//	テクスチャの取得
//******************************************************************************
const Texture* IndicateData::GetTex( Label Name ) const
{
	TexResMap::const_iterator it = m_mapTexRes.find( Name );
	if( it != m_mapTexRes.end() )
	{
		return (it->second)->GetData();
	}

	return NULL;
}

//******************************************************************************
//	マーカーの取得
//******************************************************************************
const INDICATE_MARKER* IndicateData::GetMarker( Label Name ) const
{
	MapMarker::const_iterator it = m_mapMarker.find( Name );
	if( it != m_mapMarker.end() )
	{
		return &(it->second);
	}
	return NULL;
}

//******************************************************************************
//	回転パラメータの取得
//******************************************************************************
const INDICATE_SPIN* IndicateData::GetSpin( Label Name ) const
{
	MapSpin::const_iterator it = m_mapSpin.find( Name );
	if( it != m_mapSpin.end() )
	{
		return &(it->second);
	}
	return NULL;
}

//******************************************************************************
//	線グラフの取得
//******************************************************************************
const INDICATE_LINE_GRAPH* IndicateData::GetLineGraph( Label Name ) const
{
	MapLineGraph::const_iterator it = m_mapLineGraph.find( Name );
	if( it != m_mapLineGraph.end() )
	{
		return &(it->second);
	}
	return NULL;
}

//******************************************************************************
//	マーカーパラメータの読み込み
//******************************************************************************
void IndicateData::LoadMarker( INDICATE_MARKER& Param, const XML::Node* pNode )
{
	Param.BoneName	= pNode->GetAttr( ATTR_BONE ).GetLabel();
	Param.MinPos	= pNode->GetAttr( ATTR_MIN ).GetPoint();
	Param.MaxPos	= pNode->GetAttr( ATTR_MAX ).GetPoint();
}

//******************************************************************************
//	回転パラメータの読み込み
//******************************************************************************
void IndicateData::LoadSpin( INDICATE_SPIN& Param, const XML::Node* pNode )
{
	Param.BoneName	= pNode->GetAttr( ATTR_BONE ).GetLabel();
	Param.MinRot	= pNode->GetAttr( ATTR_MIN ).GetDegree();
	Param.MaxRot	= pNode->GetAttr( ATTR_MAX ).GetDegree();
}

//******************************************************************************
//	線グラフパラメータの読み込み
//******************************************************************************
void IndicateData::LoadLineGraph( INDICATE_LINE_GRAPH& Param, const XML::Node* pNode )
{
	Param.BoneName		= pNode->GetAttr( ATTR_BONE		).GetLabel();
	Param.PieceName		= pNode->GetAttr( ATTR_PIECE	).GetLabel();

	Param.Width			= pNode->GetAttr( ATTR_WIDTH	).GetFloat();
	Param.Divide		= pNode->GetAttr( ATTR_DIVIDE	).GetInt();
	Param.ColMin		= pNode->GetAttr( ATTR_COLOR_MIN, COLOR_DEFAULT	).GetColor();
	Param.ColMax		= pNode->GetAttr( ATTR_COLOR_MAX, COLOR_DEFAULT	).GetColor();
	Param.Phase			= pNode->GetAttr( ATTR_PHASE	).GetInt();
	Param.Blend			= Graphix::StrToBlendMode( pNode->GetAttr( ATTR_BLEND ).GetStr() );

	Param.ValMin		= pNode->GetAttr( ATTR_VAL_MIN	).GetFloat();
	Param.ValMax		= pNode->GetAttr( ATTR_VAL_MAX	).GetFloat();
	Param.TimeMax		= pNode->GetAttr( ATTR_TIME_MAX ).GetInt();

	Param.ValAxis		= pNode->GetAttr( ATTR_VAL_AXIS	).GetPoint();
	Param.TimeAxis		= pNode->GetAttr( ATTR_TIME_AXIS ).GetPoint();

	Param.SeparatePiece	= pNode->GetAttr( ATTR_SEPARATE_PIECE ).GetLabel();
	Param.SeparateColor	= pNode->GetAttr( ATTR_SEPARATE_COLOR, COLOR_DEFAULT ).GetColor();
}

//------------------------------------------------------------------------------
//	パラメータ読み込み
//------------------------------------------------------------------------------
void IndicateData::_LoadParam( const XML::Node* pNode )
{
	m_Param.SpriteName	= AddResourceMap< SpriteFrameResource >( m_mapSpriteRes, pNode->GetAttr( ATTR_SPRITE ).GetStr(), GetApp()->GetResMng() );

	m_Param.OpenAnime	= pNode->GetAttr( ATTR_OPEN_ANIME ).GetLabel();
	m_Param.CloseAnime	= pNode->GetAttr( ATTR_CLOSE_ANIME ).GetLabel();
	m_Param.Pos			= pNode->GetAttr( ATTR_POS ).GetPoint();

	m_Param.bInitOpen	= pNode->GetAttr( ATTR_INIT_OPEN, "true" ).GetBool();
}

//------------------------------------------------------------------------------
//	マーカー読み込み
//------------------------------------------------------------------------------
void IndicateData::_LoadMarker( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pChild = pNode->GetChild( i );
		if( !pChild ) continue;

		INDICATE_MARKER Marker;
		LoadMarker( Marker, pChild );

		m_mapMarker.insert( MapMarker::value_type( Marker.BoneName, Marker ) );
	}
}

//------------------------------------------------------------------------------
//	回転パラメータ読み込み
//------------------------------------------------------------------------------
void IndicateData::_LoadSpin( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pChild = pNode->GetChild( i );
		if( !pChild ) continue;

		INDICATE_SPIN Spin;
		LoadSpin( Spin, pChild );

		m_mapSpin.insert( MapSpin::value_type( Spin.BoneName, Spin ) );
	}
}

//------------------------------------------------------------------------------
//	線グラフ読み込み
//------------------------------------------------------------------------------
void IndicateData::_LoadLineGraph( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pChild = pNode->GetChild( i );
		if( !pChild ) continue;

		INDICATE_LINE_GRAPH LineGraph;
		LoadLineGraph( LineGraph, pChild );

		m_mapLineGraph.insert( MapLineGraph::value_type( LineGraph.BoneName, LineGraph ) );
	}
}