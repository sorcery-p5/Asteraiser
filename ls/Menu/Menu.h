#pragma once

#include "MenuTypes.h"

class MenuData;
class MenuLayer;
class MenuObject;
class MenuListener;
class Effect;


////////////////////////////////////////////////////////////////////////////////
//
//	ÉÅÉjÉÖÅ[
//
////////////////////////////////////////////////////////////////////////////////
class Menu
{
public:
	Menu( MenuListener* pListener );
	~Menu();

	void				Init( const MenuData* pData, Matrix3 Mat );
	void				Update( void );
	void				Draw( const Matrix3& OffsetMat );

	MenuLayer*			GetLayer( Label Name )									const;
	MenuLayer*			GetCurLayer( void )										const;
	bool				IsEmpty( void )											const	{ return GetCurLayer() == NULL && m_NextLayer.IsEmpty(); }
	void				OpenLayer( Label Name );
	void				CloseLayer( Label Name );
	void				CloseCurLayer( void );

	const MenuData*		GetData( void )											const	{ return m_pData; }
	const Matrix3&		GetMatrix( void )										const	{ return m_Matrix; }

	bool				IsVisible( void )										const	{ return m_bVisible; }
	void				SetVisible( bool bVisible );

	MenuListener*		GetListener( void )										const	{ return m_pListener; }
	void				SetListener( MenuListener* pListener )							{ m_pListener = pListener; }
	void				OnNotify( Label Name, float Param );
	void				OnNotify( MenuLayer* pLayer, Label Name, float Param );
	void				OnNotify( MenuObject* pObject, Label Name, float Param );
	void				OnChangeValue( MenuObject* pObject );
	void				OnSelectObject( MenuObject* pObject, MenuObject* pPrevObject );

	Sound*				PlaySound( Label Name, float Vol );
	Effect*				CreateEffect( Label Name, const Matrix3& Mat );

private:
	typedef std::vector< MenuLayer* >		MenuLayerVec;
	typedef std::vector< MenuListener* >	ListenerVec;

private:
	void				_PushLayer( Label Name );
	void				_OpenInactiveLayer( Label Name );
	void				_UpdateTimeEvent( void );
	void				_ExecActionList( const MENU_ACTION_VEC& vAction );
	void				_ExecAction( const MENU_ACTION& Action );

private:
	const MenuData*		m_pData;
	Matrix3				m_Matrix;
	int					m_FrameCount;
	bool				m_bVisible;
	
	MenuLayerVec		m_vLayer;
	MenuLayerVec		m_vInactiveLayer;
	MenuListener*		m_pListener;
	Label				m_NextLayer;
};