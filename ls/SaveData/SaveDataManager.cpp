#include "stdafx.h"
#include "SaveDataManager.h"


namespace
{
	pstr		SAVEDATA_PATH				= "SaveData.xml";

	pstr		NODE_SAVEDATA				= "SaveData";
	pstr		NODE_SCORE					= "Score";

	pstr		ATTR_ARRIVAL				= "Arrival";
	pstr		ATTR_PLAY_ID				= "PlayID";
	pstr		ATTR_PLAY_NUM				= "PlayNum";
	pstr		ATTR_CLEAR_NUM				= "ClearNum";

	pstr		ATTR_SCORE					= "Score";
	pstr		ATTR_TOTAL_SCORE			= "TotalScore";
	pstr		ATTR_ITEM_COUNT				= "ItemCount";
	pstr		ATTR_DESTROY_COUNT			= "DestroyCount";
	pstr		ATTR_FRAME_COUNT			= "FrameCount";

	pstr		ATTR_DAMAGE					= "Damage";
	pstr		ATTR_BREAK					= "Break";
	pstr		ATTR_CONTINUE				= "Continue";
	pstr		ATTR_MISS					= "Miss";
	pstr		ATTR_CRASH					= "Crash";
	pstr		ATTR_ITEM_LOST				= "ItemLost";

	pstr		ATTR_RATE_AVERAGE			= "RateAverage";
	pstr		ATTR_HEAT_AVERAGE			= "HeatAverage";

	pstr		ATTR_DATE_TIME				= "DateTime";
	pstr		ATTR_LAST_DATE_TIME			= "LastDateTime";


	const int	SCORE_RECORD_MAX			= 10;

	const uint	PLAY_NUM_MAX				= 99999999;
	const uint	CLEAR_NUM_MAX				= 999;
	const uint	TOTAL_SCORE_MAX				= 999999999;
	const uint	ITEM_COUNT_MAX				= 999999999;
	const uint	DESTROY_COUNT_MAX			= 999999999;
	const uint	FRAME_COUNT_MAX				= 999999999;

	//	�X�R�A��r�֐�
	class ScoreComp
	{
	public:
		bool operator() ( const SAVEDATA_SCORE& A, const SAVEDATA_SCORE& B )
		{
			if( A.Score > B.Score ) return true;
			if( A.Score < B.Score ) return false;
			
			if( A.ItemCount > B.ItemCount ) return true;
			if( A.ItemCount < B.ItemCount ) return false;
			
			if( A.DestroyCount > B.DestroyCount ) return true;
			if( A.DestroyCount < B.DestroyCount ) return false;

			return A.FrameCount < B.FrameCount;
		}
		
	};
}


////////////////////////////////////////////////////////////////////////////////
//
//	�Z�[�u�f�[�^�Ǘ�
//
////////////////////////////////////////////////////////////////////////////////
SaveDataManager::SaveDataManager( void )
{

}
SaveDataManager::~SaveDataManager()
{

}

//******************************************************************************
//	�Z�[�u�f�[�^��ǂݍ���
//******************************************************************************
bool SaveDataManager::Load( void )
{
	//	������
	Reset();

	//	�t�@�C���ǂݍ���
	XML Xml;
	if( !Xml.Load( SAVEDATA_PATH ) )
	{
		return false;
	}

	//	�Z�[�u�f�[�^�ǂݍ���
	const XML::Node* pSaveDataNode = Xml.GetRoot().FindChild( NODE_SAVEDATA );
	if( pSaveDataNode ) _LoadSaveData( pSaveDataNode );

	//	�X�R�A�ǂݍ���
	const XML::Node* pScoreNode = Xml.GetRoot().FindChild( NODE_SCORE );
	if( pScoreNode ) _LoadScore( pScoreNode );

	return true;
}

//******************************************************************************
//	�Z�[�u�f�[�^��ۑ�
//******************************************************************************
bool SaveDataManager::Save( void )
{
	XML Xml;

	//	�Z�[�u�f�[�^
	XML::Node* pSaveDataNode = new XML::Node( NODE_SAVEDATA );
	if( pSaveDataNode )
	{
		_SaveSaveData( pSaveDataNode );
		Xml.GetRoot().AddChild( pSaveDataNode );
	}

	//	�X�R�A
	XML::Node* pScoreNode = new XML::Node( NODE_SCORE );
	if( pScoreNode )
	{
		_SaveScore( pScoreNode );
		Xml.GetRoot().AddChild( pScoreNode );
	}

	//	��������
	return Xml.Save( SAVEDATA_PATH );	
}

//******************************************************************************
//	���Z�b�g
//******************************************************************************
void SaveDataManager::Reset( void )
{
	m_SaveData.Reset();

	m_vScoreRecord.clear();
}

//******************************************************************************
//	�X�R�A���L�^
//******************************************************************************
uint SaveDataManager::RecordScore( SAVEDATA_SCORE Score, bool bClear )
{
	//	ID��o�^
	Score.PlayID	= m_SaveData.PlayNum;

	//	�݌v�ɒǉ�
	_UpdateTotalData( Score, bClear );

	//	�L�^�ɑ}��
	_InsertRecord( Score );

	return Score.PlayID;
}

//******************************************************************************
//	�X�R�A�f�[�^�̌���
//******************************************************************************
int SaveDataManager::FindRecord( uint PlayID ) const
{
	for( int i = 0; i < GetScoreNum(); i++ )
	{
		const SAVEDATA_SCORE& Score = m_vScoreRecord[i];

		if( Score.PlayID == PlayID )
		{
			return i;
		}
	}

	return -1;
}

//------------------------------------------------------------------------------
//	�Z�[�u�f�[�^�̓ǂݍ���
//------------------------------------------------------------------------------
void SaveDataManager::_LoadSaveData( const XML::Node* pNode )
{
	m_SaveData.Arrival			= pNode->GetAttr( ATTR_ARRIVAL ).GetInt();
	m_SaveData.PlayNum			= pNode->GetAttr( ATTR_PLAY_NUM ).GetInt();
	m_SaveData.ClearNum			= pNode->GetAttr( ATTR_CLEAR_NUM ).GetInt();

	m_SaveData.TotalScore		= pNode->GetAttr( ATTR_TOTAL_SCORE ).GetInt();
	m_SaveData.ItemCount		= pNode->GetAttr( ATTR_ITEM_COUNT ).GetInt();
	m_SaveData.DestroyCount		= pNode->GetAttr( ATTR_DESTROY_COUNT ).GetInt();
	m_SaveData.FrameCount		= pNode->GetAttr( ATTR_FRAME_COUNT ).GetInt();

	m_SaveData.bNoDamage		= pNode->GetAttr( ATTR_DAMAGE ).GetBool();
	m_SaveData.bNoBreak			= pNode->GetAttr( ATTR_BREAK ).GetBool();
	m_SaveData.bNoContinue		= pNode->GetAttr( ATTR_CONTINUE ).GetBool();
	m_SaveData.bNoMiss			= pNode->GetAttr( ATTR_MISS ).GetBool();
	m_SaveData.bNoCrash			= pNode->GetAttr( ATTR_CRASH ).GetBool();
	m_SaveData.bNoLost			= pNode->GetAttr( ATTR_ITEM_LOST ).GetBool();
	
	m_SaveData.LastDateTime		= pNode->GetAttr( ATTR_LAST_DATE_TIME ).GetStr();
}

//------------------------------------------------------------------------------
//	�X�R�A�f�[�^��ǂݍ���
//------------------------------------------------------------------------------
void SaveDataManager::_LoadScore( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pChildNode = pNode->GetChild(i);

		SAVEDATA_SCORE Score;
		_LoadScore( Score, pChildNode );

		m_vScoreRecord.push_back( Score );
	}
}

//------------------------------------------------------------------------------
//	�X�R�A�f�[�^��ǂݍ���
//------------------------------------------------------------------------------
void SaveDataManager::_LoadScore( SAVEDATA_SCORE& Score, const XML::Node* pNode )
{
	Score.Arrival			= pNode->GetAttr( ATTR_ARRIVAL ).GetInt();
	Score.PlayID			= pNode->GetAttr( ATTR_PLAY_ID ).GetInt();

	Score.Score				= pNode->GetAttr( ATTR_SCORE ).GetInt();
	Score.ItemCount			= pNode->GetAttr( ATTR_ITEM_COUNT ).GetInt();
	Score.DestroyCount		= pNode->GetAttr( ATTR_DESTROY_COUNT ).GetInt();
	Score.FrameCount		= pNode->GetAttr( ATTR_FRAME_COUNT ).GetInt();

	Score.DamageCount		= pNode->GetAttr( ATTR_DAMAGE ).GetInt();
	Score.BreakCount		= pNode->GetAttr( ATTR_BREAK ).GetInt();
	Score.ContinueCount		= pNode->GetAttr( ATTR_CONTINUE ).GetInt();
	Score.MissCount			= pNode->GetAttr( ATTR_MISS ).GetInt();
	Score.CrashCount		= pNode->GetAttr( ATTR_CRASH ).GetInt();
	Score.ItemLostCount		= pNode->GetAttr( ATTR_ITEM_LOST ).GetInt();

	Score.RateAverage		= pNode->GetAttr( ATTR_RATE_AVERAGE ).GetFloat();
	Score.HeatAverage		= pNode->GetAttr( ATTR_HEAT_AVERAGE ).GetFloat();

	Score.DateTime			= pNode->GetAttr( ATTR_DATE_TIME ).GetStr();
}

//------------------------------------------------------------------------------
//	�Z�[�u�f�[�^����������
//------------------------------------------------------------------------------
void SaveDataManager::_SaveSaveData( XML::Node* pNode )
{
	_ASSERT( pNode );

	pNode->AddAttr( ATTR_ARRIVAL,			XML::Value( m_SaveData.Arrival				 ) );
	pNode->AddAttr( ATTR_PLAY_NUM,			XML::Value( m_SaveData.PlayNum				 ) );
	pNode->AddAttr( ATTR_CLEAR_NUM,			XML::Value( m_SaveData.ClearNum				 ) );

	pNode->AddAttr( ATTR_TOTAL_SCORE,		XML::Value( m_SaveData.TotalScore			 ) );
	pNode->AddAttr( ATTR_ITEM_COUNT,		XML::Value( m_SaveData.ItemCount			 ) );
	pNode->AddAttr( ATTR_DESTROY_COUNT,		XML::Value( m_SaveData.DestroyCount			 ) );
	pNode->AddAttr( ATTR_FRAME_COUNT,		XML::Value( m_SaveData.FrameCount			 ) );

	pNode->AddAttr( ATTR_DAMAGE,			XML::Value( m_SaveData.bNoDamage			 ) );
	pNode->AddAttr( ATTR_BREAK,				XML::Value( m_SaveData.bNoBreak				 ) );
	pNode->AddAttr( ATTR_CONTINUE,			XML::Value( m_SaveData.bNoContinue			 ) );
	pNode->AddAttr( ATTR_MISS,				XML::Value( m_SaveData.bNoMiss				 ) );
	pNode->AddAttr( ATTR_CRASH,				XML::Value( m_SaveData.bNoCrash				 ) );
	pNode->AddAttr( ATTR_ITEM_LOST,			XML::Value( m_SaveData.bNoLost				 ) );

	pNode->AddAttr( ATTR_LAST_DATE_TIME,	XML::Value( m_SaveData.LastDateTime.c_str()	 ) );
}

//------------------------------------------------------------------------------
//	�X�R�A�f�[�^����������
//------------------------------------------------------------------------------
void SaveDataManager::_SaveScore( XML::Node* pNode )
{
	for( int i = 0; i < GetScoreNum(); i++ )
	{
		const SAVEDATA_SCORE& Score = m_vScoreRecord[i];

		XML::Node* pChildNode = new XML::Node( NODE_SCORE );

		_SaveScore( pChildNode, Score );
		pNode->AddChild( pChildNode );
	}
}

//------------------------------------------------------------------------------
//	�X�R�A�f�[�^����������
//------------------------------------------------------------------------------
void SaveDataManager::_SaveScore( XML::Node* pNode, const SAVEDATA_SCORE& Score )
{
	pNode->AddAttr( ATTR_ARRIVAL,		XML::Value( Score.Arrival			 ) );
	pNode->AddAttr( ATTR_PLAY_ID,		XML::Value( Score.PlayID			 ) );

	pNode->AddAttr( ATTR_SCORE,			XML::Value( Score.Score				 ) );
	pNode->AddAttr( ATTR_ITEM_COUNT,	XML::Value( Score.ItemCount			 ) );
	pNode->AddAttr( ATTR_DESTROY_COUNT,	XML::Value( Score.DestroyCount		 ) );
	pNode->AddAttr( ATTR_FRAME_COUNT,	XML::Value( Score.FrameCount		 ) );

	pNode->AddAttr( ATTR_DAMAGE,		XML::Value( Score.DamageCount		 ) );
	pNode->AddAttr( ATTR_BREAK,			XML::Value( Score.BreakCount		 ) );
	pNode->AddAttr( ATTR_CONTINUE,		XML::Value( Score.ContinueCount		 ) );
	pNode->AddAttr( ATTR_MISS,			XML::Value( Score.MissCount			 ) );
	pNode->AddAttr( ATTR_CRASH,			XML::Value( Score.CrashCount		 ) );
	pNode->AddAttr( ATTR_ITEM_LOST,		XML::Value( Score.ItemLostCount		 ) );

	pNode->AddAttr( ATTR_RATE_AVERAGE, 	XML::Value( Score.RateAverage		 ) );
	pNode->AddAttr( ATTR_HEAT_AVERAGE, 	XML::Value( Score.HeatAverage		 ) );

	pNode->AddAttr( ATTR_DATE_TIME,		XML::Value( Score.DateTime.c_str()	 ) );
}

//------------------------------------------------------------------------------
//	�݌v�����X�V
//------------------------------------------------------------------------------
void SaveDataManager::_UpdateTotalData( const SAVEDATA_SCORE& Score, bool bClear )
{
	//	���B�ꏊ
	m_SaveData.Arrival = Max( m_SaveData.Arrival, Score.Arrival );

	//	�݌v
	Increase( m_SaveData.PlayNum,		PLAY_NUM_MAX,		1u );
	Increase( m_SaveData.ClearNum,		CLEAR_NUM_MAX,		bClear? 1u : 0 );
	Increase( m_SaveData.TotalScore,	TOTAL_SCORE_MAX,	Score.Score );
	Increase( m_SaveData.ItemCount,		ITEM_COUNT_MAX,		Score.ItemCount );
	Increase( m_SaveData.DestroyCount,	DESTROY_COUNT_MAX,	Score.DestroyCount );
	Increase( m_SaveData.FrameCount,	FRAME_COUNT_MAX,	Score.FrameCount );

	//	�t���O
	if( bClear )
	{
		m_SaveData.bNoDamage	= m_SaveData.bNoDamage		|| Score.DamageCount	== 0;
		m_SaveData.bNoBreak		= m_SaveData.bNoBreak		|| Score.BreakCount		== 0;
		m_SaveData.bNoContinue	= m_SaveData.bNoContinue	|| Score.ContinueCount	== 0;
		m_SaveData.bNoMiss		= m_SaveData.bNoMiss		|| Score.MissCount		== 0;
		m_SaveData.bNoCrash		= m_SaveData.bNoCrash		|| Score.CrashCount		== 0;
		m_SaveData.bNoLost		= m_SaveData.bNoLost		|| Score.ItemLostCount	== 0;
	}

	//	�X�V����
	m_SaveData.LastDateTime	= Score.DateTime;
}

//------------------------------------------------------------------------------
//	�X�R�A�L�^��}��
//------------------------------------------------------------------------------
void SaveDataManager::_InsertRecord( const SAVEDATA_SCORE& Score )
{
	//	�ǉ�
	m_vScoreRecord.push_back( Score );

	//	�\�[�g
	std::sort( m_vScoreRecord.begin(), m_vScoreRecord.end(), ScoreComp() );

	//	�ő吔�𒴂��Ă���Ζ���������
	if( m_vScoreRecord.size() > SCORE_RECORD_MAX )
	{
		m_vScoreRecord.pop_back();
	}
}