#include "stdafx.h"
#include "SpriteFrameUtil.h"


////////////////////////////////////////////////////////////////////////////////
//
//	スプライトフレーム：ユーティリティ
//
////////////////////////////////////////////////////////////////////////////////

bool SpriteFrameUtil::s_bDrawDummyBone		= false;

//******************************************************************************
//	ダミーボーンの描画設定
//******************************************************************************
void SpriteFrameUtil::SetDrawDummyBone( bool bEnable )
{
	s_bDrawDummyBone = bEnable;
}

//******************************************************************************
//	ダミーボーンの描画をするか
//******************************************************************************
bool SpriteFrameUtil::IsDrawDummyBone( void )
{
	return s_bDrawDummyBone;
}