#include "stdafx.h"
#include "HitTestScene.h"

#include "SpriteFrame/SpriteFrameFile.h"
#include "SpriteFrame/SpriteFrame.h"

#include "Collide/CollideSystem.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"




///////////////////////////////////////////////////////////////////////////////
//
//	当たりテスト用シーン
//
///////////////////////////////////////////////////////////////////////////////
HitTestScene::HitTestScene( Label Name )
:Scene( Name )
{
	m_pCollidePoint		= NULL;
	m_pCollideLine		= NULL;
	m_pCollideRect		= NULL;
	m_pCollideMove		= NULL;

	m_pCasterPoint		= NULL;
	m_pCasterLine		= NULL;

	m_pShapeHit			= NULL;
	m_pShapeSolve		= NULL;
	m_pShapeNormal		= NULL;
}
HitTestScene::~HitTestScene()
{
	SafeDelete( m_pCollidePoint );
	SafeDelete( m_pCollideLine );
	SafeDelete( m_pCollideRect );
	SafeDelete( m_pCollideMove );

	SafeDelete( m_pCasterPoint );
	SafeDelete( m_pCasterLine );
	
	SafeDelete( m_pShapeHit );
	SafeDelete( m_pShapeSolve );
	SafeDelete( m_pShapeNormal );
}

//*****************************************************************************
//	初期化
//*****************************************************************************
void HitTestScene::OnInit( void )
{
	GetCollideSystem()->SetDrawShapeEnable( true );

	//	OBJ
	m_pCollidePoint		= new CollideObject;
	m_pCollideLine		= new CollideObject;
	m_pCollideRect		= new CollideObject;
	m_pCollideMove		= new CollideObject;

	m_pCollidePoint->Init( new CollideShapePoint( 50.0f ), COLLIDE_LAYER_GROUND );
	m_pCollideLine->Init( new CollideShapeLine( 200.0f, 30.0f ), COLLIDE_LAYER_GROUND );
	m_pCollideRect->Init( new CollideShapeRect( 200.0f, 100.0f ), COLLIDE_LAYER_GROUND );
	m_pCollideMove->Init( new CollideShapeRect( 200.0f, 180.0f ), COLLIDE_LAYER_GROUND );

	m_pCollidePoint->UpdateMatrix( &(Matrix3::Scale(-1,1) *  Matrix3::Trans(Point(100.0f, 100.0f))) );
	m_pCollideLine->UpdateMatrix( &(Matrix3::Scale(-1,1) *  Matrix3::TransRotZ(Point(200.0f, 200.0f), Angle::ToRad(45.0f) ))  );
	m_pCollideRect->UpdateMatrix( &(Matrix3::Scale(-1,1) *  Matrix3::TransRotZ(Point(500.0f, 300.0f), Angle::ToRad(120.0f) )) );
	m_pCollideMove->UpdateMatrix( &Matrix3::Trans(Point(400.0f, 350.0f)) );

	//	Caster
	m_pCasterPoint		= new CollideCaster;
	m_pCasterLine		= new CollideCaster;
	m_pCasterPoint->Init( new CollideShapePoint( 25.0f ) );
	m_pCasterLine->Init( new CollideShapeLine( 300.0f, 10.0f ) );
	m_pCasterPoint->SetCalcType( COLLIDE_CALC_ALL );
	m_pCasterLine->SetCalcType( COLLIDE_CALC_ALL );

	//	情報用
	m_pShapeHit		= new ShapeCircle;
	m_pShapeSolve	= new ShapeCircle;
	m_pShapeNormal	= new ShapeLine;

	m_pShapeHit->SetColor( Color(128, 192, 255) );
	m_pShapeSolve->SetColor( Color(0, 255, 0) );
	m_pShapeNormal->SetColor( Color(255, 255, 0) );

	//	背景色変更
	P5::GetGraphix()->GetDevice()->SetScreenColor( Color(32,32,32) );
}

//*****************************************************************************
//	更新
//*****************************************************************************
void HitTestScene::Update( void )
{
	Mouse* pMouse = GetSign()->GetMouse();

	//	左クリックで点キャスト
	if( pMouse->IsButtonInput( MOUSE_L, INPUT_PRESS ) )
	{
		m_pCasterPoint->UpdateMatrix( &Matrix3::Trans( pMouse->GetPos() ) );

		COLLIDE_CAST_RESULT Result;
		if( m_pCasterPoint->Cast( Result ) )
		{
			if( !Result.ResultList.empty() )
			{
				COLLIDE_RESULT_LIST::iterator it = Result.ResultList.begin();

				Point HitPos = it->HitPos;
				m_pShapeHit->Draw( HitPos, 5.0f );
				m_pShapeNormal->Draw( HitPos, HitPos + it->Normal * 30.0f );
			}
			m_pShapeSolve->Draw( Result.SolvePos, 20.0f );
		}
	}
	else
	{
		//	右クリックで線キャスト
		if( pMouse->IsButtonInput( MOUSE_R, INPUT_TRIG ) )
		{
			m_DragPos = pMouse->GetPos();
		}
		if( pMouse->IsButtonInput( MOUSE_R, INPUT_PRESS ) )
		{
			m_pCasterLine->UpdateMatrix( &Matrix3::FromTo( m_DragPos, pMouse->GetPos() ) );

			COLLIDE_CAST_RESULT Result;
			if( m_pCasterLine->Cast( Result ) )
			{
				if( !Result.ResultList.empty() )
				{
					COLLIDE_RESULT_LIST::iterator it = Result.ResultList.begin();

					Point HitPos = it->HitPos;
					m_pShapeHit->Draw( HitPos, 3.0f );
					m_pShapeNormal->Draw( HitPos, HitPos + it->Normal * 30.0f );
				}
			}
		}
	}

	//	ホイールで地形移動
	if( pMouse->IsButtonInput( MOUSE_M, INPUT_PRESS ) )
	{
		m_MoveRectAngle += Angle::ToRad( pMouse->GetWheel() * 5.0f );

		m_pCollideMove->UpdateMatrix( &Matrix3::TransRotZ( pMouse->GetPos(), m_MoveRectAngle) );
	}
}