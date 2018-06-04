#include "stdafx.h"
#include "IndicateBase.h"

#include "SpriteFrame/SpriteFrame.h"

#include "Config/Config.h"


////////////////////////////////////////////////////////////////////////////////
//
//	インジケート（基底）
//
////////////////////////////////////////////////////////////////////////////////
IndicateBase::IndicateBase( void )
{
}
IndicateBase::~IndicateBase()
{
	for( MapLineGraph::iterator it = m_mapLineGraph.begin(); it != m_mapLineGraph.end(); ++it )
	{
		SafeDelete( (it->second).pSprite );
		DeleteVec( (it->second).vSeparateSprite );
	}
}

//******************************************************************************
//	更新
//******************************************************************************
void IndicateBase::Update( void )
{
	SpriteFrame* pSprite = _GetSprite();

	if( pSprite )
	{
		//	拡縮の設定
		for( MapBonePoint::iterator it = m_mapBoneScale.begin(); it != m_mapBoneScale.end(); ++it )
		{
			pSprite->SetBoneScale( it->first, it->second );
		}

		//	位置の設定
		for( MapBonePoint::iterator it = m_mapBonePos.begin(); it != m_mapBonePos.end(); ++it )
		{
			pSprite->SetBonePos( it->first, it->second );
		}

		//	角度の設定
		for( MapBoneRot::iterator it = m_mapBoneRot.begin(); it != m_mapBoneRot.end(); ++it )
		{
			pSprite->SetBoneRot( it->first, it->second );
		}
	}
}

//******************************************************************************
//	描画
//******************************************************************************
void IndicateBase::Draw( const Matrix3& OffsetMat )
{
	if( !_GetSprite()->IsVisible() ) return;

	for( MapLineGraph::iterator it = m_mapLineGraph.begin(); it != m_mapLineGraph.end(); ++it )
	{
		_DrawLineGraph( it->first, it->second );
	}
}

//******************************************************************************
//	アニメ再生
//******************************************************************************
void IndicateBase::ChangeAnime( Label Name, int Intp )
{
	SpriteFrame* pSprite = _GetSprite();
	SafePtr( pSprite )->ChangeAnime( Name, Intp );
}

//******************************************************************************
//	色変更
//******************************************************************************
void IndicateBase::SetColor( Color Col )
{
	SpriteFrame* pSprite = _GetSprite();
	SafePtr( pSprite )->SetColor( Col );
}

//******************************************************************************
//	ゲージの設定
//******************************************************************************	
void IndicateBase::SetGauge( Label BoneName, float X, float Y )
{
	m_mapBoneScale[ BoneName ] = Point( X, Y );
}

//******************************************************************************
//	マーカーの設定
//******************************************************************************
void IndicateBase::SetMarker( Label BoneName, float Value )
{
	const INDICATE_MARKER* pMarker = _GetMarker( BoneName );
	if( !pMarker ) return;

	//	座標を求める
	Point Pos = Lerp<Point>( pMarker->MinPos, pMarker->MaxPos, Value );
	
	//	ボーンに設定
	m_mapBonePos[ BoneName ] = Pos;
}

//******************************************************************************
//	位置の設定
//******************************************************************************
void IndicateBase::SetPos( Label BoneName, Point Pos )
{
	//	ボーンに設定
	m_mapBonePos[ BoneName ] = Pos;
}

//******************************************************************************
//	回転の設定
//******************************************************************************
void IndicateBase::SetSpin( Label BoneName, float Value )
{
	const INDICATE_SPIN* pSpin = _GetSpin( BoneName );

	float Rot = 0.0f;
	if( pSpin )
	{
		Rot = Lerp<float>( pSpin->MinRot, pSpin->MaxRot, Value );
	}
	else
	{
		Rot = Value * PI2;
	}

	//	ボーンに設定
	m_mapBoneRot[ BoneName ] = Rot;
}

//******************************************************************************
//	線グラフの設定
//******************************************************************************
void IndicateBase::SetLineGraph( Label BoneName, const FloatVec& vValue, const IntVec* pSeparate )
{
	//	既存のグラフがあるか
	MapLineGraph::iterator it = m_mapLineGraph.find( BoneName );
	if( it != m_mapLineGraph.end() )
	{
		_MakeLineGraph( it->second, BoneName, vValue, pSeparate );
	}

	//	新規
	else
	{
		LINE_GRAPH Graph;

		_MakeLineGraph( Graph, BoneName, vValue, pSeparate );

		m_mapLineGraph.insert( MapLineGraph::value_type( BoneName, Graph ) );
	}

}

//******************************************************************************
//	文字列の設定
//******************************************************************************
void IndicateBase::SetText( Label BoneName, pstr pFormat, ... )
{
	String256 Str;
	Str.FormatV( pFormat, (char*)(&pFormat + 1) );

	//	文字列の置換
	GetConfig()->ReplaceKeyName( Str );

	SpriteFrame* pSprite = _GetSprite();
	SafePtr( pSprite )->SetBoneText( BoneName, Str.c_str() );
}

//******************************************************************************
//	表示の設定
//******************************************************************************
void IndicateBase::SetBoneVisible( Label BoneName, bool bVisible )
{
	SpriteFrame* pSprite = _GetSprite();
	SafePtr( pSprite )->SetBoneVisible( BoneName, bVisible );
}

//******************************************************************************
//	色の設定
//******************************************************************************
void IndicateBase::SetColor( Label BoneName, Color Col )
{
	SpriteFrame* pSprite = _GetSprite();
	SafePtr( pSprite )->SetBoneColor( BoneName, Col );
}

//******************************************************************************
//	ピースインデクスの設定
//******************************************************************************
void IndicateBase::SetPieceIndex( Label BoneName, int Index )
{
	SpriteFrame* pSprite = _GetSprite();
	SafePtr( pSprite )->SetBonePieceIndex( BoneName, Index );
}

//******************************************************************************
//	中心角度の設定
//******************************************************************************
void IndicateBase::SetAngle( Label BoneName, float Ang )
{
	SpriteFrame* pSprite = _GetSprite();
	SafePtr( pSprite )->SetBoneAngle( BoneName, Ang * PI2 );
}

//******************************************************************************
//	アニメ名の取得
//******************************************************************************
Label IndicateBase::GetAnimeName( void ) const
{
	SpriteFrame* pSprite = _GetSprite();
	if( pSprite ) return pSprite->GetAnimeName();

	return Label();
}

//------------------------------------------------------------------------------
//	線グラフパラメータ生成
//------------------------------------------------------------------------------
void IndicateBase::_MakeLineGraph( LINE_GRAPH& Graph, Label BoneName, const FloatVec& vValue, const IntVec* pSeparate )
{
	const INDICATE_LINE_GRAPH* pLineGraph = _GetLineGraph( BoneName );
	if( !pLineGraph ) return;

	//	ボーンの存在チェック
	if( _GetSprite()->FindBone( BoneName ) < 0 ) return;

	//	パラメータ
	Graph.pParam = pLineGraph;

	//	スプライト
	if( vValue.size() != Graph.vValue.size() )
	{
		SafeDelete( Graph.pSprite );

		Graph.pSprite = new LineSprite( vValue.size(), pLineGraph->Divide );
		if( Graph.pSprite )
		{
			const Piece* pPiece = _GetPiece( pLineGraph->PieceName );

			Graph.pSprite->SetPiece( pPiece, pPiece? _GetTex( pPiece->GetTexName() ) : NULL );
			Graph.pSprite->SetWidth( pLineGraph->Width );
			Graph.pSprite->SetColor( pLineGraph->ColMin );
			Graph.pSprite->SetPhase( pLineGraph->Phase );
			Graph.pSprite->SetBlend( pLineGraph->Blend );
		}
	}

	//	値を登録
	Graph.vValue	= vValue;

	//	セパレート
	if( pSeparate )
	{
		//	スプライト生成
		if( pSeparate->size() != Graph.vSeparate.size() )
		{
			DeleteVec( Graph.vSeparateSprite );

			for( int i = 0; i < (int)pSeparate->size(); i++ )
			{
				RectSprite* pSprite = new RectSprite();
				if( pSprite )
				{
					const Piece* pPiece = _GetPiece( pLineGraph->SeparatePiece );

					pSprite->SetPiece( pPiece, pPiece? _GetTex( pPiece->GetTexName() ) : NULL );
					pSprite->SetColor( pLineGraph->ColMin );
					pSprite->SetPhase( pLineGraph->Phase );
					pSprite->SetBlend( pLineGraph->Blend );

					Graph.vSeparateSprite.push_back( pSprite );
				}
			}
		}

		Graph.vSeparate = *pSeparate;
	}
}

//------------------------------------------------------------------------------
//	線グラフの描画
//------------------------------------------------------------------------------
void IndicateBase::_DrawLineGraph( Label BoneName, const LINE_GRAPH& Graph )
{
	if( !Graph.pSprite ) return;

	std::vector<Point> vPt;

	Matrix3 BoneMat = *_GetSprite()->GetBoneMatrix( BoneName );
	Color BoneColor	= _GetSprite()->GetBoneColor( BoneName );

	float TimeMax = Max( (float)Graph.pParam->TimeMax, (float)Graph.vValue.size() );
	for( int i = 0; i < (int)Graph.vValue.size(); i++ )
	{
		Point Pt;

		//	数値軸
		float Val = Clamp( Graph.vValue[i], Graph.pParam->ValMin, Graph.pParam->ValMax );
		Val = Coef( Val, Graph.pParam->ValMin, Graph.pParam->ValMax );
		Pt += Graph.pParam->ValAxis * Val;

		//	時間軸
		float Time = (float)i / TimeMax;
		Pt += Graph.pParam->TimeAxis * Time;

		//	ボーンのマトリクスへ変換
		vPt.push_back( BoneMat.Transform( Pt ) );
		
		//	色の調整
		Graph.pSprite->SetColor( i, Color::Lerp(Graph.pParam->ColMin,Graph.pParam->ColMax,Val) * BoneColor );
	}
	
	//	セパレータ
	for( int i = 0; i < (int)Graph.vSeparate.size(); i++ )
	{
		RectSprite* pSprite = Graph.vSeparateSprite[i];

		Point Pt = -(pSprite->GetSize() * 0.5f);

		//	数値軸
		Pt += Graph.pParam->ValAxis * 0.5f;

		//	時間軸
		float Time = (float)Graph.vSeparate[i] / TimeMax;
		Pt += Graph.pParam->TimeAxis * Time;

		//	描画
		pSprite->SetColor( Graph.pParam->SeparateColor * BoneColor );
		pSprite->Draw( Matrix3::Trans( Pt ) * BoneMat );
	}

	//	ライン描画	
	Graph.pSprite->Draw( &vPt[0] );
}