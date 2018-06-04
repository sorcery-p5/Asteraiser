#include "stdafx.h"
#include "CollideSystem.h"
#include "CollideObject.h"
#include "CollideObjectListener.h"
#include "CollideCaster.h"
#include "Shape/CollideShape.h"


///////////////////////////////////////////////////////////////////////////////
//
//	������V�X�e��
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
//	�X�V
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
//	������I�u�W�F�N�g�ǉ�
//*****************************************************************************
void CollideSystem::AddObject( CollideObject* pObject )
{
	_ASSERT( pObject && pObject->GetShape() );

	//	���C���[�擾
	ObjectList& List = m_aObjectList[ pObject->GetHitLayer() ];

	//	�x�N�^�ɒǉ�	
	List.push_back( pObject );
}

//*****************************************************************************
//	������I�u�W�F�N�g����
//*****************************************************************************
void CollideSystem::RemoveObject( CollideObject* pObject )
{
	_ASSERT( pObject );

	//	���C���[�擾
	ObjectList& List = m_aObjectList[ pObject->GetHitLayer() ];

	//	�x�N�^����
	ObjectList::iterator it = StdFind( List, pObject );
	if( it != List.end() )
	{
		List.erase( it );
	}
}

//******************************************************************************
//	�`��L���X�g
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
		//	�t�B���^�����O
		if( (Caster.GetFilter() & (1 << Layer)) == 0 ) continue;

		ObjectList& List = m_aObjectList[Layer];

		for( ObjectList::iterator it = List.begin(); it != List.end(); ++it )
		{
			CollideObject* pObj = (*it);

			//	������̗L���E�����`�F�b�N
			if( !pObj->IsHitEnable() )
			{
				continue;
			}

			//	����ɂ���ē����邩�ǂ������ׂ�B
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

			//	������`�󓯎m�̔���
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

	//	�`��`��
	if( IsDrawShape() )
	{
		Caster.GetShape()->DrawShape( GetDrawShapeColor( true, bHit, Caster.GetFilter() ) );
	}

	return bHit;
}

//******************************************************************************
//	�`��`��p�̐F���擾����
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
//	�C���X�^���X�Ǘ�
//-----------------------------------------------------------------------------
namespace
{
	CollideSystem* s_pCollideSystem	= NULL;
}

//*****************************************************************************
//	�C���X�^���X������
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
//	�C���X�^���X�I��
//*****************************************************************************
void ExitCollideSystem( void )
{
	SafeDelete( s_pCollideSystem );
}

//*****************************************************************************
//	�C���X�^���X�擾
//*****************************************************************************
CollideSystem* GetCollideSystem( void )
{
	return s_pCollideSystem;
}