#pragma once

#include "Resource.h"


////////////////////////////////////////////////////////////////////////////////
//
//	テクスチャリソース
//
////////////////////////////////////////////////////////////////////////////////
class TextureResource : public Resource
{
public:
	static const RES_TYPE	TYPE = RES_TEX;

public:
	TextureResource( void );
	virtual ~TextureResource();

	virtual bool			Load( pstr pPath );

	virtual void			Reset( void );
	virtual void			Restore( void );

	const Texture*			GetData( void )		const	{ return m_pTexture; }

private:
	Texture*				m_pTexture;
};

typedef std::map< Label, TextureResource* >			TexResMap;