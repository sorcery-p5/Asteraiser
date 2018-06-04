#include "stdafx.h"
#include "Attack.h"
#include "AttackManager.h"
#include "AttackData.h"

#include "App/App.h"
#include "World/World.h"

#include "Effect/Effect.h"
#include "Effect/EffectManager.h"

#include "Collide/CollideObject.h"
#include "Collide/CollideCaster.h"
#include "Collide/Shape/CollideShape.h"

#include "Player/Player.h"
#include "Item/ItemManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�U���i���j
//
////////////////////////////////////////////////////////////////////////////////
Attack::Attack( AttackManager* pManager )
{
	_ASSERT( pManager );

	m_pManager		= pManager;
	m_pData			= NULL;
	m_bDelete		= false;
	m_FrameCount	= 0;

	m_pCaster		= NULL;
	m_pTimeKey		= NULL;
}
Attack::~Attack()
{
	EffectEndAppear( m_pEffect );

	SafeDelete( m_pTimeKey );
	SafeDelete( m_pCaster );
}

//******************************************************************************
//	������
//******************************************************************************
void Attack::Init( const AttackData* pData, const Matrix3& Mat )
{
	_ASSERT( pData );

	//	�p�����[�^
	m_pData			= pData;
	m_FrameCount	= 0;
	m_Matrix		= Mat;

	//	�h����
	_OnInit();

	//	�G�t�F�N�g�����
	m_pEffect = _AppearEffect( m_pData->GetParam().EffectName, Mat );

	//	�����蔻������
	CollideShape* pCastShape = _CreateCollideShape();
	if( pCastShape )
	{
		m_pCaster = new CollideCaster;
		MEM_CHECK( m_pCaster );

		m_pCaster->Init( pCastShape );
		m_pCaster->SetCalcType( COLLIDE_CALC_POS_NORMAL );
		m_pCaster->SetOwner( COLLIDE_OWNER_ATTACK, this );
		m_pCaster->SetFilter( COLLIDE_FILTER_ATTACK );
		m_pCaster->SetListener( this );
	}

	//	�^�C���L�[
	m_pTimeKey = new AttackTimeKey( this );
	MEM_CHECK( m_pTimeKey );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void Attack::Update( void )
{
	if( IsDelete() ) return;

	//	�h����
	_OnUpdate();

	//	�����蔻����s
	if( m_pCaster )
	{
		m_pCaster->UpdateMatrix( &m_Matrix );

		COLLIDE_CAST_RESULT Result;
		if( m_pCaster->Cast( Result ) )
		{
			for( COLLIDE_RESULT_LIST::iterator it = Result.ResultList.begin(); it != Result.ResultList.end(); ++it )
			{
				if( !_OnCastHit( *it ) )
				{
					//	���蒆�f
					break;
				}
			}
		}
	}

	//	�^�C���L�[
	int TimeKeyLoopFrame = m_pData->GetParam().TimeKeyLoopFrame;
	int TimeKeyFrame = TimeKeyLoopFrame <= 0? m_FrameCount : m_FrameCount % TimeKeyLoopFrame;
	m_pTimeKey->Update( TimeKeyFrame );

	//	�t���[���J�E���g
	m_FrameCount++;

	//	�^�C���L�[�N���A
	if( TimeKeyLoopFrame > 0 && m_FrameCount % TimeKeyLoopFrame == 0 )
	{
		m_ApplyKeyList.clear();
		m_pTimeKey->Reset();
	}

	//	�I�����ԃ`�F�b�N
	if( GetEndFrame() &&
		m_FrameCount >= GetEndFrame() )
	{
		_OnEndFrame();
		Delete();
	}
	
	//	��ʊO�`�F�b�N
	float OutRad = _GetOutRadius();
	if( OutRad != 0.0f )
	{
		Rect FieldRect = m_pManager->GetWorld()->GetFieldRect();
		FieldRect.Widen( OutRad );
		if( !FieldRect.IsInside( m_Matrix.GetPos() ) )
		{
			_OnOutField();
			Delete();
		}
	}
}

//******************************************************************************
//	����
//******************************************************************************
void Attack::Erase( void )
{
	//	�A�C�e���o��
	m_pManager->GetWorld()->GetItemManager()->AppearItem( m_pData->GetParam().ItemNum, GetMatrix().GetPos(), m_pData->GetParam().ItemSpeed );

	//	�폜
	Delete();
}

//******************************************************************************
//	�}�g���N�X�ݒ�
//******************************************************************************
void Attack::SetMatrix( const Matrix3& Mat )
{
	m_Matrix = Mat;

	//	�G�t�F�N�g
	if( m_pEffect ) m_pEffect->SetMatrix( Mat );
}

//******************************************************************************
//	�I���t���[�����擾
//******************************************************************************
int Attack::GetEndFrame( void ) const
{
	return m_pData->GetParam().EndFrame;
}

//******************************************************************************
//	�^�C���L�[�̐��擾
//******************************************************************************
uint Attack::GetTimeKeyNum( ATTACK_TIME_KEY_TYPE Type ) const
{
	return m_pData->GetTimeKeyNum( Type );
}

//******************************************************************************
//	�^�C���L�[�p�����[�^�擾
//******************************************************************************
const ATTACK_TIME_KEY* Attack::GetTimeKey( ATTACK_TIME_KEY_TYPE Type, int Index ) const
{
	return m_pData->GetTimeKey( Type, Index );
}

//******************************************************************************
//	�^�C���L�[�p�����[�^�̓K�p�i�P�Ɓj
//******************************************************************************
void Attack::ApplyTimeKey( ATTACK_TIME_KEY_TYPE Type, const ATTACK_TIME_KEY_PARAM& Key )
{
	KeyList::iterator it = m_ApplyKeyList.find( &Key );
	if( it != m_ApplyKeyList.end() ) return;

	switch( Type )
	{
	case ATTACK_TIME_KEY_EFFECT:
		_AppearEffect( Label(Key.Appear.Name), Matrix3::RotZ(Key.Appear.Rot) * GetMatrix() );
		break;

	case ATTACK_TIME_KEY_ATTACK:
		_AppearAttack( Label(Key.Appear.Name), Matrix3::RotZ(Key.Appear.Rot) * GetMatrix() );
		break;
		
	case ATTACK_TIME_KEY_COLOR:
		_SetEffectColor( Color(Key.Col) );
		break;
		
	case ATTACK_TIME_KEY_HIT:
		_SetHitEnable( Key.bFlag );
		break;

	case ATTACK_TIME_KEY_RADIUS:
		_SetHitRadius( Key.Val[0] );
		break;

	case ATTACK_TIME_KEY_LENGTH:
		_SetHitLength( Key.Val[0] );
		break;

	default:
		return;
	}
	
	m_ApplyKeyList.insert( &Key );
}

//******************************************************************************
//	�^�C���L�[�p�����[�^�̓K�p�i��ԁj
//******************************************************************************
void Attack::ApplyTimeKey( ATTACK_TIME_KEY_TYPE Type, const ATTACK_TIME_KEY_PARAM& Key, const ATTACK_TIME_KEY_PARAM& Next, float f )
{
	switch( Type )
	{
	case ATTACK_TIME_KEY_COLOR:
		_SetEffectColor( Color::Lerp( Key.Col, Next.Col, f ) );
		break;
		
	case ATTACK_TIME_KEY_RADIUS:
		_SetHitRadius( Lerp( Key.Val[0], Next.Val[0], f ) );
		break;

	case ATTACK_TIME_KEY_LENGTH:
		_SetHitLength( Lerp( Key.Val[0], Next.Val[0], f ) );
		break;
	}
}

//------------------------------------------------------------------------------
//	�����蔻��̔��a��ύX
//------------------------------------------------------------------------------
void Attack::_SetHitRadius( float Rad )
{
	if( m_pCaster )
	{
		CollideShape* pShape = m_pCaster->GetShape();
		if( pShape ) pShape->SetRadius( Rad );
	}
}

//------------------------------------------------------------------------------
//	�����蔻��̒�����ύX
//------------------------------------------------------------------------------
void Attack::_SetHitLength( float Len )
{
	if( m_pCaster )
	{
		CollideShape* pShape = m_pCaster->GetShape();
		if( pShape ) pShape->SetLength( Len );
	}
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�̔���
//------------------------------------------------------------------------------
Effect* Attack::_AppearEffect( Label Name )
{
	return _AppearEffect( Name, GetMatrix() );
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�̔���
//------------------------------------------------------------------------------
Effect* Attack::_AppearEffect( Label Name, const Matrix3& Mat )
{
	if( Name.IsEmpty() ) return NULL;

	const EffectFile* pEffectFile = m_pData->GetEffectFile( Name );
	if( !pEffectFile ) return NULL;
	
	return GetApp()->GetEffectMng()->CreateEffect( pEffectFile, Mat );
}

//------------------------------------------------------------------------------
//	�U���̔���
//------------------------------------------------------------------------------
void Attack::_AppearAttack( Label Name )
{
	_AppearAttack( Name, GetMatrix() );
}

//------------------------------------------------------------------------------
//	�U���̔���
//------------------------------------------------------------------------------
void Attack::_AppearAttack( Label Name, const Matrix3& Mat )
{
	if( Name.IsEmpty() ) return;

	const AttackData* pData = m_pData->GetAttackData( Name );
	if( pData )
	{
		m_pManager->AddAttack( pData, Mat );
	}
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�̐F�ύX
//------------------------------------------------------------------------------
void Attack::_SetEffectColor( Color Col )
{
	if( m_pEffect )
	{
		m_pEffect->SetColor( Col );
	}
}

//------------------------------------------------------------------------------
//	�����蔻��̗L����ݒ�
//------------------------------------------------------------------------------
void Attack::_SetHitEnable( bool bEnable )
{
	if( m_pCaster ) m_pCaster->SetHitEnable( bEnable );
}

//------------------------------------------------------------------------------
//	�q�b�g���̔��������𓾂�
//------------------------------------------------------------------------------
Point Attack::_GetHitVel( Point Normal ) const
{
	switch( m_pData->GetParam().HitVelType )
	{
	case ATTACK_VEL_NORMAL:		return -Normal;
	case ATTACK_VEL_DIR:		return m_Matrix.GetAxisY();
	}
	return Point();
}

//------------------------------------------------------------------------------
//	�����蔻������邩
//------------------------------------------------------------------------------
bool Attack::IsHit( const COLLIDE_OWNER& Owner ) const
{
	switch( Owner.Type )
	{
	case COLLIDE_OWNER_PLAYER:
		{
			if( m_pData->GetParam().bNotPlayerHit ) return false;
			
			//	���G���
			Player* pPlayer = (Player*)Owner.pData;
			if( pPlayer->IsInvincible() ) return false;
		}
		return true;
	case COLLIDE_OWNER_BLADE:
	case COLLIDE_OWNER_SPEAR:	return !m_pData->GetParam().bNotAttackHit;
	case COLLIDE_OWNER_GROUND:	return !m_pData->GetParam().bNotGroundHit;
	}
	return true;
}