#pragma once

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>


namespace P5
{

class SoundFile;


////////////////////////////////////////////////////////////////////////////////
//
//	OGGをストリーミングでロードするクラス
//
////////////////////////////////////////////////////////////////////////////////
class OggLoader
{
public:
	struct BUFFER
	{
		Memory		Mem;
		uint		Size;
		bool		bEnd;

		BUFFER( void )
		{
			Size = 0;
			bEnd = false;
		}

		BUFFER( uint s )
		{
			Mem.Alloc( s );
			Size = 0;
			bEnd = false;
		}

		char* GetWritePos( void )
		{
			return ((char*)Mem.GetData()) + Size;
		}
	};

public:
	OggLoader( void );
	~OggLoader();

	bool				Load( const SoundFile* pFile );
	void				Read( BUFFER& Buf, bool bLoop );

	void				Reset( void );

	WAVEFORMATEX*		GetFormat( void )					{ return &m_Format; }

private:
	FileHandle			m_File;
	OggVorbis_File		m_VorbisFile;
	WAVEFORMATEX		m_Format;
};


};