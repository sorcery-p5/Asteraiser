#pragma once

#include "ControllerTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	ÉRÉìÉgÉçÅ[ÉâäÓíÍ
//
////////////////////////////////////////////////////////////////////////////////
class Controller
{
public:
	Controller( void );
	virtual ~Controller();

	virtual void			Update( uint FrameCount ) = 0;

	void					CopyInfo( const Controller& Ctrl );

	bool					IsInput( CONTROLLER_INPUT Input, INPUT_TYPE Type )		const;
	Point					GetDir( void )											const;
	bool					IsNeutral( void )										const;
	bool					IsHNeutral( void )										const;
	bool					IsVNeutral( void )										const;

	static Point			InfoToDir( byte Info );

protected:
	void					_UpdateCurInfo( byte Info );

private:
	byte					m_CurInfo;
	byte					m_PreInfo;
};