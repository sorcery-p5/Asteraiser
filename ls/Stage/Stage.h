#pragma once

#include "StageTypes.h"

class StageManager;
class StageData;
class StageState;
class StageTelop;
class StageAction;
class World;


////////////////////////////////////////////////////////////////////////////////
//
//	ステージ
//
////////////////////////////////////////////////////////////////////////////////
class Stage
{
public:
	Stage( StageManager* pManager );
	~Stage();

	void				Init( const StageData* pData, Label State );
	void				Update( void );
	void				Draw( void );

	void				AddAction( const STAGE_ACTION* pActionParam );
	void				AddTelop( Label Name, const Matrix3& Mat );
	void				AddNotify( Label Notify );

	void				PlayBGM( Label Name, float Volume );
	void				FadeBGM( int Frame );
	void				StopBGM( void );

	void				End( void )							{ m_bEnd = true; }

	bool				IsEnd( void )				const	{ return m_bEnd; }
	int					GetFrameCount( void )		const	{ return m_FrameCount; }
	bool				IsTelopEnd( Label Name )	const;
	StageTelop*			GetTelop( Label Name )		const;
	World*				GetWorld( void );

private:
	typedef std::list<StageAction*>	ActionList;
	typedef std::list<StageTelop*>	TelopList;
	typedef std::vector<Label>		NotifyVec;

private:
	StageAction*		_CreateAction( const STAGE_ACTION* pActionParam );

private:
	StageManager*		m_pManager;
	const StageData*	m_pData;

	int					m_FrameCount;
	bool				m_bEnd;
	StageState*			m_pState;

	ActionList			m_ActionList;
	TelopList			m_TelopList;
	NotifyVec			m_vNotify;
};