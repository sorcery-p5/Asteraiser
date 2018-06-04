#pragma once

#include "CollideTypes.h"

class CollideShapeLine;

////////////////////////////////////////////////////////////////////////////////
//
//	当たり判定関数
//
////////////////////////////////////////////////////////////////////////////////

//	点と点の判定
bool CollideCalcPointToPoint( COLLIDE_RESULT& Result, Point PosA, float RadA, Point PosB, float RadB, dword CalcType, Point* pSolvePos = NULL );

//	点と線の判定
bool CollideCalcPointToLine( COLLIDE_RESULT& Result, Point Pos, float RadA, Point St, Point Ed, float Len, float RadB, Point Normal, dword CalcType, Point* pSolvePos = NULL );

//	線と点の判定
bool CollideCalcLineToPoint( COLLIDE_RESULT& Result, const CollideShapeLine* pShape, Point Pos, float Rad, dword CalcType );

//	線と線の判定
bool CollideCalcLineToLine( COLLIDE_RESULT& Result, const CollideShapeLine* pShape, Point St, Point Ed, float Len, Point Normal, dword CalcType, bool bTipCast = false );