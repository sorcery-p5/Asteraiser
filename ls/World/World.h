#pragma once

#include "WorldTypes.h"

class ControllerManager;
class Controller;
class StageManager;
class ReplayManager;
class PlayerManager;
class EnemyManager;
class AttackManager;
class GroundManager;
class ItemManager;
class IndicateManager;
class ScoreManager;

class Menu;
class WorldData;
class WorldPause;
class WorldContinue;
class WorldResult;
class WorldReplayMenu;
class FadeScreen;


///////////////////////////////////////////////////////////////////////////////
//
//	ÉèÅ[ÉãÉh
//
///////////////////////////////////////////////////////////////////////////////
class World
{
public:
	static const Label	EXITCODE_EXIT;
	static const Label	EXITCODE_CLEAR;
	static const Label	EXITCODE_DEAD;

	enum MODE
	{
		MODE_ALL,
		MODE_TUTORIAL,
		MODE_STAGE_SELECT,
		MODE_REPLAY,
	};

	struct INIT_PARAM 
	{
		INIT_PARAM()
		{
			Mode		= MODE_ALL;
			Stage		= 0;
			bPauseExit	= false;
			bDemoMenu	= false;
		}
	
		MODE				Mode;
		int					Stage;
		Label				State;
		std::string			ReplayPath;
		bool				bPauseExit;
		bool				bDemoMenu;
	};

public:
	World( void );
	~World();

	void					Init( const WorldData* pData, const INIT_PARAM& InitParam );
	void					Update( void );
	void					Draw( void );

	void					Exit( Label ExitCode );
	bool					IsExit( void )								const;

	void					OnPlayerDead( void );
	void					OnContinue( void );
	void					OnChangeStage( int StageIndex );
	void					OpenResult( void );
	void					OpenReplayMenu( bool bCancelEnable );
	void					ChangeReplayManual( void );

	void					SetUpdateEnable( bool bEnable )						{ m_bUpdateEnable	= bEnable; }
	void					SetDrawEnable( bool bEnable )						{ m_bDrawEnable		= bEnable; }

	const Controller*		GetController( void )						const;
	ReplayManager*			GetReplayManager( void )					const	{ return m_pReplayManager; }
	StageManager*			GetStageManager( void )						const	{ return m_pStageManager; }
	PlayerManager*			GetPlayerManager( void )					const	{ return m_pPlayerManager; }
	EnemyManager*			GetEnemyManager( void )						const	{ return m_pEnemyManager; }
	AttackManager*			GetAttackManager( void )					const	{ return m_pAttackManager; }
	GroundManager*			GetGroundManager( void )					const	{ return m_pGroundManager; }
	IndicateManager*		GetIndicateManager( void )					const	{ return m_pIndicateManager; }
	ItemManager*			GetItemManager( void )						const	{ return m_pItemManager; }
	ScoreManager*			GetScoreManager( void )						const	{ return m_pScoreManager; }

	const WorldData*		GetData( void )								const	{ return m_pData; }
	const Matrix3&			GetDrawOffset( void )						const	{ return Matrix3::Identity(); }
	Rect					GetFieldRect( void )						const	{ return m_FieldRect; }
	Point					GetFieldOffset( void )						const	{ return Point( m_FieldRect.CenterX(), m_FieldRect.t ); }
	Label					GetExitCode( void )							const	{ return m_ExitCode; }
	bool					IsFade( void )								const;
	bool					IsReplayMode( void )						const	{ return m_Mode == MODE_REPLAY; }
	bool					IsOneStageMode( void )						const	{ return m_Mode == MODE_STAGE_SELECT; }

private:
	void					_ResetRand( void );

private:
	const WorldData*		m_pData;

	ControllerManager*		m_pControllerMng;
	ReplayManager*			m_pReplayManager;
	StageManager*			m_pStageManager;
	PlayerManager*			m_pPlayerManager;
	EnemyManager*			m_pEnemyManager;
	AttackManager*			m_pAttackManager;
	GroundManager*			m_pGroundManager;
	IndicateManager*		m_pIndicateManager;
	ItemManager*			m_pItemManager;
	ScoreManager*			m_pScoreManager;

	WorldPause*				m_pPause;
	WorldContinue*			m_pContinue;
	WorldResult*			m_pResult;
	WorldReplayMenu*		m_pReplayMenu;
	Menu*					m_pDemoMenu;
	FadeScreen*				m_pFade;

	MODE					m_Mode;
	uint					m_FrameCount;
	Rect					m_FieldRect;
	bool					m_bExit;
	Label					m_ExitCode;

	bool					m_bUpdateEnable;
	bool					m_bDrawEnable;
};