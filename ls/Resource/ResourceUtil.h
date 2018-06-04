#pragma once

#include "ResourceManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	���\�[�X�֘A���[�e�B���e�B
//
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	���\�[�X�}�b�v�̒ǉ�
//******************************************************************************
template< typename T >
Label AddResourceMap( std::map<Label,T*>& Map, pstr pPath, ResourceManager* pResMng )
{
	Label Name( GetFileTitle<256>(pPath) );

	//	���ɑ��݂��邩
	if( Map.find(Name) == Map.end() )
	{
		//	�V�K�쐬
		T* pRes = pResMng->LoadResource<T>( pPath );

		if( pRes )
		{
			Map.insert( std::map<Label,T*>::value_type( pRes->GetName(), pRes ) );
		}
	}

	return Name;
}

//******************************************************************************
//	���\�[�X�}�b�v�̌���
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
