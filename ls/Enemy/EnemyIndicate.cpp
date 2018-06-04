#include "stdafx.h"
#include "EnemyIndicate.h"

#include "Enemy.h"

#include "World/World.h"
#include "World/WorldData.h"
#include "Indicate/IndicateManager.h"
#include "Indicate/Indicate.h"


namespace
{
	pstr		INDICATE_NAME_FMT		= "Enemy%d";

	const Label	INDICATE_ENEMY			= "Enemy";
	const Label	INDICATE_BOSS			= "Boss";

	const Label	INDICATE_BONE_NAME		= "Name";
	const Label	INDICATE_BONE_LIFE		= "Life";
	const Label	INDICATE_BONE_BACK		= "Back";
	const Label	INDICATE_BONE_DAMAGE	= "Damage";

	const int	INDICATE_MAX			= 3;
	const float	INDICATE_HEIGHT			= 30.0f;
	const float	INDICATE_HEIGHT_BOSS	= 40.0f;

	const int	LIFE_LIMIT				= 400;
	const int	LIFE_LIMIT_BOSS			= 700;
	const int	LEVEL_MAX				= 10;

	const float DAMAGE_COEF				= 0.1f;
	const float LIFE_COEF				= 0.01f;
	const int	LIFE_LERP_MAX			= 30;

	const Color GAUGE_COLOR[] =
	{
		Color( 160,0,0 ),
		Color( 255,192,64 ),
		Color( 255,255,192 ),
		Color( 160,255,64 ),
		Color( 96,160,96 ),
		Color( 160,255,192 ),
		Color( 96,96,255 ),
		Color( 160,192,255 ),
		Color( 160,96,255 ),
		Color( 255,192,255 ),
		Color( 255,64,192 ),
		Color( 240,240,240 ),
	};
	
	const Color GAUGE_MAX_COLOR[] =
	{
		Color( 96,96,96 ),
		Color( 240,240,240 ),
	};
}


////////////////////////////////////////////////////////////////////////////////
//
//	�G�̃C���W�P�[�g�Ǘ�
//
////////////////////////////////////////////////////////////////////////////////
EnemyIndicate::EnemyIndicate( World* pWorld )
{
	m_pWorld = pWorld;	
}

EnemyIndicate::~EnemyIndicate()
{

}

//******************************************************************************
//	�X�V
//******************************************************************************
void EnemyIndicate::Update( void )
{
	for( InfoList::iterator it = m_InfoList.begin(); it != m_InfoList.end(); )
	{
		if( _UpdateInfo( *it ) )
		{
			++it;
		}
		else
		{
			//	�폜
			_DeleteInfo( *it );
			it = m_InfoList.erase( it );
		}
	}

	//	�ʒu�̍X�V
	_UpdatePosition();
}

//******************************************************************************
//	�G��ǉ�
//******************************************************************************
void EnemyIndicate::AddEnemy( Enemy* pEnemy )
{
	if( !pEnemy ) return;

	InfoList::iterator it = StdFindIf( m_InfoList, EnemyComp(pEnemy) );
	if( it != m_InfoList.end() ) return;

	INFO Info;
	{
		Info.pEnemy			= pEnemy;
		Info.Life			= pEnemy->IsBoss()? 0 : pEnemy->GetLife();
		Info.DamageLife		= pEnemy->GetLife();
		Info.MaxLife		= pEnemy->GetMaxLife();
		Info.bBoss			= pEnemy->IsBoss();
		Info.bClose			= false;
		Info.bUp			= pEnemy->IsBoss();

		//	�C���W�P�[�gID�ݒ�
		Info.IndicateID = _GetUnuseIndicateID();

		String256 IndicateName;
		IndicateName.Format( INDICATE_NAME_FMT, Info.IndicateID );
		Info.IndicateName = IndicateName.c_str();

		//	�C���W�P�[�g�J��
		m_pWorld->GetIndicateManager()->AddIndicate( Info.IndicateName, m_pWorld->GetData()->GetIndicateData( pEnemy->IsBoss()? INDICATE_BOSS : INDICATE_ENEMY ) );

		//	�����ݒ�
		Indicate* pIndicate	= m_pWorld->GetIndicateManager()->GetIndicate( Info.IndicateName );
		if( pIndicate )
		{
			pIndicate->SetText( INDICATE_BONE_NAME, pEnemy->GetPreviewName() );

			//	�X�V
			_UpdateIndicate( pIndicate, Info );
		}
	}

	//	�ǉ�
	m_InfoList.push_front( Info );
	StdStableSortPred( m_InfoList, SortComp() );

	//	�������폜
	if( m_InfoList.size() > INDICATE_MAX )
	{
		_DeleteInfo( m_InfoList.back() );
		m_InfoList.pop_back();
	}

	//	�ʒu�̍X�V
	_UpdatePosition();
}

//------------------------------------------------------------------------------
//	�g���Ă��Ȃ��C���W�P�[�gID��T��
//------------------------------------------------------------------------------
int EnemyIndicate::_GetUnuseIndicateID( void )
{
	for( int i = 0; i <= INDICATE_MAX; i++ )
	{
		InfoList::iterator it = StdFindIf( m_InfoList, IndicateIDComp( i ) );
		if( it == m_InfoList.end() ) return i;
	}

	_ASSERT( false );
	return -1;
}

//------------------------------------------------------------------------------
//	�����X�V����
//------------------------------------------------------------------------------
bool EnemyIndicate::_UpdateInfo( INFO& Info )
{
	Indicate* pIndicate	= m_pWorld->GetIndicateManager()->GetIndicate( Info.IndicateName );
	if( !pIndicate )
	{
		return false;
	}

	if( !Info.bClose )
	{
		//	�Ώۂ�����
		if( !Info.pEnemy )
		{
			return false;
		}

		if( Info.Life >= Info.pEnemy->GetLife() )
		{
			//	�̗͎擾
			Info.Life = Info.pEnemy->GetLife();
				
			//	�_���[�W����
			if( !Info.pEnemy->IsRulerHit() )
			{
				Decrease( Info.DamageLife, Info.Life, (int)ceilf( (float)(Info.DamageLife - Info.Life) * DAMAGE_COEF ) );
			}
			Info.bUp = false;
		}
		//	������
		else
		{
			int t = (int)ceilf( (float)Info.MaxLife * LIFE_COEF );
			Increase( Info.Life, Info.pEnemy->GetLife(), Min( t, LIFE_LERP_MAX ) );
			Info.bUp = true;
		}

		//	���S�F����
		if( Info.pEnemy->IsDead() )
		{
			pIndicate->Close( true );
			Info.Life = 0;
			Info.bClose = true;
			Info.bUp = false;
		}
	}
	//	�����
	else
	{
		//	�_���[�W����
		Decrease( Info.DamageLife, 0, (int)ceilf( (float)Info.DamageLife * DAMAGE_COEF ) );
		Info.bUp = false;
	}

	//	�C���W�P�[�g�̍X�V
	_UpdateIndicate( pIndicate, Info );

	return true;
}

//------------------------------------------------------------------------------
//	�C���W�P�[�g�̍X�V
//------------------------------------------------------------------------------
void EnemyIndicate::_UpdateIndicate( Indicate* pIndicate, const INFO& Info )
{
	_ASSERT( pIndicate );

	//	�Q�[�W�̒l�����߂�
	int		Limit		= Info.bBoss? LIFE_LIMIT_BOSS : LIFE_LIMIT;
	int		Level		= ( Max(Info.Life - 1, 0) / Limit ) + 1;
	int		DamageLevel	= ( Max(Info.DamageLife - 1, 0) / Limit ) + 1;
	int		BackLevel	= Level - 1;
	float	LifeVal		= (float)(Info.Life - Limit * BackLevel) / (float)Limit;
	float	DamageVal	= (Level == DamageLevel)? ((float)(Info.DamageLife - Limit * BackLevel) / (float)Limit) : 1.0f;
	float	BackVal		= (float)Min( Info.MaxLife, Limit ) / (float)Limit;

	if( Info.bUp ) DamageVal = LifeVal;

	//	�F�̐ݒ�
	Color	LifeCol;
	Color	BackCol;

	if( Level >= ArrayNum( GAUGE_COLOR ) )
	{
		Level		= ( Level - ArrayNum( GAUGE_COLOR ) ) % 2;
		BackLevel	= (Level == 1)? 0 : 1;

		LifeCol	= GAUGE_MAX_COLOR[Level];
		BackCol	= GAUGE_MAX_COLOR[BackLevel];
	}
	else
	{
		LifeCol	= GAUGE_COLOR[Level];
		BackCol	= GAUGE_COLOR[BackLevel];
	}

	//	�̗�
	pIndicate->SetGauge( INDICATE_BONE_LIFE,		LifeVal, 1.0f );
	pIndicate->SetColor( INDICATE_BONE_LIFE,		LifeCol );

	//	�_���[�W
	pIndicate->SetMarker( INDICATE_BONE_DAMAGE,		LifeVal );
	pIndicate->SetGauge( INDICATE_BONE_DAMAGE,		DamageVal - LifeVal, 1.0f );

	//	�o�b�N
	pIndicate->SetMarker( INDICATE_BONE_BACK,		DamageVal );
	pIndicate->SetGauge( INDICATE_BONE_BACK,		BackVal - DamageVal, 1.0f );
	pIndicate->SetColor( INDICATE_BONE_BACK,		BackCol );
}

//------------------------------------------------------------------------------
//	�����폜����
//------------------------------------------------------------------------------
void EnemyIndicate::_DeleteInfo( INFO& Info )
{
	//	�C���W�P�[�g����
	m_pWorld->GetIndicateManager()->DeleteIndicate( Info.IndicateName );
}

//------------------------------------------------------------------------------
//	�ʒu�̍X�V
//------------------------------------------------------------------------------
void EnemyIndicate::_UpdatePosition( void )
{
	Point Pos = m_pWorld->GetFieldOffset();

	for( InfoList::iterator it = m_InfoList.begin(); it != m_InfoList.end(); ++it )
	{
		Indicate* pIndicate	= m_pWorld->GetIndicateManager()->GetIndicate( it->IndicateName );
		if( pIndicate )
		{
			pIndicate->SetMatrix( Matrix3::Trans( Pos ) );

			Pos.y += it->bBoss? INDICATE_HEIGHT_BOSS : INDICATE_HEIGHT;
		}
	}
}