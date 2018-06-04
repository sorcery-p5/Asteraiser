#include "stdafx.h"
#include "ReplaySelectScene.h"
#include "TitleScene.h"
#include "ReplayScene.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"
#include "BGM/BGMManager.h"

#include "Menu/Menu.h"
#include "Menu/MenuData.h"
#include "Menu/MenuLayer.h"
#include "Menu/Object/MenuObject.h"

#include "Replay/ReplayDirectory.h"

namespace
{
	pstr		MENU_PATH		= "Data/Menu/Replay.xml";
	pstr		WORLD_PATH		= "Data/World/World.xml";

	pstr		SCENE_REPLAY	= "Replay";
	pstr		SCENE_TITLE		= "Title";
	
	pstr		REPLAY_SAVE_DIR	= "Save";

	const Label	LAYER_MAIN		= "Main";
	const Label	LAYER_AREA		= "Area";

	const Label	NOTIFY_EXIT		= "Exit";
	const Label	NOTIFY_DECIDE	= "Decide";
	const Label	NOTIFY_START	= "Start";

	const Label BONE_LIST		= "List";
	const Label BONE_DIR		= "Directory";
	const Label BONE_DIR_NAME	= "DirName";
	
	const Label BONE_PLAY_ID	= "PlayID";
	const Label BONE_ARRIVAL	= "Arrival";
	const Label BONE_SCORE		= "Score";
	const Label BONE_TIME		= "Time";
	const Label BONE_DATE		= "Date";
	const Label BONE_REWRITE	= "Rewrite";

	const StrPair<int> ARRIVAL_NAME_TABLE[] =
	{
		{ "1-1",	10, },
		{ "1-1 B",	11, },
		{ "1-2",	12, },
		{ "1-2 B",	13, },
					
		{ "2-1",	20, },
		{ "2-1 B",	21, },
		{ "2-2",	22, },
		{ "2-2 B",	23, },
					
		{ "3",		30, },
		{ "3 B",	31, },
					
		{ "4-1",	40, },
		{ "4-1 B",	41, },
		{ "4-2",	42, },
		{ "4-2 B",	43, },
					
		{ "All",	50, },
	};

	const LabelPair<int> OBJECT_STAGE_TABLE[] =
	{
		{ "Area1",	0, },
		{ "Area2",	1, },
		{ "Area3",	2, },
		{ "Area4",	3, },
	};

}


///////////////////////////////////////////////////////////////////////////////
//
//	���v���C�I���V�[��
//
///////////////////////////////////////////////////////////////////////////////
ReplaySelectScene::ReplaySelectScene( Label Name, pstr pWorldPath, pstr pReplayPath )
:Scene( Name )
{
	m_pMenuRes		= NULL;
	m_pMenu			= NULL;

	m_WorldPath		= pWorldPath;
	m_ReplayPath	= pReplayPath;

	m_pDirectory	= NULL;
	m_DirIndex		= 0;
	m_SelectIndex	= 0;
	m_SelectStage	= 0;
}
ReplaySelectScene::~ReplaySelectScene()
{
	SafeDelete( m_pMenu );
	SafeRelease( m_pMenuRes );
	
	SafeDelete( m_pDirectory );
}

//*****************************************************************************
//	������
//*****************************************************************************
void ReplaySelectScene::OnInit( void )
{
	//	�f�B���N�g��
	m_pDirectory = new ReplayDirectory;
	MEM_CHECK( m_pDirectory );

	m_pDirectory->Open();
	_ASSERT( m_pDirectory->GetDirNum() > 0 );

	//	�����I��
	int DirIndex = m_pDirectory->FindDir( GetFileDirName<256>( m_ReplayPath ).c_str() );
	int FileIndex = -1;
	if( DirIndex >= 0 )
	{
		//	�J�����t�@�C����I��
		m_DirIndex = DirIndex;
		FileIndex = m_pDirectory->FindFile( DirIndex, m_ReplayPath.c_str() );
	}
	else if( m_pDirectory->GetFileNum(0) == 0 )
	{
		//	�Z�[�u�t�H���_��D��
		DirIndex = m_pDirectory->FindDir( REPLAY_SAVE_DIR );
		if( DirIndex >= 0 )
		{
			m_DirIndex = DirIndex;
		}
	}

	//	���j���[
	m_pMenuRes = GetApp()->GetResMng()->LoadResource<MenuResource>( MENU_PATH );
	if( m_pMenuRes && m_pMenuRes->GetData() )
	{
		m_pMenu = new Menu( this );
		MEM_CHECK( m_pMenu );

		m_pMenu->Init( m_pMenuRes->GetData(), Matrix3::Trans( 0, 0 ) );

		MenuLayer* pLayer = m_pMenu->GetCurLayer();
		if( pLayer )
		{
			//	�f�B���N�g�����f
			MenuObject* pDirObject = pLayer->GetObject( BONE_DIR );
			if( pDirObject )
			{
				pDirObject->SetMinMax( 0, m_pDirectory->GetDirNum()-1 );
				pDirObject->SetValue( m_DirIndex );
			}
			
			//	���X�g�I��
			if( FileIndex >= 0 )
			{
				pLayer->SelectObject( BONE_LIST );

				MenuObject* pList = pLayer->GetObject( BONE_LIST );
				if( pList )
				{
					pList->SetSelectIndex( FileIndex );
				}
			}
		}

	}
	_ASSERT( m_pMenu );

	//	���j���[�X�V
	_UpdateMenu();

	m_SelectNotify.Clear();

	//	�w�i�F�ύX
	P5::GetGraphix()->GetDevice()->SetScreenColor( Color(0,0,0) );
}

//*****************************************************************************
//	�X�V
//*****************************************************************************
void ReplaySelectScene::Update( void )
{
	if( !m_pMenu ) return;

	m_pMenu->Update();

	if( m_pMenu->IsEmpty() )
	{
		//	���v���C�J�n
		if( m_SelectNotify == NOTIFY_DECIDE )
		{
			ChangeScene( new ReplayScene( SCENE_REPLAY, WORLD_PATH, m_pDirectory->GetFilePath( m_DirIndex, m_SelectIndex ), m_SelectStage ) );
			return;
		}

		//	�^�C�g���֖߂�
		else 
		{
			ChangeScene( new TitleScene( SCENE_TITLE, GetName() ) );
			return;
		}
	}

	m_pMenu->Draw( Matrix3::Identity() );
}

//******************************************************************************
//	���j���[����̒ʒm(���C���[�J��)
//******************************************************************************
void ReplaySelectScene::OnOpenLayer( MenuLayer* pLayer )
{
	if( pLayer->GetName() == LAYER_AREA )
	{
		//	�I���G���A�̍i�荞��
		const REPLAY_HEADER& Header = m_pDirectory->GetFileHeader( m_DirIndex, m_SelectIndex );

		for( int i = 0; i < ArrayNum(OBJECT_STAGE_TABLE); i++ )
		{
			bool bEnable = OBJECT_STAGE_TABLE[i].Value <= Max( (Header.Arrival / 10) - 1, 0 );

			pLayer->SetObjectEnable( OBJECT_STAGE_TABLE[i].Name, bEnable );
			pLayer->SetBoneVisible( OBJECT_STAGE_TABLE[i].Name, bEnable );
		}
	}
}

//******************************************************************************
//	���j���[����̒ʒm�i���C���[�ʒm�j
//******************************************************************************
void ReplaySelectScene::OnLayerNotify( MenuLayer* pLayer, Label Name, float Param )
{
	m_SelectNotify = Name;
}

//******************************************************************************
//	���j���[����̒ʒm�i�I�u�W�F�N�g�ʒm�j
//******************************************************************************
void ReplaySelectScene::OnObjectNotify( MenuObject* pObject, Label Name, float Param )
{
	m_SelectNotify = Name;

	//	���X�g����
	if( Name == NOTIFY_DECIDE )
	{
		//	�C���f�N�X���L��
		m_SelectIndex = pObject->GetSelectIndex();

		//	�J�n
		if( m_SelectIndex < m_pDirectory->GetFileNum( m_DirIndex ) )
		{
			m_pMenu->CloseCurLayer();

			GetApp()->GetBGMMng()->Fade( 15 );
		}
	}

	//	�G���A����
	else if( Name == NOTIFY_START )
	{
		//	�X�e�[�W���L��
		m_SelectStage = LabelToEnum<int>( pObject->GetName(), OBJECT_STAGE_TABLE, ArrayNum(OBJECT_STAGE_TABLE), 0 );
	}
}

//******************************************************************************
//	���j���[����̒ʒm�i�I�u�W�F�N�g�̒l�ύX�j
//******************************************************************************
void ReplaySelectScene::OnObjectChangeValue( MenuObject* pObject )
{
	if( pObject->GetName() == BONE_DIR )
	{
		m_DirIndex = (int)pObject->GetValue();
		_ASSERT( m_DirIndex >= 0 && m_DirIndex < m_pDirectory->GetDirNum() );

		//	���j���[�X�V
		_UpdateMenu();
	}
}

//------------------------------------------------------------------------------
//	���j���[�̍X�V
//------------------------------------------------------------------------------
void ReplaySelectScene::_UpdateMenu( void )
{
	MenuLayer* pLayer = m_pMenu->GetCurLayer();
	if( !pLayer ) return;

	//	�f�B���N�g�����o�^
	pLayer->SetText( BONE_DIR_NAME, m_pDirectory->GetDirName( m_DirIndex ) );

	//	���X�g�ύX
	MenuObject* pList = pLayer->GetObject( BONE_LIST );
	if( !pList ) return;

	pList->SetCellNum( m_pDirectory->GetFileNum( m_DirIndex ) );

	for( int i = 0; i < m_pDirectory->GetFileNum( m_DirIndex ); i++ )
	{
		const REPLAY_HEADER& Header = m_pDirectory->GetFileHeader( m_DirIndex, i );

		pList->SetCellText( i, BONE_PLAY_ID,	"%d",		Header.PlayID );
		pList->SetCellText( i, BONE_ARRIVAL,	"%s",		EnumToStr<int>( Header.Arrival, ARRIVAL_NAME_TABLE, ArrayNum(ARRIVAL_NAME_TABLE), "" ) );
		pList->SetCellText( i, BONE_SCORE,		"%d",		Header.Score );
		pList->SetCellText( i, BONE_TIME,		"%2d:%02d", Header.FrameCount / 3600, Header.FrameCount / 60 % 60 );
		pList->SetCellText( i, BONE_DATE,		"%s",		Header.DateTime );

		if( Header.OverwriteCount <= 0 )
		{
			pList->SetCellText( i, BONE_REWRITE,	"" );
		}
		else
		{
			pList->SetCellText( i, BONE_REWRITE,	"RW %d",	Header.OverwriteCount );
		}
	}
}