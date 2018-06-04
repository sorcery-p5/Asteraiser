#pragma once

#include "GroundTypes.h"
#include "Collide/CollideObjectListener.h"

class World;
class GroundManager;
class GroundData;
class SpriteFrame;
class CollideObject;
class EffectFile;


////////////////////////////////////////////////////////////////////////////////
//
//	ínå`
//
////////////////////////////////////////////////////////////////////////////////
class Ground : public CollideObjectListener
{
public:
	Ground( GroundManager* pManager );
	~Ground();

	void				Init( const GroundData* pData, const Matrix3& Mat, Label Name, bool bNoStart );
	void				Update( void );
	void				Draw( void );

	void				Delete( void )											{ m_bDelete = true; }
	void				End( void );
	void				PushAnime( Label Name );
	
	void				CreateHitEffect( const Matrix3& Mat )			const;

	bool				IsDelete( void )								const	{ return m_bDelete; }
	SpriteFrame*		GetSprite( void )								const	{ return m_pSprite; }
	Label				GetName( void )									const	{ return m_Name; }
	Point				GetPos( void )									const	{ return m_Pos; }
	float				GetRot( void )									const	{ return m_Rot; }
	bool				IsAttackErase( void )							const;
	bool				IsAttackHitEffect( void )						const;

	GroundManager*		GetManager( void )								const	{ return m_pManager; }
	World*				GetWorld( void )								const;

protected:
	virtual bool		IsHit( const COLLIDE_OWNER& Owner )				const;

private:
	enum STATE
	{
		STATE_START,
		STATE_MAIN,
		STATE_END,
	};
	typedef std::deque< Label >	LabelQueue;

private:
	void				_UpdateState( void );

private:
	GroundManager*		m_pManager;
	const GroundData*	m_pData;
	
	SpriteFrame*		m_pSprite;
	CollideObject*		m_pCollide;

	bool				m_bDelete;
	STATE				m_State;
	Label				m_Name;
	Matrix3				m_Matrix;
	Point				m_Pos;
	Angle				m_Rot;
	int					m_FrameCount;
	bool				m_bEnd;

	LabelQueue			m_PushAnimeList;
};