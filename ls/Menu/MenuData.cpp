#include "stdafx.h"
#include "MenuData.h"

#include "App/App.h"

#include "Resource/ResourceManager.h"
#include "Resource/ResourceUtil.h"
#include "Resource/SoundResource.h"
#include "Resource/TextureResource.h"

#include "SpriteFrame/SpriteFrameFile.h"
#include "Effect/EffectFile.h"
#include "Indicate/IndicateData.h"
#include "Util/XMLLoadUtil.h"

namespace
{
	pstr	NODE_MENU			= "Menu";
	pstr	NODE_LAYER			= "Layer";

	pstr	NODE_CURSOR			= "Cursor";
	pstr	NODE_OBJECT			= "Object";
	pstr	NODE_SELECTOR		= "Selector";
	pstr	NODE_SLIDER			= "Slider";
	pstr	NODE_LIST			= "List";
	pstr	NODE_SCROLL			= "Scroll";
	pstr	NODE_KEY_EVENT		= "KeyEvent";
	pstr	NODE_TIME_EVENT		= "TimeEvent";
	pstr	NODE_ACTION			= "Action";
	pstr	NODE_INIT_ACTION	= "InitAction";
	pstr	NODE_CLOSE_ACTION	= "CloseAction";
	pstr	NODE_MARKER			= "Marker";
	pstr	NODE_SPIN			= "Spin";
	pstr	NODE_LINE_GRAPH		= "LineGraph";

	pstr	NODE_SPRITE			= "Sprite";
	pstr	NODE_TEX			= "Tex";
	pstr	NODE_PIECE			= "Piece";
	pstr	NODE_EFFECT			= "Effect";
	pstr	NODE_SOUND			= "Sound";

	pstr	ATTR_PATH			= "Path";
	pstr	ATTR_TEXT			= "Text";
	pstr	ATTR_NAME			= "Name";
	pstr	ATTR_TYPE			= "Type";
	pstr	ATTR_BONE			= "Bone";
	pstr	ATTR_PARAM			= "Param";
	pstr	ATTR_FRAME			= "Frame";

	pstr	ATTR_ROOT			= "Root";
	pstr	ATTR_SPRITE			= "Sprite";
	pstr	ATTR_OPEN_ANIME		= "OpenAnime";
	pstr	ATTR_WAIT_ANIME		= "WaitAnime";
	pstr	ATTR_CLOSE_ANIME	= "CloseAnime";
	pstr	ATTR_POS			= "Pos";
	pstr	ATTR_BACK_COLOR		= "BackColor";
	pstr	ATTR_SKIP_OPEN		= "SkipOpen";
	pstr	ATTR_ANIME_WAIT_END	= "AnimeWaitEnd";
	pstr	ATTR_INACTIVE		= "Inactive";
	pstr	ATTR_CLOSE_INTP		= "CloseIntp";
	pstr	ATTR_HELP_BONE		= "HelpBone";
	pstr	ATTR_HELP_TEXT		= "HelpText";

	pstr	ATTR_NUM_H			= "NumH";
	pstr	ATTR_NUM_V			= "NumV";
	pstr	ATTR_LINE_NUM		= "LineNum";
	pstr	ATTR_MOVE_SPEED		= "MoveSpeed";
	pstr	ATTR_MOVE_SOUND		= "MoveSound";
	pstr	ATTR_MOVE_SOUND_VOL	= "MoveSoundVol";

	pstr	ATTR_LEFT			= "Left";
	pstr	ATTR_UP				= "Up";
	pstr	ATTR_RIGHT			= "Right";
	pstr	ATTR_DOWN			= "Down";
	
	pstr	ATTR_CURSOR			= "Cursor";
	pstr	ATTR_SELECT_DIR		= "SelectDir";
	pstr	ATTR_TARGET			= "Target";
	pstr	ATTR_BASE			= "Base";
	pstr	ATTR_VALUE			= "Value";
	pstr	ATTR_MIN			= "Min";
	pstr	ATTR_MAX			= "Max";
	pstr	ATTR_STEP			= "Step";


	const LabelPair<MENU_KEY_EVENT_TYPE> MENU_KEY_EVENT_TABLE[] =
	{
		{ "Decide",	MENU_KEY_EVENT_DECIDE,	},
		{ "Cancel",	MENU_KEY_EVENT_CANCEL,	},
		{ "Pause",	MENU_KEY_EVENT_PAUSE,	},
		{ "Exit",	MENU_KEY_EVENT_EXIT,	},
		{ "Any",	MENU_KEY_EVENT_ANY,		},
	};

	const LabelPair<MENU_SELECT_DIR> MENU_SELECT_DIR_TABLE[] =
	{
		{ "H",		MENU_SELECT_H,	},
		{ "V",		MENU_SELECT_V,	},
	};
	
	const StrPair<MENU_ACTION_TYPE> MENU_ACTION_TABLE[] =
	{
		{ "Open",		MENU_ACTION_OPEN,		},
		{ "Close",		MENU_ACTION_CLOSE,		},
		{ "Anime",		MENU_ACTION_ANIME,		},
		{ "Notify",		MENU_ACTION_NOTIFY,		},
		{ "Select",		MENU_ACTION_SELECT,		},
		{ "Effect",		MENU_ACTION_EFFECT,		},
		{ "Sound",		MENU_ACTION_SOUND,		},
		{ "PlayBGM",	MENU_ACTION_BGM_PLAY,	},
		{ "StopBGM",	MENU_ACTION_BGM_STOP,	},
		{ "FadeBGM",	MENU_ACTION_BGM_FADE,	},
	};

	pstr COLOR_DEFAULT		= "255,255,255,255";
};


////////////////////////////////////////////////////////////////////////////////
//
//	メニューデータ
//
////////////////////////////////////////////////////////////////////////////////
MenuData::MenuData( void )
{
	
}
MenuData::~MenuData()
{
	DeleteMap( m_mapPiece );
	ReleaseMap( m_mapTexRes );
	ReleaseMap( m_mapSpriteRes );
	ReleaseMap( m_mapEffectRes );
	ReleaseMap( m_mapSoundRes );
}

//******************************************************************************
//	読み込み
//******************************************************************************
bool MenuData::Load( pstr pPath )
{
	//	ファイル読み込む
	XML Xml;
	if( !Xml.Load( pPath ) )
	{
		return false;
	}
	
	//	スプライト
	const XML::Node* pSpriteNode = Xml.GetRoot().FindChild( NODE_SPRITE );
	if( pSpriteNode ) _LoadSprite( pSpriteNode );
	
	//	エフェクト
	const XML::Node* pEffectNode = Xml.GetRoot().FindChild( NODE_EFFECT );
	if( pEffectNode ) _LoadEffect( pEffectNode );

	//	サウンド
	const XML::Node* pSoundNode = Xml.GetRoot().FindChild( NODE_SOUND );
	if( pSoundNode ) _LoadSound( pSoundNode );

	//	パラメータ
	const XML::Node* pParamNode = Xml.GetRoot().FindChild( NODE_MENU );
	if( pParamNode ) _LoadParam( pParamNode );

	//	レイヤー
	const XML::Node* pLayerNode = Xml.GetRoot().FindChild( NODE_LAYER );
	if( pLayerNode ) _LoadLayer( pLayerNode );

	//	テクスチャ
	const XML::Node* pTexNode = Xml.GetRoot().FindChild( NODE_TEX );
	if( pTexNode ) LoadTexNode( m_mapTexRes, GetApp()->GetResMng(), pTexNode );

	//	ピース
	const XML::Node* pPieceNode = Xml.GetRoot().FindChild( NODE_PIECE );
	if( pPieceNode ) LoadPieceNode( m_mapPiece, pPieceNode );

	return true;
}

//******************************************************************************
//	レイヤー取得
//******************************************************************************
const MENU_LAYER* MenuData::GetLayer( Label Name ) const
{
	MapLayer::const_iterator it = m_mapLayer.find( Name );
	if( it != m_mapLayer.end() ) return &it->second;

	return NULL;
}

//******************************************************************************
//	スプライトフレームファイル取得
//******************************************************************************
const SpriteFrameFile* MenuData::GetSpriteFrameFile( Label Name ) const
{
	return FindResourceMap<SpriteFrameFile, SpriteFrameResource>( m_mapSpriteRes, Name );
}

//******************************************************************************
//	ピースの取得
//******************************************************************************
const Piece* MenuData::GetPiece( Label Name ) const
{
	PieceMap::const_iterator it = m_mapPiece.find( Name );
	if( it != m_mapPiece.end() ) return it->second;

	return NULL;
}

//******************************************************************************
//	テクスチャの取得
//******************************************************************************
const Texture* MenuData::GetTex( Label Name ) const
{
	TexResMap::const_iterator it = m_mapTexRes.find( Name );
	if( it != m_mapTexRes.end() )
	{
		return (it->second)->GetData();
	}

	return NULL;
}

//******************************************************************************
//	エフェクトファイル取得
//******************************************************************************
const EffectFile* MenuData::GetEffectFile( Label Name ) const
{
	return FindResourceMap<EffectFile, EffectResource>( m_mapEffectRes, Name );
}

//******************************************************************************
//	サウンドファイル取得
//******************************************************************************
const SoundFile* MenuData::GetSoundFile( Label Name ) const
{
	return FindResourceMap<SoundFile, SoundResource>( m_mapSoundRes, Name );
}

//------------------------------------------------------------------------------
//	パラメータ読み込み
//------------------------------------------------------------------------------
void MenuData::_LoadParam( const XML::Node* pNode )
{
	m_Param.RootLayer	= pNode->GetAttr( ATTR_ROOT ).GetLabel();

	//	初期化アクション
	const XML::Node* pInitNode = pNode->FindChild( NODE_INIT_ACTION );
	if( pInitNode ) _LoadActionList( m_Param.vInitAction, pInitNode );
	
	//	終了アクション
	const XML::Node* pCloseNode = pNode->FindChild( NODE_CLOSE_ACTION );
	if( pCloseNode ) _LoadActionList( m_Param.vCloseAction, pCloseNode );

	//	キーイベント
	_LoadKeyEvent( m_Param.vKeyEvent, pNode );

	//	時間イベント
	const XML::Node* pTimeNode = pNode->FindChild( NODE_TIME_EVENT );
	if( pTimeNode ) _LoadTimeEvent(	m_Param.vTimeEvent, pTimeNode );
}
	
//------------------------------------------------------------------------------
//	レイヤー読み込み
//------------------------------------------------------------------------------
void MenuData::_LoadLayer( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pLayerNode = pNode->GetChild( i );

		Label Name = pLayerNode->GetAttr( ATTR_NAME ).GetLabel();
		if( Name.IsEmpty() ) continue;

		MENU_LAYER Layer;
		Layer.Name			= Name;
		Layer.SpriteName	= AddResourceMap< SpriteFrameResource >( m_mapSpriteRes, pLayerNode->GetAttr( ATTR_SPRITE ).GetStr(), GetApp()->GetResMng() );
		Layer.OpenAnime		= pLayerNode->GetAttr( ATTR_OPEN_ANIME ).GetLabel();
		Layer.WaitAnime		= pLayerNode->GetAttr( ATTR_WAIT_ANIME ).GetLabel();
		Layer.CloseAnime	= pLayerNode->GetAttr( ATTR_CLOSE_ANIME ).GetLabel();
		Layer.RootObject	= pLayerNode->GetAttr( ATTR_ROOT ).GetLabel();
		Layer.HelpBoneName	= pLayerNode->GetAttr( ATTR_HELP_BONE ).GetLabel();
		Layer.Pos			= pLayerNode->GetAttr( ATTR_POS ).GetPoint();
		Layer.BackColor		= pLayerNode->GetAttr( ATTR_BACK_COLOR, COLOR_DEFAULT ).GetColor();
		Layer.bSkipOpen		= pLayerNode->GetAttr( ATTR_SKIP_OPEN ).GetBool();
		Layer.bAnimeWaitEnd	= pLayerNode->GetAttr( ATTR_ANIME_WAIT_END ).GetBool();
		Layer.bInactive		= pLayerNode->GetAttr( ATTR_INACTIVE ).GetBool();
		Layer.CloseIntp		= pLayerNode->GetAttr( ATTR_CLOSE_INTP ).GetInt();
		Layer.HelpText		= pLayerNode->GetAttr( ATTR_HELP_TEXT ).GetStr();

		for( int i = 0; i < pLayerNode->GetChildNum(); i++ )
		{
			const XML::Node* pChildNode = pLayerNode->GetChild( i );

			//	カーソル
			if( stricmp( pChildNode->GetName(), NODE_CURSOR ) == 0 )
			{
				_LoadCursor( Layer.Cursor, pChildNode );
			}

			//	オブジェクト
			else if( stricmp( pChildNode->GetName(), NODE_OBJECT ) == 0 )
			{
				MENU_OBJECT Object;
				_LoadObject( Object, pChildNode );

				Layer.vObject.push_back( Object );
			}

			//	マーカー
			else if( stricmp( pChildNode->GetName(), NODE_MARKER ) == 0 )
			{
				INDICATE_MARKER Marker;
				IndicateData::LoadMarker( Marker, pChildNode );

				Layer.mapMarker.insert( std::map< Label, INDICATE_MARKER >::value_type( Marker.BoneName, Marker ) );
			}

			//	回転パラメータ
			else if( stricmp( pChildNode->GetName(), NODE_SPIN ) == 0 )
			{
				INDICATE_SPIN Spin;
				IndicateData::LoadSpin( Spin, pChildNode );

				Layer.mapSpin.insert( std::map< Label, INDICATE_SPIN >::value_type( Spin.BoneName, Spin ) );
			}

			//	線グラフ
			else if( stricmp( pChildNode->GetName(), NODE_LINE_GRAPH ) == 0 )
			{
				INDICATE_LINE_GRAPH LineGraph;
				IndicateData::LoadLineGraph( LineGraph, pChildNode );

				Layer.mapLineGraph.insert( std::map< Label, INDICATE_LINE_GRAPH >::value_type( LineGraph.BoneName, LineGraph ) );
			}
		}

		//	初期化アクション
		const XML::Node* pInitNode = pLayerNode->FindChild( NODE_INIT_ACTION );
		if( pInitNode ) _LoadActionList( Layer.vInitAction, pInitNode );

		//	閉じるアクション
		const XML::Node* pCloseNode = pLayerNode->FindChild( NODE_CLOSE_ACTION );
		if( pCloseNode ) _LoadActionList( Layer.vCloseAction, pCloseNode );

		//	キーイベント
		_LoadKeyEvent( Layer.vKeyEvent, pLayerNode );

		//	時間イベント
		const XML::Node* pTimeNode = pLayerNode->FindChild( NODE_TIME_EVENT );
		if( pTimeNode ) _LoadTimeEvent(	Layer.vTimeEvent, pTimeNode );

		m_mapLayer.insert( MapLayer::value_type( Name, Layer ) );
	}
}

//------------------------------------------------------------------------------
//	カーソル読み込み
//------------------------------------------------------------------------------
void MenuData::_LoadCursor( MENU_CURSOR& Param, const XML::Node* pNode )
{
	Param.BoneName		= pNode->GetAttr( ATTR_BONE ).GetLabel();
	Param.MoveSpeed		= pNode->GetAttr( ATTR_MOVE_SPEED, "1.0" ).GetFloat();

	//	サウンド
	Param.MoveSound		= AddResourceMap< SoundResource >( m_mapSoundRes, pNode->GetAttr( ATTR_MOVE_SOUND ).GetStr(), GetApp()->GetResMng() );
	Param.MoveSoundVol	= pNode->GetAttr( ATTR_MOVE_SOUND_VOL ).GetFloat();
}

//------------------------------------------------------------------------------
//	オブジェクト読み込み
//------------------------------------------------------------------------------
void MenuData::_LoadObject( MENU_OBJECT& Param, const XML::Node* pNode )
{
	Param.BoneName		= pNode->GetAttr( ATTR_BONE ).GetLabel();
	Param.HelpText		= pNode->GetAttr( ATTR_HELP_TEXT ).GetStr();

	//	方向の取得
	Param.DirBoneName[MENU_DIR_L]	= pNode->GetAttr( ATTR_LEFT ).GetLabel();
	Param.DirBoneName[MENU_DIR_U]	= pNode->GetAttr( ATTR_UP ).GetLabel();
	Param.DirBoneName[MENU_DIR_R]	= pNode->GetAttr( ATTR_RIGHT ).GetLabel();
	Param.DirBoneName[MENU_DIR_D]	= pNode->GetAttr( ATTR_DOWN ).GetLabel();

	//	種別
	Param.Type = MENU_OBJECT_NONE;
	const XML::Node* pSelectorNode	= pNode->FindChild( NODE_SELECTOR );
	const XML::Node* pSliderNode	= pNode->FindChild( NODE_SLIDER );
	const XML::Node* pListNode		= pNode->FindChild( NODE_LIST );
	const XML::Node* pScrollNode	= pNode->FindChild( NODE_SCROLL );

	//	セレクター
	if( pSelectorNode )
	{
		Param.Type	= MENU_OBJECT_SELECTOR;
		_LoadSelector( Param.Selector, pSelectorNode );
	}
	//	スライダー
	else if( pSliderNode )
	{
		Param.Type	= MENU_OBJECT_SLIDER;
		_LoadSlider( Param.Slider, pSliderNode );
	}
	//	リスト
	else if( pListNode )
	{
		Param.Type	= MENU_OBJECT_LIST;
		_LoadList( Param.List, pListNode );
	}
	//	スクロール
	else if( pScrollNode )
	{
		Param.Type	= MENU_OBJECT_SCROLL;
		_LoadScroll( Param.Scroll, pScrollNode );
	}

	//	キーイベント
	_LoadKeyEvent( Param.vKeyEvent, pNode );
}

//------------------------------------------------------------------------------
//	セレクター読み込み
//------------------------------------------------------------------------------
void MenuData::_LoadSelector( MENU_SELECTOR& Param, const XML::Node* pNode )
{
	Param.CursorBone	= pNode->GetAttr( ATTR_CURSOR ).GetLabel();
	Param.SelectDir		= LabelToEnum< MENU_SELECT_DIR >( pNode->GetAttr(ATTR_SELECT_DIR).GetLabel(), MENU_SELECT_DIR_TABLE, ArrayNum(MENU_SELECT_DIR_TABLE), MENU_SELECT_H );
	Param.MoveSpeed		= pNode->GetAttr( ATTR_MOVE_SPEED, "1.0" ).GetFloat();

	//	選択肢
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pChildNode = pNode->GetChild( i );

		LabelPair< float >	Pair;
		Pair.Name		= pChildNode->GetAttr( ATTR_BONE ).GetLabel();
		Pair.Value		= pChildNode->GetAttr( ATTR_VALUE ).GetFloat();
		
		Param.vSelect.push_back( Pair );
	}

	//	サウンド
	Param.MoveSound		= AddResourceMap< SoundResource >( m_mapSoundRes, pNode->GetAttr( ATTR_MOVE_SOUND ).GetStr(), GetApp()->GetResMng() );
	Param.MoveSoundVol	= pNode->GetAttr( ATTR_MOVE_SOUND_VOL ).GetFloat();
}

//------------------------------------------------------------------------------
//	スライダー読み込み
//------------------------------------------------------------------------------
void MenuData::_LoadSlider( MENU_SLIDER& Param, const XML::Node* pNode )
{
	Param.SelectDir		= LabelToEnum< MENU_SELECT_DIR >( pNode->GetAttr(ATTR_SELECT_DIR).GetLabel(), MENU_SELECT_DIR_TABLE, ArrayNum(MENU_SELECT_DIR_TABLE), MENU_SELECT_H );
	Param.MinValue		= pNode->GetAttr( ATTR_MIN ).GetFloat();
	Param.MaxValue		= pNode->GetAttr( ATTR_MAX ).GetFloat();
	Param.Step			= pNode->GetAttr( ATTR_STEP ).GetFloat();

	//	サウンド
	Param.MoveSound		= AddResourceMap< SoundResource >( m_mapSoundRes, pNode->GetAttr( ATTR_MOVE_SOUND ).GetStr(), GetApp()->GetResMng() );
	Param.MoveSoundVol	= pNode->GetAttr( ATTR_MOVE_SOUND_VOL ).GetFloat();
}

//------------------------------------------------------------------------------
//	リスト読み込み
//------------------------------------------------------------------------------
void MenuData::_LoadList( MENU_LIST& Param, const XML::Node* pNode )
{
	Param.NumH			= Max( pNode->GetAttr( ATTR_NUM_H, "1" ).GetInt(), 1 );
	Param.NumV			= Max( pNode->GetAttr( ATTR_NUM_V, "1" ).GetInt(), 1 );
	_ASSERT( Param.NumH	> 1 || Param.NumV > 1 );

	Param.LineNum		= Max( pNode->GetAttr( ATTR_LINE_NUM, "1" ).GetInt(), 1 );

	Param.CellSprite	= AddResourceMap< SpriteFrameResource >( m_mapSpriteRes, pNode->GetAttr( ATTR_SPRITE ).GetStr(), GetApp()->GetResMng() );

	Param.CursorBone	= pNode->GetAttr( ATTR_CURSOR ).GetLabel();
	Param.MoveSpeed		= pNode->GetAttr( ATTR_MOVE_SPEED, "1.0" ).GetFloat();

	//	サウンド
	Param.MoveSound		= AddResourceMap< SoundResource >( m_mapSoundRes, pNode->GetAttr( ATTR_MOVE_SOUND ).GetStr(), GetApp()->GetResMng() );
	Param.MoveSoundVol	= pNode->GetAttr( ATTR_MOVE_SOUND_VOL ).GetFloat();
}

//------------------------------------------------------------------------------
//	スクロール読み込み
//------------------------------------------------------------------------------
void MenuData::_LoadScroll( MENU_SCROLL& Param, const XML::Node* pNode )
{
	Param.SelectDir		= LabelToEnum< MENU_SELECT_DIR >( pNode->GetAttr(ATTR_SELECT_DIR).GetLabel(), MENU_SELECT_DIR_TABLE, ArrayNum(MENU_SELECT_DIR_TABLE), MENU_SELECT_H );
	Param.TargetBone	= pNode->GetAttr( ATTR_TARGET ).GetLabel();
	Param.BaseBone		= pNode->GetAttr( ATTR_BASE ).GetLabel();
	Param.MoveSpeed		= pNode->GetAttr( ATTR_MOVE_SPEED, "1.0" ).GetFloat();
}

//------------------------------------------------------------------------------
//	キーイベント読み込み
//------------------------------------------------------------------------------
void MenuData::_LoadKeyEvent( MENU_KEY_EVENT_VEC& vEvent, const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pKeyNode = pNode->GetChild( i );
		if( stricmp( pKeyNode->GetName(), NODE_KEY_EVENT ) == 0 )
		{
			MENU_KEY_EVENT KeyEvent;
			_LoadKeyEvent( KeyEvent, pKeyNode );

			vEvent.push_back( KeyEvent );
		}
	}
}

//------------------------------------------------------------------------------
//	キーイベント読み込み
//------------------------------------------------------------------------------
void MenuData::_LoadKeyEvent( MENU_KEY_EVENT& Param, const XML::Node* pNode )
{
	Param.Type = LabelToEnum< MENU_KEY_EVENT_TYPE >( pNode->GetAttr(ATTR_TYPE).GetLabel(), MENU_KEY_EVENT_TABLE, ArrayNum(MENU_KEY_EVENT_TABLE), MENU_KEY_EVENT_NULL );

	//	アクション
	_LoadActionList( Param.vAction, pNode );
}

//------------------------------------------------------------------------------
//	時間イベント読み込み
//------------------------------------------------------------------------------
void MenuData::_LoadTimeEvent( MENU_TIME_EVENT_VEC& vEvent, const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pChildNode = pNode->GetChild( i );

		MENU_TIME_EVENT Event;
		Event.Frame		= pChildNode->GetAttr( ATTR_FRAME ).GetInt();
		_LoadAction( Event.Action, pChildNode );

		vEvent.push_back( Event );
	}
}

//------------------------------------------------------------------------------
//	アクションリスト読み込み
//------------------------------------------------------------------------------
void MenuData::_LoadActionList( MENU_ACTION_VEC& vAction, const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pChildNode = pNode->GetChild( i );

		MENU_ACTION Action;
		_LoadAction( Action, pChildNode );

		vAction.push_back( Action );
	}
}

//------------------------------------------------------------------------------
//	アクション読み込み
//------------------------------------------------------------------------------
void MenuData::_LoadAction( MENU_ACTION& Param, const XML::Node* pNode )
{
	Param.Type	= StrToEnum< MENU_ACTION_TYPE >( pNode->GetName(), MENU_ACTION_TABLE, ArrayNum(MENU_ACTION_TABLE), MENU_ACTION_NULL );
	Param.Name	= pNode->GetAttr( ATTR_NAME ).GetLabel();
	Param.Param	= pNode->GetAttr( ATTR_PARAM ).GetFloat();

	const XML::Value* pTextVal = pNode->FindAttr( ATTR_TEXT );
	const XML::Value* pPathVal = pNode->FindAttr( ATTR_PATH );
	if( pTextVal ) Param.Text = pTextVal->GetStr();
	if( pPathVal ) Param.Text = pPathVal->GetStr();
}

//------------------------------------------------------------------------------
//	スプライトフレーム読み込み
//------------------------------------------------------------------------------
void MenuData::_LoadSprite( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pFileNode = pNode->GetChild( i );

		//	読み込む
		const XML::Value* pValue = pFileNode->FindAttr( ATTR_PATH );
		if( pValue && !pValue->IsEmpty() )
		{
			AddResourceMap< SpriteFrameResource >( m_mapSpriteRes, pValue->GetStr(), GetApp()->GetResMng() );
		}
	}
}
	
//------------------------------------------------------------------------------
//	エフェクト読み込み
//------------------------------------------------------------------------------
void MenuData::_LoadEffect( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pFileNode = pNode->GetChild( i );

		//	読み込む
		const XML::Value* pValue = pFileNode->FindAttr( ATTR_PATH );
		if( pValue && !pValue->IsEmpty() )
		{
			AddResourceMap< EffectResource >( m_mapEffectRes, pValue->GetStr(), GetApp()->GetResMng() );
		}
	}
}
	
//------------------------------------------------------------------------------
//	サウンド読み込み
//------------------------------------------------------------------------------
void MenuData::_LoadSound( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pFileNode = pNode->GetChild( i );

		//	読み込む
		const XML::Value* pValue = pFileNode->FindAttr( ATTR_PATH );
		if( pValue && !pValue->IsEmpty() )
		{
			AddResourceMap< SoundResource >( m_mapSoundRes, pValue->GetStr(), GetApp()->GetResMng() );
		}
	}
}