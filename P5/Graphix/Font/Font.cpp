#include "P5.h"
#include "Font.h"

namespace
{

	//	全角文字コード生成
	#define MAKE_CODE( ch, next )		(short)(((ch) << 8) + (next) )

	//	XMLノード
	pstr	NODE_HEADER			= "Header";
	pstr	NODE_LETTER			= "Letter";
	pstr	NODE_WIDTH			= "Width";
	pstr	NODE_STRING			= "String";

	pstr	ATTR_TEX			= "Tex";
	pstr	ATTR_TRANS			= "Trans";
	pstr	ATTR_SIZE			= "Size";
	pstr	ATTR_OFFSET			= "Offset";
	pstr	ATTR_WIDTH			= "Width";

	pstr	ATTR_TEXT			= "Text";
	pstr	ATTR_PATH			= "Path";
	pstr	ATTR_STRING			= "String";
}

namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	フォント
//
////////////////////////////////////////////////////////////////////////////////
Font::Font( void )
{
}

Font::~Font()
{
	for( uint i = 0; i < m_vLetterSet.size(); i++ )
	{
		LETTER_SET* pLetter = m_vLetterSet[i];
		SafeDelete( pLetter );
	}
	SafeDelete( m_pTexture );
}

//******************************************************************************
//	読み込み
//******************************************************************************
bool Font::Load( pstr pPath )
{
	//	ファイル読み込む
	XML Xml;
	if( !Xml.Load( pPath ) )
	{
		return false;
	}

	const XML::Node* pHeaderNode = Xml.GetRoot().FindChild( NODE_HEADER );
	if( !pHeaderNode )
	{
		return false;
	}

	//	幅データ
	WidthVec vWidth;
	const XML::Node* pWidthNode = Xml.GetRoot().FindChild( NODE_WIDTH );
	if( pWidthNode )
	{
		_LoadWidth( vWidth, pWidthNode );
	}

	//	ヘッダデータ
	if( !_LoadHeader( pHeaderNode, vWidth ) )
	{
		return false;
	}

	return true;
}

//******************************************************************************
//	文字から矩形を取得
//******************************************************************************
Rect Font::GetRect( char Ch, char Next ) const
{
	for( uint i = 0; i < m_vLetterSet.size(); i++ )
	{
		LETTER_SET* pLetter = m_vLetterSet[i];
		
		LetterMap::const_iterator it = pLetter->mapLetter.find( MAKE_CODE( Ch, Next ) );
		if( it == pLetter->mapLetter.end() ) continue;

		const LETTER& Let = it->second;
		float Left	= (Let.X * pLetter->Size.x) + (pLetter->Size.x - Let.Width) / 2;

		return Rect( pLetter->Offset.x + Left,
					 pLetter->Offset.y + Let.Y * pLetter->Size.y,
					 pLetter->Offset.x + Left + Let.Width,
					 pLetter->Offset.y + (Let.Y+1) * pLetter->Size.y );
	}

	return Rect( 0.0f, 0.0f, m_LetterSize.x, m_LetterSize.y );
}

//******************************************************************************
//	文字の幅取得
//******************************************************************************
float Font::GetLetterWidth( char Ch, char Next ) const
{
	for( uint i = 0; i < m_vLetterSet.size(); i++ )
	{
		LETTER_SET* pLetter = m_vLetterSet[i];
			
		LetterMap::const_iterator it = pLetter->mapLetter.find( MAKE_CODE( Ch, Next ) );
		if( it != pLetter->mapLetter.end() ) 
		{
			return (it->second).Width;
		}
	}

	return m_LetterSize.x;
}

//------------------------------------------------------------------------------
//	ヘッダデータ読み込み
//------------------------------------------------------------------------------
bool Font::_LoadHeader( const XML::Node* pNode, const WidthVec& vWidth )
{
	const XML::Node* pLetterNode = pNode->FindChild( NODE_LETTER );

	//	テクスチャ取得
	const XML::Value* pTexAttr = pNode->FindAttr( ATTR_TEX );
	if( !pTexAttr ) return false;

	//	透明色の設定
	Color Trans = Color::Zero();

	const XML::Value* pTransAttr = pNode->FindAttr( ATTR_TRANS );
	if( pTransAttr && !pTransAttr->IsEmpty() ) Trans = pTransAttr->GetColor();

	P5::GetGraphix()->GetDevice()->SetTransparentColor( Trans );

	//	読み込む
	m_pTexture = new Texture;
	if( !m_pTexture ) return false;

	if( !m_pTexture->Load( pTexAttr->GetStr() ) )
	{
		return false;
	}

	while( pLetterNode )
	{
		LETTER_SET* pLetter = new LETTER_SET;
		MEM_CHECK( pLetter );

		//	文字セット読み込む
		_LoadLetter( pLetter, vWidth, pLetterNode );
		m_vLetterSet.push_back( pLetter );

		pLetterNode = pNode->FindChild( NODE_LETTER, pLetterNode );
	}

	//	サイズ取得
	m_LetterSize = pNode->GetAttr( ATTR_SIZE ).GetPoint();

	return true;
}

//------------------------------------------------------------------------------
//	幅データの読み込み
//------------------------------------------------------------------------------
void Font::_LoadWidth( WidthVec& vWidth, const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pFileNode = pNode->GetChild( i );
		
		WIDTH Wid;
		Wid.Width	= pFileNode->GetAttr( ATTR_WIDTH ).GetInt();
		Wid.Str		= pFileNode->GetAttr( ATTR_STRING ).GetStr();

		vWidth.push_back( Wid );
	}
}

//------------------------------------------------------------------------------
//	文字セットのデータを読み込む
//------------------------------------------------------------------------------
void Font::_LoadLetter( LETTER_SET* pLetter, const WidthVec& vWidth, const XML::Node* pNode )
{
	//	サイズ取得
	pLetter->Size = pNode->GetAttr( ATTR_SIZE ).GetPoint();

	//	オフセット取得
	pLetter->Offset = pNode->GetAttr( ATTR_OFFSET ).GetPoint();

	//	文字列指定形式
	const XML::Value* pText = pNode->FindAttr( ATTR_TEXT );
	if( pText )
	{
		_LoadStringText( pLetter, pText->GetStr(), vWidth );
	}

	//	テキストファイル形式
	const XML::Value* pPath = pNode->FindAttr( ATTR_PATH );
	if( pPath )
	{
		_LoadStringFile( pLetter, pPath->GetStr(), vWidth );
	}
}

//------------------------------------------------------------------------------
//	文字列から文字コード読み出し
//------------------------------------------------------------------------------
void Font::_LoadStringText( LETTER_SET* pLetter, pstr pStr, const WidthVec& vWidth )
{
	int X = 0, Y = 0;
	while( *pStr != '\0' )
	{
		//	全角
		if( IsEmQuad(*pStr) )
		{
			_AddLetter( pLetter->mapLetter, X, Y, *pStr, *(pStr+1), vWidth, (int)pLetter->Size.x );
			X++;

			pStr += 2;
		}
		//	半角
		else
		{
			//	改行
			if( *pStr == '\n' )
			{
				Y++;
				X = 0;
			}
			
			//	非文字かどうか
			else if( !IsNotLetter( *pStr ) )
			{
				_AddLetter( pLetter->mapLetter, X, Y, *pStr, '\0', vWidth, (int)pLetter->Size.x );
				X++;
			}	
			pStr += 1;
		}
	}
}

//------------------------------------------------------------------------------
//	テキストファイルから文字コード読み出し
//------------------------------------------------------------------------------
void Font::_LoadStringFile( LETTER_SET* pLetter, pstr pPath, const WidthVec& vWidth )
{
	FileHandle Handle( pPath, "r" );
	if( !Handle ) return;

	int X = 0, Y = 0;
	int	Prev	= '\0';
	bool bEmQuad	= false;

	while( true )
	{
		int Char = fgetc( Handle );
		
		//	終了
		if( Char == EOF )
		{
			break;
		}
	
		//	全角文字
		if( bEmQuad )
		{
			_AddLetter( pLetter->mapLetter, X, Y, (char)Prev, (char)Char, vWidth, (int)pLetter->Size.x );
			X++;

			bEmQuad = false;
		}
		//	全角開始
		else if( IsEmQuad(Char) )
		{
			bEmQuad = true;
		}
		//	半角
		else
		{
			//	改行
			if( Char == '\n' )
			{
				Y++;
				X = 0;
			}
			
			//	非文字かどうか
			else if( !IsNotLetter( (char)Char ) )
			{
				_AddLetter( pLetter->mapLetter, X, Y, (char)Char, '\0', vWidth, (int)pLetter->Size.x );
				X++;
			}
		}

		Prev = Char;
	}
}

//------------------------------------------------------------------------------
//	キャラクタコード追加
//------------------------------------------------------------------------------
void Font::_AddLetter( LetterMap& mapLetter, int X, int Y, char Ch, char Next, const WidthVec& vWidth, int SizeW ) const
{
	short Code = MAKE_CODE( Ch, Next );
	LetterMap::const_iterator it = mapLetter.find( Code );
	if( it != mapLetter.end() ) return;

	LETTER Let;
	Let.X		= X;
	Let.Y		= Y;
	Let.Width	= Next == '\0'? _GetWidth( Ch, vWidth, SizeW ) : (char)SizeW;

	mapLetter.insert( LetterMap::value_type(Code, Let) );
}

//------------------------------------------------------------------------------
//	文字の幅を得る
//------------------------------------------------------------------------------
char Font::_GetWidth( char Ch, const WidthVec& vWidth, int SizeW ) const
{
	for( uint i = 0; i < vWidth.size(); i++ )
	{
		const WIDTH& Wid = vWidth[i];

		if( strchr( Wid.Str.c_str(), Ch ) != NULL )
		{
			return Wid.Width;
		}
	}

	return (char)SizeW;
}

}