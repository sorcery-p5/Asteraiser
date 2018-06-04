#pragma once

#include "ResourceManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	リソース関連ユーティリティ
//
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	リソースマップの追加
//******************************************************************************
template< typename T >
Label AddResourceMap( std::map<Label,T*>& Map, pstr pPath, ResourceManager* pResMng )
{
	Label Name( GetFileTitle<256>(pPath) );

	//	既に存在するか
	if( Map.find(Name) == Map.end() )
	{
		//	新規作成
		T* pRes = pResMng->LoadResource<T>( pPath );

		if( pRes )
		{
			Map.insert( std::map<Label,T*>::value_type( pRes->GetName(), pRes ) );
		}
	}

	return Name;
}

//******************************************************************************
//	リソースマップの検索
//******************************************************************************
template< typename T, typename RES >
const T* FindResourceMap( const std::map<Label, RES* >& Map, Label Name )
{
	std::map<Label, RES* >::const_iterator it = Map.find(Name);
	
	if( it != Map.end() )
	{
		return (it->second)->GetData();
	}

	return NULL;
}
