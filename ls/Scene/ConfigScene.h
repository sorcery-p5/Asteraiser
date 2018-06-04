#pragma once

#include "Scene.h"
#include "Config/ConfigTypes.h"
#include "Menu/MenuListener.h"

class MenuResource;
class Menu;
class MenuLayer;
class MenuObject;


///////////////////////////////////////////////////////////////////////////////
//
//	コンフィグシーン
//
///////////////////////////////////////////////////////////////////////////////
class ConfigScene : public Scene, public MenuListener
{
public:
	ConfigScene( Label Name );
	virtual ~ConfigScene();

	virtual void				OnInit( void );
	virtual void				Update( void );

private:
	virtual void				OnOpenLayer( MenuLayer* pLayer );
	virtual void				OnCloseLayer( MenuLayer* pLayer );
	virtual void				OnObjectNotify( MenuObject* pObject, Label Name, float Param );
	virtual void				OnObjectChangeValue( MenuObject* pObject );

private:
	enum INPUT
	{
		INPUT_DIR,
		INPUT_SYSTEM,
		INPUT_BUTTON,
	};

private:
	void						_UpdateInputWait( void );
	void						_UpdateDefaultWait( void );
	void						_UpdatePadEnable( void );
	void						_SolveKeyDuplication( byte OldValue, byte NewValue, INPUT InputType );
	void						_SolvePadDuplication( byte OldValue, byte NewValue, INPUT InputType );
	void						_SolveAxisDuplication( PAD_AXIS OldValue, bool bOldRev, PAD_AXIS NewValue );

	void						_SetMainLayer( MenuLayer* pLayer );
	void						_SetKeyLayer( MenuLayer* pLayer );
	void						_SetPadLayer( MenuLayer* pLayer );

	std::string					_GetAxisString( PAD_AXIS Axis, bool bRev )	const;

private:
	MenuResource*				m_pMenuRes;
	Menu*						m_pMenu;

	bool						m_bDefaultWait;
	CONFIG_SYSTEM				m_SystemInfo;
	CONFIG_INPUT				m_InputInfo;

	Label						m_InputLayerName;
	Label						m_InputObjectName;
	bool						m_bChangeScreen;
};