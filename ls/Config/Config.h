#pragma once

#include "ConfigTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	コンフィグ
//
////////////////////////////////////////////////////////////////////////////////
class Config
{
public:
	Config();
	~Config();

	bool					Load( pstr pPath );
	bool					Save( void )											const;
	void					Reset( void );
	void					Apply( void );

	const CONFIG_SYSTEM&	GetSystemInfo( void )									const	{ return m_System; }
	int						GetInputInfoNum( void )									const	{ return (int)m_vInput.size(); }
	const CONFIG_INPUT&		GetInputInfo( int PlayerID )							const;
	const CONFIG_DEBUG&		GetDebugInfo( void )									const	{ return m_Debug; }
	
	void					SetSystemInfo( const CONFIG_SYSTEM& Info )						{ m_System = Info; }
	void					SetInputInfo( int PlayerID, const CONFIG_INPUT& Info );
	void					SetDebugInfo( const CONFIG_DEBUG& Info )						{ m_Debug = Info; }

	void					ReplaceKeyName( String256& Str )						const;

	static byte				StrToKeycode( pstr pStr );
	static std::string		KeycodeToStr( byte Code );
	static pstr				InputTypeToStr( CONTROLLER_INPUT Type );
	static pstr				SystemKeyTypeToStr( CONTROLLER_SYSTEM_KEY Type );
	static pstr				PadAxisToStr( PAD_AXIS Axis );

private:
	typedef std::vector<CONFIG_INPUT>		InputVec;

private:
	void					_LoadSystem( const XML::Node* pNode );
	void					_LoadInput( const XML::Node* pNode );
	void					_LoadDebug( const XML::Node* pNode );
	void					_LoadKeyboard( CONFIG_INPUT& Input, const XML::Node* pNode );
	void					_LoadPad( CONFIG_INPUT& Input, const XML::Node* pNode );
	CONCURRENT				_LoadConcurrent( byte b, const XML::Node* pNode );

	void					_SaveSystem( XML::Node* pNode )									const;
	void					_SaveInput( XML::Node* pNode )									const;
	void					_SaveDebug( XML::Node* pNode )									const;
	void					_SaveKeyboard( XML::Node* pNode, const CONFIG_INPUT& Input )	const;
	void					_SavePad( XML::Node* pNode, const CONFIG_INPUT& Input )			const;
	
	void					_GetKeyNameStr( String256& Out, int Type, int ID )				const;

private:
	std::string				m_Path;
	CONFIG_SYSTEM			m_System;
	InputVec				m_vInput;
	CONFIG_DEBUG			m_Debug;
	bool					m_bLoadDebug;
};


//------------------------------------------------------------------------------
//	インスタンス管理
//------------------------------------------------------------------------------
void				InitConfig( void );
void				ExitConfig( void );
Config*				GetConfig( void );