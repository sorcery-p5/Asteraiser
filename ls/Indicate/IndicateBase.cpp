#include "stdafx.h"
#include "IndicateBase.h"

#include "SpriteFrame/SpriteFrame.h"

#include "Config/Config.h"


////////////////////////////////////////////////////////////////////////////////
//
//	�C���W�P�[�g�i���j
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
//	�X�V
//******************************************************************************
void IndicateBase::Update( void )
{
	SpriteFrame* pSprite = _GetSprite();

	if( pSprite )
	{
		//	�g�k�̐ݒ�
		for( MapBonePoint::iterator it = m_mapBoneScale.begin(); it != m_mapBoneScale.end(); ++it )
		{
			pSprite->SetBoneScale( it->first, it->second );
		}

		//	�ʒu�̐ݒ�
		for( MapBonePoint::iterator it = m_mapBonePos.begin(); it != m_mapBonePos.end(); ++it )
		{
			pSprite->SetBonePos( it->first, it->second );
		}

		//	�p�x�̐ݒ�
		for( MapBoneRot::iterator it = m_mapBoneRot.begin(); it != m_mapBoneRot.end(); ++it )
		{
			pSprite->SetBoneRot( it->first, it->second );
		}
	}
}

//******************************************************************************
//	�`��
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
//	�A�j���Đ�
//******************************************************************************
void IndicateBase::ChangeAnime( Label Name, int Intp )
{
	SpriteFrame* pSprite = _GetSprite();
	SafePtr( pSprite )->ChangeAnime( Name, Intp );
}

//******************************************************************************
//	�F�ύX
//******************************************************************************
void IndicateBase::SetColor( Color Col )
{
	SpriteFrame* pSprite = _GetSprite();
	SafePtr( pSprite )->SetColor( Col );
}

//******************************************************************************
//	�Q�[�W�̐ݒ�
//******************************************************************************	
void IndicateBase::SetGauge( Label BoneName, float X, float Y )
{
	m_mapBoneScale[ BoneName ] = Point( X, Y );
}

//******************************************************************************
//	�}�[�J�[�̐ݒ�
//******************************************************************************
void IndicateBase::SetMarker( Label BoneName, float Value )
{
	const INDICATE_MARKER* pMarker = _GetMarker( BoneName );
	if( !pMarker ) return;

	//	���W�����߂�
	Point Pos = Lerp<Point>( pMarker->MinPos, pMarker->MaxPos, Value );
	
	//	�{�[���ɐݒ�
	m_mapBonePos[ BoneName ] = Pos;
}

//******************************************************************************
//	�ʒu�̐ݒ�
//******************************************************************************
void IndicateBase::SetPos( Label BoneName, Point Pos )
{
	//	�{�[���ɐݒ�
	m_mapBonePos[ BoneName ] = Pos;
}

//******************************************************************************
//	��]�̐ݒ�
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

	//	�{�[���ɐݒ�
	m_mapBoneRot[ BoneName ] = Rot;
}

//******************************************************************************
//	���O���t�̐ݒ�
//******************************************************************************
void IndicateBase::SetLineGraph( Label BoneName, const FloatVec& vValue, const IntVec* pSeparate )
{
	//	�����̃O���t�����邩
	MapLineGraph::iterator it = m_mapLineGraph.find( BoneName );
	if( it != m_mapLineGraph.end() )
	{
		_MakeLineGraph( it->second, BoneName, vValue, pSeparate );
	}

	//	�V�K
	else
	{
		LINE_GRAPH Graph;

		_MakeLineGraph( Graph, BoneName, vValue, pSeparate );

		m_mapLineGraph.insert( MapLineGraph::value_type( BoneName, Graph ) );
	}

}

//******************************************************************************
//	������̐ݒ�
//******************************************************************************
void IndicateBase::SetText( Label BoneName, pstr pFormat, ... )
{
	String256 Str;
	Str.FormatV( pFormat, (char*)(&pFormat + 1) );

	//	������̒u��
	GetConfig()->ReplaceKeyName( Str );

	SpriteFrame* pSprite = _GetSprite();
	SafePtr( pSprite )->SetBoneText( BoneName, Str.c_str() );
}

//******************************************************************************
//	�\���̐ݒ�
//******************************************************************************
void IndicateBase::SetBoneVisible( Label BoneName, bool bVisible )
{
	SpriteFrame* pSprite = _GetSprite();
	SafePtr( pSprite )->SetBoneVisible( BoneName, bVisible );
}

//******************************************************************************
//	�F�̐ݒ�
//******************************************************************************
void IndicateBase::SetColor( Label BoneName, Color Col )
{
	SpriteFrame* pSprite = _GetSprite();
	SafePtr( pSprite )->SetBoneColor( BoneName, Col );
}

//******************************************************************************
//	�s�[�X�C���f�N�X�̐ݒ�
//******************************************************************************
void IndicateBase::SetPieceIndex( Label BoneName, int Index )
{
	SpriteFrame* pSprite = _GetSprite();
	SafePtr( pSprite )->SetBonePieceIndex( BoneName, Index );
}

//******************************************************************************
//	���S�p�x�̐ݒ�
//******************************************************************************
void IndicateBase::SetAngle( Label BoneName, float Ang )
{
	SpriteFrame* pSprite = _GetSprite();
	SafePtr( pSprite )->SetBoneAngle( BoneName, Ang * PI2 );
}

//******************************************************************************
//	�A�j�����̎擾
//******************************************************************************
Label IndicateBase::GetAnimeName( void ) const
{
	SpriteFrame* pSprite = _GetSprite();
	if( pSprite ) return pSprite->GetAnimeName();

	return Label();
}

//------------------------------------------------------------------------------
//	���O���t�p�����[�^����
//------------------------------------------------------------------------------
void IndicateBase::_MakeLineGraph( LINE_GRAPH& Graph, Label BoneName, const FloatVec& vValue, const IntVec* pSeparate )
{
	const INDICATE_LINE_GRAPH* pLineGraph = _GetLineGraph( BoneName );
	if( !pLineGraph ) return;

	//	�{�[���̑��݃`�F�b�N
	if( _GetSprite()->FindBone( BoneName ) < 0 ) return;

	//	�p�����[�^
	Graph.pParam = pLineGraph;

	//	�X�v���C�g
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

	//	�l��o�^
	Graph.vValue	= vValue;

	//	�Z�p���[�g
	if( pSeparate )
	{
		//	�X�v���C�g����
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
//	���O���t�̕`��
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

		//	���l��
		float Val = Clamp( Graph.vValue[i], Graph.pParam->ValMin, Graph.pParam->ValMax );
		Val = Coef( Val, Graph.pParam->ValMin, Graph.pParam->ValMax );
		Pt += Graph.pParam->ValAxis * Val;

		//	���Ԏ�
		float Time = (float)i / TimeMax;
		Pt += Graph.pParam->TimeAxis * Time;

		//	�{�[���̃}�g���N�X�֕ϊ�
		vPt.push_back( BoneMat.Transform( Pt ) );
		
		//	�F�̒���
		Graph.pSprite->SetColor( i, Color::Lerp(Graph.pParam->ColMin,Graph.pParam->ColMax,Val) * BoneColor );
	}
	
	//	�Z�p���[�^
	for( int i = 0; i < (int)Graph.vSeparate.size(); i++ )
	{
		RectSprite* pSprite = Graph.vSeparateSprite[i];

		Point Pt = -(pSprite->GetSize() * 0.5f);

		//	���l��
		Pt += Graph.pParam->ValAxis * 0.5f;

		//	���Ԏ�
		float Time = (float)Graph.vSeparate[i] / TimeMax;
		Pt += Graph.pParam->TimeAxis * Time;

		//	�`��
		pSprite->SetColor( Graph.pParam->SeparateColor * BoneColor );
		pSprite->Draw( Matrix3::Trans( Pt ) * BoneMat );
	}

	//	���C���`��	
	Graph.pSprite->Draw( &vPt[0] );
}