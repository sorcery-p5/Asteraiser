#pragma once

#include "StageManager.h"

class World;
class Stage;
class StageData;


////////////////////////////////////////////////////////////////////////////////
//
//	ステージ管理
//
////////////////////////////////////////////////////////////////////////////////
class StageManager
{
public:
	StageManager( World* pWorld );
	~StageManager();

	void			Update( void );
	void			Draw( void );

	void			Start( int InitStage, Label InitState );
	void			PushStageData( const StageData* pData );

	bool			IsEndAllStage( void )		const	{ return m_bEndAll; }
	World*			GetWorld( void )					{ return m_pWorld; }
	Stage*			GetCurStage( void )			const	{ return m_pCurStage; }

private:
	typedef std::vector<const StageData*>	VecStageData;

private:
	void			_ChangeNextStage( void ); 

private:
	World*			m_pWorld;
	VecStageData	m_vStageData;
	int				m_StageIndex;
	Stage*			m_pCurStage;
	bool			m_bEndAll;
};