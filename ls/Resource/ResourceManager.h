#pragma once

#include "ResourceTypes.h"
#include "Resource.h"


////////////////////////////////////////////////////////////////////////////////
//
//	���\�[�X�Ǘ�
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
	
	//	�f�o�C�X�̃��Z�b�g�ʒm
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
//	���\�[�X�̃��[�h���s�Ȃ�(�Q�ƃJ�E���^�ǉ�)
//******************************************************************************
template<typename T>
T* ResourceManager::LoadResource( pstr pPath )
{
	Label Name( GetFileTitle<256>(pPath) );

	//	���Ƀ��[�h�ς݂��ǂ���
	Resource* pRes = _GetResource<T>( T::TYPE, Name );
	if( !pRes )
	{
		//	�V�K�Ƀ��[�h
		pRes = _CreateResource<T>( pPath );
	
		if( !pRes ) return NULL;

		//	�o�^
		pRes->SetName( Name );
		pRes->SetType( T::TYPE );
		m_aResMap[T::TYPE].insert( ResMap::value_type(Name,pRes) );
	}

	pRes->AddRef();

	return (T*)pRes;
}

//------------------------------------------------------------------------------
//	���[�h�ς݂̃��\�[�X���擾����(�Q�ƃJ�E���^�͑����Ȃ�)
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
//	���\�[�X�N���X�̐���
//------------------------------------------------------------------------------
template<typename T>
T* ResourceManager::_CreateResource( pstr pPath )
{
	T* pRes = new T;

	//	�ǂݍ���
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