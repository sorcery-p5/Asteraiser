#pragma once


namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	描画デバイス
//
///////////////////////////////////////////////////////////////////////////////
class DrawDevice
{
public:
	DrawDevice( void );
	~DrawDevice();

	bool				Init( Window* pWindow );
	bool				Begin( void );
	bool				End( void );
	
	void				Reset( void );
	bool				SaveScreenShot( pstr pPath = NULL );

	void				SetScreenColor( Color Col )					{ m_ScreenColor = Col; }
	void				SetTransparentColor( Color Col )			{ m_Transparent = Col; }
	void				SetBlend( BLEND_MODE Blend );
	void				SetTexture( const Texture* pTex );

	LPDIRECT3DDEVICE9	GetDevice( void )							{ return m_pDevice; }
	bool				IsScreenChange( void )				const	{ return m_bModeChange; }
	Color				GetTransparentColor( void )			const	{ return m_Transparent; }
	D3DFORMAT			GetBackFormat( void )				const	{ return _GetCurFormat().BackFormat; }
	D3DFORMAT			GetTexFormat( void )				const	{ return _GetCurFormat().TexFormat; }
	bool				IsTimerWait( void )					const	{ return _GetCurFormat().RefreshRate != FPS; }

private:
	struct DISP_FORMAT
	{
		uint			RefreshRate;
		D3DFORMAT		BackFormat;
		D3DFORMAT		DepthFormat;
		D3DFORMAT		TexFormat;
	};

private:
	bool				_InitDevParam( void );
	bool				_InitDevice( void );
	bool				_GetWindowFormat( DISP_FORMAT& DispFormat );
	bool				_GetFullFormat( DISP_FORMAT& DispFormat, const Point& Size, bool bFullColor );
	D3DFORMAT			_GetDepthFormat( D3DFORMAT BackFormat );
	D3DFORMAT			_GetTexFormat( D3DFORMAT BackFormat );
	void				_GetDefaultPresentParam( D3DPRESENT_PARAMETERS& Param );
	void				_ResetRenderState( void );
	bool				_RestoreDevice( void );

	const DISP_FORMAT&	_GetCurFormat( void )			const;

	static bool			_IsFullColor( D3DFORMAT Format );

private:
	Window*				m_pWindow;
	LPDIRECT3D9			m_pD3D;
	LPDIRECT3DDEVICE9	m_pDevice;

	DISP_FORMAT			m_FullFormat;
	DISP_FORMAT			m_WindowFormat;
	bool				m_bModeChange;

	Color				m_ScreenColor;
	Color				m_Transparent;
	BLEND_MODE			m_CurBlend;
	const Texture*		m_pCurTexture;
};

}