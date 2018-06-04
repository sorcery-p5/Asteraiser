#include "stdafx.h"
#include "ReplayManager.h"
#include "ReplayData.h"

#include "World/World.h"

namespace
{
	pstr			PATH_REPLAY_DIR					= "Replay/";
	pstr			PATH_REPLAY_SAVE_DIR_FIND		= "%s/*.rep";
	pstr			PATH_REPLAY_SAVE_DIR_FORMAT		= "%s/%d %s.rep";
}

////////////////////////////////////////////////////////////////////////////////
//
//	���v���C�Ǘ�
//
////////////////////////////////////////////////////////////////////////////////
ReplayManager::ReplayManager( World* pWorld )
{
	m_pLoadData		= NULL;
	m_pRecordData	= NULL;

	m_FrameOffset	= 0;
	m_PlayID		= 0;
}
ReplayManager::~ReplayManager()
{
	SafeDelete( m_pLoadData );
	SafeDelete( m_pRecordData );
}

//******************************************************************************
//	�L�^���[�h�ŏ�����
//******************************************************************************
void ReplayManager::InitRecord( void )
{
	m_pRecordData = new ReplayData;
	MEM_CHECK( m_pRecordData );
}

//******************************************************************************
//	�Đ����[�h�ŏ�����
//******************************************************************************
bool ReplayManager::InitLoad( pstr pPath, int Stage )
{
	m_pLoadData = new ReplayData;
	MEM_CHECK( m_pLoadData );

	if( !m_pLoadData->Load( pPath ) ) return false;

	//	�J�n�I�t�Z�b�g���v�Z
	m_FrameOffset = 0;
	for( uint i = 0; i < m_pLoadData->GetHeader().EventNum; i++ )
	{
		const REPLAY_EVENT& Event = m_pLoadData->GetEvent(i);
		if( Event.Type == REPLAY_EVENT_CHANGE_STAGE &&
			Event.ParamInt[0] == Stage )
		{
			m_FrameOffset = Event.Frame;
		}
	}

	//	�v���CID���擾
	m_PlayID = m_pLoadData->GetHeader().PlayID;

	return true;
}

//******************************************************************************
//	���v���C�f�[�^��ۑ�
//******************************************************************************
bool ReplayManager::Save( const SAVEDATA_SCORE& Score, pstr pDirName, int SaveMax )
{
#if DEMO_VER == DEMO_PLAY_VER
	//	�f�����[�h�ł͕ۑ����Ȃ�
	return false;
#endif

	if( !m_pRecordData ) return false;

	//	�ۑ��f�B���N�g���𐶐�
	::CreateDirectory( PATH_REPLAY_DIR, NULL );
	::CreateDirectory( pDirName, NULL );

	//	�����f�[�^�̍폜
	if( SaveMax > 0 )
	{
		//	�ۑ��f�B���N�g�����̃t�@�C�����擾
		String256 FindPath;
		FindPath.Format( PATH_REPLAY_SAVE_DIR_FIND, pDirName );
		Directory Dir( FindPath.c_str(), Directory::FILTER_FILE );
		if( Dir.GetChildNum() >= SaveMax )
		{
			//	�ۑ����ȏ゠�����ꍇ�A�ł��Â����̂�����
			int DelIndex	= -1;
			int MinPlayID	= INT_MAX;
			for( int i = 0; i < Dir.GetChildNum(); i++ )
			{
				int PlayID = 0;
				sscanf( Dir.GetChildTitle(i).c_str(), "%d ", &PlayID );

				if( PlayID < MinPlayID )
				{
					MinPlayID	= PlayID;
					DelIndex	= i;
				}
			}

			if( DelIndex >= 0 )
			{
				::DeleteFile( Dir.GetChildPath(DelIndex) );
			}
		}
	}

	//	�����̍����ւ�
	String256 FileDate = Score.DateTime.c_str();
	FileDate.Replace( "/", "_" );
	FileDate.Replace( ":", "_" );

	//	�ۑ��p�X
	String256 SavePath;
	SavePath.Format( PATH_REPLAY_SAVE_DIR_FORMAT, pDirName, Score.PlayID, FileDate.c_str() );

	return m_pRecordData->Save( SavePath.c_str(), Score );
}

//******************************************************************************
//	������}�j���A���ɐ؂�ւ���
//******************************************************************************
void ReplayManager::ChangeManual( uint FrameCount )
{
	if( !m_pLoadData || m_pRecordData ) return;

	//	�ڂ��ς�
	m_pRecordData = m_pLoadData;
	m_pLoadData = NULL;

	//	�L�^�f�[�^�̔��f
	m_pRecordData->ChangeManual( FrameCount );
}

//******************************************************************************
//	�Đ��f�[�^�̋L�^
//******************************************************************************
void ReplayManager::RecordData( byte Data )
{
	if( !m_pRecordData ) return;
	
	m_pRecordData->AddData( Data );
}

//******************************************************************************
//	�X�e�[�W�ύX�̒ʒm
//******************************************************************************
void ReplayManager::RecordChangeStage( uint FrameCount, int Stage )
{
	if( !m_pRecordData ) return;
	
	//	�C�x���g�̓o�^
	REPLAY_EVENT Event;
	Event.Frame			= FrameCount + m_FrameOffset;
	Event.Type			= REPLAY_EVENT_CHANGE_STAGE;
	Event.ParamInt[0]	= Stage;

	m_pRecordData->AddEvent( Event );
}

//******************************************************************************
//	���v���C���I�����Ă��邩�ǂ���
//******************************************************************************
bool ReplayManager::IsReplayOver( uint FrameCount ) const
{
	if( m_pLoadData )
	{
		return FrameCount >= m_pLoadData->GetHeader().FrameCount;
	}
	return true;
}

//******************************************************************************
//	�Đ��f�[�^���擾
//******************************************************************************
byte ReplayManager::GetReplayData( uint FrameCount ) const
{
	FrameCount += m_FrameOffset;
	if( m_pLoadData && FrameCount < m_pLoadData->GetHeader().FrameCount )
	{
		return m_pLoadData->GetData( FrameCount );
	}
	return 0;
}

//******************************************************************************
//	�R���e�B�j���[�񐔂𓾂�
//******************************************************************************
int ReplayManager::GetContinueCount( void ) const
{
	if( m_pLoadData )
	{
		return m_pLoadData->GetHeader().ContinueCount;
	}
	return 0;
}