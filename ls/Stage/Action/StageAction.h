#pragma once

#include "../StageTypes.h"

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
	virtual ~StageAction( void );

	void					Init( const STAGE_ACTION* pParam );
	void					Update( void );

	void					Delete( void )								{ m_bDelete = true; }

	bool					IsDelete( void )					const	{ return m_bDelete; }

protected:
	virtual void			_OnInit( void )								{}
	virtual void			_OnUpdate( void )							{}

	int						_GetFrameCount( void )				const	{ return m_FrameCount; }

protected:
	Stage*					m_pParent;
	const STAGE_ACTION*		m_pParam;

private:
	bool					m_bDelete;
	int						m_FrameCount;
};