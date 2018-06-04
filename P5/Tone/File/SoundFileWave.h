#pragma once

#include "SoundFile.h"

namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	サウンドファイル（wave）
//
////////////////////////////////////////////////////////////////////////////////
class SoundFileWave : public SoundFile
{
public:
	SoundFileWave( void );
	virtual ~SoundFileWave();

	virtual const byte*			GetData( void )				const	{ return (const byte*)m_Memory.GetData(); }
	virtual uint				GetSize( void )				const	{ return m_Memory.GetSize(); }
	virtual const WAVEFORMATEX*	GetFormat( void )			const	{ return &m_Format; }

protected:
	virtual bool				_OnLoad( void );

private:
	Memory						m_Memory;
	WAVEFORMATEX				m_Format;
};


}