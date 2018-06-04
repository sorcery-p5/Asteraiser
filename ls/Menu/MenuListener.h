#pragma once

class Menu;
class MenuLayer;
class MenuObject;


////////////////////////////////////////////////////////////////////////////////
//
//	メニュー：リスナー
//
////////////////////////////////////////////////////////////////////////////////
class MenuListener
{
public:
	MenuListener( void )																{}
	virtual ~MenuListener()																{}

	virtual void		OnOpenLayer( MenuLayer* pLayer )								{}
	virtual void		OnCloseLayer( MenuLayer* pLayer )								{}
	virtual void		OnMenuNotify( Menu* pMenu, Label Name, float Param )			{}
	virtual void		OnLayerNotify( MenuLayer* pLayer, Label Name, float Param )		{}
	virtual void		OnObjectNotify( MenuObject* pObject, Label Name, float Param )	{}
	virtual void		OnObjectChangeValue( MenuObject* pObject )						{}
	virtual void		OnObjectSelect( MenuObject* pObject, MenuObject* pPrevObject )	{}
};