#include "stdafx.h"
#include "ReplayData.h"


////////////////////////////////////////////////////////////////////////////////
//
//	���v���C�f�[�^
//
////////////////////////////////////////////////////////////////////////////////
ReplayData::ReplayData( void )
{
	ZeroStruct( m_Header );
}
ReplayData::~ReplayData()
{
	
}

//******************************************************************************
//	���v���C�f�[�^��ǂݍ���
//******************************************************************************
bool ReplayData::Load( pstr pPath )
{
	//	������
	Reset();

	//	�t�@�C���I�[�v��
	FileHandle	File;
	if( !File.Open( pPath, "rb" ) ) return false;

	//	�w�b�_�ǂݍ���
	if( fread( &m_Header, sizeof(REPLAY_HEADER), 1, File ) != 1 ) return false;

	//	�t�H�[�}�b�g�E�o�[�W�����擾
	if( m_Header.Format != REPLAY_FORMAT || m_Header.Version != REPLAY_VERSION ) return false;

	//	�C�x���g�ǂݍ���
	if( m_Header.EventNum > 0 )
	{
		m_vEvent.resize( m_Header.EventNum );
		if( fread( &m_vEvent[0], sizeof(REPLAY_EVENT), m_Header.EventNum, File ) != m_Header.EventNum ) return false;
	}

	//	�f�[�^�ǂݍ���
	if( m_Header.FrameCount > 0 )
	{
		m_vData.resize( m_Header.FrameCount );
		if( fread( &m_vData[0], sizeof(byte), m_Header.FrameCount, File ) != m_Header.FrameCount ) return false;
	}

	return true;
}

//******************************************************************************
//	�w�b�_�݂̂��J��
//******************************************************************************
bool ReplayData::LoadHeader( pstr pPath )
{
	//	������
	Reset();

	//	�t�@�C���I�[�v��
	FileHandle	File;
	if( !File.Open( pPath, "rb" ) ) return false;

	//	�w�b�_�ǂݍ���
	if( fread( &m_Header, sizeof(REPLAY_HEADER), 1, File ) != 1 ) return false;

	//	�t�H�[�}�b�g�E�o�[�W�����擾
	if( m_Header.Format != REPLAY_FORMAT || m_Header.Version != REPLAY_VERSION ) return false;

	return true;
}

//******************************************************************************
//	���v���C�f�[�^��ۑ�
//******************************************************************************
bool ReplayData::Save( pstr pPath, const SAVEDATA_SCORE& Score )
{
	//	�w�b�_�f�[�^�ݒ�
	m_Header.Format			= REPLAY_FORMAT;
	m_Header.Version		= REPLAY_VERSION;
	strncpy( m_Header.DateTime, Score.DateTime.c_str(), sizeof(m_Header.DateTime) );

	m_Header.FrameCount		= m_vData.size();
	m_Header.EventNum		= m_vEvent.size();

	m_Header.PlayID			= Score.PlayID;
	m_Header.Arrival		= Score.Arrival;
	m_Header.Score			= Score.Score;
	m_Header.ContinueCount	= Score.ContinueCount;

	//	�t�@�C���I�[�v��
	FileHandle	File;
	if( !File.Open( pPath, "wb" ) ) return false;

	//	�w�b�_��������
	if( fwrite( &m_Header, sizeof(REPLAY_HEADER), 1, File ) != 1 ) return false;

	//	�C�x���g��������
	if( m_Header.EventNum > 0 )
	{
		if( fwrite( &m_vEvent[0], sizeof(REPLAY_EVENT), m_Header.EventNum, File ) != m_Header.EventNum ) return false;
	}

	//	�f�[�^��������
	if( m_Header.FrameCount > 0 )
	{
		if( fwrite( &m_vData[0], sizeof(byte), m_Header.FrameCount, File ) != m_Header.FrameCount ) return false;
	}

	return true;
}

//******************************************************************************
//	���Z�b�g
//******************************************************************************
void ReplayData::Reset( void )
{
	ZeroStruct( m_Header );

	m_vData.clear();
	m_vEvent.clear();
}

//******************************************************************************
//	�C�x���g�̒ǉ�
//******************************************************************************
void ReplayData::AddEvent( const REPLAY_EVENT& Event )
{
	m_vEvent.push_back( Event );
}

//******************************************************************************
//	�f�[�^�̒ǉ�
//******************************************************************************
void ReplayData::AddData( byte Data )
{
	m_vData.push_back( Data );
}

//******************************************************************************
//	�}�j���A������֐؂�ւ�
//******************************************************************************
void ReplayData::ChangeManual( uint FrameCount )
{
	//	�ǋL�񐔑���
	m_Header.OverwriteCount++;

	//	�C�x���g
	for( VecEvent::iterator it = m_vEvent.begin(); it != m_vEvent.end(); )
	{
		if( it->Frame >= FrameCount )
		{
			it = m_vEvent.erase( it );
		}
		else
		{
			++it;
		}
	}

	//	�f�[�^
	if( FrameCount < m_vData.size() )
	{
		m_vData.resize( FrameCount );
	}
}