#pragma once

#include "StageTypes.h"

class Stage;
class SpriteFrame;
class SpriteFrameFile;


////////////////////////////////////////////////////////////////////////////////
//
//	ステージテロップ
//
////////////////////////////////////////////////////////////////////////////////
class StageTelop
{
public:
	StageTelop( Stage* pStage );
	~StageTelop();

	void				Init( const SpriteFrameFile* pFile, const STAGE_TELOP* pParam, Label Name, const Matrix3& Mat );
	void				Update( void );
	void				Draw( void );

	void				SetText( Label Bone, pstr pText );
	void				ChangeAnime( Label AnimeName, int Intp );

	void				Close( void );

	bool				IsDelete( void )			const		{ return m_State == STATE_DELETE; }
	Label				GetName( void )				const		{ return m_Name; }

private:
	enum STATE
	{
		STATE_OPEN,
		STATE_CLOSE,
		STATE_DELETE,
	};

private:
	void				_Delete( void )							{ m_State = STATE_DELETE; }

private:
	Stage*				m_pStage;
	SpriteFrame*		m_pSprite;
	const STAGE_TELOP*	m_pParam;
	Label				m_Name;
	STATE				m_State;
	int					m_FrameCount;
};