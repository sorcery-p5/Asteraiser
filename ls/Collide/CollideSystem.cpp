#include "stdafx.h"
#include "CollideSystem.h"
#include "CollideObject.h"
#include "CollideObjectListener.h"
#include "CollideCaster.h"
#include "Shape/CollideShape.h"


///////////////////////////////////////////////////////////////////////////////
//
//	当たりシステム
//
///////////////////////////////////////////////////////////////////////////////
CollideSystem::CollideSystem( void )
{
	m_bDrawShape = false;	
}

CollideSystem::~CollideSystem()
{

}

//*****************************************************************************
//	更新
//*****************************************************************************
void CollideSystem::Update( void )
{
	for( uint i = 0; i < COLLIDE_LAYER_NUM; i++ )
	{
		ObjectList& List = m_aObjectList[i];

		for( ObjectList::iterator it = List.begin(); it != List.end(); ++it )
		{
			(*it)->Update();
		}
	}
}

//*****************************************************************************
//	当たりオブジェクト追加
//*****************************************************************************
void CollideSystem::AddObject( CollideObject* pObject )
{
	_ASSERT( pObject && pObject->GetShape() );

	//	レイヤー取得
	ObjectList& List = m_aObjectList[ pObject->GetHitLayer() ];

	//	ベクタに追加	
	List.push_back( pObject );
}

//*****************************************************************************
//	当たりオブジェクト解除
//*****************************************************************************
void CollideSystem::RemoveObject( CollideObject* pObject )
{
	_ASSERT( pObject );

	//	レイヤー取得
	ObjectList& List = m_aObjectList[ pObject->GetHitLayer() ];

	//	ベクタ解除
	ObjectList::iterator it = StdFind( List, pObject );
	if( it != List.end() )
	{
		List.erase( it );
	}
}

//******************************************************************************
//	形状キャスト
//******************************************************************************
bool CollideSystem::ShapeCast( COLLIDE_CAST_RESULT& CastResult, const CollideCaster& Caster )
{
	_ASSERT( Caster.GetShape() );

	if( Caster.GetShape()->GetType() == COLLIDE_SHAPE_POINT )
	{
		CastResult.SolvePos = Caster.GetShape()->GetPos();
	}

	bool bHit = false;
	for( uint Layer = 0; Layer < COLLIDE_LAYER_NUM; Layer++ )
	{
		//	フィルタリング
		if( (Caster.GetFilter() & (1 << Layer)) == 0 ) continue;

		ObjectList& List = m_aObjectList[Layer];

		for( ObjectList::iterator it = List.begin(); it != List.end(); ++it )
		{
			CollideObject* pObj = (*it);

			//	当たりの有効・無効チェック
			if( !pObj->IsHitEnable() )
			{
				continue;
			}

			//	相手によって当たるかどうか調べる。
			{
				CollideObjectListener* pListener = pObj->GetListener();
				if( pListener && !pListener->IsHit( Caster.GetOwner() ) )
				{
					continue;
				}
			}
			{
				CollideObjectListener* pListener = Caster.GetListener();
				if( pListener && !pListener->IsHit( pObj->GetOwner() ) )
				{
					continue;
				}
			}

			//	当たり形状同士の判定
			COLLIDE_RESULT Result;
			if( pObj->GetShape()->ShapeCast( Result, &Caster, pObj->IsSolveEnable()? &CastResult.SolvePos : NULL ) )
			{
				Result.Layer	= COLLIDE_LAYER( Layer );
				Result.pOwner	= &pObj->GetOwner();
				CastResult.ResultList.insert( Result );

				pObj->SetCastHit( true );

				bHit = true;
			}
		}
	}

	//	形状描画
	if( IsDrawShape() )
	{
		Caster.GetShape()->DrawShape( GetDrawShapeColor( true, bHit, Caster.GetFilter() ) );
	}

	return bHit;
}

//******************************************************************************
//	形状描画用の色を取得する
//******************************************************************************
Color CollideSystem::GetDrawShapeColor( bool bCast, bool bHit, uint Layer ) const
{
	if( bCast )
	{
		return bHit? Color( 255, 0, 0 ) : Color( 128, 0, 0 );
	}
	else
	{
		return bHit? Color( 255, 192, 192 ) : Color( 128, 128, 128 );
	}
}

//-----------------------------------------------------------------------------
//	インスタンス管理
//-----------------------------------------------------------------------------
namespace
{
	CollideSystem* s_pCollideSystem	= NULL;
}

//*****************************************************************************
//	インスタンス初期化
//*****************************************************************************
void InitCollideSystem( void )
{
	if( !s_pCollideSystem )
	{
		s_pCollideSystem = new CollideSystem;
		MEM_CHECK( s_pCollideSystem );
	}
}

//*****************************************************************************
//	インスタンス終了
//*****************************************************************************
void ExitCollideSystem( void )
{
	SafeDelete( s_pCollideSystem );
}

//*****************************************************************************
//	インスタンス取得
//*****************************************************************************
CollideSystem* GetCollideSystem( void )
{
	return s_pCollideSystem;
}