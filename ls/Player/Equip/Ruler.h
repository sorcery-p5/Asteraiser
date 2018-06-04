#pragma once

#include "../PlayerTypes.h"
#include "Collide/CollideTypes.h"

class Player;
class Effect;
class CollideObject;


////////////////////////////////////////////////////////////////////////////////
//
//	オーヴァドライヴ・ルーラー
//
////////////////////////////////////////////////////////////////////////////////
class Ruler
{
public:
	Ruler( Player* pParent );
	~Ruler();

	void					Init( const RULER_PARAM* pParam, const Matrix3& Mat, float AddEnergy );
	void					Update( void );
	
	void					Crash( void );
	void					Delete( void )											{ m_bDelete = true; }

	bool					IsDelete( void )								const	{ return m_bDelete; }
	const Matrix3&			GetMatrix( void )								const	{ return m_Matrix; }

private:
	void					_UpdateIndicate( void );

private:
	Player*					m_pParent;
	const RULER_PARAM*		m_pParam;

	CollideObject*			m_pCollide;
	RefPtr<Effect>			m_pEffect;

	bool					m_bDelete;
	Matrix3					m_Matrix;
	float					m_Radius;
	int						m_Time;
	int						m_FrameCount;
};