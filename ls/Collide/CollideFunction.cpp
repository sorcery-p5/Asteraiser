#include "stdafx.h"

#include "CollideFunction.h"
#include "Shape/CollideShapeLine.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�����蔻��֐�
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	�_�Ɠ_�̓����蔻��
//------------------------------------------------------------------------------
bool CollideCalcPointToPoint( COLLIDE_RESULT& Result, Point PosA, float RadA, Point PosB, float RadB, dword CalcType, Point* pSolvePos )
{
	Point Dif = PosB - PosA;

	float Len = Dif.LengthSq();
	if( Len > (RadA + RadB) * (RadA + RadB) ) return false;

	if( CalcType != 0 )
	{
		if( Len == FLT_EPSILON )
		{
			Result.HitPos = PosA;
		}
		else
		{
			Len = Sqrt( Len );
			Result.Length = Len;

			//	������ʒu�̎Z�o
			if( CalcType & COLLIDE_CALC_POS )
			{
				Result.HitPos = PosA + Dif * ((Len-RadB) / Len);
			}
			
			//	�@���̎Z�o
			if( CalcType & COLLIDE_CALC_NORMAL )
			{
				Result.Normal = -(Dif / Len);
			}

			//	�߂荞�݉���
			if( pSolvePos &&
				CalcType & COLLIDE_CALC_SOLVE )
			{
				*pSolvePos = PosB - Dif * ((RadA + RadB) / Len);
			}
		}
	}

	return true;
}

//------------------------------------------------------------------------------
//	�_�Ɛ��̔���
//------------------------------------------------------------------------------
bool CollideCalcPointToLine( COLLIDE_RESULT& Result, Point Pos, float RadA, Point St, Point Ed, float Len, float RadB, Point Normal, dword CalcType, Point* pSolvePos )
{
	Point Dir		= Ed - St;
	
	float StDot		= Dir.Dot( Pos - St );
	float EdDot		= (-Dir).Dot( Pos - Ed );

	//	������ɂ���ꍇ
	if( StDot > FLT_EPSILON && EdDot > FLT_EPSILON )
	{
		float Dist		= Dir.Cross(Pos - St) / Len;
		float AbsDist	= Abs(Dist);

		if( AbsDist <= (RadA + RadB) )
		{
			Result.Length = AbsDist - RadB;

			if( CalcType != 0 )
			{
				//	�@���̎Z�o
				Result.Normal = Dist < 0.0f? Normal : -Normal;

				//	������ʒu�̎Z�o
				if( CalcType & COLLIDE_CALC_POS )
				{
					Result.HitPos = Pos - Result.Normal * Result.Length;
				}
				
				//	�߂荞�݉���
				if( pSolvePos &&
					CalcType & COLLIDE_CALC_SOLVE )
				{
					*pSolvePos = Pos + Result.Normal * (RadA + RadB - AbsDist);
				}
			}

			return true;
		}
	}
	//	�����̊O�ɂ���ꍇ
	else
	{
		//	�߂��ق��̒��_�Ɠ_����
		if( StDot < FLT_EPSILON )
		{
			//	�n�_�̗���
			if( CollideCalcPointToPoint( Result, Pos, RadA, St, RadB, CalcType, pSolvePos ) )
			{
				return true;
			}
		}
		else
		{
			//	�I�_�̗���
			if( CollideCalcPointToPoint( Result, Pos, RadA, Ed, RadB, CalcType, pSolvePos ) )
			{
				return true;
			}
		}
	}

	return false;
}

//------------------------------------------------------------------------------
//	���Ɠ_�̔���
//------------------------------------------------------------------------------
bool CollideCalcLineToPoint( COLLIDE_RESULT& Result, const CollideShapeLine* pShape, Point Pos, float Rad, dword CalcType )
{
	//	�n�_�̗���
	if( CollideCalcPointToPoint( Result, pShape->GetStart(), pShape->GetRadius(), Pos, Rad, CalcType ) )
	{
		Result.Length = -Result.Length;
		Result.HitPos = pShape->GetStart();
		return true;
	}

	Point Dir		= pShape->GetEnd() - pShape->GetStart();
	
	float StDot		= Dir.Dot( Pos - pShape->GetStart() );
	float EdDot		= (-Dir).Dot( Pos - pShape->GetEnd() );

	//	������ɂ���ꍇ
	if( StDot > FLT_EPSILON && EdDot > FLT_EPSILON )
	{
		float Dist = Dir.Cross(Pos - pShape->GetStart()) / pShape->GetLength();
		float AbsDist = Abs(Dist);

		if( AbsDist <= (pShape->GetRadius() + Rad) )
		{
			//	�������ړ�����ꍇ
			if( AbsDist <= Rad )
			{
				Result.Length = (StDot / pShape->GetLength()) - Sqrt(Rad*Rad - Dist*Dist);
				Result.HitPos = pShape->GetStart() + pShape->GetAxis() * Result.Length;
				Result.Normal = (Result.HitPos - Pos) / Rad;
			}
			else
			{
				Result.Length = (StDot / pShape->GetLength());
				Result.Normal = Dist < 0.0f? -pShape->GetNormal(): pShape->GetNormal();
				Result.HitPos = pShape->GetStart() + pShape->GetAxis() * Result.Length - Result.Normal * (AbsDist-Rad);
			}

			return true;
		}
	}
	//	�����̊O�ɂ���ꍇ
	else
	{
		//	�I�_�̗���
		if( CollideCalcPointToPoint( Result, pShape->GetEnd(), pShape->GetRadius(), Pos, Rad, CalcType ) )
		{
			Result.Length = pShape->GetLength() + Result.Length;
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------
//	���Ɛ��̔���
//------------------------------------------------------------------------------
bool CollideCalcLineToLine( COLLIDE_RESULT& Result, const CollideShapeLine* pShape, Point St, Point Ed, float Len, Point Normal, dword CalcType, bool bTipCast )
{
	Point CastDir = pShape->GetEnd() - pShape->GetStart();
	Point BaseDir = Ed - St;

	float CrossA1 = CastDir.Cross( St					- pShape->GetStart() ); 
	float CrossA2 = CastDir.Cross( Ed					- pShape->GetStart() );
	float CrossB1 = BaseDir.Cross( pShape->GetStart()	- St );
	float CrossB2 = BaseDir.Cross( pShape->GetEnd()		- St );

	//	�����������Ă��邩�H
	if(	CrossA1 * CrossA2 < -FLT_EPSILON && CrossB1 * CrossB2 < -FLT_EPSILON )
	{
		//	��_�𓾂�
		float d1 = Abs( CrossB1 ) / Len;
		float d2 = Abs( CrossB2 ) / Len;

		Result.Length = pShape->GetLength() * (d1 / (d1 + d2) );
		Result.HitPos = pShape->GetStart() + pShape->GetAxis() * Result.Length;
		Result.Normal = CrossB1 < 0.0f? Normal : -Normal;

		//	������ɂ���ꍇ
		if( BaseDir.Dot( Result.HitPos - St ) > FLT_EPSILON &&
			(-BaseDir).Dot( Result.HitPos - Ed ) > FLT_EPSILON )
		{
			return true;
		}
	}

	if( pShape->GetRadius() > FLT_EPSILON )
	{
		//	�n�_�ƏI�_�œ_�L���X�g
		if( CollideCalcPointToLine( Result, pShape->GetEnd(), pShape->GetRadius(), St, Ed, Len, 0.0f, Normal, CalcType ) )
		{
			Result.Length = pShape->GetLength() + Result.Length;
			return true;
		}
		if( CollideCalcPointToLine( Result, pShape->GetStart(), pShape->GetRadius(), St, Ed, Len, 0.0f, Normal, CalcType ) )
		{
			return true;
		}
			
		//	����̎n�_�E�I�_�ɐ��L���X�g
		if( bTipCast &&
			BaseDir.Dot( pShape->GetStart() - St ) < FLT_EPSILON )
		{
			//	�n�_�̕����߂�
			if( CollideCalcLineToPoint( Result, pShape, St, 0.0f, CalcType ) )
			{
				return true;
			}
			if( CollideCalcLineToPoint( Result, pShape, Ed, 0.0f, CalcType ) )
			{
				return true;
			}
		}
		else
		{
			//	�I�_�̕����߂�
			if( CollideCalcLineToPoint( Result, pShape, Ed, 0.0f, CalcType ) )
			{
				return true;
			}
			if( CollideCalcLineToPoint( Result, pShape, St, 0.0f, CalcType ) )
			{
				return true;
			}
		}
	}

	return false;
}
