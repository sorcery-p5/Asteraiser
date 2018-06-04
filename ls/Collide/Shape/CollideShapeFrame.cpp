#include "stdafx.h"
#include "CollideShapeFrame.h"
#include "CollideShapePoint.h"
#include "CollideShapeLine.h"
#include "CollideShapeRect.h"

#include "../CollideCaster.h"

#include "SpriteFrame/SpriteFrame.h"


///////////////////////////////////////////////////////////////////////////////
//
//	当たり形状：スプライトフレーム
//
///////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	初期化
//******************************************************************************
void CollideShapeFrame::Init( const SpriteFrame* pSprite )
{
	m_pSprite = pSprite;

	//	形状を取り出し
	for( int BoneID = 0; BoneID < pSprite->GetBoneNum(); BoneID++ )
	{
		SPRITE_FRAME_COLLIDE Collide = pSprite->GetCollide( BoneID );
		
		switch( Collide.Type )
		{
		case SPRITE_FRAME_COLLIDE_POINT:
			m_ShapeMap.insert( ShapeMap::value_type( BoneID, new CollideShapePoint(Collide.Point.Radius) ) );
			break;
		case SPRITE_FRAME_COLLIDE_LINE:
			m_ShapeMap.insert( ShapeMap::value_type( BoneID, new CollideShapeLine(Collide.Line.Length, Collide.Line.Width) ) );
			break;
		case SPRITE_FRAME_COLLIDE_RECT:
			m_ShapeMap.insert( ShapeMap::value_type( BoneID, new CollideShapeRect(Rect(Collide.Rect.L,Collide.Rect.T,Collide.Rect.R,Collide.Rect.B)) ) );
			break;
		}
	}

	//	行列更新
	UpdateMatrix();
}

//******************************************************************************
//	行列の更新
//******************************************************************************
void CollideShapeFrame::UpdateMatrix( const Matrix3* pMat )
{
	_ASSERT( m_pSprite );

	//	ボーンの行列取得
	for( ShapeMap::iterator it = m_ShapeMap.begin(); it != m_ShapeMap.end(); ++it )
	{
		const Matrix3& Mat = m_pSprite->GetBoneMatrix( it->first );

		(it->second)->UpdateMatrix( &Mat );
	}

	_CalcAabb();
}

//******************************************************************************
//	当たり形状のキャスト
//******************************************************************************
bool CollideShapeFrame::ShapeCast( COLLIDE_RESULT& Result, const CollideCaster* pCaster, Point* pSolvePos )
{
	//	AABBのチェック
	if( !GetAabb().IsIntersect( pCaster->GetShape()->GetAabb() ) )
	{
		return false;
	}

	//	各ボーンと判定する
	bool bRet = false;
	for( ShapeMap::iterator it = m_ShapeMap.begin(); it != m_ShapeMap.end(); ++it )
	{
		COLLIDE_RESULT Ret;
		if( (it->second)->ShapeCast( Ret, pCaster, pSolvePos ) )
		{
			//	距離の近いほうを採用
			if( Ret < Result )
			{
				Result = Ret;
				Result.BoneID = (it->first);
			}

			bRet = true;
		}
	}

	return bRet;
}

//******************************************************************************
//	形状の描画
//******************************************************************************
void CollideShapeFrame::DrawShape( Color Col ) const
{
	for( ShapeMap::const_iterator it = m_ShapeMap.begin(); it != m_ShapeMap.end(); ++it )
	{
		(it->second)->DrawShape( Col );
	}
}

//------------------------------------------------------------------------------
//	AABBの再計算
//------------------------------------------------------------------------------
void CollideShapeFrame::_CalcAabb( void )
{
	Rect Aabb;

	if( !m_ShapeMap.empty() )
	{
		//	各シェイプの合同を取る
		Aabb.Set( FLT_MAX, FLT_MAX, FLT_MIN, FLT_MIN );
		for( ShapeMap::iterator it = m_ShapeMap.begin(); it != m_ShapeMap.end(); ++it )
		{
			Aabb = Rect::Union( Aabb, (it->second)->GetAabb() );
		}
	}

	_SetAabb( Aabb );
}
