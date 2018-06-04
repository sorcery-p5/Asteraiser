#pragma once

#include "MenuTypes.h"
#include "Resource/Resource.h"

class TextureResource;
class SpriteFrameFile;
class SpriteFrameResource;
class EffectFile;
class EffectResource;
class SoundResource;

typedef std::map< Label, TextureResource* > TexResMap;


////////////////////////////////////////////////////////////////////////////////
//
//	メニューデータ
//
////////////////////////////////////////////////////////////////////////////////
class MenuData
{
public:
	MenuData( void );
	~MenuData();

	bool					Load( pstr pPath );

	const MENU_PARAM&		GetParam( void )						const	{ return m_Param; }
	const MENU_LAYER*		GetLayer( Label Name )					const;

	const SpriteFrameFile*	GetSpriteFrameFile( Label Name )		const;
	const Piece*			GetPiece( Label Name )					const;
	const Texture*			GetTex( Label Name )					const;
	const EffectFile*		GetEffectFile( Label Name )				const;
	const SoundFile*		GetSoundFile( Label Name )				const;

private:
	typedef std::map< Label, MENU_LAYER >			MapLayer;
	typedef std::map< Label, SpriteFrameResource* >	MapSpriteRes;
	typedef std::map< Label, EffectResource* >		MapEffectRes;
	typedef std::map< Label, SoundResource* >		MapSoundRes;

private:
	void					_LoadParam( const XML::Node* pNode );
	void					_LoadLayer( const XML::Node* pNode );

	void					_LoadCursor( MENU_CURSOR& Param, const XML::Node* pNode );
	void					_LoadObject( MENU_OBJECT& Param, const XML::Node* pNode );
	void					_LoadSelector( MENU_SELECTOR& Param, const XML::Node* pNode );
	void					_LoadSlider( MENU_SLIDER& Param, const XML::Node* pNode );
	void					_LoadList( MENU_LIST& Param, const XML::Node* pNode );
	void					_LoadScroll( MENU_SCROLL& Param, const XML::Node* pNode );
	void					_LoadKeyEvent( MENU_KEY_EVENT_VEC& vEvent, const XML::Node* pNode );
	void					_LoadKeyEvent( MENU_KEY_EVENT& Param, const XML::Node* pNode );
	void					_LoadTimeEvent( MENU_TIME_EVENT_VEC& vEvent, const XML::Node* pNode );
	void					_LoadActionList( MENU_ACTION_VEC& vAction, const XML::Node* pNode );
	void					_LoadAction( MENU_ACTION& Param, const XML::Node* pNode );

	void					_LoadSprite( const XML::Node* pNode );
	void					_LoadEffect( const XML::Node* pNode );
	void					_LoadSound( const XML::Node* pNode );

private:
	MENU_PARAM				m_Param;
	MapLayer				m_mapLayer;

	MapSpriteRes			m_mapSpriteRes;
	TexResMap				m_mapTexRes;
	PieceMap				m_mapPiece;
	MapEffectRes			m_mapEffectRes;
	MapSoundRes				m_mapSoundRes;
};

//------------------------------------------------------------------------------
//	メニューリソース
//------------------------------------------------------------------------------
class MenuResource : public ResourceTemplate< MenuData, RES_MENU >
{
};