#pragma once

#include "AttackTypes.h"
#include "TimeKey/TimeKey.h"
#include "Collide/CollideObjectListener.h"

class AttackData;
class AttackManager;
class CollideShape;
class CollideObject;
class CollideCaster;
class Effect;
class Enemy;

typedef TimeKey< ATTACK_TIME_KEY_PARAM, ATTACK_TIME_KEY_TYPE, ATTACK_TIME_KEY_TYPE_NUM >	AttackTimeKey;


////////////////////////////////////////////////////////////////////////////////
//
//	攻撃（基底）
//
////////////////////////////////////////////////////////////////////////////////
class Attack : public CollideObjectListener, AttackTimeKey::Owner
{
	REFERENCE_IMPL( Attack );

public:
	Attack( AttackManager* pManager );
	virtual ~Attack();

	void							Init( const AttackData* pData, const Matrix3& Mat );
	void							Update( void );

	void							Delete( void )									{ m_bDelete = true; }
	void							Erase( void );
	void							SetMatrix( const Matrix3& Mat );

	bool							IsDelete( void )						const	{ return m_bDelete; }
	const Matrix3&					GetMatrix( void )						const	{ return m_Matrix; }
	const AttackData*				GetData( void )							const	{ return m_pData; }

	virtual bool					IsAttach( void )						const	{ return false; }
	virtual bool					IsRepulse( void )						const	{ return false; }
	virtual bool					IsHide( void )							const	{ return false; }
	virtual int						GetEndFrame( void )						const;
	virtual void					SetOwner( Enemy* pEnemy )						{}

	virtual	void					OnNotify( ATTACK_NOTIFY Notify )				{}

	//	タイムキー
	virtual uint					GetTimeKeyNum( ATTACK_TIME_KEY_TYPE Type )													const;
	virtual const ATTACK_TIME_KEY*	GetTimeKey( ATTACK_TIME_KEY_TYPE Type, int Index )											const;
	virtual void					ApplyTimeKey( ATTACK_TIME_KEY_TYPE Type, const ATTACK_TIME_KEY_PARAM& Key );
	virtual void					ApplyTimeKey( ATTACK_TIME_KEY_TYPE Type, const ATTACK_TIME_KEY_PARAM& Key, const ATTACK_TIME_KEY_PARAM& Next, float f );

protected:
	virtual void					_OnInit( void )									= 0;
	virtual void					_OnUpdate( void )								= 0;
	virtual void					_OnEndFrame( void )								{}
	virtual void					_OnOutField( void )								{}

	virtual float					_GetOutRadius( void )					const	{ return 0.0f; }
	virtual CollideShape*			_CreateCollideShape( void )				const	{ return NULL; }
	virtual bool					_OnCastHit( const COLLIDE_RESULT& Result )		{ return false; }

	void							_SetHitRadius( float Rad );
	virtual void					_SetHitLength( float Len );
	Effect*							_AppearEffect( Label Name );
	Effect*							_AppearEffect( Label Name, const Matrix3& Mat );
	void							_AppearAttack( Label Name );
	void							_AppearAttack( Label Name, const Matrix3& Mat );
	void							_SetEffectColor( Color Col );
	void							_SetHitEnable( bool bEnable );

	Effect*							_GetEffect( void )								{ return m_pEffect; }
	CollideCaster*					_GetCollideCaster( void )						{ return m_pCaster; }
	int								_GetFrameCount( void )					const	{ return m_FrameCount; }
	Point							_GetHitVel( Point Normal )				const;

	virtual bool					IsHit( const COLLIDE_OWNER& Owner )		const;

protected:
	AttackManager*					m_pManager;
	const AttackData*				m_pData;

private:
	typedef std::set<const ATTACK_TIME_KEY_PARAM*>	KeyList;

private:
	bool							m_bDelete;
	int								m_FrameCount;

	RefPtr<Effect>					m_pEffect;
	CollideCaster*					m_pCaster;
	AttackTimeKey*					m_pTimeKey;
	Matrix3							m_Matrix;
	KeyList							m_ApplyKeyList;
};