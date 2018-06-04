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
//	�`��f�o�C�X
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
//	������
//*****************************************************************************
bool DrawDevice::Init( Window* pWindow )
{
	Log( "�@�@���f�o�C�X�������J�n" );

	m_pWindow = pWindow;

	//	D3D�̏�����
	m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	if( !m_pD3D )
	{		
		Log( "�@�@�~D3DCreate ���������s" );
		return false;
	}

	//	�f�o�C�X�p�����[�^������
	if( !_InitDevParam() ) return false;

	//	�f�o�C�X��������
	if( !_InitDevice() ) return false;

	Log( "�@�@���f�o�C�X����������" );
	Log( "�@�@----------------------------------------" );

	return true;
}

//*****************************************************************************
//	�`��J�n
//*****************************************************************************
bool DrawDevice::Begin( void )
{
	//	�o�b�t�@�̃N���A
	m_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_ScreenColor, 1.0f, 0 );

	//	�V�[���J�n
	return SUCCEEDED( m_pDevice->BeginScene() );
}


//*****************************************************************************
//	�`��I��
//*****************************************************************************
bool DrawDevice::End( void )
{
	//	�V�[���I��
	m_pDevice->EndScene();

	//	�X�V
	HRESULT hr = m_pDevice->Present( NULL, NULL, NULL, NULL );
	if( FAILED(hr) )
	{		
		//	�h���C�o�G���[�A�����I��
		if( hr != D3DERR_DEVICELOST ) return false;

		//	���X�g�`�F�b�N
		if( !_RestoreDevice() ) return false;
	}

	return true;
}

//*****************************************************************************
//	���Z�b�g
//*****************************************************************************
void DrawDevice::Reset( void )
{
	//	�v���[���g�p�����[�^���Đݒ�
	D3DPRESENT_PARAMETERS PresentParam;
	_GetDefaultPresentParam( PresentParam );

	//	�f�o�C�X�����Z�b�g
	m_pDevice->Reset( &PresentParam );

	//	�����_�[�X�e�[�g�Đݒ�
	_ResetRenderState();

	//	�u�����h�Đݒ�
	SetBlend( m_CurBlend );
}

//*****************************************************************************
//	�X�N���[���V���b�g��ۑ�
//*****************************************************************************
bool DrawDevice::SaveScreenShot( pstr pPath )
{
	if( !m_pDevice ) return false;

	Point Size = m_pWindow->GetSize();

	//	�t�@�C�����ݒ�
	String256 Path;
	if( pPath )
	{
		Path = pPath;
	}
	else
	{
		//	�ۑ��f�B���N�g���𐶐�
		::CreateDirectory( PATH_SNAPSHOT_DIR, NULL );

		//	�t�@�C���������ݎ��Ԃ���擾
		SYSTEMTIME t;
		::GetLocalTime(&t); 
		Path.Format( PATH_SNAPSHOT_FORMAT, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds );
	}

	//	BMP�t�@�C���̃w�b�_���쐬
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


	//	�t�@�C�����J��
	FileHandle fp( Path, "wb" );
	if( !fp ) return false;

	//	�o�b�N�o�b�t�@�T�[�t�F�C�X���擾
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
//	�u�����h���[�h��o�^
//*****************************************************************************
void DrawDevice::SetBlend( BLEND_MODE Blend )
{
	if( m_CurBlend == Blend ) return;

	//	�f�o�C�X�ɐݒ�
	switch( Blend )
	{
	case BLEND_NONE:	//�s����
		m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
		m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		break;

	case BLEND_ALPHA:	//������
		m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		break;

	case BLEND_ADD:		//���Z
		m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		break;

	case BLEND_SUB:		//���Z(�^��)
		m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );
		m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
		break;

	case BLEND_MUL:		//��Z
		m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );
		m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
		break;

	case BLEND_REV:		//���]
		m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_INVDESTCOLOR );
		m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
		break;
	}

	m_CurBlend = Blend;
}
	
//*****************************************************************************
//	�e�N�X�`����o�^
//*****************************************************************************
void DrawDevice::SetTexture( const Texture* pTex )
{
	if( m_pCurTexture == pTex ) return;

	//	�f�o�C�X�ɐݒ�
	if( pTex )	m_pDevice->SetTexture( 0, pTex->GetTex() );
	else		m_pDevice->SetTexture( 0, NULL );

	m_pCurTexture = pTex;
}

//-----------------------------------------------------------------------------
//	�f�o�C�X�̃p�����[�^��������
//-----------------------------------------------------------------------------
bool DrawDevice::_InitDevParam( void )
{
	Log( "�@�@�@���X�N���[���t�H�[�}�b�g�擾" );

	//	�E�B���h�E���[�h�̃t�H�[�}�b�g�擾
	if( !_GetWindowFormat( m_WindowFormat ) )
	{
		Log( "�@�@�@�~�E�B���h�E���[�h�E�t�H�[�}�b�g�擾���s" );
		return false;
	}

	Log( "�@�@�@���E�B���h�E���[�h�E�t�H�[�}�b�g�擾[Back = %d /Depth = %d / Tex = %d / %dHz]",
			m_WindowFormat.BackFormat, m_WindowFormat.DepthFormat, m_WindowFormat.TexFormat, m_WindowFormat.RefreshRate );

	//	�t���X�N���[�����[�h�̃t�H�[�}�b�g�擾
	bool bFullColor = _IsFullColor( m_WindowFormat.BackFormat ); 
	if( !_GetFullFormat( m_FullFormat, m_pWindow->GetSize(), bFullColor) )
	{
		Log( "�@�@�@�~�t���X�N���[�����[�h�E�t�H�[�}�b�g�擾���s" );
		return false;
	}

	Log( "�@�@�@���t���X�N���[�����[�h�E�t�H�[�}�b�g�擾[Back = %d /Depth = %d / Tex = %d / %dHz]",
			m_FullFormat.BackFormat, m_FullFormat.DepthFormat, m_FullFormat.TexFormat, m_FullFormat.RefreshRate );

	//	�t���X�N���[�����\���H
	m_bModeChange = m_FullFormat.BackFormat != D3DFMT_UNKNOWN;
	if( !m_bModeChange )
	{
		//	�؂�ւ��s�\�F�����E�B���h�E���[�h
		m_pWindow->SetWindowed();
		
		Log( "�@�@�@���t���X�N���[���؂�ւ��s�\�E�����E�B���h�E���[�h" );
	}

	Log( "�@�@�@���X�N���[���t�H�[�}�b�g�擾����" );
	Log( "�@�@�@----------------------------------------" );

	return true;
}

//-----------------------------------------------------------------------------
//	�f�o�C�X��������
//-----------------------------------------------------------------------------
bool DrawDevice::_InitDevice( void )
{
	Log( "�@�@�@���`��f�o�C�X������" );

	HRESULT	hr;

	D3DCAPS9 DevCaps;
	hr = m_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DevCaps );
	if( FAILED(hr) )
	{
		Log( "�@�@�@�~�f�o�C�X���\�擾���s" );
		return false;
	}

	//	�n�[�h�E�F�ATnL���\�����ׂ�
	uint Behavior = ( DevCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )?	D3DCREATE_HARDWARE_VERTEXPROCESSING :
																			D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	
	Log( "�@�@�@��TnL����[%s]", (DevCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)? "�n�[�h�E�F�A" : "�\�t�g�E�F�A" );

	//	FPU�ݒ�
	Behavior |= D3DCREATE_FPU_PRESERVE;

	//	�v���[���g�p�����[�^���擾
	D3DPRESENT_PARAMETERS PresentParam;
	_GetDefaultPresentParam( PresentParam );

	//	�f�o�C�X���쐬����
	hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_pWindow->GetHwnd(), Behavior, &PresentParam, &m_pDevice );
	if( FAILED(hr) )
	{
		Log( "�@�@�@�~�f�o�C�X�쐬���s" );
		return false;
	}

	//	�����_�[�X�e�[�g������
	_ResetRenderState();

	//	�N���A
	m_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_ScreenColor, 1.0f, 0 );

	Log( "�@�@�@���`��f�o�C�X����������" );
	Log( "�@�@�@----------------------------------------" );

	return true;
}

//-----------------------------------------------------------------------------
//	�E�B���h�E�X�N���[���t�H�[�}�b�g�����o
//-----------------------------------------------------------------------------
bool DrawDevice::_GetWindowFormat( DISP_FORMAT& DispFormat )
{
	HRESULT hr;

	//	���݂̃f�B�X�v���C���[�h�擾
	D3DDISPLAYMODE	DispMode;
	hr = m_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &DispMode );
	if( FAILED(hr) ) return false;

	DispFormat.BackFormat	= DispMode.Format;
	DispFormat.RefreshRate	= DispMode.RefreshRate;

	//	�[�x�o�b�t�@�擾
	DispFormat.DepthFormat	= _GetDepthFormat( DispFormat.BackFormat );

	//	�e�N�X�`���t�H�[�}�b�g�擾
	DispFormat.TexFormat	= _GetTexFormat( DispFormat.BackFormat );

	return true;
}

//-----------------------------------------------------------------------------
//	�t���X�N���[���t�H�[�}�b�g�����o
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
		//	�A�_�v�^���[�h�̐����擾
		int ModeNum = m_pD3D->GetAdapterModeCount( D3DADAPTER_DEFAULT, aFormat[i] );

		for( int Mode = 0; Mode < ModeNum; Mode++ )
		{
			//	�f�B�X�v���C���[�h���擾����
			D3DDISPLAYMODE	DispMode;
			HRESULT hr = m_pD3D->EnumAdapterModes( D3DADAPTER_DEFAULT, aFormat[i], Mode, &DispMode );
			if( FAILED(hr) ) return false;

			//	�T�C�Y�̒���
			if( DispMode.Width != (uint)Size.x || DispMode.Height != (uint)Size.y )
			{
				continue;
			}

			DispFormat.BackFormat	= DispMode.Format;
			DispFormat.RefreshRate	= DispMode.RefreshRate;

			//	���t���b�V�����[�g���U�O�Ȃ炻�̂܂ܔ�����
			if( DispMode.RefreshRate == FPS )
			{
				break;
			}
		}
	}
	
	//	�[�x�o�b�t�@�擾
	DispFormat.DepthFormat	= _GetDepthFormat( DispFormat.BackFormat );

	//	�e�N�X�`���t�H�[�}�b�g�擾
	DispFormat.TexFormat	= _GetTexFormat( DispFormat.BackFormat );

	return true;
}

//-----------------------------------------------------------------------------
//	�[�x�o�b�t�@�t�H�[�}�b�g�擾
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

	//	������Ȃ�������Ƃ肠����16�r�b�g
	return D3DFMT_D16;
}

//-----------------------------------------------------------------------------
//	�e�N�X�`���t�H�[�}�b�g���擾	
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

	//	������Ȃ�������Ƃ肠����A�t��32�r�b�g
	return D3DFMT_A8R8G8B8;
}


//-----------------------------------------------------------------------------
//	�f�t�H���g�̃v���[���g�p�����[�^���擾
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
//	�����_�[�X�e�[�g�����Z�b�g
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

	//	���_�t�H�[�}�b�g�ݒ�
	m_pDevice->SetFVF( DEFAULT_FVF ); 

	m_CurBlend = BLEND_NONE;
}

//-----------------------------------------------------------------------------
//	���X�g�����f�o�C�X�𕜋A����
//-----------------------------------------------------------------------------
bool DrawDevice::_RestoreDevice( void )
{
	//	�����\�ɂȂ�܂ŌJ��Ԃ�
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
//	���݂̕`��t�H�[�}�b�g�𓾂�
//-----------------------------------------------------------------------------
const DrawDevice::DISP_FORMAT& DrawDevice::_GetCurFormat( void ) const
{
	return m_pWindow->IsWindowed()? m_WindowFormat : m_FullFormat;
}

//-----------------------------------------------------------------------------
//	�t�H�[�}�b�g���t���J���[���ǂ���
//-----------------------------------------------------------------------------
bool DrawDevice::_IsFullColor( D3DFORMAT Format )
{
	return (Format == D3DFMT_X8R8G8B8 || Format == D3DFMT_A8R8G8B8 || Format == D3DFMT_R8G8B8 );
}



}