#include "P5.h"

namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	XMLファイル
//
////////////////////////////////////////////////////////////////////////////////
	
//******************************************************************************
//	読み込み
//******************************************************************************
bool XML::Load( pstr pPath )
{
	m_RootNode.Delete();

	FileHandle Handle( pPath, "r" );
	if( !Handle ) return false;

	std::vector< Node* > NodeQueue;
	NodeQueue.push_back( &m_RootNode );

	bool bNode		= false;
	bool bNodeOpen	= false;
	bool bEmQuad	= false;
	bool bComment	= false;
	
	enum
	{
		QUOT_NONE,
		QUOT_SINGLE,
		QUOT_DOUBLE,
	};
	byte Quot		= QUOT_NONE;

	int Prev = 0;
	std::string Buf;

	while( true )
	{
		int Char = fgetc( Handle );

		//	終了
		if( Char == EOF )
		{
			break;
		}

		//	コメント中
		if( bComment )
		{
			//	コメント終了
			if( Char == '>' && Prev == '-' )
			{
				bComment	= false;
				Buf.clear();
			}
		}
		//	ノード未開始
		else if( !bNode )
		{
			//	ノード開始
			if( Char == '<' && Quot == QUOT_NONE )
			{
				bNode		= true;
				bNodeOpen	= true;
				Buf.clear();
			}
		}
		//	ノード開始中
		else
		{
			//	全角文字
			if( bEmQuad )
			{
				Buf += (char)Prev;
				Buf += (char)Char;
				
				bEmQuad = false;
			}
			//	全角の先頭バイト
			else if( IsEmQuad( Char ) )
			{
				bEmQuad = true; 
			}
			//	ノード閉じ
			else if( Char == '>' && Quot == QUOT_NONE )
			{
				//	閉じタグ
				if( Buf[0] == '/' )
				{
					NodeQueue.pop_back();
				}
				else
				{
					//	ノード追加
					Node* pNode = new Node;
					MEM_CHECK( pNode );

					_LoadNode( pNode, Buf.c_str() );
					NodeQueue.back()->AddChild( pNode );

					//	子持ちノードかどうか？
					if( Prev != '/' )
					{
						NodeQueue.push_back( pNode );
					}
				}

				bNode		= false;
				Buf.clear();
			}
			//	通常文字
			else
			{				
				Buf += (char)Char;

				//	クオーテーション
				if( Char == '\'' || Char == '\"' )
				{
					if( Quot == QUOT_SINGLE && Char == '\'' ||
						Quot == QUOT_DOUBLE && Char == '\"' )
					{
						Quot = QUOT_NONE;
					}
					else if( Quot == QUOT_NONE )
					{
						Quot = Char == '\''? QUOT_SINGLE : QUOT_DOUBLE;
					}
				}

				//	ノード開始直後
				if( bNodeOpen )
				{
					//	コメント
					if( Char == '!' )
					{
						bComment	= true;
						bNode		= false;				
						Buf.clear();
					}
				}
			}

			bNodeOpen = false;
		}

		Prev = Char;
	}

	return true;
}

//******************************************************************************
//	書き込み
//******************************************************************************
bool XML::Save( pstr pPath ) const
{
	FileHandle Handle( pPath, "w" );
	if( !Handle ) return false;

	//	書き込み
	for( int i = 0; i < m_RootNode.GetChildNum(); i++ )
	{
		_SaveNode( Handle, m_RootNode.GetChild(i), 0 );
	}

	return true;
}

//------------------------------------------------------------------------------
//	ノードを読み込み
//------------------------------------------------------------------------------
void XML::_LoadNode( Node* pNode, pstr pStr )
{
	enum
	{
		NODE_NAME,
		ATTR_NAME,
		ATTR_VALUE,
	};

	enum
	{
		QUOT_NONE,
		QUOT_SINGLE,
		QUOT_DOUBLE,
	};

	int	 State		= NODE_NAME;
	bool bEmQuad	= false;
	int  Quot		= QUOT_NONE;
	
	Value Val;
	std::string ValName;
	
	char Char = *pStr;
	char Prev = 0;
	std::string Buf;

	while( Char != '\0' )
	{
		Char = *pStr;

		//	全角文字
		if( bEmQuad )
		{
			Buf += Prev;
			Buf += Char;

			bEmQuad =false;
		}
		//	全角の先頭バイト
		else if( IsEmQuad( Char ) )
		{
			bEmQuad = true; 
		}
		//	半角文字
		else
		{
			switch( State )
			{
			case NODE_NAME:
				//	区切り文字
				if( Char == '/' || Char == ' ' || Char == '\t' || Char == '\n' || Char == '\r' || Char == '\0' )
				{
					//	ノード名確定
					if( !Buf.empty() )
					{
						pNode->SetName( Buf.c_str() );

						State = ATTR_NAME;
						Buf.clear();
					}
				}
				//	通常文字
				else
				{
					Buf += Char;
				}
				break;
			case ATTR_NAME:
				//	区切り文字
				if( Char == '/' || Char == ' ' || Char == '=' || Char == '\t' || Char == '\n' || Char == '\r' || Char == '\0' )
				{
					//	アトリビュート名確定
					if( !Buf.empty() )
					{
						ValName = Buf;

						State = ATTR_VALUE;
						Buf.clear();
					}
				}
				//	通常文字
				else
				{
					Buf += Char;
				}
				break;
			case ATTR_VALUE:
				if( Quot == QUOT_NONE )
				{
					//	区切り文字
					if( Char == '/' || Char == ' ' || Char == '=' || Char == '\t' || Char == '\n' || Char == '\r' || Char == '\0' )
					{
						//	アトリビュート追加
						if( !Buf.empty() )
						{
							Val.Set( Buf.c_str() );
							pNode->AddAttr( ValName.c_str(), Val );

							State = ATTR_NAME;
							ValName.clear();
							Buf.clear();
						}
					}
					//	クオーテーション
					else if( Char == '\'' || Char == '\"' )
					{
						Quot = Char == '\''? QUOT_SINGLE : QUOT_DOUBLE;
					}
					//	通常文字
					else
					{
						Buf += Char;
					}
				}
				else
				{
					//	クォーテーション
					if( Quot == QUOT_SINGLE && Char == '\'' ||
						Quot == QUOT_DOUBLE && Char == '\"' )
					{
						//	アトリビュート追加
						Val.Set( Buf.c_str() );
						pNode->AddAttr( ValName.c_str(), Val );

						Quot  = QUOT_NONE;
						State = ATTR_NAME;
						ValName.clear();
						Buf.clear();
					}
					//	通常文字
					else
					{
						Buf += Char;
					}
				}
				break;
			}
		}

		Prev = Char;
		pStr++;
	}
}

//------------------------------------------------------------------------------
//	ノードを書き込み
//------------------------------------------------------------------------------
void XML::_SaveNode( FileHandle& Handle, const Node* pNode, int Indent ) const
{
	//	インデント追加
	std::string StrIndent;
	for( int i = 0; i < Indent; i++ )
	{
		StrIndent += '\t';
	}

	//	自身を書き込む
	fprintf( Handle, "%s%s\n", StrIndent.c_str(), pNode->GetNodeStr(Indent).c_str() );

	//	子供の追加
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		_SaveNode( Handle, pNode->GetChild(i), Indent + 1 );
	}
	
	//	閉じノード
	if( !pNode->IsChildEmpty() )
	{
		fprintf( Handle, "%s</%s>\n", StrIndent.c_str(), pNode->GetName() );
	}
}


////////////////////////////////////////////////////////////////////////////////
//
//	XML:ノード
//
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	名前から子ノードを検索
//******************************************************************************
const XML::Node* XML::Node::FindChild( pstr pName, const Node* pStartNode ) const
{
	for( VecNode::const_iterator it = m_vChild.begin(); it != m_vChild.end(); ++it )
	{
		if( pStartNode )
		{
			if( pStartNode == *it )
			{
				pStartNode = NULL;
			}
		}
		else
		{
			if( stricmp((*it)->GetName(), pName) == 0 )
			{
				return *it;
			}
		}
	}
	return NULL;
}

//******************************************************************************
//	アトリビュートを検索
//******************************************************************************
const XML::Value* XML::Node::FindAttr( pstr pName ) const
{
	for( VecAttr::const_iterator it = m_vAttr.begin(); it != m_vAttr.end(); ++it )
	{
		if( stricmp(it->Name.c_str(), pName) == 0)
		{
			return &it->Val;
		}
	}
	return NULL;
}

//******************************************************************************
//	アトリビュートを取得
//******************************************************************************
XML::Value XML::Node::GetAttr( pstr pName, pstr pDefault ) const
{
	for( VecAttr::const_iterator it = m_vAttr.begin(); it != m_vAttr.end(); ++it )
	{
		if( stricmp(it->Name.c_str(), pName) == 0 )
		{
			return it->Val;
		}
	}
	return Value( pDefault );
}

//******************************************************************************
//	ノード書式文字列の取得
//******************************************************************************
std::string XML::Node::GetNodeStr( int Indent ) const
{
	std::string Str;

	//	開始
	Str += "<";

	//	ノード名を追加
	Str += m_Name;
	
	//	アトリビュートを追加
	int Len = 0;
	for( VecAttr::const_iterator it = m_vAttr.begin(); it != m_vAttr.end(); ++it )
	{
		const Value& Val = it->Val;
		Str += " ";
		Str += it->Name.c_str();
		Str += " = \"";
		Str += Val.GetStr();
		Str += "\"";

		Len += strlen( it->Name.c_str() ) + strlen( Val.GetStr() );

		//	改行
		if( Len >= 80 )
		{
			Str += "\n";
			
			for( int i = 0; i < Indent + 1; i++ )
			{
				Str += '\t';
			}

			Len = 0;
		}
	}

	//	終端
	if( m_vChild.empty() )	Str += " />";
	else					Str += ">";

	return Str;
}

}
