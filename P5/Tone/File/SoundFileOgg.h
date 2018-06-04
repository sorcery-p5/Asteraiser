#pragma once

namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	サウンドファイル(ogg)
//
////////////////////////////////////////////////////////////////////////////////
class SoundFileOgg : public SoundFile
{
public:
	SoundFileOgg( void );
	virtual ~SoundFileOgg();
	
	virtual bool				IsStream( void )			const	{ return true; }

protected:
	virtual bool				_OnLoad( void );
};


}