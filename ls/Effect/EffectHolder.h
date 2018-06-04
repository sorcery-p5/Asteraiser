#pragma once

#include "EffectTypes.h"

class Effect;


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクトをまとめて持つ
//
////////////////////////////////////////////////////////////////////////////////
class EffectHolder
{
public:
	EffectHolder( void );
	~EffectHolder();

	void						Update( const Matrix3* pMat );
	void						SetPause( bool bPause );
	void						SetColor( Color Col );

	void						AddEffect( Effect* pEffect, bool bAttach );

private:
	struct EFFECT
	{
		RefPtr<Effect>			pEffect;
		bool					bAttach;
	};
	typedef std::list<EFFECT>	EffectList;

private:
	EffectList					m_EffectList;
};
