#include "stdafx.h"
#include "Config.h"

#include "App/App.h"
#include "Controller/Controller.h"

namespace
{
	pstr		CONFIG_PATH				= "Config.xml";

	pstr		NODE_SYSTEM				= "System";
	pstr		NODE_INPUT				= "Input";
	pstr		NODE_DEBUG				= "Debug";

	pstr		NODE_CONTROLLER			= "Controller";
	pstr		NODE_KEYBOARD			= "Keyboard";
	pstr		NODE_PAD				= "Pad";
	pstr		NODE_CONCURRENT			= "Concurrent";
	pstr		NODE_AXIS_X				= "AxisX";
	pstr		NODE_AXIS_Y				= "AxisY";
	pstr		NODE_LEVER				= "Lever";

	pstr		ATTR_BGM_VOL			= "BGMVol";
	pstr		ATTR_SOUND_VOL			= "SoundVol";
	pstr		ATTR_FULL_SCREEN		= "FullScreen";

	pstr		ATTR_KEY				= "Key";
	pstr		ATTR_TARGET				= "Target";
	pstr		ATTR_AXIS				= "Axis";
	pstr		ATTR_REVERSE			= "Reverse";
	pstr		ATTR_DEADZONE			= "Deadzone";

	pstr		ATTR_DRAW_DUMMY_BONE	= "DrawDummyBone";
	pstr		ATTR_DRAW_COLLIDE_SHAPE	= "DrawCollideShape";
	pstr		ATTR_OVERDRIVE_INFINITE	= "OverdriveInfinite";
	pstr		ATTR_SHIELD_INFINITE	= "ShieldInfinite";
	pstr		ATTR_BLADE_INFINITE		= "BladeInfinite";
	pstr		ATTR_PLAYER_INVINCIBLE	= "PlayerInvincible";
	pstr		ATTR_PLAYER_NODAMAGE	= "PlayerNodamage";
	pstr		ATTR_PLAYER_HYPERATTACK	= "PlayerHyperAttack";
	pstr		ATTR_ENEMY_HYPERATTACK	= "EnemyHyperAttack";

	const StrPair<CONTROLLER_INPUT>			INPUT_TABLE[] =
	{
		{ "Blade",		CONTROLLER_BLADE,		},
		{ "Reload",		CONTROLLER_RELOAD,		},
		{ "Dash",		CONTROLLER_DASH,		},

		{ "Up",			CONTROLLER_U,			},
		{ "Down",		CONTROLLER_D,			},
		{ "Left",		CONTROLLER_L,			},
		{ "Right",		CONTROLLER_R,			},
	};
	
	const StrPair<CONTROLLER_SYSTEM_KEY>	SYSTEM_KEY_TABLE[] =
	{
		{ "Ok",			CONTROLLER_SYSTEM_OK,		},
		{ "Cancel",		CONTROLLER_SYSTEM_CANCEL,	},
		{ "Pause",		CONTROLLER_SYSTEM_PAUSE,	},
		{ "Exit",		CONTROLLER_SYSTEM_EXIT,		},
	};
	
	const StrPair<PAD_AXIS>	PAD_AXIS_TABLE[] =
	{
		{ "AXIS_X",				PAD_AXIS_X,			},
		{ "AXIS_Y",				PAD_AXIS_Y,			},
		{ "AXIS_Z",				PAD_AXIS_Z,			},
		{ "AXIS_RX",			PAD_AXIS_RX,		},
		{ "AXIS_RY",			PAD_AXIS_RY,		},
		{ "AXIS_RZ",			PAD_AXIS_RZ,		},
		{ "AXIS_SL1",			PAD_AXIS_SL1,		},
		{ "AXIS_SL2",			PAD_AXIS_SL2,		},
		{ "AXIS_POV1X",			PAD_AXIS_POV1X,		},
		{ "AXIS_POV1Y",			PAD_AXIS_POV1Y,		},
		{ "AXIS_POV2X",			PAD_AXIS_POV2X,		},
		{ "AXIS_POV2Y",			PAD_AXIS_POV2Y,		},
		{ "AXIS_POV3X",			PAD_AXIS_POV3X,		},
		{ "AXIS_POV3Y",			PAD_AXIS_POV3Y,		},
		{ "AXIS_POV4X",			PAD_AXIS_POV4X,		},
		{ "AXIS_POV4Y",			PAD_AXIS_POV4Y,		},
	};
	
	const StrPair<byte>		KEYCODE_TABLE[] =
	{
		{ "BACK",						VK_BACK,			},
		{ "TAB",						VK_TAB,				},
		{ "ENTER",						VK_RETURN,			},
		{ "SHIFT",						VK_SHIFT,			},
		{ "CTRL",						VK_CONTROL    		},
		{ "ALT",						VK_MENU,			},
		{ "PAUSE",						VK_PAUSE,			},
		{ "ESC",						VK_ESCAPE,			},
		{ "SPACE",						VK_SPACE,			},
		{ "INS",						VK_INSERT,			},
		{ "DEL",						VK_DELETE,			},
		{ "END",						VK_END,				},
		{ "HOME",						VK_HOME,			},
		{ "PG UP",						VK_PRIOR,			},
		{ "PG DN",						VK_NEXT,			},
		{ "LEFT",						VK_LEFT,			},
		{ "UP",							VK_UP,				},
		{ "RIGHT",						VK_RIGHT,			},
		{ "DOWN",						VK_DOWN,			},
		{ "NUM 0",						VK_NUMPAD0,			},
		{ "NUM 1",						VK_NUMPAD1,			},
		{ "NUM 2",						VK_NUMPAD2,			},
		{ "NUM 3",						VK_NUMPAD3,			},
		{ "NUM 4",						VK_NUMPAD4,			},
		{ "NUM 5",						VK_NUMPAD5,			},
		{ "NUM 6",						VK_NUMPAD6,			},
		{ "NUM 7",						VK_NUMPAD7,			},
		{ "NUM 8",						VK_NUMPAD8,			},
		{ "NUM 9",						VK_NUMPAD9,			},
		{ "NUM *",						VK_MULTIPLY,		},
		{ "NUM +",						VK_ADD,				},
		{ "NUM -",						VK_SUBTRACT,		},
		{ "NUM .",						VK_DECIMAL,			},
		{ "NUM /",						VK_DIVIDE,			},
		{ "F1",							VK_F1,				},
		{ "F2",							VK_F2,				},
		{ "F3",							VK_F3,				},
		{ "F4",							VK_F4,				},
		{ "F5",							VK_F5,				},
		{ "F6",							VK_F6,				},
		{ "F7",							VK_F7,				},
		{ "F8",							VK_F8,				},
		{ "F9",							VK_F9,				},
		{ "F10",						VK_F10,				},
		{ "F11",						VK_F11,				},
		{ "F12",						VK_F12,				},
		{ "F13",						VK_F13,				},
		{ "F14",						VK_F14,				},
		{ "F15",						VK_F15,				},
		{ "F16",						VK_F16,				},
		{ "F17",						VK_F17,				},
		{ "F18",						VK_F18,				},
		{ "F19",						VK_F19,				},
		{ "F20",						VK_F20,				},
		{ "F21",						VK_F21,				},
		{ "F22",						VK_F22,				},
		{ "F23",						VK_F23,				},
		{ "F24",						VK_F24,				},
		{ ":",							VK_OEM_1,			},
		{ "/",							VK_OEM_2,			},
		{ "@",							VK_OEM_3,			},
		{ "[",							VK_OEM_4,			},
		{ "\\",							VK_OEM_5,			},
		{ "]",							VK_OEM_6,			},
		{ "~",							VK_OEM_7,			},
		{ ";",							VK_OEM_PLUS,		},
		{ ",",							VK_OEM_COMMA,		},
		{ ".",							VK_OEM_PERIOD,		},
		{ "-",							VK_OEM_MINUS,		},
		{ "_",							VK_OEM_102,			},
		{ "L_SHT",						VK_LSHIFT,			},
		{ "R_SHT",						VK_RSHIFT,			},
		{ "L_CTR",						VK_LCONTROL,		},
		{ "R_CTR",						VK_RCONTROL,		},
		{ "L_ALT",						VK_LMENU,			},
		{ "R_ALT",						VK_RMENU,			},
	};

	pstr		KEYNAME_BLADE		= "[[Blade]]";
	pstr		KEYNAME_RELOAD		= "[[Reload]]";
	pstr		KEYNAME_DASH		= "[[Dash]]";
	pstr		KEYNAME_SPEAR		= "[[Spear]]";
	pstr		KEYNAME_RULER		= "[[Ruler]]";

	pstr		KEYNAME_OK			= "[[Ok]]";
	pstr		KEYNAME_CANCEL		= "[[Cancel]]";
	pstr		KEYNAME_PAUSE		= "[[Pause]]";
	pstr		KEYNAME_EXIT		= "[[Exit]]";

	pstr		KEYNAME_STR			= "%dボタン／%sキー";

	enum KEYNAME_TYPE
	{
		KEYNAME_BUTTON,
		KEYNAME_CONCURRENT,
		KEYNAME_SYSTEM,
	};

	//	キー文字列の情報
	struct KEYNAME_INFO
	{
		pstr			KeyName;
		KEYNAME_TYPE	Type;
		int				InputID;
	};

	const KEYNAME_INFO KEYNAME_INFO_ARRAY[] =
	{
		{	KEYNAME_BLADE,		KEYNAME_BUTTON,		CONTROLLER_BLADE,	},
		{	KEYNAME_RELOAD,		KEYNAME_BUTTON,		CONTROLLER_RELOAD,	},
		{	KEYNAME_DASH,		KEYNAME_BUTTON,		CONTROLLER_DASH,	},
		{	KEYNAME_SPEAR,		KEYNAME_CONCURRENT,	0	},
		{	KEYNAME_RULER,		KEYNAME_CONCURRENT,	1	},
		{	KEYNAME_OK,			KEYNAME_SYSTEM,		CONTROLLER_SYSTEM_OK,		},
		{	KEYNAME_CANCEL,		KEYNAME_SYSTEM,		CONTROLLER_SYSTEM_CANCEL,	},
		{	KEYNAME_PAUSE,		KEYNAME_SYSTEM,		CONTROLLER_SYSTEM_PAUSE,	},
		{	KEYNAME_EXIT,		KEYNAME_SYSTEM,		CONTROLLER_SYSTEM_EXIT,		},
	};
}

////////////////////////////////////////////////////////////////////////////////
//
//	コンフィグ
//
////////////////////////////////////////////////////////////////////////////////
Config::Config()
{
	Reset();

	m_bLoadDebug = false;
}
Config::~Config()
{
}

//******************************************************************************
//	読み込み
//******************************************************************************
bool Config::Load( pstr pPath )
{
	m_Path = pPath;

	Log( "□コンフィグファイル読み込み開始[%s]", pPath );

	//	ファイル読み込む
	XML Xml;
	if( !Xml.Load( pPath ) )
	{
		Log( "×コンフィグファイル読み込み失敗" );
		return false;
	}

	//	システム
	const XML::Node* pSystemNode = Xml.GetRoot().FindChild( NODE_SYSTEM );
	if( pSystemNode ) _LoadSystem( pSystemNode );

	//	インプット
	const XML::Node* pInputNode = Xml.GetRoot().FindChild( NODE_INPUT );
	if( pInputNode ) _LoadInput( pInputNode );

	//	デバッグ
	const XML::Node* pDebugNode = Xml.GetRoot().FindChild( NODE_DEBUG );
	if( pDebugNode ) _LoadDebug( pDebugNode );

	Log( "○コンフィグファイル読み込み完了" );
	Log( "----------------------------------------" );

	return true;
}

//******************************************************************************
//	保存
//******************************************************************************
bool Config::Save( void ) const
{
	XML Xml;

	//	システム
	XML::Node* pSystemNode = new XML::Node( NODE_SYSTEM );
	if( pSystemNode )
	{
		_SaveSystem( pSystemNode );
		Xml.GetRoot().AddChild( pSystemNode );
	}

	//	インプット
	XML::Node* pInputNode = new XML::Node( NODE_INPUT );
	if( pInputNode )
	{
		_SaveInput( pInputNode );
		Xml.GetRoot().AddChild( pInputNode );
	}

	//	デバッグ設定
	if( m_bLoadDebug )
	{
		XML::Node* pDebugNode = new XML::Node( NODE_DEBUG );
		if( pDebugNode )
		{
			_SaveDebug( pDebugNode );
			Xml.GetRoot().AddChild( pDebugNode );
		}		
	}

	//	書き込み
	return Xml.Save( m_Path.c_str() );
}

//******************************************************************************
//	リセット
//******************************************************************************
void Config::Reset( void )
{
	m_System.Default();

	m_vInput.clear();
	m_vInput.push_back( CONFIG_INPUT() );
}

//******************************************************************************
//	設定の適用
//******************************************************************************
void Config::Apply( void )
{
	//	音量の設定
	GetTone()->SetChannelVolume( SOUND_CHANNEL_BGM, m_System.BGMVolume );
	GetTone()->SetChannelVolume( SOUND_CHANNEL_SE,	m_System.SoundVolume );

	//	パッドの設定
	for( uint i = 0; i < m_vInput.size(); i++ )
	{
		const CONFIG_INPUT& Input = m_vInput[i];

		Pad* pPad = GetSign()->GetPad( i );
		if( pPad ) pPad->SetDeadzone( Input.PadDeadzone );
	}
}

//******************************************************************************
//	入力情報の取得
//******************************************************************************
const CONFIG_INPUT& Config::GetInputInfo( int PlayerID ) const
{
	PlayerID = PlayerID >= (int)m_vInput.size()? 0 : PlayerID;
	return m_vInput[PlayerID];
}

//******************************************************************************
//	入力情報の設定
//******************************************************************************
void Config::SetInputInfo( int PlayerID, const CONFIG_INPUT& Info )
{
	PlayerID = PlayerID >= (int)m_vInput.size()? 0 : PlayerID;
	m_vInput[PlayerID] = Info;
}

//******************************************************************************
//	キー名文字の置換を行う
//******************************************************************************
void Config::ReplaceKeyName( String256& Str ) const
{
	for( int i = 0; i < ArrayNum(KEYNAME_INFO_ARRAY); i++ )
	{
		const KEYNAME_INFO& Info = KEYNAME_INFO_ARRAY[i];
		char* pPos = Str.Find( Info.KeyName );
		while( pPos )
		{
			String256 RepStr;
			_GetKeyNameStr( RepStr, Info.Type, Info.InputID );

			Str.Replace( Info.KeyName, RepStr.c_str() );

			pPos = Str.Find( Info.KeyName );
		}
	}
}

//*****************************************************************************
//	文字列をキーコードに
//*****************************************************************************
byte Config::StrToKeycode( pstr pStr )
{
	//	一文字＝文字コード
	if( strlen( pStr ) == 1 )
	{
		return (byte)pStr[0];
	}
	
	//	コード表と照合
	return StrToEnum( pStr, KEYCODE_TABLE, ArrayNum( KEYCODE_TABLE ), (byte)UCHAR_MAX );
}

//*****************************************************************************
//	キーコードを文字列に
//*****************************************************************************
std::string Config::KeycodeToStr( byte Code )
{
	std::string Str;

	//	コード表と照合
	pstr pStr = EnumToStr( Code, KEYCODE_TABLE, ArrayNum( KEYCODE_TABLE ), NULL );
	if( pStr )
	{
		Str = pStr;
	}
	//	一文字＝文字コード
	else if( Code >= '0' && Code <= '9' ||
			 Code >= 'A' && Code <= 'Z' )
	{
		Str += (char)Code;
	}

	return Str;
}

//*****************************************************************************
//	入力種別を文字列に
//*****************************************************************************
pstr Config::InputTypeToStr( CONTROLLER_INPUT Type )
{
	return EnumToStr( Type, INPUT_TABLE, ArrayNum(INPUT_TABLE) );
}
	
//*****************************************************************************
//	システムキー種別を文字列に
//*****************************************************************************
pstr Config::SystemKeyTypeToStr( CONTROLLER_SYSTEM_KEY Type )
{
	return EnumToStr( Type, SYSTEM_KEY_TABLE, ArrayNum(SYSTEM_KEY_TABLE) );
}

//*****************************************************************************
//	パッド軸を文字列に
//*****************************************************************************
pstr Config::PadAxisToStr( PAD_AXIS Axis )
{
	return EnumToStr( Axis, PAD_AXIS_TABLE, ArrayNum(PAD_AXIS_TABLE) );
}

//------------------------------------------------------------------------------
//	システム設定の読み込み
//------------------------------------------------------------------------------
void Config::_LoadSystem( const XML::Node* pNode )
{
	m_System.BGMVolume			= pNode->GetAttr( ATTR_BGM_VOL ).GetFloat();
	m_System.SoundVolume		= pNode->GetAttr( ATTR_SOUND_VOL ).GetFloat();

	m_System.bFullScreen		= pNode->GetAttr( ATTR_FULL_SCREEN ).GetBool();
}

//------------------------------------------------------------------------------
//	インプット設定の読み込み
//------------------------------------------------------------------------------
void Config::_LoadInput( const XML::Node* pNode )
{
	m_vInput.clear();

	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pChildNode = pNode->GetChild( i );
		if( !pChildNode ) continue;

		const XML::Node* pKeyNode = pChildNode->FindChild( NODE_KEYBOARD );
		const XML::Node* pPadNode = pChildNode->FindChild( NODE_PAD );
		if( pKeyNode || pPadNode )
		{
			CONFIG_INPUT Input;

			if( pKeyNode ) _LoadKeyboard( Input, pKeyNode );
			if( pPadNode ) _LoadPad( Input, pPadNode );

			m_vInput.push_back( Input );
		}
	}
}

//------------------------------------------------------------------------------
//	デバッグ情報の読み込み
//------------------------------------------------------------------------------
void Config::_LoadDebug( const XML::Node* pNode )
{
	m_Debug.bDrawDummyBone		= pNode->GetAttr( ATTR_DRAW_DUMMY_BONE ).GetBool();
	m_Debug.bDrawCollideShape	= pNode->GetAttr( ATTR_DRAW_COLLIDE_SHAPE ).GetBool();
	m_Debug.bOverdriveInfinite	= pNode->GetAttr( ATTR_OVERDRIVE_INFINITE ).GetBool();
	m_Debug.bShieldInfinite		= pNode->GetAttr( ATTR_SHIELD_INFINITE ).GetBool();
	m_Debug.bBladeInfinite		= pNode->GetAttr( ATTR_BLADE_INFINITE ).GetBool();
	m_Debug.bPlayerInvincible	= pNode->GetAttr( ATTR_PLAYER_INVINCIBLE ).GetBool();
	m_Debug.bPlayerNodamage		= pNode->GetAttr( ATTR_PLAYER_NODAMAGE ).GetBool();
	m_Debug.bPlayerHyperAttack	= pNode->GetAttr( ATTR_PLAYER_HYPERATTACK ).GetBool();
	m_Debug.bEnemyHyperAttack	= pNode->GetAttr( ATTR_ENEMY_HYPERATTACK ).GetBool();

	m_bLoadDebug = true;
}

//------------------------------------------------------------------------------
//	キーボード情報の読み込み
//------------------------------------------------------------------------------
void Config::_LoadKeyboard( CONFIG_INPUT& Input, const XML::Node* pNode )
{
	Input.vKeyConcurrent.clear();
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pChildNode = pNode->GetChild(i);

		CONTROLLER_INPUT		InputType	= StrToEnum( pChildNode->GetName(), INPUT_TABLE, ArrayNum(INPUT_TABLE), CONTROLLER_NONE );
		CONTROLLER_SYSTEM_KEY	SystemKey	= StrToEnum( pChildNode->GetName(), SYSTEM_KEY_TABLE, ArrayNum(SYSTEM_KEY_TABLE), CONTROLLER_SYSTEM_NONE );

		//	方向
		if( InputType >= CONTROLLER_DIR_BEGIN && InputType <= CONTROLLER_DIR_END )
		{
			byte Keycode = StrToKeycode( pChildNode->GetAttr( ATTR_KEY ).GetStr() );
			if( Keycode != UCHAR_MAX ) Input.aKeyDir[ InputType - CONTROLLER_DIR_BEGIN ] = Keycode;
		}

		//	キー
		else if( InputType >= CONTROLLER_BUTTON_BEGIN && InputType <= CONTROLLER_BUTTON_END )
		{
			byte Keycode = StrToKeycode( pChildNode->GetAttr( ATTR_KEY ).GetStr() );
			if( Keycode != UCHAR_MAX ) Input.aKeyButton[ InputType ]	= Keycode;
		}

		//	システムキー
		else if( SystemKey != CONTROLLER_SYSTEM_NONE )
		{
			byte Keycode = StrToKeycode( pChildNode->GetAttr( ATTR_KEY ).GetStr() );
			if( Keycode != UCHAR_MAX ) Input.aKeySystem[ SystemKey ] = Keycode;
		}
	
		//	同時押し
		else if( stricmp( pChildNode->GetName(), NODE_CONCURRENT ) == 0 )
		{
			byte Keycode = StrToKeycode( pChildNode->GetAttr( ATTR_KEY ).GetStr() );
			if( Keycode != UCHAR_MAX )
			{
				Input.vKeyConcurrent.push_back( _LoadConcurrent( Keycode, pChildNode ) );
			}
		}
	}
}

//------------------------------------------------------------------------------
//	パッド情報の読み込み
//------------------------------------------------------------------------------
void Config::_LoadPad( CONFIG_INPUT& Input, const XML::Node* pNode )
{
	Input.vPadConcurrent.clear();
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pChildNode = pNode->GetChild(i);

		CONTROLLER_INPUT		InputType	= StrToEnum( pChildNode->GetName(), INPUT_TABLE, ArrayNum(INPUT_TABLE), CONTROLLER_NONE );
		CONTROLLER_SYSTEM_KEY	SystemKey	= StrToEnum( pChildNode->GetName(), SYSTEM_KEY_TABLE, ArrayNum(SYSTEM_KEY_TABLE), CONTROLLER_SYSTEM_NONE );

		//	X軸
		if( stricmp( pChildNode->GetName(), NODE_AXIS_X ) == 0 )
		{
			Input.PadAxisX		= StrToEnum( pChildNode->GetAttr( ATTR_AXIS ).GetStr(), PAD_AXIS_TABLE, ArrayNum(PAD_AXIS_TABLE), PAD_AXIS_X );
			Input.bPadAxisXRev	= pChildNode->GetAttr( ATTR_REVERSE ).GetBool();
		}

		//	Y軸
		else if( stricmp( pChildNode->GetName(), NODE_AXIS_Y ) == 0 )
		{
			Input.PadAxisY		= StrToEnum( pChildNode->GetAttr( ATTR_AXIS ).GetStr(), PAD_AXIS_TABLE, ArrayNum(PAD_AXIS_TABLE), PAD_AXIS_Y );
			Input.bPadAxisYRev	= pChildNode->GetAttr( ATTR_REVERSE ).GetBool();
		}

		//	システムキー
		else if( SystemKey != CONTROLLER_SYSTEM_NONE )
		{
			int ID = pChildNode->GetAttr( ATTR_KEY, "-1" ).GetInt();
			if( ID >= 0 ) Input.aPadSystem[ SystemKey ]	= ID;
		}

		//	キー
		else if( InputType >= CONTROLLER_BUTTON_BEGIN && InputType <= CONTROLLER_BUTTON_END )
		{
			int ID = pChildNode->GetAttr( ATTR_KEY, "-1" ).GetInt();
			if( ID >= 0 ) Input.aPadButton[ InputType ]	= ID;
		}

		//	同時押し
		else if( stricmp( pChildNode->GetName(), NODE_CONCURRENT ) == 0 )
		{
			int ID = pChildNode->GetAttr( ATTR_KEY, "-1" ).GetInt();
			if( ID >= 0 ) 
			{
				Input.vPadConcurrent.push_back( _LoadConcurrent( ID, pChildNode ) );
			}
		}

		//	デッドゾーン
		else if( stricmp( pChildNode->GetName(), NODE_LEVER ) == 0 )
		{
			int Deadzone = pChildNode->GetAttr( ATTR_DEADZONE, "-1" ).GetInt();
			if( Deadzone > 0 ) 
			{
				Input.PadDeadzone = Deadzone;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	同時押し情報の読み込み
//------------------------------------------------------------------------------
CONCURRENT Config::_LoadConcurrent( byte b, const XML::Node* pNode )
{
	byte Concurrent[CONCURRENT_NUM];
	FillArray( Concurrent, CONCURRENT_NUM, (byte)CONTROLLER_NONE );
	
	int Num = 0;
	for( int i = 0; i < pNode->GetAttrNum(); i++ )
	{
		if( stricmp( pNode->GetAttrName(i), ATTR_TARGET ) == 0 )
		{
			Concurrent[Num] = StrToEnum( pNode->GetAttrVal(i).GetStr(), INPUT_TABLE, ArrayNum(INPUT_TABLE), CONTROLLER_NONE );
			Num++;
		}
	}

	return CONCURRENT( b, Concurrent, Num );
}

//------------------------------------------------------------------------------
//	システム設定の書き込み
//------------------------------------------------------------------------------
void Config::_SaveSystem( XML::Node* pNode ) const
{
	_ASSERT( pNode );

	pNode->AddAttr( ATTR_FULL_SCREEN,	XML::Value( m_System.bFullScreen ) );
	pNode->AddAttr( ATTR_BGM_VOL,		XML::Value( m_System.BGMVolume ) );
	pNode->AddAttr( ATTR_SOUND_VOL,		XML::Value( m_System.SoundVolume ) );
}

//------------------------------------------------------------------------------
//	入力設定の書き込み
//------------------------------------------------------------------------------
void Config::_SaveInput( XML::Node* pNode ) const
{
	_ASSERT( pNode );

	for( int i = 0; i < (int)m_vInput.size(); i++ )
	{
		const CONFIG_INPUT& Input = m_vInput[i];

		XML::Node* pChildNode = new XML::Node( NODE_CONTROLLER );
		
		//	キーボード
		XML::Node* pKeyNode = new XML::Node( NODE_KEYBOARD );
		if( pKeyNode )
		{
			_SaveKeyboard( pKeyNode, Input );
			pChildNode->AddChild( pKeyNode );
		}
		
		//	パッド
		XML::Node* pPadNode = new XML::Node( NODE_PAD );
		if( pPadNode )
		{
			_SavePad( pPadNode, Input );
			pChildNode->AddChild( pPadNode );
		}

		pNode->AddChild( pChildNode );
	}
}

//------------------------------------------------------------------------------
//	デバッグ設定の書き込み
//------------------------------------------------------------------------------
void Config::_SaveDebug( XML::Node* pNode ) const
{
	_ASSERT( pNode );

	pNode->AddAttr( ATTR_DRAW_DUMMY_BONE,		XML::Value( m_Debug.bDrawDummyBone		 ) );
	pNode->AddAttr( ATTR_DRAW_COLLIDE_SHAPE,	XML::Value( m_Debug.bDrawCollideShape	 ) );
	pNode->AddAttr( ATTR_OVERDRIVE_INFINITE,	XML::Value( m_Debug.bOverdriveInfinite	 ) );
	pNode->AddAttr( ATTR_SHIELD_INFINITE,		XML::Value( m_Debug.bShieldInfinite		 ) );
	pNode->AddAttr( ATTR_BLADE_INFINITE,		XML::Value( m_Debug.bBladeInfinite		 ) );
	pNode->AddAttr( ATTR_PLAYER_INVINCIBLE,		XML::Value( m_Debug.bPlayerInvincible	 ) );
	pNode->AddAttr( ATTR_PLAYER_NODAMAGE,		XML::Value( m_Debug.bPlayerNodamage		 ) );
	pNode->AddAttr( ATTR_PLAYER_HYPERATTACK,	XML::Value( m_Debug.bPlayerHyperAttack	 ) );
	pNode->AddAttr( ATTR_ENEMY_HYPERATTACK,		XML::Value( m_Debug.bEnemyHyperAttack	 ) );
}

//------------------------------------------------------------------------------
//	キーボード情報の書き込み
//------------------------------------------------------------------------------
void Config::_SaveKeyboard( XML::Node* pNode, const CONFIG_INPUT& Input ) const
{
	_ASSERT( pNode );

	//	方向
	for( int i = 0; i < INPUT_AXIS_NUM; i++ )
	{
		CONTROLLER_INPUT Type = (CONTROLLER_INPUT)(CONTROLLER_DIR_BEGIN + i );

		XML::Node* pChildNode = new XML::Node( InputTypeToStr(Type) );
		pChildNode->AddAttr( ATTR_KEY, XML::Value( KeycodeToStr(Input.aKeyDir[i]).c_str() ) );

		pNode->AddChild( pChildNode );
	}
	
	//	システムキー
	for( int i = 0; i < INPUT_SYSTEM_KEY_NUM; i++ )
	{
		CONTROLLER_SYSTEM_KEY Type = (CONTROLLER_SYSTEM_KEY)(CONTROLLER_BUTTON_BEGIN + i );

		XML::Node* pChildNode = new XML::Node( SystemKeyTypeToStr(Type) );
		pChildNode->AddAttr( ATTR_KEY, XML::Value( KeycodeToStr(Input.aKeySystem[i]).c_str() ) );

		pNode->AddChild( pChildNode );
	}

	//	ボタン
	for( int i = 0; i < INPUT_BUTTON_NUM; i++ )
	{
		CONTROLLER_INPUT Type = (CONTROLLER_INPUT)(CONTROLLER_BUTTON_BEGIN + i );

		XML::Node* pChildNode = new XML::Node( InputTypeToStr(Type) );
		pChildNode->AddAttr( ATTR_KEY, XML::Value( KeycodeToStr(Input.aKeyButton[i]).c_str() ) );

		pNode->AddChild( pChildNode );
	}
	
	//	同時押し
	for( int i = 0; i < (int)Input.vKeyConcurrent.size(); i++ )
	{
		const CONCURRENT& Param = Input.vKeyConcurrent[i];

		XML::Node* pChildNode = new XML::Node( NODE_CONCURRENT );
		pChildNode->AddAttr( ATTR_KEY, XML::Value( KeycodeToStr(Param.Button).c_str() ) );

		for( int i = 0; i < CONCURRENT_NUM; i++ )
		{
			if( Param.Array[i] != UCHAR_MAX )
			{
				pChildNode->AddAttr( ATTR_TARGET, XML::Value( InputTypeToStr( (CONTROLLER_INPUT)Param.Array[i] ) ) );
			}
		}

		pNode->AddChild( pChildNode );
	}
}

//------------------------------------------------------------------------------
//	パッド設定の書き込み
//------------------------------------------------------------------------------
void Config::_SavePad( XML::Node* pNode, const CONFIG_INPUT& Input ) const
{
	_ASSERT( pNode );

	//	X軸
	{
		XML::Node* pAxisNode = new XML::Node( NODE_AXIS_X );
		pAxisNode->AddAttr( ATTR_AXIS,		XML::Value( PadAxisToStr(Input.PadAxisX) ) );
		pAxisNode->AddAttr( ATTR_REVERSE,	XML::Value( Input.bPadAxisXRev ) );

		pNode->AddChild( pAxisNode );
	}

	//	Y軸
	{
		XML::Node* pAxisNode = new XML::Node( NODE_AXIS_Y );
		pAxisNode->AddAttr( ATTR_AXIS,		XML::Value( PadAxisToStr(Input.PadAxisY) ) );
		pAxisNode->AddAttr( ATTR_REVERSE,	XML::Value( Input.bPadAxisYRev ) );

		pNode->AddChild( pAxisNode );
	}

	//	システムキー
	for( int i = 0; i < INPUT_SYSTEM_KEY_NUM; i++ )
	{
		CONTROLLER_SYSTEM_KEY Type = (CONTROLLER_SYSTEM_KEY)(CONTROLLER_BUTTON_BEGIN + i );

		XML::Node* pChildNode = new XML::Node( SystemKeyTypeToStr(Type) );
		pChildNode->AddAttr( ATTR_KEY, XML::Value( Input.aPadSystem[i] ) );

		pNode->AddChild( pChildNode );
	}

	//	ボタン
	for( int i = 0; i < INPUT_BUTTON_NUM; i++ )
	{
		CONTROLLER_INPUT Type = (CONTROLLER_INPUT)(CONTROLLER_BUTTON_BEGIN + i );

		XML::Node* pChildNode = new XML::Node( InputTypeToStr(Type) );
		pChildNode->AddAttr( ATTR_KEY, XML::Value( Input.aPadButton[i] ) );

		pNode->AddChild( pChildNode );
	}
	
	//	同時押し
	for( int i = 0; i < (int)Input.vKeyConcurrent.size(); i++ )
	{
		const CONCURRENT& Param = Input.vPadConcurrent[i];

		XML::Node* pChildNode = new XML::Node( NODE_CONCURRENT );
		pChildNode->AddAttr( ATTR_KEY, XML::Value( Param.Button ) );

		for( int i = 0; i < CONCURRENT_NUM; i++ )
		{
			if( Param.Array[i] != UCHAR_MAX )
			{
				pChildNode->AddAttr( ATTR_TARGET, XML::Value( InputTypeToStr( (CONTROLLER_INPUT)Param.Array[i] ) ) );
			}
		}

		pNode->AddChild( pChildNode );
	}

	//	レバー設定
	{
		XML::Node* pLeverNode = new XML::Node( NODE_LEVER );
		pLeverNode->AddAttr( ATTR_DEADZONE,	XML::Value( Input.PadDeadzone ) );

		pNode->AddChild( pLeverNode );
	}
}

//------------------------------------------------------------------------------
//	キー文字列を取得する
//------------------------------------------------------------------------------
void Config::_GetKeyNameStr( String256& Out, int Type, int ID ) const
{
	byte Keycode	= 0;
	byte ButtonID	= 0;

	const CONFIG_INPUT& InputInfo = GetInputInfo(0);

	switch( Type )
	{
	case KEYNAME_BUTTON:
		{
			Keycode		= InputInfo.aKeyButton[ ID ];
			ButtonID	= InputInfo.aPadButton[ ID ];
		}
		break;
	case KEYNAME_CONCURRENT:
		{
			Keycode		= InputInfo.vKeyConcurrent[ ID ].Button;
			ButtonID	= InputInfo.vPadConcurrent[ ID ].Button;
		}
		break;
	case KEYNAME_SYSTEM:
		{
			Keycode		= InputInfo.aKeySystem[ ID ];
			ButtonID	= InputInfo.aPadSystem[ ID ];
		}
		break;
	}

#ifdef DEMO_VER
	const StrPair<byte>		BUTTON_TABLE[] =
	{
		{ "１ボタン",					0,			},
		{ "２ボタン",					1,			},
		{ "３ボタン",					2,			},
		{ "４ボタン",					3,			},
		{ "５ボタン",					4,			},
		{ "６ボタン",					5,			},

		{ "セレクトボタン",				6,			},
		{ "スタートボタン",				9,    		},
	};

	Out.Format( EnumToStr( ButtonID, BUTTON_TABLE, ArrayNum(BUTTON_TABLE), "" ) );
#else
	Out.Format( KEYNAME_STR, ButtonID + 1, KeycodeToStr( Keycode ).c_str() );
#endif
}

//-----------------------------------------------------------------------------
//	インスタンス管理
//-----------------------------------------------------------------------------
namespace
{
	Config* s_pConfig	= NULL;
}

//*****************************************************************************
//	インスタンス初期化
//*****************************************************************************
void InitConfig( void )
{
	if( !s_pConfig )
	{
		s_pConfig = new Config;
		MEM_CHECK( s_pConfig );

		s_pConfig->Load( CONFIG_PATH );
	}
}

//*****************************************************************************
//	インスタンス終了
//*****************************************************************************
void ExitConfig( void )
{
	SafeDelete( s_pConfig );
}

//******************************************************************************
//	インスタンス取得
//******************************************************************************
Config* GetConfig( void )
{
	_ASSERT( s_pConfig );
	return s_pConfig;
}
