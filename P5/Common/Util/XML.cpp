#include "P5.h"

namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	XML�t�@�C��
//
////////////////////////////////////////////////////////////////////////////////
	
//******************************************************************************
//	�ǂݍ���
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

		//	�I��
		if( Char == EOF )
		{
			break;
		}

		//	�R�����g��
		if( bComment )
		{
			//	�R�����g�I��
			if( Char == '>' && Prev == '-' )
			{
				bComment	= false;
				Buf.clear();
			}
		}
		//	�m�[�h���J�n
		else if( !bNode )
		{
			//	�m�[�h�J�n
			if( Char == '<' && Quot == QUOT_NONE )
			{
				bNode		= true;
				bNodeOpen	= true;
				Buf.clear();
			}
		}
		//	�m�[�h�J�n��
		else
		{
			//	�S�p����
			if( bEmQuad )
			{
				Buf += (char)Prev;
				Buf += (char)Char;
				
				bEmQuad = false;
			}
			//	�S�p�̐擪�o�C�g
			else if( IsEmQuad( Char ) )
			{
				bEmQuad = true; 
			}
			//	�m�[�h��
			else if( Char == '>' && Quot == QUOT_NONE )
			{
				//	���^�O
				if( Buf[0] == '/' )
				{
					NodeQueue.pop_back();
				}
				else
				{
					//	�m�[�h�ǉ�
					Node* pNode = new Node;
					MEM_CHECK( pNode );

					_LoadNode( pNode, Buf.c_str() );
					NodeQueue.back()->AddChild( pNode );

					//	�q�����m�[�h���ǂ����H
					if( Prev != '/' )
					{
						NodeQueue.push_back( pNode );
					}
				}

				bNode		= false;
				Buf.clear();
			}
			//	�ʏ핶��
			else
			{				
				Buf += (char)Char;

				//	�N�I�[�e�[�V����
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

				//	�m�[�h�J�n����
				if( bNodeOpen )
				{
					//	�R�����g
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
//	��������
//******************************************************************************
bool XML::Save( pstr pPath ) const
{
	FileHandle Handle( pPath, "w" );
	if( !Handle ) return false;

	//	��������
	for( int i = 0; i < m_RootNode.GetChildNum(); i++ )
	{
		_SaveNode( Handle, m_RootNode.GetChild(i), 0 );
	}

	return true;
}

//------------------------------------------------------------------------------
//	�m�[�h��ǂݍ���
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

		//	�S�p����
		if( bEmQuad )
		{
			Buf += Prev;
			Buf += Char;

			bEmQuad =false;
		}
		//	�S�p�̐擪�o�C�g
		else if( IsEmQuad( Char ) )
		{
			bEmQuad = true; 
		}
		//	���p����
		else
		{
			switch( State )
			{
			case NODE_NAME:
				//	��؂蕶��
				if( Char == '/' || Char == ' ' || Char == '\t' || Char == '\n' || Char == '\r' || Char == '\0' )
				{
					//	�m�[�h���m��
					if( !Buf.empty() )
					{
						pNode->SetName( Buf.c_str() );

						State = ATTR_NAME;
						Buf.clear();
					}
				}
				//	�ʏ핶��
				else
				{
					Buf += Char;
				}
				break;
			case ATTR_NAME:
				//	��؂蕶��
				if( Char == '/' || Char == ' ' || Char == '=' || Char == '\t' || Char == '\n' || Char == '\r' || Char == '\0' )
				{
					//	�A�g���r���[�g���m��
					if( !Buf.empty() )
					{
						ValName = Buf;

						State = ATTR_VALUE;
						Buf.clear();
					}
				}
				//	�ʏ핶��
				else
				{
					Buf += Char;
				}
				break;
			case ATTR_VALUE:
				if( Quot == QUOT_NONE )
				{
					//	��؂蕶��
					if( Char == '/' || Char == ' ' || Char == '=' || Char == '\t' || Char == '\n' || Char == '\r' || Char == '\0' )
					{
						//	�A�g���r���[�g�ǉ�
						if( !Buf.empty() )
						{
							Val.Set( Buf.c_str() );
							pNode->AddAttr( ValName.c_str(), Val );

							State = ATTR_NAME;
							ValName.clear();
							Buf.clear();
						}
					}
					//	�N�I�[�e�[�V����
					else if( Char == '\'' || Char == '\"' )
					{
						Quot = Char == '\''? QUOT_SINGLE : QUOT_DOUBLE;
					}
					//	�ʏ핶��
					else
					{
						Buf += Char;
					}
				}
				else
				{
					//	�N�H�[�e�[�V����
					if( Quot == QUOT_SINGLE && Char == '\'' ||
						Quot == QUOT_DOUBLE && Char == '\"' )
					{
						//	�A�g���r���[�g�ǉ�
						Val.Set( Buf.c_str() );
						pNode->AddAttr( ValName.c_str(), Val );

						Quot  = QUOT_NONE;
						State = ATTR_NAME;
						ValName.clear();
						Buf.clear();
					}
					//	�ʏ핶��
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
//	�m�[�h����������
//------------------------------------------------------------------------------
void XML::_SaveNode( FileHandle& Handle, const Node* pNode, int Indent ) const
{
	//	�C���f���g�ǉ�
	std::string StrIndent;
	for( int i = 0; i < Indent; i++ )
	{
		StrIndent += '\t';
	}

	//	���g����������
	fprintf( Handle, "%s%s\n", StrIndent.c_str(), pNode->GetNodeStr(Indent).c_str() );

	//	�q���̒ǉ�
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		_SaveNode( Handle, pNode->GetChild(i), Indent + 1 );
	}
	
	//	���m�[�h
	if( !pNode->IsChildEmpty() )
	{
		fprintf( Handle, "%s</%s>\n", StrIndent.c_str(), pNode->GetName() );
	}
}


////////////////////////////////////////////////////////////////////////////////
//
//	XML:�m�[�h
//
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	���O����q�m�[�h������
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
//	�A�g���r���[�g������
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
//	�A�g���r���[�g���擾
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
//	�m�[�h����������̎擾
//******************************************************************************
std::string XML::Node::GetNodeStr( int Indent ) const
{
	std::string Str;

	//	�J�n
	Str += "<";

	//	�m�[�h����ǉ�
	Str += m_Name;
	
	//	�A�g���r���[�g��ǉ�
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

		//	���s
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

	//	�I�[
	if( m_vChild.empty() )	Str += " />";
	else					Str += ">";

	return Str;
}

}
