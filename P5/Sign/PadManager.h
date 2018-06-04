#pragma once


namespace P5
{


class Pad;

///////////////////////////////////////////////////////////////////////////////
//
//	ƒpƒbƒhŠÇ—
//
///////////////////////////////////////////////////////////////////////////////
class PadManager
{
public:
	PadManager( void );
	~PadManager();

	bool			Init( HINSTANCE Hinst );
	bool			Update( void );

	void			AddPad( Pad* pPad )				{ return m_vPad.push_back( pPad ); }

	LPDIRECTINPUT8	GetDInput( void )				{ return m_pDinput; }
	Pad*			GetPad( uint Index )			{ return (Index >= 0 && Index < GetPadNum())? m_vPad[Index] : NULL; }
	uint			GetPadNum( void )		const	{ return (uint)m_vPad.size(); }

private:
	typedef std::vector<Pad*>	VecPad;

private:
	LPDIRECTINPUT8				m_pDinput;
	VecPad						m_vPad;
};

}