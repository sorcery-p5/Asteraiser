#include "stdafx.h"
#include "EffectTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	エフェクト型定義
//
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	エフェクトパラメータ
//******************************************************************************
EFFECT_PARAM::EFFECT_PARAM( void )
{
	bLoop		= false;
	EndFrame	= 0;
}

//******************************************************************************
//	パーティクルパラメータ
//******************************************************************************
EFFECT_PARTICLE::EFFECT_PARTICLE( void )
{
	EndFrame	= 0;

	Type		= EFFECT_PARTICLE_SPRITE;
	Blend		= BLEND_NONE;
	Phase		= 0;

	pSprite		= NULL;
	pVelocity	= NULL;
}

//------------------------------------------------------------------------------
//	削除
//------------------------------------------------------------------------------
EFFECT_PARTICLE::~EFFECT_PARTICLE( void )
{
	switch( Type )
	{
	case EFFECT_PARTICLE_SPRITE:		SafeDelete(pSprite);		break;
	case EFFECT_PARTICLE_SPRITE_FRAME:	SafeDelete(pSpriteFrame	);	break;
	case EFFECT_PARTICLE_LINE:			SafeDelete(pLine);			break;
	case EFFECT_PARTICLE_FONT:			SafeDelete(pFont);			break;
	case EFFECT_PARTICLE_SOUND:			SafeDelete(pSound);			break;
	case EFFECT_PARTICLE_EFFECT:		SafeDelete(pEffect);		break;
	}
	SafeDelete(pVelocity);
}
