#pragma once

#include "EnemyTypes.h"
#include "Attack/AttackTypes.h"
#include "Collide/CollideObjectListener.h"

class World;
class EnemyManager;
class EnemyData;
class EnemyState;
class Attack;
class SpriteFrame;
class Effect;
class EffectHolder;
class CollideObject;
class CollideCaster;


////////////////////////////////////////////////////////////////////////////////
//
//	“G
//
////////////////////////////////////////////////////////////////////////////////
class Enemy : public CollideObjectListener
{
	REFERENCE_IMPL( Enemy );

public:
	Enemy( EnemyManager* pManager );
	~Enemy();

	void				Init( const EnemyData* pData, const Matrix3& Mat, Label Name );
	void				Update( void );
	void				Draw( void );

	float				Damage( float Danmage, Point Vel, int Knockback, bool bRepulse, bool bAntiHide );
	void				RegisterAttack( Attack* pAttack, Label Name, pstr pBone, bool bAttach );
	void				DeleteLinkAttack( Label Name );
	void				OnAttackNotify( Label Notify );
	void				OnNotify( Label Notify );
	bool				IsNotify( Label Notify )						const;

	void				SetAttach( Enemy* pTarget, Label Bone );
	void				AddChildEnemy( Label Name, Label BoneName, const Matrix3& Mat, bool bAttach, Label AppearName );

	void				SetParent( Enemy* pParent )								{ m_pParent = pParent; }
	void				SetChild( Enemy* pChild )								{ m_vChild.push_back( pChild ); }
	void				NotifyParent( Label Name );
	void				NotifyChild( Label AppearName, Label Name );
	bool				IsChildDefeat( Label Name )						const;
	bool				IsChildDefeatAll( void )						const;
	bool				IsChildEmpty( void )							const;

	Effect*				CreateEffect( Label Name );
	void				AddHitEffect( Effect* pEffect );
	Sound*				PlaySound( Label Name, float Vol );

	void				Delete( void )											{ m_bDelete = true; }
	void				Destroy( void );
	void				Escape( void );
	void				SetPos( Point Pos )										{ m_Pos = Pos; }
	void				SetRot( float Rot )										{ m_Rot = Rot; }
	void				AddVelocity( Point Vel )								{ m_Vel += Vel; }
	void				AddMoveVelocity( Point Vel )							{ m_MoveVel += Vel; }
	void				AddLife( int Value );
	void				AddDefense( float Value )								{ m_Defense += Value; }
	void				SetHitEnable( bool bEnable );
	void				SetGroundHitEnable( bool bEnable );
	void				SetInvincible( bool bInvincible )						{ m_bInvincible = bInvincible; }
	void				SetHide( bool bHide )									{ m_bHide = bHide; }
	void				SetNoDamage( bool bNoDamage )							{ m_bNoDamage = bNoDamage; }
	void				SetOutField( bool bOutField )							{ m_bOutField = bOutField; }
	void				SetNoCount( bool bNoCount )								{ m_bNoCount = bNoCount; }
	void				SetRepulse( bool bRepulse )								{ m_bRepulse = bRepulse; }
	void				SetPause( bool bPause )									{ m_bPause = bPause; }

	bool				IsDelete( void )								const	{ return m_bDelete; }
	SpriteFrame*		GetSprite( void )								const	{ return m_pSprite; }
	const EnemyData*	GetData( void )									const	{ return m_pData; }
	Label				GetName( void )									const	{ return m_Name; }
	Point				GetPos( void )									const	{ return m_Pos; }
	float				GetRot( void )									const	{ return m_Rot; }
	Point				GetDir( void )									const	{ return m_Rot.GetDir(); }
	const Matrix3&		GetMatrix( void )								const	{ return m_Matrix; }
	const Matrix3&		GetBoneMatrix( Label BoneName )					const;

	int					GetLife( void )									const	{ return m_Life; }
	int					GetMaxLife( void )								const;
	pstr				GetPreviewName( void )							const;
	bool				IsDamage( void )								const	{ return m_bDamage; }
	bool				IsDead( void )									const	{ return m_Life <= 0 && !m_bRulerHit; }
	bool				IsKnockback( void )								const	{ return m_KnockbackCount > 0; }
	bool				IsRepulseDamage( void )							const	{ return m_bRepulseDamage; }
	bool				IsRulerHit( void )								const	{ return m_bRulerHit; }
	bool				IsHitEnable( void )								const;
	bool				IsInvincible( void )							const;
	bool				IsHide( void )									const;
	bool				IsNoDamage( void )								const	{ return m_bNoDamage; }
	bool				IsOutField( void )								const	{ return m_bOutField; }
	bool				IsRepulse( void )								const;
	bool				IsNoCount( void )								const	{ return m_bNoCount; }
	bool				IsBoss( void )									const;
	float				GetBodyDamage( void )							const;
	float				GetBodyDamageVel( void )						const;
	float				GetScoreRate( void )							const;
	int					GetComboNum( void )								const	{ return m_ComboNum; }

	const Point*		GetMoveTarget( void )							const;
	const Point*		GetRotTarget( void )							const;

	EnemyManager*		GetManager( void )								const	{ return m_pManager; }
	World*				GetWorld( void )								const;
	Rand&				GetRand( void )									const;

protected:
	virtual bool		IsHit( const COLLIDE_OWNER& Owner )				const;

private:
	struct LINK_ATTACK
	{
		RefPtr<Attack>		pAttack;
		Label				Name;
		Label				Bone;
		bool				bAttach;
	};
	struct ATTACH_MATRIX
	{
		RefPtr< Enemy >		pTarget;
		Label				Bone;
		Matrix3				PrevMat;
	};
	typedef std::list< LINK_ATTACK >		LinkAttackList;
	typedef std::vector< Label >			NotifyVec;
	typedef std::vector< RefPtr<Enemy> >	EnemyVec;	

private:
	void				_CastCollide( void );
	void				_UpdateAttachMatrix( void );
	void				_ApplyAttachMatrix( void );
	void				_UpdateLinkAttack( void );
	void				_NotifyLinkAttack( ATTACK_NOTIFY Notify );

private:
	EnemyManager*		m_pManager;
	const EnemyData*	m_pData;

	SpriteFrame*		m_pSprite;
	EnemyState*			m_pState;
	CollideObject*		m_pCollide;
	CollideCaster*		m_pCaster;

	bool				m_bDelete;
	Label				m_Name;
	Matrix3				m_Matrix;
	Point				m_MoveVel;
	Point				m_Vel;
	Point				m_Pos;
	Angle				m_Rot;

	int					m_Life;
	float				m_Defense;
	int					m_KnockbackCount;
	int					m_InvincibleCount;
	int					m_HideCount;
	bool				m_bInvincible;
	bool				m_bHide;
	bool				m_bNoDamage;
	bool				m_bOutField;
	bool				m_bNoCount;
	bool				m_bRepulse;

	bool				m_bDamage;
	bool				m_bRepulseDamage;
	bool				m_bRulerHit;
	bool				m_bPause;
	int					m_ComboNum;

	LinkAttackList		m_listLinkAttack;
	NotifyVec			m_vNotify;
	EffectHolder*		m_pEffectHolder;

	ATTACH_MATRIX*		m_pAttachMatrix;
	RefPtr<Enemy>		m_pParent;
	EnemyVec			m_vChild;
};