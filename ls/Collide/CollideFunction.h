#pragma once

#include "CollideTypes.h"

class CollideShapeLine;

////////////////////////////////////////////////////////////////////////////////
//
//	�����蔻��֐�
//
////////////////////////////////////////////////////////////////////////////////

//	�_�Ɠ_�̔���
bool CollideCalcPointToPoint( COLLIDE_RESULT& Result, Point PosA, float RadA, Point PosB, float RadB, dword CalcType, Point* pSolvePos = NULL );

//	�_�Ɛ��̔���
bool CollideCalcPointToLine( COLLIDE_RESULT& Result, Point Pos, float RadA, Point St, Point Ed, float Len, float RadB, Point Normal, dword CalcType, Point* pSolvePos = NULL );

//	���Ɠ_�̔���
bool CollideCalcLineToPoint( COLLIDE_RESULT& Result, const CollideShapeLine* pShape, Point Pos, float Rad, dword CalcType );

//	���Ɛ��̔���
bool CollideCalcLineToLine( COLLIDE_RESULT& Result, const CollideShapeLine* pShape, Point St, Point Ed, float Len, Point Normal, dword CalcType, bool bTipCast = false );