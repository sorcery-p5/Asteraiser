#include "stdafx.h"
#include "Enemy.h"
#include "EnemyData.h"
#include "EnemyManager.h"
#include "EnemyIndicate.h"

#include "State/EnemyState.h"

#include "App/App.h"
#include "World/World.h"
#include "Config/Config.h"

#include "SpriteFrame/SpriteFrame.h"
#include "Effect/EffectManager.h"
#include "Effect/Effect.h"
#include "Effect/EffectHolder.h"
#include "Collide/CollideSystem.h"
#include "Collide/Shape/CollideShape.h"
#include "Collide/Shape/CollideShapePoint.h"

#include "Attack/Attack.h"
#include "Player/Player.h"
#include "Item/ItemManager.h"
#include "Score/ScoreManager.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G
//
////////////////////////////////////////////////////////////////////////////////
Enemy::Enemy( EnemyManager* pManager )
{
	_ASSERT( pManager );
	m_pManager	= pManager;

	m_pData				= NULL;
	m_pSprite			= NULL;
	m_pState			= NULL;
	m_pCollide			= NULL;
	m_pCaster			= NULL;
	m_pEffectHolder		= NULL;

	m_bDelete			= false;
	m_Life				= 0;
	m_Defense			= 1.0f;
	m_KnockbackCount	= 0;
	m_InvincibleCount	= 0;
	m_HideCount			= 0;
	m_bInvincible		= false;
	m_bHide				= false;
	m_bNoDamage			= false;
	m_bOutField			= false;
	m_bNoCount			= false;
	m_bRepulse			= false;

	m_bDamage			= false;
	m_bRepulseDamage	= false;
	m_bRulerHit			= false;
	m_bPause			= false;
	m_ComboNum			= 0;

	m_pAttachMatrix		= NULL;
}
Enemy::~Enemy()
{
	SafeDelete( m_pAttachMatrix );
	SafeDelete( m_pEffectHolder );
	SafeDelete( m_pState );
	SafeDelete( m_pCollide );
	SafeDelete( m_pCaster );
	SafeDelete( m_pSprite );
}

//******************************************************************************
//	������
//******************************************************************************
void Enemy::Init( const EnemyData* pData, const Matrix3& Mat, Label Name )
{
	_ASSERT( pData );

	m_pData		= pData;
	m_Name		= Name;
	m_Matrix	= Mat;
	m_Pos		= Mat.GetPos();
	m_Rot		= Mat.GetRotZ();
	m_Life		= m_pData->GetParam().Life;
	m_Defense	= m_pData->GetParam().Defense;
	m_bOutField	= m_pData->GetParam().bOutField;
	m_bNoCount	= m_pData->GetParam().bNoCount;
	m_bRepulse	= m_pData->GetParam().bRepulse;

	//	�X�v���C�g�쐬
	m_pSprite = new SpriteFrame;
	MEM_CHECK( m_pSprite );
	m_pSprite->Init( m_pData->GetSpriteFrameFile( m_pData->GetParam().SpriteName ), Mat );

	//	�X�e�[�g�쐬
	m_pState = new EnemyState( this );
	MEM_CHECK( m_pState );
	m_pState->Init();

	//	�����蔻�萶��
	m_pCollide		= new CollideObject;
	m_pCaster		= new CollideCaster;
	MEM_CHECK( m_pCollide && m_pCaster );

	CollideShape* pShape = NULL;
	if( m_pData->GetParam().bCollideFrame )	pShape = new CollideShapeFrame( m_pSprite );
	else									pShape = new CollideShapePoint( m_pData->GetParam().CollideRadius );

	m_pCollide->Init( pShape, COLLIDE_LAYER_ENEMY );
	m_pCollide->SetOwner( COLLIDE_OWNER_ENEMY, this );
	m_pCollide->SetHitEnable( m_pData->GetParam().bHitEnable );
	m_pCollide->SetSolveEnable( true );
	m_pCollide->SetListener( this );

	m_pCaster->Init( new CollideShapePoint( m_pData->GetParam().CollideRadius ) );
	m_pCaster->SetOwner( COLLIDE_OWNER_ENEMY, this );
	m_pCaster->SetFilter( COLLIDE_FILTER_ENEMY );
	m_pCaster->SetCalcType( COLLIDE_CALC_ALL );
	m_pCaster->SetHitEnable( m_pData->GetParam().bHitEnable );

	//	�G�t�F�N�g�Ǘ�
	m_pEffectHolder = new EffectHolder;
	MEM_CHECK( m_pEffectHolder );
}

//******************************************************************************
//	�X�V
//******************************************************************************
void Enemy::Update( void )
{
	if( IsDelete() ) return;

	//	�Ǐ]�̍X�V
	_UpdateAttachMatrix();

	if( !m_bRulerHit )
	{
		//	�X�e�[�g�X�V
		m_pState->Update();

		//	�ʒu
		m_Pos += m_Vel + m_MoveVel;

		//	��ʓ��ɔ[�߂�
		if( !IsOutField() )
		{
			Rect FieldRect = GetWorld()->GetFieldRect();
			m_Pos = FieldRect.Inside( m_Pos );
		}

		//	���x�̒���
		m_Vel *= m_pData->GetParam().Decel;
		m_MoveVel.Zero();

		//	�}�g���N�X���X�V
		m_Matrix = Matrix3::TransRotZ( m_Pos, m_Rot );
			
		//	�ʒm���N���A
		m_vNotify.clear();
	}
	else
	{
		m_bRulerHit = false;
	}

	//	�����蔻��
	_CastCollide();

	//	�q�b�g�G�t�F�N�g�̍X�V
	m_pEffectHolder->SetPause( m_bRulerHit );
	m_pEffectHolder->Update( NULL );

	//	�X�v���C�g�X�V
	m_pSprite->Pause( m_bPause || m_bRulerHit );
	m_pSprite->Update( &m_Matrix );

	//	������ʒu�X�V
	m_pCollide->UpdateMatrix( &m_Matrix );

	//	�U���̍X�V
	_UpdateLinkAttack();

	//	�_���[�W�t���O���N���A
	m_bDamage			= false;
	m_bRepulseDamage	= false;

	DecreaseZero( m_InvincibleCount, 1 );
	DecreaseZero( m_HideCount, 1 );

	//	���S�`�F�b�N
	if( m_Life <= 0 )
	{
		//	���[���[���͎��ȂȂ�
		if( !m_bRulerHit )
		{
			//	�����蔻�����
			if( m_pCollide ) m_pCollide->SetHitEnable( false );

			//	�X�e�[�g�ɒʒm
			m_pState->ChangeHandlerState( ENEMY_HANDLER_STATE_DEAD );
		}

		//	�F�̐ݒ�
		SafePtr( m_pSprite )->SetColor( m_pData->GetParam().DeadColor );
	}

	if( !m_bRulerHit )
	{
		//	�J�E���g�_�E��
		DecreaseZero( m_KnockbackCount, 1 );

		//	�R���{�J�E���g�����Z�b�g
		if( !IsKnockback() )
		{
			m_ComboNum = 0;
		}
	}
}

//******************************************************************************
//	�`��
//******************************************************************************
void Enemy::Draw( void )
{
	m_pSprite->Draw( GetWorld()->GetDrawOffset() );
}

//******************************************************************************
//	�_���[�W��^����
//******************************************************************************
float Enemy::Damage( float Damage, Point Vel, int Knockback, bool bRepulse, bool bAntiHide )
{
	//	���G
	if( IsInvincible() )
	{
		return -1.0f;
	}

	//	�_���[�W����
	if( IsNoDamage() )
	{
		return -1.0f;
	}

	//	�B�ꑮ��
	if( !bAntiHide && IsHide() )
	{
		return -1.0f;
	}

	//	�C���W�P�[�g�\��
	if( !m_pData->GetParam().bNoIndicate )
	{
		m_pManager->GetIndicate()->AddEnemy( this );
	}

	float Exhaust = Min( (float)m_Life, Damage );

	//	���_���[�W
	if( GetConfig()->GetDebugInfo().bPlayerHyperAttack )
	{
		Damage *= 100.0f;
	}

	//	�h��͂ɂ��y��
	Damage *= m_Defense;

	//	�_���[�W�󂯂�
	DecreaseZero( m_Life, (int)Damage );

	//	���S
	if( m_Life <= 0 && !m_bRulerHit )
	{
		m_Vel += Vel * m_pData->GetParam().ReduceVel;
	
		m_InvincibleCount	= 0;
		m_HideCount			= 0;
		m_KnockbackCount	= 0;
		m_ComboNum			= 0;
	
		//	�����蔻�����
		if( m_pCollide ) m_pCollide->SetHitEnable( false );

		//	�X�e�[�g�ɒʒm
		m_pState->ChangeHandlerState( ENEMY_HANDLER_STATE_DEAD );
	}
	//	����
	else if( IsRepulse() && bRepulse )
	{
		m_Vel.Zero();

		m_InvincibleCount	= 0;
		m_HideCount			= 0;
		m_KnockbackCount	= 0;
		m_ComboNum			= 0;
		m_bRepulseDamage	= true;

		//	�X�e�[�g�ɒʒm
		if( !m_pData->GetParam().bNotRepulseNotify )
		{
			m_pState->ChangeHandlerState( ENEMY_HANDLER_STATE_REPULSE );
		}
	}
	//	�ʏ�_���[�W
	else
	{
		m_Vel += Vel * m_pData->GetParam().ReduceVel;
		
		int	Invincible		= m_pData->GetParam().DamageInvincible;

		//	���[���[���͖��G���Ԍy��
		if( m_bRulerHit ) Invincible *= 0.4f;

		m_InvincibleCount	= Max( m_InvincibleCount, Invincible );
		m_KnockbackCount	= Max( m_KnockbackCount, (int)(Knockback * m_pData->GetParam().ReduceKnockback) );

		m_ComboNum++;
		
		//	�X�e�[�g�ɒʒm
		m_pState->ChangeHandlerState( ENEMY_HANDLER_STATE_DAMAGE );

		//	�U���ɒʒm
		_NotifyLinkAttack( ATTACK_NOTIFY_DAMAGE );
	}

	//	���Ճ��[�g�|����
	Exhaust *= m_pData->GetParam().ExhaustRate;

	return Exhaust;
}

//******************************************************************************
//	�U����o�^
//******************************************************************************
void Enemy::RegisterAttack( Attack* pAttack, Label Name, pstr pBone, bool bAttach )
{
	if( !pAttack || !pBone ) return;

	//	�G�ɓo�^
	pAttack->SetOwner( this );

	LINK_ATTACK LinkAttack;
	LinkAttack.pAttack	= pAttack;
	LinkAttack.Name		= Name;
	LinkAttack.Bone		= pBone;
	LinkAttack.bAttach	= bAttach;

	m_listLinkAttack.push_back( LinkAttack );
}

//******************************************************************************
//	�o�^���ꂽ�U�����폜
//******************************************************************************
void Enemy::DeleteLinkAttack( Label Name )
{
	for( LinkAttackList::iterator it = m_listLinkAttack.begin(); it != m_listLinkAttack.end(); ++it )
	{
		if( it->pAttack &&
			(Name.IsEmpty() || it->Name == Name ) )
		{
			it->pAttack->Delete();
		}
	}
}

//******************************************************************************
//	�U������̒ʒm
//******************************************************************************
void Enemy::OnAttackNotify( Label Notify )
{
	//	�V�[���h�j��
	if( Notify == ENEMY_ATTACK_NOTIFY_BREAK )
	{
		m_HideCount	= m_pData->GetParam().DamageInvincible;

		m_pState->ChangeHandlerState( ENEMY_HANDLER_STATE_BREAK );
		
		//	�C���W�P�[�g
		if( !m_pData->GetParam().bNoIndicate )
		{
			m_pManager->GetIndicate()->AddEnemy( this );
		}
	}
	//	�u���[�h�܂ꂽ
	else if( Notify == ENEMY_ATTACK_NOTIFY_CRASH )
	{
		m_HideCount			= m_pData->GetParam().DamageInvincible;
		m_KnockbackCount	= Max( m_KnockbackCount, (int)(m_pData->GetParam().CrashKnockback * m_pData->GetParam().ReduceKnockback) );
		
		//	���
		m_Vel += -m_Matrix.GetAxisY() * m_pData->GetParam().CrashVel;

		m_pState->ChangeHandlerState( ENEMY_HANDLER_STATE_CRASH );

		//	�C���W�P�[�g
		if( !m_pData->GetParam().bNoIndicate )
		{
			m_pManager->GetIndicate()->AddEnemy( this );
		}
	}
	//	����
	else if( Notify == ENEMY_ATTACK_NOTIFY_REPULSE )
	{
		m_pState->ChangeHandlerState( ENEMY_HANDLER_STATE_REPULSE );

		m_bRepulseDamage = true;
		
		//	�C���W�P�[�g
		if( !m_pData->GetParam().bNoIndicate )
		{
			m_pManager->GetIndicate()->AddEnemy( this );
		}
	}

	m_vNotify.push_back( Notify );
}

//******************************************************************************
//	�ʒm
//******************************************************************************
void Enemy::OnNotify( Label Notify )
{
	m_vNotify.push_back( Notify );

	//	�n���h���[�ʒm
	m_pState->ChangeHandlerState( Notify );
}

//******************************************************************************
//	�Ǐ]�Ώۂ�o�^
//******************************************************************************
void Enemy::SetAttach( Enemy* pTarget, Label Bone )
{
	if( !pTarget ) return;

	SafeDelete( m_pAttachMatrix );

	m_pAttachMatrix = new ATTACH_MATRIX;
	m_pAttachMatrix->pTarget	= pTarget;
	m_pAttachMatrix->Bone		= Bone;
	m_pAttachMatrix->PrevMat	= pTarget->GetBoneMatrix( Bone );
}

//******************************************************************************
//	�q�ɑ�������G�𐶐�����
//******************************************************************************
void Enemy::AddChildEnemy( Label Name, Label BoneName, const Matrix3& Mat, bool bAttach, Label AppearName )
{
	//	�G�𐶐�
	const EnemyData*	pEnemyData	= m_pData->GetEnemyData( Name );
	Matrix3				BaseMat		= Mat * GetBoneMatrix( BoneName );
	
	Enemy* pChildEnemy = m_pManager->AddEnemy( pEnemyData, BaseMat, AppearName );

	//	�e�q�o�^
	m_vChild.push_back( pChildEnemy );
	pChildEnemy->SetParent( this );

	//	�Ǐ]�ݒ�
	if( bAttach )
	{
		pChildEnemy->SetAttach( this, BoneName );
	}
}

//******************************************************************************
//	�e�֒ʒm
//******************************************************************************
void Enemy::NotifyParent( Label Name )
{
	SafePtr( m_pParent )->OnNotify( Name );
}

//******************************************************************************
//	�q�֒ʒm
//******************************************************************************
void Enemy::NotifyChild( Label AppearName, Label Name )
{
	for( EnemyVec::iterator it = m_vChild.begin(); it != m_vChild.end(); ++it )
	{
		if( !(*it) ) continue;

		if( AppearName.IsEmpty() ||
			(*it)->GetName() == AppearName )
		{
			(*it)->OnNotify( Name );
		}
	}
}

//******************************************************************************
//	�G�����S�ς݂��ǂ���
//******************************************************************************
bool Enemy::IsChildDefeat( Label Name ) const
{
	for( EnemyVec::const_iterator it = m_vChild.begin(); it != m_vChild.end(); ++it )
	{
		if( !(*it) ) continue;

		if( (*it)->GetName() == Name &&
			((*it)->IsDead() || (*it)->IsNoCount() ) )
		{
			return true;
		}
	}

	return false;
}

//******************************************************************************
//	���ׂĂ̓G�����S�ς݂��ǂ���
//******************************************************************************
bool Enemy::IsChildDefeatAll( void ) const
{
	for( EnemyVec::const_iterator it = m_vChild.begin(); it != m_vChild.end(); ++it )
	{
		if( !(*it) ) continue;

		if( !(*it)->IsNoCount() && !(*it)->IsDead() )
		{
			return false;
		}
	}
	return true;
}

//******************************************************************************
//	���ׂĂ̓G�����Ȃ����ǂ���
//******************************************************************************
bool Enemy::IsChildEmpty( void ) const
{
	for( EnemyVec::const_iterator it = m_vChild.begin(); it != m_vChild.end(); ++it )
	{
		if( !(*it)->IsNoCount() )
		{
			return false;
		}
	}
	return true;
}

//******************************************************************************
//	�ʒm�����邩�ǂ���
//******************************************************************************
bool Enemy::IsNotify( Label Notify ) const
{
	NotifyVec::const_iterator it = StdFind( m_vNotify, Notify );
	return it != m_vNotify.end();
}

//******************************************************************************
//	�G�t�F�N�g�쐬
//******************************************************************************
Effect* Enemy::CreateEffect( Label Name )
{
	if( Name.IsEmpty() ) return NULL;

	const EffectFile* pEffectFile = m_pData->GetEffectFile( Name );
	if( !pEffectFile ) return NULL;
	
	return GetApp()->GetEffectMng()->CreateEffect( pEffectFile, m_Matrix );
}

//******************************************************************************
//	�q�b�g�G�t�F�N�g�ǉ�
//******************************************************************************
void Enemy::AddHitEffect( Effect* pEffect )
{
	m_pEffectHolder->AddEffect( pEffect, false );
}

//******************************************************************************
//	�T�E���h���Đ�
//******************************************************************************
Sound* Enemy::PlaySound( Label Name, float Vol )
{
	const SoundFile* pFile = m_pData->GetSoundFile( Name );
	if( !pFile ) return NULL;

	Sound* pSound = GetTone()->PlaySound( pFile, SOUND_CHANNEL_SE, false, Vol );
	if( !pSound ) return NULL;

	return pSound;
}

//******************************************************************************
//	�j��
//******************************************************************************
void Enemy::Destroy( void )
{
	//	�A�C�e������
	GetWorld()->GetItemManager()->AppearItem( m_pData->GetParam().ItemNum, m_Matrix.GetPos(), m_pData->GetParam().ItemSpeed );
	
	//	�X�R�A�ʒm
	GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_ENEMY_DESTROY, GetScoreRate() );

	//	�폜
	Delete();	
}

//******************************************************************************
//	����
//******************************************************************************
void Enemy::Escape( void )
{
	//	�X�R�A�ʒm
	if( !IsNoCount() )
	{
		GetWorld()->GetScoreManager()->OnEvent( SCORE_EVENT_ENEMY_ESCAPE );
	}
	
	//	�폜
	Delete();
}

//******************************************************************************
//	�ϋv�͂̑���
//******************************************************************************
void Enemy::AddLife( int Value )
{
	m_Life = Clamp( m_Life + Value, 0, GetMaxLife() );

	//	�C���W�P�[�g�\��
	if( !m_pData->GetParam().bNoIndicate )
	{
		m_pManager->GetIndicate()->AddEnemy( this );
	}
}

//******************************************************************************
//	�����蔻��̗L���ݒ�
//******************************************************************************
void Enemy::SetHitEnable( bool bEnable )
{
	if( m_pCollide ) m_pCollide->SetHitEnable( bEnable );
	if( m_pCaster ) m_pCaster->SetHitEnable( bEnable );
}

//******************************************************************************
//	�n�`�Ƃ̓����蔻��̗L���ݒ�
//******************************************************************************
void Enemy::SetGroundHitEnable( bool bEnable )
{	
	if( m_pCaster ) m_pCaster->SetHitEnable( bEnable );
}

//******************************************************************************
//	�{�[���̃}�g���N�X���擾
//******************************************************************************
const Matrix3& Enemy::GetBoneMatrix( Label BoneName ) const
{
	if( m_pSprite )
	{
		const Matrix3* pMat = m_pSprite->GetBoneMatrix( BoneName );
		if( pMat )
		{
			return *pMat;
		}
	}
	return GetMatrix();
}

//******************************************************************************
//	�ő僉�C�t�擾
//******************************************************************************
int Enemy::GetMaxLife( void ) const
{
	return m_pData->GetParam().Life;
}

//******************************************************************************
//	�\���p�̖��O���擾
//******************************************************************************
pstr Enemy::GetPreviewName( void ) const
{
	return m_pData->GetParam().PreviewName.c_str();
}

//******************************************************************************
//	�����蔻�肪�L����
//******************************************************************************
bool Enemy::IsHitEnable( void ) const
{
	if( m_pCollide && m_pCollide->IsHitEnable() ) return true;
	if( m_pCaster && m_pCaster->IsHitEnable() ) return true;

	return false;
}

//******************************************************************************
//	���G���ǂ���
//******************************************************************************
bool Enemy::IsInvincible( void ) const
{
	if( m_bInvincible || m_InvincibleCount > 0 ) return true;

	return false;
}

//******************************************************************************
//	�B�ꑮ�����ǂ���
//******************************************************************************
bool Enemy::IsHide( void ) const
{
	if( m_bHide || m_HideCount > 0 ) return true;

	for( LinkAttackList::const_iterator it = m_listLinkAttack.begin(); it != m_listLinkAttack.end(); ++it )
	{
		//	��������U������B�ꑮ����T��
		if( it->pAttack )
		{
			if( it->pAttack->IsHide() ) return true;
		}
	}

	return false;
}

//******************************************************************************
//	�������邩�ǂ���
//******************************************************************************
bool Enemy::IsRepulse( void ) const
{
	return m_bRepulse && !m_bRulerHit;
}

//******************************************************************************
//	�{�X���ǂ���
//******************************************************************************
bool Enemy::IsBoss( void ) const
{
	return m_pData->GetParam().bBoss;
}

//******************************************************************************
//	�̓�����ɂ��U���͂𓾂�
//******************************************************************************
float Enemy::GetBodyDamage( void ) const
{
	return m_pData->GetParam().BodyDamage;
}

//******************************************************************************
//	�̓�����ɂ�锽���𓾂�
//******************************************************************************
float Enemy::GetBodyDamageVel( void ) const
{
	return m_pData->GetParam().BodyDamageVel;
}

//******************************************************************************
//	�X�R�A�{���𓾂�
//******************************************************************************
float Enemy::GetScoreRate( void ) const
{
	return m_pData->GetParam().ScoreRate;
}

//******************************************************************************
//	�ړ��ڕW�ʒu���擾
//******************************************************************************
const Point* Enemy::GetMoveTarget( void ) const
{
	if( !m_pState ) return NULL;

	return m_pState->GetMoveTarget();
}

//******************************************************************************
//	����ڕW�ʒu���擾
//******************************************************************************
const Point* Enemy::GetRotTarget( void ) const
{
	if( !m_pState ) return NULL;

	return m_pState->GetRotTarget();
}

//******************************************************************************
//	���[���h�擾
//******************************************************************************
World* Enemy::GetWorld( void ) const
{
	return m_pManager->GetWorld();
}

//******************************************************************************
//	���������̎擾
//******************************************************************************
Rand& Enemy::GetRand( void ) const
{
	return m_pManager->GetRand();
}

//******************************************************************************
//	������Ƀq�b�g���邩�ǂ������ׂ�
//******************************************************************************
bool Enemy::IsHit( const COLLIDE_OWNER& Owner ) const
{
	switch( Owner.Type )
	{
	case COLLIDE_OWNER_PLAYER:
		{
			Player* pPlayer = (Player*)(Owner.pData);
			if( pPlayer->IsInvincible() )			return false;
			if( IsInvincible() )					return false;
			if( IsHide() )							return false;
			if( !m_pData->GetParam().bPlayerHit )	return false;
		}
		break;
	}

	return true;
}

//------------------------------------------------------------------------------
//	�����蔻��̃L���X�g���s��
//------------------------------------------------------------------------------
void Enemy::_CastCollide( void )
{
	m_pCaster->UpdateMatrix( &m_Matrix );

	COLLIDE_CAST_RESULT Result;
	if( m_pCaster->Cast( Result ) )
	{
		for( COLLIDE_RESULT_LIST::iterator it = Result.ResultList.begin(); it != Result.ResultList.end(); ++it )
		{
			switch( it->pOwner->Type )
			{
			case COLLIDE_OWNER_RULER:
				{
					if( !IsNoDamage() )
					{
						m_bRulerHit = true;
					}
				}
				break;
			}
		}

		//	�n�`������̋���
		if( m_pData->GetParam().bGroundHit )
		{
			m_Matrix.SetPos( Result.SolvePos );
		}
	}
}

//------------------------------------------------------------------------------
//	�ʒu�Ǐ]�̍X�V
//------------------------------------------------------------------------------
void Enemy::_UpdateAttachMatrix( void )
{
	if( !m_pAttachMatrix ) return;
	
	//	�Ώۂ�����
	if( !m_pAttachMatrix->pTarget )
	{
		SafeDelete( m_pAttachMatrix );
		return;
	}

	//	���������
	const Matrix3& TargetMat = m_pAttachMatrix->pTarget->GetBoneMatrix( m_pAttachMatrix->Bone );
	Point RelPos = TargetMat.GetPos() - m_pAttachMatrix->PrevMat.GetPos();
	float RelRot = TargetMat.GetRotZ() - m_pAttachMatrix->PrevMat.GetRotZ();

	//	�ʒu���X�V
	m_Pos	+= RelPos;
	m_Rot	+= RelRot;
	
	//	�s��X�V
	m_Matrix = Matrix3::TransRotZ( m_Pos, m_Rot );
	m_pAttachMatrix->PrevMat = TargetMat;
}

//------------------------------------------------------------------------------
//	�U���̍X�V
//------------------------------------------------------------------------------
void Enemy::_UpdateLinkAttack( void )
{
	for( LinkAttackList::iterator it = m_listLinkAttack.begin(); it != m_listLinkAttack.end(); )
	{
		if( it->pAttack )
		{
			if( it->bAttach )
			{
				//	�ʒu�擾
				it->pAttack->SetMatrix( GetBoneMatrix( it->Bone ) );
			}

			++it;
		}
		else
		{
			//	�폜
			it = m_listLinkAttack.erase( it );
		}
	}
}

//------------------------------------------------------------------------------
//	�U���ɒʒm
//------------------------------------------------------------------------------
void Enemy::_NotifyLinkAttack( ATTACK_NOTIFY Notify )
{
	for( LinkAttackList::iterator it = m_listLinkAttack.begin(); it != m_listLinkAttack.end(); ++it )
	{
		if( it->pAttack )
		{
			it->pAttack->OnNotify( Notify );
		}
	}
}