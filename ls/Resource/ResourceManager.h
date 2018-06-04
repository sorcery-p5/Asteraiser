#pragma once

#include "ResourceTypes.h"
#include "Resource.h"


////////////////////////////////////////////////////////////////////////////////
//
//	リソース管理
//
////////////////////////////////////////////////////////////////////////////////
class ResourceManager : public DeviceResetListener
{
friend Resource;

public:
	ResourceManager( void );
	~ResourceManager();

	void				Update( void );

	template<typename T>
	T*					LoadResource( pstr pPath );
	
	//	デバイスのリセット通知
	virtual void		OnDrawDeviceReset( void );
	virtual void		OnDrawDeviceRestore( void );

private:
	void				DeleteResource( Resource* pRes );

private:
	typedef std::map< Label, Resource* >	ResMap;

private:
	template<typename T>
	T*					_GetResource( RES_TYPE Type, Label Name );

	template<typename T>
	T*					_CreateResource( pstr pPath );

private:
	ResMap				m_aResMap[ RES_TYPE_NUM ];
};


//******************************************************************************
//	リソースのロードを行なう(参照カウンタ追加)
//******************************************************************************
template<typename T>
T* ResourceManager::LoadResource( pstr pPath )
{
	Label Name( GetFileTitle<256>(pPath) );

	//	既にロード済みかどうか
	Resource* pRes = _GetResource<T>( T::TYPE, Name );
	if( !pRes )
	{
		//	新規にロード
		pRes = _CreateResource<T>( pPath );
	
		if( !pRes ) return NULL;

		//	登録
		pRes->SetName( Name );
		pRes->SetType( T::TYPE );
		m_aResMap[T::TYPE].insert( ResMap::value_type(Name,pRes) );
	}

	pRes->AddRef();

	return (T*)pRes;
}

//------------------------------------------------------------------------------
//	ロード済みのリソースを取得する(参照カウンタは増えない)
//------------------------------------------------------------------------------
template<typename T>
T* ResourceManager::_GetResource( RES_TYPE Type, Label Name )
{
	ResMap::iterator it = m_aResMap[Type].find( Name );
	if( it == m_aResMap[Type].end() )
	{
		return NULL;
	}
	return (T*)it->second;
}

//------------------------------------------------------------------------------
//	リソースクラスの生成
//------------------------------------------------------------------------------
template<typename T>
T* ResourceManager::_CreateResource( pstr pPath )
{
	T* pRes = new T;

	//	読み込み
	if( pRes )
	{
		if( !pRes->Load( pPath ) )
		{
			SafeDelete( pRes );
			return NULL;
		}
	}
	return pRes;
}