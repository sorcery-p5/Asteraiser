#include "stdafx.h"
#include "ConfigScene.h"
#include "TitleScene.h"

#include "App/App.h"
#include "Config/Config.h"
#include "Resource/ResourceManager.h"
#include "Controller/SystemController.h"

#include "Menu/Menu.h"
#include "Menu/MenuData.h"
#include "Menu/MenuLayer.h"
#include "Menu/Object/MenuObject.h"

namespace
{
	pstr		MENU_PATH			= "Data/Menu/Config.xml";
	pstr		SCENE_TITLE			= "Title";

	pstr		STRING_REVERSE			= " REV";
	pstr		STRING_SETTING_KEY		= "Push Setting Key";
	pstr		STRING_SETTING_PAD		= "Push Setting Button";
	pstr		STRING_SETTING_X_AXIS	= "Push Left Direction";
	pstr		STRING_SETTING_Y_AXIS	= "Push Up Direction";
	pstr		STRING_DEFAULT			= "Reset Default Setting?\n Push OK or CANCEL";

	pstr		HELP_TEXT_STRING_SETTING_KEY		= "割り当てるキーを押してください。";
	pstr		HELP_TEXT_STRING_SETTING_PAD		= "割り当てるボタンを押してください。";
	pstr		HELP_TEXT_STRING_SETTING_X_AXIS		= "割り当てるレバーの左を入力してください。";
	pstr		HELP_TEXT_STRING_SETTING_Y_AXIS		= "割り当てるレバーの上を入力してください。";
	pstr		HELP_TEXT_STRING_DEFAULT			= "決定キーで初期化／キャンセルで戻ります。";

	const Label	LAYER_MAIN			= "Main";
	const Label	LAYER_KEY_CONFIG	= "KeyConfig";
	const Label	LAYER_PAD_CONFIG	= "PadConfig";
	const Label	LAYER_INPUT_WAIT	= "InputWait";

	const Label OBJECT_SCREEN		= "Screen";
	const Label OBJECT_BGM			= "BGM";
	const Label OBJECT_SOUND		= "Sound";
	const Label OBJECT_PAD			= "Pad";

	const Label OBJECT_WINDOW		= "Window";
	const Label OBJECT_FULL			= "Full";
	const Label OBJECT_BGM_VOL		= "BGMVol";
	const Label OBJECT_SOUND_VOL	= "SoundVol";

	const Label OBJECT_AXIS_X		= "AxisX";
	const Label OBJECT_AXIS_Y		= "AxisY";
	const Label OBJECT_KEY_X		= "KeyX";
	const Label OBJECT_KEY_Y		= "KeyY";
	const Label OBJECT_DEADZONE		= "DeadZone";
	const Label OBJECT_DEAD_VALUE	= "DeadV";

	const Label OBJECT_MESSAGE		= "Message";

	const Label	NOTIFY_INPUT		= "Input";
	const Label	NOTIFY_DEFUALT		= "Default";


	struct INPUT_OBJECT
	{
		byte		Value;
		Label		ObjectName;
		Label		TextName;
	};

	const INPUT_OBJECT BUTTON_OBJECT_TABLE[] =
	{
		{	CONTROLLER_BLADE,	"Blade",	"KeyBlade"		},
		{	CONTROLLER_RELOAD,	"Reload",	"KeyReload"		},
		{	CONTROLLER_DASH,	"Dash",		"KeyDash"		},
	};

	const INPUT_OBJECT SYSTEM_OBJECT_TABLE[] =
	{
		{	CONTROLLER_SYSTEM_OK,			"Ok",		"KeyOk"			},
		{	CONTROLLER_SYSTEM_CANCEL,		"Cancel",	"KeyCancel"		},
		{	CONTROLLER_SYSTEM_PAUSE,		"Pause",	"KeyPause"		},
		{	CONTROLLER_SYSTEM_EXIT,			"Exit",		"KeyExit"		},
	};

	const INPUT_OBJECT DIR_OBJECT_TABLE[] =
	{
		{	CONTROLLER_L - CONTROLLER_DIR_BEGIN,	"Left",		"KeyLeft"		},
		{	CONTROLLER_R - CONTROLLER_DIR_BEGIN,	"Right",	"KeyRight"		},
		{	CONTROLLER_U - CONTROLLER_DIR_BEGIN,	"Up",		"KeyUp"			},
		{	CONTROLLER_D - CONTROLLER_DIR_BEGIN,	"Down",		"KeyDown"		},
	};
	

	struct CONCURRENT_OBJECT
	{
		byte		Value[CONCURRENT_NUM];
		Label		ObjectName;
		Label		TextName;
	};

	const CONCURRENT_OBJECT CONCURRENT_OBJECT_TABLE[] =
	{
		{	{ CONTROLLER_BLADE, CONTROLLER_DASH, UCHAR_MAX, UCHAR_MAX },	"Spear",	"KeySpear"		},
		{	{ CONTROLLER_RELOAD, CONTROLLER_DASH, UCHAR_MAX, UCHAR_MAX },	"Ruler",	"KeyRuler"		},
	};

	//------------------------------------------------------------------------------
	//	入力の構造体検索（オブジェクト名）
	//------------------------------------------------------------------------------
	const INPUT_OBJECT* FindInputObject( const INPUT_OBJECT* pTable, int Num, Label ObjectName )
	{
		for( int i = 0; i < Num; i++ )
		{
			if( pTable[i].ObjectName == ObjectName )
			{
				return &pTable[i];
			}
		}
		return NULL;
	}

	//------------------------------------------------------------------------------
	//	同時押しの構造体検索（キー内容）
	//------------------------------------------------------------------------------
	const CONCURRENT_OBJECT* FindConcurrentObject( byte* pArray )
	{
		for( int i = 0; i < ArrayNum(CONCURRENT_OBJECT_TABLE); i++ )
		{
			bool bFind = true;
			for( int j = 0; j < CONCURRENT_NUM; j++ )
			{
				if( CONCURRENT_OBJECT_TABLE[i].Value[j] != pArray[j] )
				{
					bFind = false;
					break;
				}
			}
			if( bFind )
			{
				return &CONCURRENT_OBJECT_TABLE[i];
			}
		}
		return NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	タイトルシーン
//
///////////////////////////////////////////////////////////////////////////////
ConfigScene::ConfigScene( Label Name )
:Scene( Name )
{
	m_pMenuRes		= NULL;
	m_pMenu			= NULL;

	m_bChangeScreen	= false;
}
ConfigScene::~ConfigScene()
{
	SafeDelete( m_pMenu );
	SafeRelease( m_pMenuRes );
}

//*****************************************************************************
//	初期化
//*****************************************************************************
void ConfigScene::OnInit( void )
{
	//	コンフィグデータの読み出し
	Config* pConfig = GetConfig();
	m_SystemInfo = pConfig->GetSystemInfo();
	m_InputInfo = pConfig->GetInputInfo( 0 );

	//	メニュー
	m_pMenuRes = GetApp()->GetResMng()->LoadResource<MenuResource>( MENU_PATH );
	if( m_pMenuRes && m_pMenuRes->GetData() )
	{
		m_pMenu = new Menu( this );
		MEM_CHECK( m_pMenu );

		m_pMenu->Init( m_pMenuRes->GetData(), Matrix3::Trans( 0, 0 ) );
	}
	_ASSERT( m_pMenu );

	//	背景色変更
	P5::GetGraphix()->GetDevice()->SetScreenColor( Color(0,0,0) );
}

//*****************************************************************************
//	更新
//*****************************************************************************
void ConfigScene::Update( void )
{
	if( !m_pMenu ) return;

	//	入力待ちの更新
	if( !m_InputLayerName.IsEmpty() &&
		!m_InputObjectName.IsEmpty() )
	{
		_UpdateInputWait();
	}

	//	デフォルト待機状態
	if( m_bDefaultWait )
	{
		_UpdateDefaultWait();
	}
	
	//	パッドのON/OFF
	_UpdatePadEnable();

	m_pMenu->Update();

	//	終了
	if( m_pMenu->IsEmpty() )
	{
		Config* pConfig = GetConfig();

		//	スクリーンモード
		if( m_bChangeScreen &&
			m_SystemInfo.bFullScreen != !P5::GetWindow()->IsWindowed() )
		{
			P5::GetWindow()->ChangeScreenMode();
		}

		//	コンフィグデータ保存
		pConfig->SetSystemInfo( m_SystemInfo );
		pConfig->SetInputInfo( 0, m_InputInfo );

		pConfig->Apply();
		pConfig->Save();

		ChangeScene( new TitleScene( SCENE_TITLE, GetName() ) );
		return;
	}
				
	m_pMenu->Draw( Matrix3::Identity() );
}

//******************************************************************************
//	レイヤーが開いた
//******************************************************************************
void ConfigScene::OnOpenLayer( MenuLayer* pLayer )
{
	//	メイン
	if( pLayer->GetName() == LAYER_MAIN )
	{
		//	値の設定
		_SetMainLayer( pLayer );
	}
	//	キーコンフィグ
	else if( pLayer->GetName() == LAYER_KEY_CONFIG )
	{
		//	値の設定
		_SetKeyLayer( pLayer );
	}
	//	パッドコンフィグ
	else if( pLayer->GetName() == LAYER_PAD_CONFIG )
	{
		//	値の設定
		_SetPadLayer( pLayer );
	}
	//	入力待機
	else if( pLayer->GetName() == LAYER_INPUT_WAIT )
	{
		//	キーボード
		if( m_InputLayerName == LAYER_KEY_CONFIG )
		{
			pLayer->SetText( OBJECT_MESSAGE, STRING_SETTING_KEY );
			pLayer->ReplaceHelpText( HELP_TEXT_STRING_SETTING_KEY );
		}
		//	パッド
		else if( m_InputLayerName == LAYER_PAD_CONFIG )
		{
			//	パッドがない
			if( !GetSign()->GetPad( 0 ) )
			{
				pLayer->Close();
				return;
			}

			//	X軸
			if( m_InputObjectName == OBJECT_AXIS_X )
			{
				pLayer->SetText( OBJECT_MESSAGE, STRING_SETTING_X_AXIS );
				pLayer->ReplaceHelpText( HELP_TEXT_STRING_SETTING_X_AXIS );
			}
			//	Y軸
			else if( m_InputObjectName == OBJECT_AXIS_Y )
			{
				pLayer->SetText( OBJECT_MESSAGE, STRING_SETTING_Y_AXIS );
				pLayer->ReplaceHelpText( HELP_TEXT_STRING_SETTING_Y_AXIS );
			}
			else
			{
				pLayer->SetText( OBJECT_MESSAGE, STRING_SETTING_PAD );
				pLayer->ReplaceHelpText( HELP_TEXT_STRING_SETTING_PAD );
			}
		}

		//	デフォルト
		else if( m_bDefaultWait )
		{
			pLayer->SetText( OBJECT_MESSAGE, STRING_DEFAULT );
			pLayer->ReplaceHelpText( HELP_TEXT_STRING_DEFAULT );
		}
	}
}

//******************************************************************************
//	レイヤーが閉じられた
//******************************************************************************
void ConfigScene::OnCloseLayer( MenuLayer* pLayer )
{
	//	キーorパッドコンフィグ
	if( pLayer->GetName() == LAYER_KEY_CONFIG ||
		pLayer->GetName() == LAYER_PAD_CONFIG )
	{
		//	コンフィグデータ適用
		Config* pConfig = GetConfig();

		pConfig->SetInputInfo( 0, m_InputInfo );
		pConfig->Apply();
	}
	//	入力待機
	else if( pLayer->GetName() == LAYER_INPUT_WAIT )
	{
		m_InputLayerName.Clear();
		m_InputObjectName.Clear();
		
		m_bDefaultWait = false;
	}
}

//******************************************************************************
//	メニューからの通知
//******************************************************************************
void ConfigScene::OnObjectNotify( MenuObject* pObject, Label Name, float Param )
{
	//	入力設定
	if( Name == NOTIFY_INPUT )
	{
		m_InputLayerName	= pObject->GetParent()->GetName();
		m_InputObjectName	= pObject->GetName();
	}

	//	デフォルト
	if( Name == NOTIFY_DEFUALT )
	{
		m_bDefaultWait = true;
	}
}

//******************************************************************************
//	メニューの値変更
//******************************************************************************
void ConfigScene::OnObjectChangeValue( MenuObject* pObject )
{
	Config* pConfig = GetConfig();

	//	スクリーンモード
	if( pObject->GetName() == OBJECT_SCREEN )
	{
		m_SystemInfo.bFullScreen = pObject->GetSelectName() == OBJECT_FULL;
		pConfig->SetSystemInfo( m_SystemInfo );

		m_bChangeScreen = true;
	}
	//	BGM
	else if( pObject->GetName() == OBJECT_BGM )
	{
		m_SystemInfo.BGMVolume = pObject->GetValue() / 100.0f;
		pObject->GetParent()->SetText( OBJECT_BGM_VOL,	"%.0f", m_SystemInfo.BGMVolume * 100.0f );

		pConfig->SetSystemInfo( m_SystemInfo );

		GetTone()->SetChannelVolume( SOUND_CHANNEL_BGM, m_SystemInfo.BGMVolume );
	}
	//	サウンド
	else if( pObject->GetName() == OBJECT_SOUND )
	{
		m_SystemInfo.SoundVolume = pObject->GetValue() / 100.0f;
		pObject->GetParent()->SetText( OBJECT_SOUND_VOL,	"%.0f", m_SystemInfo.SoundVolume * 100.0f );
		
		pConfig->SetSystemInfo( m_SystemInfo );

		GetTone()->SetChannelVolume( SOUND_CHANNEL_SE, m_SystemInfo.SoundVolume );
	}
	//	レバー：デッドゾーン
	else if( pObject->GetName() == OBJECT_DEADZONE )
	{
		m_InputInfo.PadDeadzone = (uint)pObject->GetValue();
		pObject->GetParent()->SetText( OBJECT_DEAD_VALUE,	"%d", m_InputInfo.PadDeadzone );
	}
}

//------------------------------------------------------------------------------
//	入力待ち状態の更新
//------------------------------------------------------------------------------
void ConfigScene::_UpdateInputWait( void )
{
	MenuLayer* pLayer = m_pMenu->GetCurLayer();
	if( !pLayer || pLayer->GetName() != LAYER_INPUT_WAIT ) return;

	MenuLayer* pParent = m_pMenu->GetLayer( m_InputLayerName );
	if( !pParent ) return;

	if( !pLayer->IsWait() ) return;

	Keyboard* pKeyboard = GetSign()->GetKeyboard();
	Pad* pPad = GetSign()->GetPad( 0 );
	_ASSERT( pKeyboard );

	//	キーボード
	if( m_InputLayerName == LAYER_KEY_CONFIG )
	{
		byte KeyCode = pKeyboard->GetInputKey( INPUT_TRIG );
		
		//	対応するキーかどうか
		if( !KeyCode || Config::KeycodeToStr(KeyCode).empty() ) return;

		bool bChange = false;

		const INPUT_OBJECT* pObject = NULL;

		//	方向
		pObject = FindInputObject( DIR_OBJECT_TABLE, ArrayNum(DIR_OBJECT_TABLE), m_InputObjectName );
		if( pObject )
		{
			_SolveKeyDuplication( m_InputInfo.aKeyDir[ pObject->Value ], KeyCode, INPUT_DIR );

			m_InputInfo.aKeyDir[ pObject->Value ] = KeyCode;
			bChange = true;
		}

		//	システムキー
		pObject = FindInputObject( SYSTEM_OBJECT_TABLE, ArrayNum(SYSTEM_OBJECT_TABLE), m_InputObjectName );
		if( pObject )
		{
			_SolveKeyDuplication( m_InputInfo.aKeySystem[ pObject->Value ], KeyCode, INPUT_SYSTEM );

			m_InputInfo.aKeySystem[ pObject->Value ] = KeyCode;
			bChange = true;
		}
		
		//	ボタン
		pObject = FindInputObject( BUTTON_OBJECT_TABLE, ArrayNum(BUTTON_OBJECT_TABLE), m_InputObjectName );
		if( pObject )
		{
			_SolveKeyDuplication( m_InputInfo.aKeyButton[ pObject->Value ], KeyCode, INPUT_BUTTON );

			m_InputInfo.aKeyButton[ pObject->Value ] = KeyCode;
			bChange = true;
		}

		//	同時押し
		for( int i = 0; i < (int)m_InputInfo.vKeyConcurrent.size(); i++ )
		{
			const CONCURRENT_OBJECT* pCuncurrent = FindConcurrentObject( m_InputInfo.vKeyConcurrent[i].Array );
			if( pCuncurrent && pCuncurrent->ObjectName == m_InputObjectName )
			{
				_SolveKeyDuplication( m_InputInfo.vKeyConcurrent[i].Button, KeyCode, INPUT_BUTTON );

				m_InputInfo.vKeyConcurrent[i].Button = KeyCode;
				bChange = true;
				break;
			}
		}

		//	メニュー反映
		if( bChange )
		{
			_SetKeyLayer( pParent );
			pLayer->Close();
		}
	}
	//	パッド
	else if( m_InputLayerName == LAYER_PAD_CONFIG )
	{
		//	パッドがない
		if( !pPad )
		{
			pLayer->Close();
			return;
		}

		bool bChange = false;

		//	軸
		if( m_InputObjectName == OBJECT_AXIS_X || m_InputObjectName == OBJECT_AXIS_Y )
		{
			PAD_AXIS	PadAxis		= PAD_AXIS_X;
			bool		bRev		= false;

			for( int i = 0; i < PAD_AXIS_NUM; i++ )
			{
				float Value = pPad->GetAxis( PAD_AXIS(i), INPUT_TRIG );
				if( Value == 0.0f ) continue;
				
				bRev = Value > 0.0f;
				PadAxis = PAD_AXIS(i);

				//	X軸
				if( m_InputObjectName == OBJECT_AXIS_X )
				{
					_SolveAxisDuplication( m_InputInfo.PadAxisX, m_InputInfo.bPadAxisXRev, PadAxis );

					m_InputInfo.PadAxisX = PadAxis;
					m_InputInfo.bPadAxisXRev = bRev;
					bChange = true;
				}
				//	Y軸
				else if( m_InputObjectName == OBJECT_AXIS_Y )
				{
					_SolveAxisDuplication( m_InputInfo.PadAxisY, m_InputInfo.bPadAxisYRev, PadAxis );

					m_InputInfo.bPadAxisYRev = bRev;
					m_InputInfo.PadAxisY = PadAxis;
					bChange = true;
				}
			}
		}
		else
		{
			byte Button = pPad->GetInputButton( INPUT_TRIG );
			if( Button == UCHAR_MAX ) return;

			const INPUT_OBJECT* pObject = NULL;

			//	システムキー
			pObject = FindInputObject( SYSTEM_OBJECT_TABLE, ArrayNum(SYSTEM_OBJECT_TABLE), m_InputObjectName );
			if( pObject )
			{
				_SolvePadDuplication( m_InputInfo.aPadSystem[ pObject->Value ], Button, INPUT_SYSTEM );

				m_InputInfo.aPadSystem[ pObject->Value ] = Button;
				bChange = true;
			}
			
			//	ボタン
			pObject = FindInputObject( BUTTON_OBJECT_TABLE, ArrayNum(BUTTON_OBJECT_TABLE), m_InputObjectName );
			if( pObject )
			{
				_SolvePadDuplication( m_InputInfo.aPadButton[ pObject->Value ], Button, INPUT_BUTTON );

				m_InputInfo.aPadButton[ pObject->Value ] = Button;
				bChange = true;
			}

			//	同時押し
			for( int i = 0; i < (int)m_InputInfo.vPadConcurrent.size(); i++ )
			{
				const CONCURRENT_OBJECT* pCuncurrent = FindConcurrentObject( m_InputInfo.vPadConcurrent[i].Array );
				if( pCuncurrent && pCuncurrent->ObjectName == m_InputObjectName )
				{
					_SolvePadDuplication( m_InputInfo.vPadConcurrent[ i ].Button, Button, INPUT_BUTTON );

					m_InputInfo.vPadConcurrent[i].Button = Button;
					bChange = true;
					break;
				}
			}
		}

		//	メニュー反映
		if( bChange )
		{
			_SetPadLayer( pParent );
			pLayer->Close();
		}
	}
}

//------------------------------------------------------------------------------
//	デフォルト待機状態
//------------------------------------------------------------------------------
void ConfigScene::_UpdateDefaultWait( void )
{
	MenuLayer* pLayer = m_pMenu->GetCurLayer();
	if( !pLayer || pLayer->GetName() != LAYER_INPUT_WAIT ) return;

	SystemController Ctrl;

	if( Ctrl.IsInput( CONTROLLER_SYSTEM_OK, INPUT_TRIG ) )
	{
		//	リセットする
		m_SystemInfo.Default();
		m_InputInfo.Default(0);
		
		//	適用
		Config* pConfig = GetConfig();
		pConfig->SetSystemInfo( m_SystemInfo );
		pConfig->SetInputInfo( 0, m_InputInfo );

		pConfig->Apply();

		_SetMainLayer( m_pMenu->GetLayer( LAYER_MAIN ) );

		pLayer->Close();
		m_bDefaultWait = false;
		m_bChangeScreen = true;
	}
	else if( Ctrl.IsInput( CONTROLLER_SYSTEM_CANCEL, INPUT_TRIG ) )
	{
		pLayer->Close();
		m_bDefaultWait = false;	
	}
}

//------------------------------------------------------------------------------
//	パッドのON/OFFを設定する
//------------------------------------------------------------------------------
void ConfigScene::_UpdatePadEnable( void )
{
	MenuLayer* pLayer = m_pMenu->GetCurLayer();
	if( !pLayer || pLayer->GetName() != LAYER_MAIN ) return;

	pLayer->SetColor( OBJECT_PAD, GetSign()->GetPad(0) != NULL? Color::White() : Color::Gray(128) );
}

//------------------------------------------------------------------------------
//	キー設定の重複を解決する
//------------------------------------------------------------------------------
void ConfigScene::_SolveKeyDuplication( byte OldValue, byte NewValue, INPUT InputType )
{
	//	方向
	for( int i = 0; i < CONTROLLER_DIR_NUM; i++ )
	{
		if( m_InputInfo.aKeyDir[i] == NewValue )
		{
			m_InputInfo.aKeyDir[i] = OldValue;
		}
	}
	
	if( InputType != INPUT_BUTTON )
	{
		//	システムキー
		for( int i = 0; i < CONTROLLER_SYSTEM_KEY_NUM; i++ )
		{
			if( m_InputInfo.aKeySystem[i] == NewValue )
			{
				m_InputInfo.aKeySystem[i] = OldValue;
			}
		}
	}
	
	if( InputType != INPUT_SYSTEM )
	{
		//	ボタン
		for( int i = 0; i < CONTROLLER_BUTTON_NUM; i++ )
		{
			if( m_InputInfo.aKeyButton[i] == NewValue )
			{
				m_InputInfo.aKeyButton[i] = OldValue;
			}
		}

		//	同時押し
		for( int i = 0; i < (int)m_InputInfo.vKeyConcurrent.size(); i++ )
		{
			if( m_InputInfo.vKeyConcurrent[i].Button == NewValue )
			{
				m_InputInfo.vKeyConcurrent[i].Button = OldValue;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	パッドボタン設定の重複を解決する
//------------------------------------------------------------------------------
void ConfigScene::_SolvePadDuplication( byte OldValue, byte NewValue, INPUT InputType )
{
	if( InputType != INPUT_BUTTON )
	{
		//	システムキー
		for( int i = 0; i < CONTROLLER_SYSTEM_KEY_NUM; i++ )
		{
			if( m_InputInfo.aPadSystem[i] == NewValue )
			{
				m_InputInfo.aPadSystem[i] = OldValue;
			}
		}
	}
	
	if( InputType != INPUT_SYSTEM )
	{
		//	ボタン
		for( int i = 0; i < CONTROLLER_BUTTON_NUM; i++ )
		{
			if( m_InputInfo.aPadButton[i] == NewValue )
			{
				m_InputInfo.aPadButton[i] = OldValue;
			}
		}

		//	同時押し
		for( int i = 0; i < (int)m_InputInfo.vPadConcurrent.size(); i++ )
		{
			if( m_InputInfo.vPadConcurrent[i].Button == NewValue )
			{
				m_InputInfo.vPadConcurrent[i].Button = OldValue;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	パッド軸設定の重複を解決する
//------------------------------------------------------------------------------
void ConfigScene::_SolveAxisDuplication( PAD_AXIS OldValue, bool bOldRev, PAD_AXIS NewValue )
{
	if( m_InputInfo.PadAxisX == NewValue )
	{
		m_InputInfo.PadAxisX = OldValue;
		m_InputInfo.bPadAxisXRev = bOldRev;
	}

	if( m_InputInfo.PadAxisY == NewValue )
	{
		m_InputInfo.PadAxisY = OldValue;
		m_InputInfo.bPadAxisYRev = bOldRev;
	}
}

//------------------------------------------------------------------------------
//	最上位レイヤーの表示設定
//------------------------------------------------------------------------------
void ConfigScene::_SetMainLayer( MenuLayer* pLayer )
{
	if( !pLayer ) return;

	//	スクリーン
	pLayer->SetObjectSelect( OBJECT_SCREEN, m_SystemInfo.bFullScreen? OBJECT_FULL : OBJECT_WINDOW );

	//	サウンド
	pLayer->SetObjectValue( OBJECT_BGM,		m_SystemInfo.BGMVolume * 100.0f );
	pLayer->SetObjectValue( OBJECT_SOUND,	m_SystemInfo.SoundVolume * 100.0f );
	pLayer->SetText( OBJECT_BGM_VOL,	"%.0f", m_SystemInfo.BGMVolume * 100.0f );
	pLayer->SetText( OBJECT_SOUND_VOL,	"%.0f", m_SystemInfo.SoundVolume * 100.0f );

	//	パッド
	pLayer->SetObjectEnable( OBJECT_PAD, GetSign()->GetPad(0) != NULL );
}

//------------------------------------------------------------------------------
//	キー設定レイヤーの表示設定
//------------------------------------------------------------------------------
void ConfigScene::_SetKeyLayer( MenuLayer* pLayer )
{
	if( !pLayer ) return;

	//	方向
	for( int i = 0; i < CONTROLLER_DIR_NUM; i++ )
	{
		pLayer->SetText( DIR_OBJECT_TABLE[i].TextName, Config::KeycodeToStr( m_InputInfo.aKeyDir[DIR_OBJECT_TABLE[i].Value] ).c_str() );
	}
	
	//	システムキー
	for( int i = 0; i < CONTROLLER_SYSTEM_KEY_NUM; i++ )
	{
		pLayer->SetText( SYSTEM_OBJECT_TABLE[i].TextName, Config::KeycodeToStr( m_InputInfo.aKeySystem[SYSTEM_OBJECT_TABLE[i].Value] ).c_str() );
	}

	//	ボタン
	for( int i = 0; i < CONTROLLER_BUTTON_NUM; i++ )
	{
		pLayer->SetText( BUTTON_OBJECT_TABLE[i].TextName, Config::KeycodeToStr( m_InputInfo.aKeyButton[BUTTON_OBJECT_TABLE[i].Value] ).c_str() );
	}

	//	同時押し
	for( int i = 0; i < (int)m_InputInfo.vKeyConcurrent.size(); i++ )
	{
		const CONCURRENT_OBJECT* pCuncurrent = FindConcurrentObject( m_InputInfo.vKeyConcurrent[i].Array );
		if( pCuncurrent )
		{
			pLayer->SetText( pCuncurrent->TextName, Config::KeycodeToStr( m_InputInfo.vKeyConcurrent[i].Button ).c_str() );
		}
	}
}

//------------------------------------------------------------------------------
//	パッド設定レイヤーの表示設定
//------------------------------------------------------------------------------
void ConfigScene::_SetPadLayer( MenuLayer* pLayer )
{
	if( !pLayer ) return;

	//	軸
	pLayer->SetText( OBJECT_KEY_X, _GetAxisString( m_InputInfo.PadAxisX, m_InputInfo.bPadAxisXRev ).c_str() );
	pLayer->SetText( OBJECT_KEY_Y, _GetAxisString( m_InputInfo.PadAxisY, m_InputInfo.bPadAxisYRev ).c_str() );

	//	レバーデッドゾーン
	pLayer->SetObjectValue( OBJECT_DEADZONE,	m_InputInfo.PadDeadzone );
	pLayer->SetText( OBJECT_DEAD_VALUE,	"%d",	m_InputInfo.PadDeadzone );

	//	システムキー
	for( int i = 0; i < CONTROLLER_SYSTEM_KEY_NUM; i++ )
	{
		pLayer->SetText( SYSTEM_OBJECT_TABLE[i].TextName, "%d", m_InputInfo.aPadSystem[SYSTEM_OBJECT_TABLE[i].Value] + 1 );
	}

	//	ボタン
	for( int i = 0; i < CONTROLLER_BUTTON_NUM; i++ )
	{
		pLayer->SetText( BUTTON_OBJECT_TABLE[i].TextName, "%d", m_InputInfo.aPadButton[BUTTON_OBJECT_TABLE[i].Value] + 1 );
	}

	//	同時押し
	for( int i = 0; i < (int)m_InputInfo.vPadConcurrent.size(); i++ )
	{
		const CONCURRENT_OBJECT* pCuncurrent = FindConcurrentObject( m_InputInfo.vPadConcurrent[i].Array );
		if( pCuncurrent )
		{
			pLayer->SetText( pCuncurrent->TextName, "%d", m_InputInfo.vPadConcurrent[i].Button + 1 );
		}
	}
}

//------------------------------------------------------------------------------
//	軸を表す文字列を得る
//------------------------------------------------------------------------------
std::string ConfigScene::_GetAxisString( PAD_AXIS Axis, bool bRev ) const
{
	std::string Str = Config::PadAxisToStr( Axis );

	if( bRev ) Str += STRING_REVERSE;

	return Str;
}