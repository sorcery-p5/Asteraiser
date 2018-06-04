#pragma once

#include "EffectTypes.h"

class Effect;
class EffectFile;
class EffectResource;


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト管理
//
////////////////////////////////////////////////////////////////////////////////
class EffectManager
{
public:
	EffectManager( void );
	~EffectManager();

	void			Init( void );
	void			Update( void );
	void			Draw( void )	const;

	Effect*			CreateEffect( const EffectFile* pFile, Point Pos );
	Effect*			CreateEffect( const EffectFile* pFile, const Matrix3& Mat );
	void			DeleteAll( void );

	void			SetDrawOffset( const Matrix3& Mat )				{ m_DrawOffset		= Mat; }
	void			SetUpdateEnable( bool bEnable )					{ m_bUpdateEnable	= bEnable; }
	void			SetDrawEnable( bool bEnable )					{ m_bDrawEnable		= bEnable; }

	Point			GetForceValue( const Point& Pos )	const		{ return Point(); }
	float			GetGravity( void )					const		{ return 1.0f; }

	Rand&			GetRand( void )									{ return m_Rand; }

private:
	typedef	std::list< Effect* >			EffectList;

private:
	EffectList		m_EffectList;
	EffectList		m_AddEffectList;

	Matrix3			m_DrawOffset;
	XORRand			m_Rand;
	
	bool			m_bUpdateEnable;
	bool			m_bDrawEnable;
};