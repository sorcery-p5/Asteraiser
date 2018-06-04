#pragma once

#include "IndicateTypes.h"
#include "IndicateBase.h"

class IndicateManager;
class IndicateData;
class SpriteFrame;


////////////////////////////////////////////////////////////////////////////////
//
//	インジケート
//
////////////////////////////////////////////////////////////////////////////////
class Indicate : public IndicateBase
{
public:
	Indicate( IndicateManager* pManager );
	~Indicate();

	void				Init( const IndicateData* pData );
	void				Delete( void )											{ m_bDelete = true; }
	
	virtual void		Update( void );
	virtual void		Draw( const Matrix3& OffsetMat );
	
	void				Open( void );
	void				Close( bool bDelete );

	void				SetMatrix( const Matrix3& Mat )							{ m_Matrix = Mat; }

	bool				IsDelete( void )								const	{ return m_bDelete; }
	bool				IsOpen( void )									const	{ return m_bOpen; }
	bool				IsAnimeEnable( void )							const;

protected:
	virtual SpriteFrame*				_GetSprite( void )				const	{ return m_pSprite; }
	virtual const Piece*				_GetPiece( Label Name )			const;
	virtual const Texture*				_GetTex( Label Name )			const;
	virtual const INDICATE_MARKER*		_GetMarker( Label BoneName )	const;
	virtual const INDICATE_SPIN*		_GetSpin( Label BoneName )		const;
	virtual const INDICATE_LINE_GRAPH*	_GetLineGraph( Label BoneName )	const;

private:
	IndicateManager*	m_pManager;
	const IndicateData*	m_pData;

	SpriteFrame*		m_pSprite;
	bool				m_bDelete;
	bool				m_bOpen;
	bool				m_bCloseDelete;
	Matrix3				m_Matrix;
};