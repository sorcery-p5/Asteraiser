#pragma once

#include "IndicateTypes.h"
#include "Resource/Resource.h"

class TextureResource;
class SpriteFrameFile;
class SpriteFrameResource;

typedef std::map< Label, TextureResource* > TexResMap;


////////////////////////////////////////////////////////////////////////////////
//
//	インジケートデータ
//
////////////////////////////////////////////////////////////////////////////////
class IndicateData
{
public:
	IndicateData( void );
	~IndicateData();

	bool						Load( pstr pPath );

	const INDICATE_PARAM&		GetParam( void )						const	{ return m_Param; }
	const SpriteFrameFile*		GetSpriteFrameFile( Label Name )		const;
	const Piece*				GetPiece( Label Name )					const;
	const Texture*				GetTex( Label Name )					const;
	const INDICATE_MARKER*		GetMarker( Label Name )					const;
	const INDICATE_SPIN*		GetSpin( Label Name )					const;
	const INDICATE_LINE_GRAPH*	GetLineGraph( Label Name )				const;

	static void					LoadMarker( INDICATE_MARKER& Param, const XML::Node* pNode );
	static void					LoadSpin( INDICATE_SPIN& Spin, const XML::Node* pNode );
	static void					LoadLineGraph( INDICATE_LINE_GRAPH& Param, const XML::Node* pNode );

private:
	typedef std::map< Label, SpriteFrameResource* >	MapSpriteRes;
	typedef std::map< Label, INDICATE_MARKER >		MapMarker;
	typedef std::map< Label, INDICATE_SPIN >		MapSpin;
	typedef std::map< Label, INDICATE_LINE_GRAPH >	MapLineGraph;

private:
	void						_LoadParam( const XML::Node* pNode );
	void						_LoadMarker( const XML::Node* pNode );
	void						_LoadSpin( const XML::Node* pNode );
	void						_LoadLineGraph( const XML::Node* pNode );

private:
	INDICATE_PARAM				m_Param;
	MapSpriteRes				m_mapSpriteRes;
	TexResMap					m_mapTexRes;
	PieceMap					m_mapPiece;
	MapMarker					m_mapMarker;
	MapSpin						m_mapSpin;
	MapLineGraph				m_mapLineGraph;
};

//------------------------------------------------------------------------------
//	インジケートリソース
//------------------------------------------------------------------------------
class IndicateResource : public ResourceTemplate< IndicateData, RES_INDICATE >
{
};