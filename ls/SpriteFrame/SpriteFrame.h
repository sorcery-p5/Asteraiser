#pragma once

#include "SpriteFrameTypes.h"

class SpriteFrameBone;
class SpriteFrameAnime;
class SpriteFrameFile;


////////////////////////////////////////////////////////////////////////////////
//
//	スプライトフレーム
//
////////////////////////////////////////////////////////////////////////////////
class SpriteFrame
{
public:
	SpriteFrame( void );
	~SpriteFrame();

	void					Init( const SpriteFrameFile* pFile, const Matrix3& BaseMat );
	void					Update( const Matrix3* pBaseMat );
	void					Draw( const Matrix3& OffsetMat );
	
	void					ChangeAnime( Label Name, uint Intp = 0 );
	void					ResetAnime( void );
	void					Pause( bool bPause );
	void					SetAnimeSpeed( float Speed );
	bool					IsAnimePlaying( void )					const;
	bool					IsAnimeLast( void )						const;
	bool					IsAnimeLoopFrame( void )				const;
	Label					GetAnimeName( void )					const;
	const char*				GetAnimeNotify( Label Key )				const;

	bool					HasAnime( Label Name )					const;
	int						GetAnimeNum( void )						const;
	Label					GetAnimeName( int Index )				const;

	const SpriteFrameFile*	GetFile( void )							const	{ return m_pFile; }
	const Matrix3&			GetBaseMatrix( void )					const	{ return m_BaseMat; }
	int						GetBoneNum( void )						const	{ return (int)m_vBone.size(); }
	int						FindBone( Label Name )					const;
	const Matrix3&			GetBoneMatrix( int Index )				const;
	const Matrix3*			GetBoneMatrix( Label Name )				const;
	const Matrix3&			GetBoneLocalMatrix( int Index )			const;
	const Matrix3*			GetBoneLocalMatrix( Label Name )		const;
	Color					GetBoneColor( int Index )				const;
	Color					GetBoneColor( Label Name )				const;
	Rect					GetBoneRect( Label Name )				const;
	SPRITE_FRAME_COLLIDE	GetCollide( int Index )					const;
	Color					GetColor( void )						const	{ return m_Color; }
	bool					IsVisible( void )						const	{ return m_bVisible; }

	void					SetVisible( bool bVisible );
	void					SetBoneVisible( int Index, bool bVisible );
	void					SetBoneVisible( Label Name, bool bVisible );
	void					SetColor( Color Col );
	void					SetBoneColor( int Index, Color Col );
	void					SetBoneColor( Label Name, Color Col );
	void					SetAlpha( byte Alpha );
	void					SetDrawPhase( uint hase );
	void					SetDrawPhase( int Index, int Phase );
	void					SetBoneText( int Index, pstr pStr );
	void					SetBoneText( Label Name, pstr pStr );
	void					SetBoneAngle( int Index, float Ang );
	void					SetBoneAngle( Label Name, float Ang );
	void					SetBonePieceIndex( int Index, int PieceIndex );
	void					SetBonePieceIndex( Label Name, int PieceIndex );

	void					SetBonePos( int Index, Point Pos );
	void					SetBonePos( Label Name, Point Pos );
	void					SetBoneRot( int Index, float Rot );
	void					SetBoneRot( Label Name, float Rot );
	void					SetBoneScale( int Index, Point Scale );
	void					SetBoneScale( Label Name, Point Scale );

private:
	typedef std::vector<SpriteFrameBone*>	VecBone;

private:
	SpriteFrameBone*		_CreateBone( int BoneID );
	void					_CalcWorldMatrix( SpriteFrameBone* pBone, const Matrix3& Mat );

private:
	const SpriteFrameFile*	m_pFile;
	VecBone					m_vBone;
	SpriteFrameAnime*		m_pAnime;
	Matrix3					m_BaseMat;
	Color					m_Color;
	bool					m_bVisible;
};