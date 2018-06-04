#include "P5.h"
#include "OggLoader.h"

namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	OGGをストリーミングでロードするクラス
//
////////////////////////////////////////////////////////////////////////////////
OggLoader::OggLoader( void )
{
	ZeroStruct( m_VorbisFile );
	ZeroStruct( m_Format );
}

OggLoader::~OggLoader()
{
	if( m_VorbisFile.datasource != 0 )
	{
		ov_clear( &m_VorbisFile );
	}
}

//******************************************************************************
//	読み込み
//******************************************************************************
bool OggLoader::Load( const SoundFile* pFile )
{
	if( !pFile ) return false;

	//	ファイル開く
	if( !m_File.Open( pFile->GetPath(), "rb" ) )
	{
		return false;
	}

	//	Ogg開く
	if( ov_open( m_File, &m_VorbisFile, NULL, 0 ) < 0 )
	{
		return false;
	}

	//	フォーマット取得
	vorbis_info* pInfo = ov_info( &m_VorbisFile, -1 );
	m_Format.wFormatTag			= 1;
	m_Format.nChannels			= pInfo->channels;
	m_Format.nSamplesPerSec 	= pInfo->rate;
	m_Format.wBitsPerSample 	= 16;
	m_Format.nBlockAlign		= m_Format.nChannels * m_Format.wBitsPerSample / 8;
	m_Format.nAvgBytesPerSec	= m_Format.nSamplesPerSec * m_Format.nBlockAlign;
	m_Format.cbSize				= 0;

	return true;
}


//******************************************************************************
//	バッファを読み込み
//******************************************************************************
void OggLoader::Read( BUFFER& Buf, bool bLoop )
{
	Buf.Size = 0;
	Buf.bEnd = false;

	uint ReqSize	= 4096;
	uint Size		= Buf.Mem.GetSize();

	if( Size < ReqSize ) ReqSize = Size;

	while( Buf.Size < Size )
	{
		int Stream = 0;
		uint ReadSize = ov_read( &m_VorbisFile, Buf.GetWritePos(), ReqSize, 0, 2, 1, &Stream );
		if( ReadSize == 0 )
		{
			//	終端に達した
			if( bLoop )
			{
				ov_time_seek( &m_VorbisFile, 0.0 );
			}
			else
			{
				Buf.bEnd = true;
			}
		}

		Buf.Size += ReadSize;

		if( Size - Buf.Size < 4096 )
		{
			ReqSize = Size - Buf.Size;
		}
	}
}

//******************************************************************************
//	読み込み位置のリセット
//******************************************************************************
void OggLoader::Reset( void )
{
	ov_time_seek( &m_VorbisFile, 0.0 );
}

};