#pragma once

#include "StageTypes.h"

class Stage;


////////////////////////////////////////////////////////////////////////////////
//
//	ステージアクション
//
////////////////////////////////////////////////////////////////////////////////
class StageAction
{
public:
	StageAction( Stage* pParent );
	~StageAction( void );

	void					Init( const STAGE_ACTION* pParam );
	void					Update( void );

	void					Delete( void )								{ m_bDelete = true; }

	bool					IsDelete( void )					const	{ return m_bDelete; }

private:
	Stage*					m_pParent;
	const STAGE_ACTION*		m_pParam;

private:
	bool					m_bDelete;
	int						m_FrameCount;
};