#include "stdafx.h"
#include "World.h"
#include "WorldData.h"
#include "WorldPause.h"
#include "WorldContinue.h"
#include "WorldResult.h"
#include "WorldReplayMenu.h"

#include "App/App.h"
#include "Config/Config.h"

#include "Controller/ControllerManager.h"
#include "Replay/ReplayManager.h"
#include "Stage/StageManager.h"
#include "Player/PlayerManager.h"
#include "Enemy/EnemyManager.h"
#include "Attack/AttackManager.h"
#include "Ground/GroundManager.h"
#include "Indicate/IndicateManager.h"
#include "Item/ItemManager.h"
#include "Score/ScoreManager.h"

#include "SpriteFrame/SpriteFrameUtil.h"
#include "Collide/CollideSystem.h"
#include "BGM/BGMManager.h"
#include "Menu/Menu.h"
#include "Fade/FadeScreen.h"

namespace
{
	const int	FADE_TIME			= 25;	
	const int	FADE_TIME_LONG		= 60;	
	const uint	FADE_PHASE			= 10000;

	const Label INDICATE_TABLE[]	= 
	{
		"Wall",
		"Score",
		"Rate",
		"Heat",
		"Blade",
		"Overdrive",
		"Shield",
	};
}

const Label	World::EXITCODE_EXIT	= "Exit";
const Label World::EXITCODE_CLEAR	= "Clear";
const Label World::EXITCODE_DEAD	= "Dead";


///////////////////////////////////////////////////////////////////////////////
//
//	���[���h
//
///////////////////////////////////////////////////////////////////////////////
World::World( void )
{
	m_pData				= NULL;
	m_pControllerMng	= NULL;
	m_pReplayManager	= NULL;
	m_pStageManager		= NULL;
	m_pPlayerManager	= NULL;
	m_pEnemyManager		= NULL;
	m_pAttackManager	= NULL;
	m_pGroundManager	= NULL;
	m_pItemManager		= NULL;
	m_pIndicateManager	= NULL;
	m_pScoreManager		= NULL;

	m_pPause			= NULL;
	m_pContinue			= NULL;
	m_pResult			= NULL;
	m_pReplayMenu		= NULL;
	m_pDemoMenu			= NULL;
	m_pFade				= NULL;

	m_Mode				= MODE_ALL;
	m_FrameCount		= 0;
	m_bExit				= false;

	m_bUpdateEnable		= true;
	m_bDrawEnable		= true;
}
World::~World()
{
	//	BGM��~
	GetApp()->GetBGMMng()->Stop();

	SafeDelete( m_pFade );
	SafeDelete( m_pDemoMenu );
	SafeDelete( m_pPause );
	SafeDelete( m_pContinue );
	SafeDelete( m_pReplayMenu );
	SafeDelete( m_pResult );

	SafeDelete( m_pScoreManager );
	SafeDelete( m_pItemManager );
	SafeDelete( m_pGroundManager );
	SafeDelete( m_pAttackManager );
	SafeDelete( m_pEnemyManager );
	SafeDelete( m_pPlayerManager );
	SafeDelete( m_pStageManager );
	SafeDelete( m_pIndicateManager );
	SafeDelete( m_pReplayManager );
	SafeDelete( m_pControllerMng );
}

//*****************************************************************************
//	������
//*****************************************************************************
void World::Init( const WorldData* pData, const INIT_PARAM& InitParam )
{
	m_pData = pData;

	//	�p�����[�^
	m_Mode			= InitParam.Mode;
	m_FrameCount	= 0;
	m_FieldRect		= m_pData->GetParam().FieldRect;

	//	���v���C�Ǘ�
	m_pReplayManager = new ReplayManager( this );
	MEM_CHECK(m_pReplayManager);

	if( InitParam.Mode == MODE_REPLAY )
	{
		m_pReplayManager->InitLoad( InitParam.ReplayPath.c_str(), InitParam.Stage );
	}
	else
	{
		m_pReplayManager->InitRecord();
	}

	//	�R���g���[��
	m_pControllerMng = new ControllerManager( this );
	MEM_CHECK(m_pControllerMng);
	
	m_pControllerMng->Init( IsReplayMode() );

	//	�\���Ǘ�
	m_pIndicateManager = new IndicateManager( this );
	MEM_CHECK(m_pIndicateManager);

	//	�v���C���[�Ǘ�
	m_pPlayerManager = new PlayerManager( this );
	MEM_CHECK(m_pPlayerManager);

	m_pPlayerManager->AddPlayer( m_pData->GetPlayerData( m_pData->GetParam().PlayerName ), m_pData->GetParam().InitPos + GetFieldOffset() );

	//	�X�e�[�W�Ǘ�
	m_pStageManager = new StageManager( this );
	MEM_CHECK(m_pStageManager);

	//	�G�Ǘ�
	m_pEnemyManager	= new EnemyManager( this );
	MEM_CHECK(m_pEnemyManager);

	//	�U���Ǘ�
	m_pAttackManager = new AttackManager( this );
	MEM_CHECK(m_pAttackManager);

	//	�n�`�Ǘ�
	m_pGroundManager = new GroundManager( this );
	MEM_CHECK(m_pGroundManager);

	//	�A�C�e���Ǘ�
	m_pItemManager = new ItemManager( this, m_pData->GetItemData() );
	MEM_CHECK(m_pItemManager);

	//	�X�R�A�Ǘ�
	m_pScoreManager = new ScoreManager( this, m_pData->GetScoreData() );
	MEM_CHECK(m_pScoreManager);

	//	�|�[�Y�Ǘ�
	m_pPause		= new WorldPause( this );
	MEM_CHECK(m_pPause);
	m_pPause->Init( m_pData->GetMenuData( m_pData->GetParam().PauseMenuName ), m_pData->GetParam().bPauseExit || InitParam.bPauseExit );

	//	�R���e�B�j���[�Ǘ�
	m_pContinue		= new WorldContinue( this );
	MEM_CHECK(m_pContinue);
	m_pContinue->Init( m_pData->GetMenuData( m_pData->GetParam().ContinueMenuName ), m_pData->GetParam().bContinueEnable? 3 : 0 );

	//	���v���C���j���[�Ǘ�
	m_pReplayMenu		= new WorldReplayMenu( this );
	MEM_CHECK(m_pReplayMenu);
	m_pReplayMenu->Init( m_pData->GetMenuData( m_pData->GetParam().ReplayMenuName ) );

	//	�X�e�[�W���U���g�Ǘ�
	m_pResult		= new WorldResult( this );
	MEM_CHECK(m_pResult);
	m_pResult->Init( m_pData->GetMenuData( m_pData->GetParam().ResultMenuName ), 
						m_pData->GetSoundFile( m_pData->GetParam().ResultScoreSoundName ) );

	//	�f�����j���[
	if( InitParam.bDemoMenu )
	{
		m_pDemoMenu		= new Menu( NULL );
		m_pDemoMenu->Init( m_pData->GetMenuData( m_pData->GetParam().DemoMenuName ), Matrix3::Trans( GetFieldRect().Center() ) );
	}

	//	�t�F�[�h�X�N���[��
	m_pFade			= new FadeScreen( FADE_PHASE );
	MEM_CHECK(m_pFade);
	m_pFade->FadeIn( FADE_TIME, Color::Black() );

	//	�X�e�[�W�o�^
	for( int i = 0; i < m_pData->GetStageNum(); i++ )
	{
		m_pStageManager->PushStageData( m_pData->GetStageData( i ) );
	}
	m_pStageManager->Start( InitParam.Stage, InitParam.State );
	
	//	�C���W�P�[�g
	for( int i = 0; i < ArrayNum(INDICATE_TABLE); i++ )
	{
		m_pIndicateManager->AddIndicate( INDICATE_TABLE[i], m_pData->GetIndicateData( INDICATE_TABLE[i] ) );
	}

	//	�R���t�B�O�̔��f
	SpriteFrameUtil::SetDrawDummyBone( GetConfig()->GetDebugInfo().bDrawDummyBone );
	GetCollideSystem()->SetDrawShapeEnable( GetConfig()->GetDebugInfo().bDrawCollideShape );
}

//*****************************************************************************
//	�X�V
//*****************************************************************************
void World::Update( void )
{
	//	�t�F�[�h�X�V
	m_pFade->Update();

	//	�f�����j���[�X�V
	if( m_pDemoMenu ) m_pDemoMenu->Update();

	if( m_bExit ) return;

	//	���v���C�I���`�F�b�N
	if( m_pReplayManager->IsReplayEnable() )
	{
		if( m_pReplayManager->IsReplayOver( m_FrameCount ) )
		{
			OpenReplayMenu( false );
		}
	}

	//	�R���e�B�j���[�X�V
	m_pContinue->Update();
	if( m_pContinue->IsActive() ) return;

	//	�X�e�[�W���U���g�X�V
	m_pResult->Update();
	if( m_pResult->IsActive() ) return;

	//	���v���C���j���[�X�V
	m_pReplayMenu->Update();
	if( m_pReplayMenu->IsActive() ) return;

	//	�|�[�Y�X�V
	m_pPause->Update();
	if( m_pPause->IsPause() ) return;

	//	�R���g���[��
	m_pControllerMng->Update( m_FrameCount );

	//	�X�e�[�W
	m_pStageManager->Update();

	//	�n�`
	m_pGroundManager->Update();

	//	�G
	m_pEnemyManager->Update();

	//	�U��
	m_pAttackManager->Update();

	//	�A�C�e��
	m_pItemManager->Update();

	//	�v���C���[
	m_pPlayerManager->Update();

	//	�X�R�A
	m_pScoreManager->Update();

	//	�C���W�P�[�g
	m_pIndicateManager->Update();

	m_FrameCount++;

	//	�S�X�e�[�W�I��
	if( m_pStageManager->IsEndAllStage() )
	{
		Exit( EXITCODE_CLEAR );
	}
}

//******************************************************************************
//	�`��
//******************************************************************************
void World::Draw( void )
{
	//	�n�`
	m_pGroundManager->Draw();

	//	�G
	m_pEnemyManager->Draw();

	//	�v���C���[
	m_pPlayerManager->Draw();

	//	�X�e�[�W
	m_pStageManager->Draw();

	//	�C���W�P�[�g
	m_pIndicateManager->Draw();

	//	�|�[�Y
	m_pPause->Draw();

	//	�R���e�B�j���[
	m_pContinue->Draw();

	//	���v���C���j���[
	m_pReplayMenu->Draw();

	//	����
	m_pResult->Draw();

	//	�f�����j���[
	SafePtr(m_pDemoMenu)->Draw( GetDrawOffset() );

	//	�t�F�[�h
	m_pFade->Draw();
}

//******************************************************************************
//	���[���h�̏I��
//******************************************************************************
void World::Exit( Label ExitCode )
{
	m_bExit		= true;
	m_ExitCode	= ExitCode;

	Color	FadeCol		= Color::Black();
	int		FadeTime	= FADE_TIME;

	//	�I�[���N���A
	if( m_Mode == MODE_ALL && ExitCode == EXITCODE_CLEAR )
	{
		FadeCol		= Color::White();
		FadeTime	= FADE_TIME_LONG;
		P5::GetGraphix()->GetDevice()->SetScreenColor( Color::White() );
	}

	m_pFade->FadeOut( FadeTime, FadeCol );
}

//******************************************************************************
//	�I����Ԃ��ǂ���
//******************************************************************************
bool World::IsExit( void ) const
{
	return m_bExit && !m_pFade->IsActive();
}

//******************************************************************************
//	���@�̎��S�ʒm
//******************************************************************************
void World::OnPlayerDead( void )
{
	if( m_pContinue->IsCountinueEnable() )
	{
		//	���v���C��
		if( m_pReplayManager->IsReplayEnable() )
		{
			//	���v���C���̃R���e�B�j���[�񐔈ȓ�
			if( m_pContinue->GetContinueCount() < m_pReplayManager->GetContinueCount() )
			{
				//	�����R���e�B�j���[
				m_pContinue->AutoDecide();
			}
			else
			{
				//	���v���C���j���[�J��
				OpenReplayMenu( false );
			}
		}
		else
		{
			//	�R���e�B�j���[�J��
			m_pContinue->Open();
		}
	}
	else
	{
		//	�Q�[���I�[�o�[
		Exit( EXITCODE_DEAD );
	}
}

//******************************************************************************
//	�R���e�B�j���[�ʒm
//******************************************************************************
void World::OnContinue( void )
{
	if( m_bExit ) return;

	//	�X�R�A�ʒm
	m_pScoreManager->OnContinue();

	//	���@�𕜋A
	m_pPlayerManager->OnContinue();
}

//******************************************************************************
//	�X�e�[�W�ύX�ʒm
//******************************************************************************
void World::OnChangeStage( int StageIndex )
{
	if( m_bExit ) return;

	//	�X�R�A�ʒm
	m_pScoreManager->EndStage();

	//	���v���C�ʒm
	m_pReplayManager->RecordChangeStage( m_FrameCount, StageIndex );

	//	����������
	_ResetRand();
}

//******************************************************************************
//	�X�e�[�W���U���g���J��
//******************************************************************************
void World::OpenResult( void )
{
	if( m_bExit ) return;

	m_pResult->Open();
}

//******************************************************************************
//	���v���C���j���[���J��
//******************************************************************************
void World::OpenReplayMenu( bool bCancelEnable )
{
	if( m_bExit ) return;

	m_pReplayMenu->Open( bCancelEnable );
}

//******************************************************************************
//	���v���C���}�j���A������ɐ؂�ւ���
//******************************************************************************
void World::ChangeReplayManual( void )
{
	//	���v���C�Ǘ������ւ�
	m_pReplayManager->ChangeManual( m_FrameCount );

	//	�R���g���[�������ւ�
	m_pControllerMng->ChangeManual();
}

//******************************************************************************
//	�R���g���[�����擾
//******************************************************************************
const Controller* World::GetController( void ) const
{
	return m_pControllerMng->GetController();
}

//******************************************************************************
//	�t�F�[�h�����ǂ���
//******************************************************************************
bool World::IsFade( void ) const
{
	return m_pFade->IsActive();
}

//------------------------------------------------------------------------------
//	���������Z�b�g����
//------------------------------------------------------------------------------
void World::_ResetRand( void )
{
	m_pPlayerManager->GetRand().Reset();
	m_pEnemyManager->GetRand().Reset();
	m_pAttackManager->GetRand().Reset();
	m_pItemManager->GetRand().Reset();
}