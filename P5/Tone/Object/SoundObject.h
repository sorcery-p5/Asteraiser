#pragma once

namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	サウンドオブジェクト
//
////////////////////////////////////////////////////////////////////////////////
class SoundObject
{
public:
	SoundObject( const SoundFile* pFile, bool bLoop )					{ m_pFile = pFile; m_bLoop = bLoop; }
	virtual ~SoundObject()												{}
	
	virtual void				Update( void )							{}

	virtual bool				Start( void )							= 0;
	virtual bool				Stop( void )							= 0;
	virtual bool				Reset( void )							= 0;

	virtual void				SetVolume( float Vol )					= 0;
	virtual void				SetPan( float Pan )						= 0;

	bool						IsLoop( void )					const	{ return m_bLoop; }
	virtual bool				IsPlaying( void )				const	= 0;

protected:
	virtual const WAVEFORMATEX*	_GetFormat( void )				const	{ return m_pFile->GetFormat(); }

protected:
	const SoundFile*			m_pFile;
	bool						m_bLoop;
};

}