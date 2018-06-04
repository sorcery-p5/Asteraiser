#pragma once


namespace P5
{

class DrawDevice;


///////////////////////////////////////////////////////////////////////////////
//
//	テクスチャ
//
///////////////////////////////////////////////////////////////////////////////
class Texture
{
public:
	bool 				Load( pstr Path );

	void 				Reset( void )				{ m_pTex = NULL; }
	bool 				Restore( void )				{ return _Load(); }

	const Point&		GetSize( void )		const	{ return m_Size; }
	LPDIRECT3DTEXTURE9	GetTex( void )		const	{ return m_pTex; }
	
private:
	typedef ReleasePtr<IDirect3DTexture9>	TexPtr;

private:
	bool				_Load( void );

private:
	TexPtr				m_pTex;
	Point				m_Size;

	String256			m_Path;
	Color				m_Transparent;
};


}