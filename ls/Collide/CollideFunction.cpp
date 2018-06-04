#include "stdafx.h"

#include "CollideFunction.h"
#include "Shape/CollideShapeLine.h"


////////////////////////////////////////////////////////////////////////////////
//
//	当たり判定関数
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//	点と点の当たり判定
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

			//	当たり位置の算出
			if( CalcType & COLLIDE_CALC_POS )
			{
				Result.HitPos = PosA + Dif * ((Len-RadB) / Len);
			}
			
			//	法線の算出
			if( CalcType & COLLIDE_CALC_NORMAL )
			{
				Result.Normal = -(Dif / Len);
			}

			//	めり込み解決
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
//	点と線の判定
//------------------------------------------------------------------------------
bool CollideCalcPointToLine( COLLIDE_RESULT& Result, Point Pos, float RadA, Point St, Point Ed, float Len, float RadB, Point Normal, dword CalcType, Point* pSolvePos )
{
	Point Dir		= Ed - St;
	
	float StDot		= Dir.Dot( Pos - St );
	float EdDot		= (-Dir).Dot( Pos - Ed );

	//	線分上にある場合
	if( StDot > FLT_EPSILON && EdDot > FLT_EPSILON )
	{
		float Dist		= Dir.Cross(Pos - St) / Len;
		float AbsDist	= Abs(Dist);

		if( AbsDist <= (RadA + RadB) )
		{
			Result.Length = AbsDist - RadB;

			if( CalcType != 0 )
			{
				//	法線の算出
				Result.Normal = Dist < 0.0f? Normal : -Normal;

				//	当たり位置の算出
				if( CalcType & COLLIDE_CALC_POS )
				{
					Result.HitPos = Pos - Result.Normal * Result.Length;
				}
				
				//	めり込み解決
				if( pSolvePos &&
					CalcType & COLLIDE_CALC_SOLVE )
				{
					*pSolvePos = Pos + Result.Normal * (RadA + RadB - AbsDist);
				}
			}

			return true;
		}
	}
	//	線分の外にある場合
	else
	{
		//	近いほうの頂点と点判定
		if( StDot < FLT_EPSILON )
		{
			//	始点の裏側
			if( CollideCalcPointToPoint( Result, Pos, RadA, St, RadB, CalcType, pSolvePos ) )
			{
				return true;
			}
		}
		else
		{
			//	終点の裏側
			if( CollideCalcPointToPoint( Result, Pos, RadA, Ed, RadB, CalcType, pSolvePos ) )
			{
				return true;
			}
		}
	}

	return false;
}

//------------------------------------------------------------------------------
//	線と点の判定
//------------------------------------------------------------------------------
bool CollideCalcLineToPoint( COLLIDE_RESULT& Result, const CollideShapeLine* pShape, Point Pos, float Rad, dword CalcType )
{
	//	始点の裏側
	if( CollideCalcPointToPoint( Result, pShape->GetStart(), pShape->GetRadius(), Pos, Rad, CalcType ) )
	{
		Result.Length = -Result.Length;
		Result.HitPos = pShape->GetStart();
		return true;
	}

	Point Dir		= pShape->GetEnd() - pShape->GetStart();
	
	float StDot		= Dir.Dot( Pos - pShape->GetStart() );
	float EdDot		= (-Dir).Dot( Pos - pShape->GetEnd() );

	//	線分上にある場合
	if( StDot > FLT_EPSILON && EdDot > FLT_EPSILON )
	{
		float Dist = Dir.Cross(Pos - pShape->GetStart()) / pShape->GetLength();
		float AbsDist = Abs(Dist);

		if( AbsDist <= (pShape->GetRadius() + Rad) )
		{
			//	線が直接当たる場合
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
	//	線分の外にある場合
	else
	{
		//	終点の裏側
		if( CollideCalcPointToPoint( Result, pShape->GetEnd(), pShape->GetRadius(), Pos, Rad, CalcType ) )
		{
			Result.Length = pShape->GetLength() + Result.Length;
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------
//	線と線の判定
//------------------------------------------------------------------------------
bool CollideCalcLineToLine( COLLIDE_RESULT& Result, const CollideShapeLine* pShape, Point St, Point Ed, float Len, Point Normal, dword CalcType, bool bTipCast )
{
	Point CastDir = pShape->GetEnd() - pShape->GetStart();
	Point BaseDir = Ed - St;

	float CrossA1 = CastDir.Cross( St					- pShape->GetStart() ); 
	float CrossA2 = CastDir.Cross( Ed					- pShape->GetStart() );
	float CrossB1 = BaseDir.Cross( pShape->GetStart()	- St );
	float CrossB2 = BaseDir.Cross( pShape->GetEnd()		- St );

	//	線が交差しているか？
	if(	CrossA1 * CrossA2 < -FLT_EPSILON && CrossB1 * CrossB2 < -FLT_EPSILON )
	{
		//	交点を得る
		float d1 = Abs( CrossB1 ) / Len;
		float d2 = Abs( CrossB2 ) / Len;

		Result.Length = pShape->GetLength() * (d1 / (d1 + d2) );
		Result.HitPos = pShape->GetStart() + pShape->GetAxis() * Result.Length;
		Result.Normal = CrossB1 < 0.0f? Normal : -Normal;

		//	線分上にある場合
		if( BaseDir.Dot( Result.HitPos - St ) > FLT_EPSILON &&
			(-BaseDir).Dot( Result.HitPos - Ed ) > FLT_EPSILON )
		{
			return true;
		}
	}

	if( pShape->GetRadius() > FLT_EPSILON )
	{
		//	始点と終点で点キャスト
		if( CollideCalcPointToLine( Result, pShape->GetEnd(), pShape->GetRadius(), St, Ed, Len, 0.0f, Normal, CalcType ) )
		{
			Result.Length = pShape->GetLength() + Result.Length;
			return true;
		}
		if( CollideCalcPointToLine( Result, pShape->GetStart(), pShape->GetRadius(), St, Ed, Len, 0.0f, Normal, CalcType ) )
		{
			return true;
		}
			
		//	相手の始点・終点に線キャスト
		if( bTipCast &&
			BaseDir.Dot( pShape->GetStart() - St ) < FLT_EPSILON )
		{
			//	始点の方が近い
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
			//	終点の方が近い
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
