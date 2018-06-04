#include "P5.h"
#include "SoundFileWave.h"


namespace P5
{


////////////////////////////////////////////////////////////////////////////////
//
//	�T�E���h�t�@�C��
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
//	�ǂݍ���
//------------------------------------------------------------------------------
bool SoundFileWave::_OnLoad( void )
{
	//	�I�[�v��
	MMIOINFO	mmioInfo;
	ZeroStruct( mmioInfo );
	
	MmioHandle hMmio = mmioOpen( (char*)m_Path.c_str(), &mmioInfo, MMIO_READ );
	if( !hMmio ) return false;

	//	�w�b�_�擾
	MMRESULT Res;
	MMCKINFO RiffChunk;
	RiffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	Res = mmioDescend( hMmio, &RiffChunk, NULL, MMIO_FINDRIFF );
	if( Res != MMSYSERR_NOERROR ) return false;

	//	�t�H�[�}�b�g�擾
	{
		MMCKINFO FormatChunk;
		FormatChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
		Res = mmioDescend( hMmio, &FormatChunk, &RiffChunk, MMIO_FINDCHUNK );
		if( Res != MMSYSERR_NOERROR ) return false;

		dword Size = mmioRead( hMmio, (char*)(&m_Format), FormatChunk.cksize );
		if( Size != FormatChunk.cksize ) return false;

		mmioAscend( hMmio, &FormatChunk, 0 );
	}

	//	�f�[�^�擾
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