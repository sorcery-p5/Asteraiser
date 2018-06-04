#include "P5.h"
#include "DrawDevice.h"

#include "Texture/Texture.h"


namespace
{
	const int		RESRORE_TRY_NUM	= 100;

	const dword		DEFAULT_FVF		= (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);

	pstr			PATH_SNAPSHOT_FORMAT	= "Snapshot/[%04d_%02d_%02d]%02d_%02d_%02d_%03d.bmp";
	pstr			PATH_SNAPSHOT_DIR		= "Snapshot/";
}

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	描画デバイス
//
///////////////////////////////////////////////////////////////////////////////
DrawDevice::DrawDevice( void )
{
	m_pWindow	= NULL;
	m_pD3D		= NULL;
	m_pDevice	= NULL;

	ZeroStruct( m_WindowFormat );
	ZeroStruct( m_FullFormat );
	m_bModeChange	= false;

	m_CurBlend		= BLEND_NONE;
	m_pCurTexture	= NULL;

	m_Transparent.SetZero();
}
DrawDevice::~DrawDevice()
{
	SafeRelease( m_pDevice );
	SafeRelease( m_pD3D );
}

//*****************************************************************************
//	初期化
//*****************************************************************************
bool DrawDevice::Init( Window* pWindow )
{
	Log( "　　□デバイス初期化開始" );

	m_pWindow = pWindow;

	//	D3Dの初期化
	m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	if( !m_pD3D )
	{		
		Log( "　　×D3DCreate 初期化失敗" );
		return false;
	}

	//	デバイスパラメータ初期化
	if( !_InitDevParam() ) return false;

	//	デバイスを初期化
	if( !_InitDevice() ) return false;

	Log( "　　○デバイス初期化完了" );
	Log( "　　----------------------------------------" );

	return true;
}

//*****************************************************************************
//	描画開始
//*****************************************************************************
bool DrawDevice::Begin( void )
{
	//	バッファのクリア
	m_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_ScreenColor, 1.0f, 0 );

	//	シーン開始
	return SUCCEEDED( m_pDevice->BeginScene() );
}


//*****************************************************************************
//	描画終了
//*****************************************************************************
bool DrawDevice::End( void )
{
	//	シーン終了
	m_pDevice->EndScene();

	//	更新
	HRESULT hr = m_pDevice->Present( NULL, NULL, NULL, NULL );
	if( FAILED(hr) )
	{		
		//	ドライバエラー、強制終了
		if( hr != D3DERR_DEVICELOST ) return false;

		//	ロストチェック
		if( !_RestoreDevice() ) return false;
	}

	return true;
}

//*****************************************************************************
//	リセット
//*****************************************************************************
void DrawDevice::Reset( void )
{
	//	プレゼントパラメータを再設定
	D3DPRESENT_PARAMETERS PresentParam;
	_GetDefaultPresentParam( PresentParam );

	//	デバイスをリセット
	m_pDevice->Reset( &PresentParam );

	//	レンダーステート再設定
	_ResetRenderState();

	//	ブレンド再設定
	SetBlend( m_CurBlend );
}

//*****************************************************************************
//	スクリーンショットを保存
//*****************************************************************************
bool DrawDevice::SaveScreenShot( pstr pPath )
{
	if( !m_pDevice ) return false;

	Point Size = m_pWindow->GetSize();

	//	ファイル名設定
	String256 Path;
	if( pPath )
	{
		Path = pPath;
	}
	else
	{
		//	保存ディレクトリを生成
		::CreateDirectory( PATH_SNAPSHOT_DIR, NULL );

		//	ファイル名を現在時間から取得
		SYSTEMTIME t;
		::GetLocalTime(&t); 
		Path.Format( PATH_SNAPSHOT_FORMAT, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds );
	}

	//	BMPファイルのヘッダを作成
	BITMAPFILEHEADER bmpfh = {0};
	BITMAPINFOHEADER bmpih = {0};
	{
		bmpfh.bfType			= ('B' << 0) | ('M' << 8);
		bmpfh.bfSize			= ((int)Size.Area() * 3) + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		bmpfh.bfOffBits			= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		bmpih.biSize			= sizeof(BITMAPINFOHEADER);
		bmpih.biWidth			= (int)Size.x;
		bmpih.biHeight			= (int)Size.y;
		bmpih.biPlanes			= 1;
		bmpih.biBitCount		= 24;
		bmpih.biCompression		= BI_RGB;
	}

	ReleasePtr<IDirect3DSurface9> pSurface;
	D3DLOCKED_RECT		LockRect	= {0};


	//	ファイルを開く
	FileHandle fp( Path, "wb" );
	if( !fp ) return false;

	//	バックバッファサーフェイスを取得
	HRESULT	hr;
	hr = m_pDevice->GetRenderTarget( 0, &pSurface );
	if( FAILED(hr) ) return false;

	hr = pSurface->LockRect( &LockRect, NULL, 0 );
	if( FAILED(hr) ) return false;

	fwrite( (void*)&bmpfh, sizeof(BITMAPFILEHEADER), 1, fp );
	fwrite( (void*)&bmpih, sizeof(BITMAPINFOHEADER), 1, fp );

	bool	bFullColor	= _IsFullColor( GetBackFormat() );
	bool	bFormat565	= GetBackFormat() == D3DFMT_R5G6B5;
	uint	pitch		= LockRect.Pitch;
	byte*	pt			= (byte*)(LockRect.pBits) + pitch * (bmpih.biHeight-1);
	byte	pixel[3];

	for( int y = 0; y < bmpih.biHeight; y++ )
	{
		for( int x = 0; x < bmpih.biWidth; x++ )
		{
			if( bFullColor )
			{
				dword* p = (dword*)pt;
				pixel[0] = (byte)( (p[x]>> 0)&0xFF );
				pixel[1] = (byte)( (p[x]>> 8)&0xFF );
				pixel[2] = (byte)( (p[x]>>16)&0xFF );
			}
			else if( bFormat565 )
			{
				word* p = (word*)pt;
				pixel[0] = (byte)( ((p[x]>> 0)&0x1F) * 8.2258);
				pixel[1] = (byte)( ((p[x]>> 5)&0x3F) * 4.047619);
				pixel[2] = (byte)( ((p[x]>>11)&0x1F) * 8.2258);	
			}
			else
			{
				word* p	= (word*)pt;
				pixel[0] = (byte)( ((p[x]>> 0)&0x1F) * 8.2258);
				pixel[1] = (byte)( ((p[x]>> 5)&0x1F) * 8.2258);
				pixel[2] = (byte)( ((p[x]>>10)&0x1F) * 8.2258);
			}
			fwrite( pixel, 1, 3, fp );	
		}
		pt -= pitch;
	}
	pSurface->UnlockRect();

	return true;
}

//*****************************************************************************
//	ブレンドモードを登録
//*****************************************************************************
void DrawDevice::SetBlend( BLEND_MODE Blend )
{
	if( m_CurBlend == Blend ) return;

	//	デバイスに設定
	switch( Blend )
	{
	case BLEND_NONE:	//不透明
		m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
		m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		break;

	case BLEND_ALPHA:	//半透明
		m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		break;

	case BLEND_ADD:		//加算
		m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		break;

	case BLEND_SUB:		//減算(疑似)
		m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );
		m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
		break;

	case BLEND_MUL:		//乗算
		m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );
		m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
		break;

	case BLEND_REV:		//反転
		m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_INVDESTCOLOR );
		m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
		break;
	}

	m_CurBlend = Blend;
}
	
//*****************************************************************************
//	テクスチャを登録
//*****************************************************************************
void DrawDevice::SetTexture( const Texture* pTex )
{
	if( m_pCurTexture == pTex ) return;

	//	デバイスに設定
	if( pTex )	m_pDevice->SetTexture( 0, pTex->GetTex() );
	else		m_pDevice->SetTexture( 0, NULL );

	m_pCurTexture = pTex;
}

//-----------------------------------------------------------------------------
//	デバイスのパラメータを初期化
//-----------------------------------------------------------------------------
bool DrawDevice::_InitDevParam( void )
{
	Log( "　　　□スクリーンフォーマット取得" );

	//	ウィンドウモードのフォーマット取得
	if( !_GetWindowFormat( m_WindowFormat ) )
	{
		Log( "　　　×ウィンドウモード・フォーマット取得失敗" );
		return false;
	}

	Log( "　　　○ウィンドウモード・フォーマット取得[Back = %d /Depth = %d / Tex = %d / %dHz]",
			m_WindowFormat.BackFormat, m_WindowFormat.DepthFormat, m_WindowFormat.TexFormat, m_WindowFormat.RefreshRate );

	//	フルスクリーンモードのフォーマット取得
	bool bFullColor = _IsFullColor( m_WindowFormat.BackFormat ); 
	if( !_GetFullFormat( m_FullFormat, m_pWindow->GetSize(), bFullColor) )
	{
		Log( "　　　×フルスクリーンモード・フォーマット取得失敗" );
		return false;
	}

	Log( "　　　○フルスクリーンモード・フォーマット取得[Back = %d /Depth = %d / Tex = %d / %dHz]",
			m_FullFormat.BackFormat, m_FullFormat.DepthFormat, m_FullFormat.TexFormat, m_FullFormat.RefreshRate );

	//	フルスクリーンが可能か？
	m_bModeChange = m_FullFormat.BackFormat != D3DFMT_UNKNOWN;
	if( !m_bModeChange )
	{
		//	切り替え不能：強制ウィンドウモード
		m_pWindow->SetWindowed();
		
		Log( "　　　△フルスクリーン切り替え不可能・強制ウィンドウモード" );
	}

	Log( "　　　○スクリーンフォーマット取得完了" );
	Log( "　　　----------------------------------------" );

	return true;
}

//-----------------------------------------------------------------------------
//	デバイスを初期化
//-----------------------------------------------------------------------------
bool DrawDevice::_InitDevice( void )
{
	Log( "　　　□描画デバイス初期化" );

	HRESULT	hr;

	D3DCAPS9 DevCaps;
	hr = m_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DevCaps );
	if( FAILED(hr) )
	{
		Log( "　　　×デバイス性能取得失敗" );
		return false;
	}

	//	ハードウェアTnLが可能か調べる
	uint Behavior = ( DevCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )?	D3DCREATE_HARDWARE_VERTEXPROCESSING :
																			D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	
	Log( "　　　▽TnL方式[%s]", (DevCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)? "ハードウェア" : "ソフトウェア" );

	//	FPU設定
	Behavior |= D3DCREATE_FPU_PRESERVE;

	//	プレゼントパラメータを取得
	D3DPRESENT_PARAMETERS PresentParam;
	_GetDefaultPresentParam( PresentParam );

	//	デバイスを作成する
	hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_pWindow->GetHwnd(), Behavior, &PresentParam, &m_pDevice );
	if( FAILED(hr) )
	{
		Log( "　　　×デバイス作成失敗" );
		return false;
	}

	//	レンダーステート初期化
	_ResetRenderState();

	//	クリア
	m_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_ScreenColor, 1.0f, 0 );

	Log( "　　　○描画デバイス初期化完了" );
	Log( "　　　----------------------------------------" );

	return true;
}

//-----------------------------------------------------------------------------
//	ウィンドウスクリーンフォーマットを検出
//-----------------------------------------------------------------------------
bool DrawDevice::_GetWindowFormat( DISP_FORMAT& DispFormat )
{
	HRESULT hr;

	//	現在のディスプレイモード取得
	D3DDISPLAYMODE	DispMode;
	hr = m_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &DispMode );
	if( FAILED(hr) ) return false;

	DispFormat.BackFormat	= DispMode.Format;
	DispFormat.RefreshRate	= DispMode.RefreshRate;

	//	深度バッファ取得
	DispFormat.DepthFormat	= _GetDepthFormat( DispFormat.BackFormat );

	//	テクスチャフォーマット取得
	DispFormat.TexFormat	= _GetTexFormat( DispFormat.BackFormat );

	return true;
}

//-----------------------------------------------------------------------------
//	フルスクリーンフォーマットを検出
//-----------------------------------------------------------------------------
bool DrawDevice::_GetFullFormat( DISP_FORMAT& DispFormat, const Point& Size, bool bFullColor )
{
	D3DFORMAT  aFormat16[]	= { D3DFMT_R5G6B5, D3DFMT_X1R5G5B5, D3DFMT_A1R5G5B5, D3DFMT_UNKNOWN };	
	D3DFORMAT  aFormat32[]	= { D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8, D3DFMT_UNKNOWN };
	D3DFORMAT* aFormat		= bFullColor? aFormat32 : aFormat16;

	DispFormat.BackFormat	= D3DFMT_UNKNOWN;
	DispFormat.RefreshRate	= 0;

	for( int i = 0; aFormat[i] != D3DFMT_UNKNOWN; i++ )
	{
		//	アダプタモードの数を取得
		int ModeNum = m_pD3D->GetAdapterModeCount( D3DADAPTER_DEFAULT, aFormat[i] );

		for( int Mode = 0; Mode < ModeNum; Mode++ )
		{
			//	ディスプレイモードを取得する
			D3DDISPLAYMODE	DispMode;
			HRESULT hr = m_pD3D->EnumAdapterModes( D3DADAPTER_DEFAULT, aFormat[i], Mode, &DispMode );
			if( FAILED(hr) ) return false;

			//	サイズの調査
			if( DispMode.Width != (uint)Size.x || DispMode.Height != (uint)Size.y )
			{
				continue;
			}

			DispFormat.BackFormat	= DispMode.Format;
			DispFormat.RefreshRate	= DispMode.RefreshRate;

			//	リフレッシュレートが６０ならそのまま抜ける
			if( DispMode.RefreshRate == FPS )
			{
				break;
			}
		}
	}
	
	//	深度バッファ取得
	DispFormat.DepthFormat	= _GetDepthFormat( DispFormat.BackFormat );

	//	テクスチャフォーマット取得
	DispFormat.TexFormat	= _GetTexFormat( DispFormat.BackFormat );

	return true;
}

//-----------------------------------------------------------------------------
//	深度バッファフォーマット取得
//-----------------------------------------------------------------------------
D3DFORMAT DrawDevice::_GetDepthFormat( D3DFORMAT BackFormat )
{	
	D3DFORMAT aFormat[] ={	D3DFMT_D16_LOCKABLE,	D3DFMT_D16,		D3DFMT_D15S1,
							D3DFMT_D32,				D3DFMT_D24S8,	D3DFMT_D24X8,	D3DFMT_D24X4S4, 
							D3DFMT_UNKNOWN };

	HRESULT hr;
	for( int i = 0; aFormat[i] != D3DFMT_UNKNOWN; i++ )
	{
		hr = m_pD3D->CheckDeviceFormat(	D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, BackFormat,	D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, aFormat[i] );
		if( SUCCEEDED(hr) )
		{
			hr = m_pD3D->CheckDepthStencilMatch( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, BackFormat, BackFormat, aFormat[i]);
			if( SUCCEEDED(hr) )
			{
				return aFormat[i];
			}
		}
	}

	//	見つからなかったらとりあえず16ビット
	return D3DFMT_D16;
}

//-----------------------------------------------------------------------------
//	テクスチャフォーマットを取得	
//-----------------------------------------------------------------------------
D3DFORMAT DrawDevice::_GetTexFormat( D3DFORMAT BackFormat )
{
	D3DFORMAT aFormat[] ={	D3DFMT_A8R8G8B8,	D3DFMT_A8B8G8R8,
							D3DFMT_A4R4G4B4,	D3DFMT_A1R5G5B5,
							D3DFMT_A2R10G10B10, D3DFMT_A2B10G10R10,
							D3DFMT_X8R8G8B8,	D3DFMT_X8B8G8R8,
							D3DFMT_R8G8B8,		D3DFMT_R5G6B5,
							D3DFMT_X1R5G5B5,	D3DFMT_R3G3B2,
							D3DFMT_UNKNOWN};

	HRESULT	hr;
	for( int i = 0; aFormat[i] != D3DFMT_UNKNOWN; i++ )
	{
		hr = m_pD3D->CheckDeviceFormat(	D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, BackFormat, 0, D3DRTYPE_TEXTURE, aFormat[i] );		
		if( SUCCEEDED( hr ) )
		{
			return aFormat[i];
		}
	}

	//	見つからなかったらとりあえずA付き32ビット
	return D3DFMT_A8R8G8B8;
}


//-----------------------------------------------------------------------------
//	デフォルトのプレゼントパラメータを取得
//-----------------------------------------------------------------------------
void DrawDevice::_GetDefaultPresentParam( D3DPRESENT_PARAMETERS& Param )
{
	ZeroStruct( Param );

	const DISP_FORMAT& DispFormat		= m_pWindow->IsWindowed()? m_WindowFormat : m_FullFormat;

	Param.BackBufferWidth				= (uint)m_pWindow->GetSize().x;
	Param.BackBufferHeight				= (uint)m_pWindow->GetSize().y;

	Param.BackBufferFormat				= DispFormat.BackFormat;
	Param.BackBufferCount				= 1;

	Param.MultiSampleType				= D3DMULTISAMPLE_NONE;
	Param.MultiSampleQuality			= 0;
	
	Param.SwapEffect					= D3DSWAPEFFECT_COPY;

	Param.hDeviceWindow					= m_pWindow->GetHwnd();
	Param.Windowed						= m_pWindow->IsWindowed();

	Param.EnableAutoDepthStencil		= TRUE;
	Param.AutoDepthStencilFormat		= DispFormat.DepthFormat;

	Param.Flags							= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	Param.FullScreen_RefreshRateInHz	= m_pWindow->IsWindowed()? 0 : m_FullFormat.RefreshRate;
	Param.PresentationInterval			= D3DPRESENT_INTERVAL_ONE;
}

//-----------------------------------------------------------------------------
//	レンダーステートをリセット
//-----------------------------------------------------------------------------
void DrawDevice::_ResetRenderState( void )
{
	m_pDevice->SetRenderState( D3DRS_ZENABLE,				TRUE );
	m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE,			TRUE );
	m_pDevice->SetRenderState( D3DRS_ZFUNC,					D3DCMP_LESSEQUAL );

	m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		TRUE );
	m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE,		TRUE );
	m_pDevice->SetRenderState( D3DRS_ALPHAREF,				0 );
	m_pDevice->SetRenderState( D3DRS_ALPHAFUNC,				D3DCMP_GREATER );
	m_pDevice->SetRenderState( D3DRS_BLENDOP,				D3DBLENDOP_ADD );

	m_pDevice->SetRenderState( D3DRS_CULLMODE,				D3DCULL_NONE );
	m_pDevice->SetRenderState( D3DRS_FILLMODE,				D3DFILL_SOLID );
	m_pDevice->SetRenderState( D3DRS_SHADEMODE,				D3DSHADE_GOURAUD );

	m_pDevice->SetRenderState( D3DRS_FOGTABLEMODE,			D3DFOG_NONE);
	m_pDevice->SetRenderState( D3DRS_FOGVERTEXMODE,			D3DFOG_LINEAR);
	m_pDevice->SetRenderState( D3DRS_RANGEFOGENABLE,		FALSE);

	m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );

	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE );
	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );

	//	頂点フォーマット設定
	m_pDevice->SetFVF( DEFAULT_FVF ); 

	m_CurBlend = BLEND_NONE;
}

//-----------------------------------------------------------------------------
//	ロストしたデバイスを復帰する
//-----------------------------------------------------------------------------
bool DrawDevice::_RestoreDevice( void )
{
	//	復元可能になるまで繰り返す
	for( int i = 0; i < RESRORE_TRY_NUM; i++ )
	{	
		if( m_pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET )
		{
			GetGraphix()->Reset();
			return true;
		}
		Sleep( 100 );
	}
	return false;
}

//-----------------------------------------------------------------------------
//	現在の描画フォーマットを得る
//-----------------------------------------------------------------------------
const DrawDevice::DISP_FORMAT& DrawDevice::_GetCurFormat( void ) const
{
	return m_pWindow->IsWindowed()? m_WindowFormat : m_FullFormat;
}

//-----------------------------------------------------------------------------
//	フォーマットがフルカラーかどうか
//-----------------------------------------------------------------------------
bool DrawDevice::_IsFullColor( D3DFORMAT Format )
{
	return (Format == D3DFMT_X8R8G8B8 || Format == D3DFMT_A8R8G8B8 || Format == D3DFMT_R8G8B8 );
}



}