#pragma once

namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	サウンドファイル(基底)
//
////////////////////////////////////////////////////////////////////////////////
class SoundFile
{
public:
	SoundFile( void );
	virtual ~SoundFile();

	bool						Load( pstr pPath );

	virtual const byte*			GetData( void )				const	{ return NULL; }
	virtual uint				GetSize( void )				const	{ return 0; }
	virtual const WAVEFORMATEX*	GetFormat( void )			const	{ return NULL; }
	virtual bool				IsStream( void )			const	{ return false; }

	pstr						GetPath( void )				const	{ return m_Path.c_str(); }
	Label						GetName( void )				const	{ return m_Name; }

protected:
	virtual bool				_OnLoad( void )						= 0;

protected:
	std::string					m_Path;
	Label						m_Name;
};


}