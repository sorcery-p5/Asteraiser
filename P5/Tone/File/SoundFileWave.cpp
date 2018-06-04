#include "P5.h"
#include "SoundFileWave.h"


namespace P5
{


////////////////////////////////////////////////////////////////////////////////
//
//	サウンドファイル
//
////////////////////////////////////////////////////////////////////////////////
SoundFileWave::SoundFileWave( void )
{
	ZeroStruct( m_Format );
}
SoundFileWave::~SoundFileWave()
{

}

//------------------------------------------------------------------------------
//	読み込み
//------------------------------------------------------------------------------
bool SoundFileWave::_OnLoad( void )
{
	//	オープン
	MMIOINFO	mmioInfo;
	ZeroStruct( mmioInfo );
	
	MmioHandle hMmio = mmioOpen( (char*)m_Path.c_str(), &mmioInfo, MMIO_READ );
	if( !hMmio ) return false;

	//	ヘッダ取得
	MMRESULT Res;
	MMCKINFO RiffChunk;
	RiffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	Res = mmioDescend( hMmio, &RiffChunk, NULL, MMIO_FINDRIFF );
	if( Res != MMSYSERR_NOERROR ) return false;

	//	フォーマット取得
	{
		MMCKINFO FormatChunk;
		FormatChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
		Res = mmioDescend( hMmio, &FormatChunk, &RiffChunk, MMIO_FINDCHUNK );
		if( Res != MMSYSERR_NOERROR ) return false;

		dword Size = mmioRead( hMmio, (char*)(&m_Format), FormatChunk.cksize );
		if( Size != FormatChunk.cksize ) return false;

		mmioAscend( hMmio, &FormatChunk, 0 );
	}

	//	データ取得
	{
		MMCKINFO DataChunk;
		DataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
		Res = mmioDescend( hMmio, &DataChunk, &RiffChunk, MMIO_FINDCHUNK );
		if( Res != MMSYSERR_NOERROR ) return false;

		m_Memory.Alloc( DataChunk.cksize );

		dword Size = mmioRead( hMmio, (char*)m_Memory.GetData(), DataChunk.cksize );
		if( Size != m_Memory.GetSize() ) return false;
		
		mmioAscend( hMmio, &DataChunk, 0 );
	}
	
	mmioAscend( hMmio, &RiffChunk, 0 );

	return true;
}

};