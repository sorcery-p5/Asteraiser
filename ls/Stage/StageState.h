#pragma once

#include "StageTypes.h"

class Stage;
class StageData;


////////////////////////////////////////////////////////////////////////////////
//
//	ステージステート
//
////////////////////////////////////////////////////////////////////////////////
class StageState
{
public:
	StageState( Stage* pStage );
	~StageState();

	void				Init( const StageData* pData, Label State );
	void				Update( void );

	void				AddNotify( Label Notify );

private:
	typedef std::vector< bool >			VecBool;
	typedef std::vector<Label>			NotifyVec;

private:
	void				_ChangeState( Label Name );
	void				_LoopFrame( int Frame );
	void				_ExecInitAction( const STAGE_STATE* pData );

	bool				_IsExecAction( const STAGE_ACTION& Action )				const;
	bool				_CheckCondition( const STAGE_CONDITION& Condition )		const;
	void				_ExecAction( const STAGE_ACTION& Action );

	bool				_IsNotify( Label Notify )								const;

private:
	Stage*				m_pStage;
	const StageData*	m_pData;

	const STAGE_STATE*	m_pCurState;
	Label				m_NextStateName;
	int					m_FrameCount;
	VecBool				m_vExecAction;
	NotifyVec			m_vNotify;
};