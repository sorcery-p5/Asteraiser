#pragma once



namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	ƒtƒHƒ“ƒg
//
////////////////////////////////////////////////////////////////////////////////
class Font
{
public:
	Font( void );
	~Font();

	bool			Load( pstr pPath );
	Rect			GetRect( char Ch, char Next )			const;
	Texture*		GetTex( void )							const	{ return m_pTexture; }
	Point			GetLetterSize( void )					const	{ return m_LetterSize; }
	float			GetLetterWidth( char Ch, char Next )	const;

	void			Reset( void )									{ SafePtr(m_pTexture)->Reset(); }
	void			Restore( void )									{ SafePtr(m_pTexture)->Restore(); }

private:
	struct WIDTH
	{
		char		Width;
		std::string	Str;
	};

	struct LETTER
	{
		char		X, Y;
		char		Width;
	};

	typedef std::map<short, LETTER>	LetterMap;
	typedef std::vector< WIDTH >	WidthVec;

	struct LETTER_SET
	{
		LetterMap	mapLetter;
		Point		Size;
		Point		Offset;
	};
	typedef std::vector<LETTER_SET*>	VecLetterSet;

private:
	bool			_LoadHeader( const XML::Node* pNode, const WidthVec& vWidth );
	void			_LoadWidth( WidthVec& vWidth, const XML::Node* pNode );
	void			_LoadLetter( LETTER_SET* pLetter, const WidthVec& vWidth, const XML::Node* pNode );
	void			_LoadStringText( LETTER_SET* pLetter, pstr pStr, const WidthVec& vWidth );
	void			_LoadStringFile( LETTER_SET* pLetter, pstr pPath, const WidthVec& vWidth );

	void			_AddLetter( LetterMap& mapLetter, int X, int Y, char Ch, char Next, const WidthVec& vWidth, int SizeW ) const;
	char			_GetWidth( char Ch, const WidthVec& vWidth, int SizeW )	const;

private:
	Texture*		m_pTexture;
	VecLetterSet	m_vLetterSet;
	Point			m_LetterSize;
};



};