#include "stdafx.h"
#include "EffectTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�G�t�F�N�g�^��`
//
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	�G�t�F�N�g�p�����[�^
//******************************************************************************
EFFECT_PARAM::EFFECT_PARAM( void )
{
	bLoop		= false;
	EndFrame	= 0;
}

//******************************************************************************
//	�p�[�e�B�N���p�����[�^
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
//	�폜
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
