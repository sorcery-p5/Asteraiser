#include "stdafx.h"
#include "CollideShapeFrame.h"
#include "CollideShapePoint.h"
#include "CollideShapeLine.h"
#include "CollideShapeRect.h"

#include "../CollideCaster.h"

#include "SpriteFrame/SpriteFrame.h"


///////////////////////////////////////////////////////////////////////////////
//
//	������`��F�X�v���C�g�t���[��
//
///////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	������
//******************************************************************************
void CollideShapeFrame::Init( const SpriteFrame* pSprite )
{
	m_pSprite = pSprite;

	//	�`������o��
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

	//	�s��X�V
	UpdateMatrix();
}

//******************************************************************************
//	�s��̍X�V
//******************************************************************************
void CollideShapeFrame::UpdateMatrix( const Matrix3* pMat )
{
	_ASSERT( m_pSprite );

	//	�{�[���̍s��擾
	for( ShapeMap::iterator it = m_ShapeMap.begin(); it != m_ShapeMap.end(); ++it )
	{
		const Matrix3& Mat = m_pSprite->GetBoneMatrix( it->first );

		(it->second)->UpdateMatrix( &Mat );
	}

	_CalcAabb();
}

//******************************************************************************
//	������`��̃L���X�g
//******************************************************************************
bool CollideShapeFrame::ShapeCast( COLLIDE_RESULT& Result, const CollideCaster* pCaster, Point* pSolvePos )
{
	//	AABB�̃`�F�b�N
	if( !GetAabb().IsIntersect( pCaster->GetShape()->GetAabb() ) )
	{
		return false;
	}

	//	�e�{�[���Ɣ��肷��
	bool bRet = false;
	for( ShapeMap::iterator it = m_ShapeMap.begin(); it != m_ShapeMap.end(); ++it )
	{
		COLLIDE_RESULT Ret;
		if( (it->second)->ShapeCast( Ret, pCaster, pSolvePos ) )
		{
			//	�����̋߂��ق����̗p
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
//	�`��̕`��
//******************************************************************************
void CollideShapeFrame::DrawShape( Color Col ) const
{
	for( ShapeMap::const_iterator it = m_ShapeMap.begin(); it != m_ShapeMap.end(); ++it )
	{
		(it->second)->DrawShape( Col );
	}
}

//------------------------------------------------------------------------------
//	AABB�̍Čv�Z
//------------------------------------------------------------------------------
void CollideShapeFrame::_CalcAabb( void )
{
	Rect Aabb;

	if( !m_ShapeMap.empty() )
	{
		//	�e�V�F�C�v�̍��������
		Aabb.Set( FLT_MAX, FLT_MAX, FLT_MIN, FLT_MIN );
		for( ShapeMap::iterator it = m_ShapeMap.begin(); it != m_ShapeMap.end(); ++it )
		{
			Aabb = Rect::Union( Aabb, (it->second)->GetAabb() );
		}
	}

	_SetAabb( Aabb );
}
