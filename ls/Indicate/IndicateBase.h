#pragma once

#include "IndicateTypes.h"

class SpriteFrame;


////////////////////////////////////////////////////////////////////////////////
//
//	インジケート（基底）
//
////////////////////////////////////////////////////////////////////////////////
class IndicateBase
{
public:
	IndicateBase( void );
	virtual ~IndicateBase();

	virtual void						Update( void );
	virtual void						Draw( const Matrix3& OffsetMat );
	
	void								ChangeAnime( Label Name, int Intp );
	void								SetColor( Color Col );
	
	void								SetGauge( Label BoneName, float X, float Y );
	void								SetMarker( Label BoneName, float Value );
	void								SetPos( Label BoneName, Point Pos );
	void								SetSpin( Label BoneName, float Value );
	void								SetLineGraph( Label BoneName, const FloatVec& vValue, const IntVec* pSeparate = NULL );

	void								SetText( Label BoneName, pstr pFormat, ... );
	void								SetBoneVisible( Label BoneName, bool bVisible );
	void								SetColor( Label BoneName, Color Col );
	void								SetPieceIndex( Label BoneName, int Index );
	void								SetAngle( Label BoneName, float Ang );

	Label								GetAnimeName( void )			const;

protected:
	virtual SpriteFrame*				_GetSprite( void )				const = 0;
	virtual const Piece*				_GetPiece( Label Name )			const = 0;
	virtual const Texture*				_GetTex( Label Name )			const = 0;
	virtual const INDICATE_MARKER*		_GetMarker( Label BoneName )	const = 0;
	virtual const INDICATE_SPIN*		_GetSpin( Label BoneName )		const = 0;
	virtual const INDICATE_LINE_GRAPH*	_GetLineGraph( Label BoneName )	const = 0;

private:
	struct LINE_GRAPH
	{
		LINE_GRAPH( void )			{ pSprite = NULL; pParam = NULL; }

		FloatVec					vValue;
		IntVec						vSeparate;
		LineSprite*					pSprite;
		std::vector<RectSprite*>	vSeparateSprite;
		const INDICATE_LINE_GRAPH*	pParam;
	};

	typedef std::map<Label, Point>		MapBonePoint;
	typedef std::map<Label, float>		MapBoneRot;
	typedef std::map<Label, LINE_GRAPH>	MapLineGraph;

private:
	void								_MakeLineGraph( LINE_GRAPH& Graph, Label BoneName, const FloatVec& vValue, const IntVec* pSeparate );
	void								_DrawLineGraph( Label BoneName, const LINE_GRAPH& Graph );

private:
	MapBonePoint						m_mapBonePos;
	MapBonePoint						m_mapBoneScale;
	MapBoneRot							m_mapBoneRot;
	MapLineGraph						m_mapLineGraph;
};