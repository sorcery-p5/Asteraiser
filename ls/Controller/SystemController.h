#pragma once

#include "ControllerTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	システム用コントローラ
//
////////////////////////////////////////////////////////////////////////////////
class SystemController
{
public:
	SystemController( void );
	~SystemController();

	bool				IsInput( CONTROLLER_SYSTEM_KEY Input, INPUT_TYPE Type )		const;
	bool				IsAnyInput( INPUT_TYPE Type )								const;
	bool				IsDirInput( CONTROLLER_INPUT Input, INPUT_TYPE Type )		const;
	Point				GetDir( INPUT_TYPE Type )									const;
	byte				GetDirBit( INPUT_TYPE Type )								const;
};