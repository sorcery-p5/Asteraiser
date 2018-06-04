#pragma once

#include "ItemTypes.h"
#include "Resource/Resource.h"

class EffectFile;
class EffectResource;
class SoundResource;


////////////////////////////////////////////////////////////////////////////////
//
//	アイテムデータ
//
////////////////////////////////////////////////////////////////////////////////
class ItemData
{
public:
	ItemData( void );
	~ItemData();

	bool				Load( pstr pPath );

	const ITEM_PARAM&	GetParam( void )					const	{ return m_Param; }
	const EffectFile*	GetEffectFile( Label Name )			const;
	const SoundFile*	GetSoundFile( void )				const;

private:
	typedef std::map< Label, EffectResource* >	MapEffectRes;

private:
	void				_LoadParam( const XML::Node* pNode );
	void				_LoadEffect( const XML::Node* pNode );

private:
	ITEM_PARAM			m_Param;
	MapEffectRes		m_mapEffectRes;
	SoundResource*		m_pSoundRes;
};

//------------------------------------------------------------------------------
//	アイテムリソース
//------------------------------------------------------------------------------
class ItemResource : public ResourceTemplate< ItemData, RES_ITEM >
{
};