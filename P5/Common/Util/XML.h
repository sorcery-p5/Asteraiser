#pragma once

namespace P5
{

////////////////////////////////////////////////////////////////////////////////
//
//	XMLファイル
//
////////////////////////////////////////////////////////////////////////////////
class XML
{
public:
	//------------------------------------------------------------------------------
	//	値
	//------------------------------------------------------------------------------
	class Value
	{
	public:
		Value( void )										{}
		Value( pstr pStr )									{ m_Str = pStr; }
		Value( int Arg )									{ Set( Arg ); }
		Value( uint Arg )									{ Set( Arg ); }
		Value( float Arg )									{ Set( Arg ); }
		Value( bool Arg )									{ Set( Arg ); }
		Value( Point Arg )									{ Set( Arg ); }
		Value( Rect Arg )									{ Set( Arg ); }
		Value( Color Arg )									{ Set( Arg ); }
		
		void		Set( pstr pStr )						{ m_Str = pStr; }
		void		Set( int Arg )							{ String256 s; s.Format( "%d", Arg ); m_Str = s.c_str(); }
		void		Set( uint Arg )							{ String256 s; s.Format( "%d", Arg ); m_Str = s.c_str(); }
		void		Set( float Arg )						{ String256 s; s.Format( "%f", Arg ); m_Str = s.c_str(); }
		void		SetDegree( float Arg )					{ String256 s; s.Format( "%f", Angle::ToDeg(Arg) ); m_Str = s.c_str(); }
		void		Set( bool Arg )							{ m_Str = Arg? "true" : "false"; }
		void		Set( Point Arg )						{ String256 s; s.Format( "%f,%f", Arg.x, Arg.y ); m_Str = s.c_str(); }
		void		Set( Rect Arg )							{ String256 s; s.Format( "%f,%f,%f,%f", Arg.l, Arg.t, Arg.r, Arg.b ); m_Str = s.c_str(); }
		void		Set( Color Arg )						{ String256 s; s.Format( "%d,%d,%d,%d", Arg.r, Arg.g, Arg.b, Arg.a ); m_Str = s.c_str(); }

		bool		IsEmpty( void )					const	{ return m_Str.empty(); }
		pstr		GetStr( void )					const	{ return m_Str.c_str(); }
		void		GetStr( char* pStr )			const	{ strcpy( pStr, m_Str.c_str() ); }
		bool		GetBool( void )					const	{ return stricmp( m_Str.c_str(), "true" ) == 0; }
		BOOL		GetBOOL( void )					const	{ return GetBool()? TRUE : FALSE; }
		int			GetInt( void )					const	{ return atoi( m_Str.c_str() ); }
		float		GetFloat( void )				const	{ return (float)atof( m_Str.c_str() ); }
		float		GetDegree( void )				const	{ return Angle::ToRad( GetFloat() ); }
		Point		GetPoint( void )				const	{ Point pt; sscanf( m_Str.c_str(), "%f,%f", &pt.x, &pt.y ); return pt; }
		Rect		GetRect( void )					const	{ Rect rc; sscanf( m_Str.c_str(), "%f,%f,%f,%f", &rc.l, &rc.t, &rc.r, &rc.b ); return rc; }
		Color		GetColor( void )				const	{ int a[4] = {0,0,0,255}; sscanf( m_Str.c_str(), "%d,%d,%d,%d", &a[0], &a[1], &a[2], &a[3] ); return Color(a); }
		
		Label		GetLabel( void )				const	{ return Label( m_Str.c_str() ); }
		void		GetLabelStr( LabelStr pStr )	const	{ Label( m_Str.c_str() ).GetStr( pStr ); }

	private:
		std::string	m_Str;
	};

	//------------------------------------------------------------------------------
	//	ノード
	//------------------------------------------------------------------------------
	class Node
	{
	public:
		Node( void )											{}
		Node( pstr pName )										{ m_Name = pName; }
		~Node()													{ DeleteVec( m_vChild ); }

		void			Delete( void )							{ DeleteVec( m_vChild ); }

		void			SetName( pstr pName )					{ m_Name = pName; }
		void			AddChild( Node* pChild )				{ m_vChild.push_back( pChild ); }
		void			AddAttr( pstr pName, const Value& Val )	{ ATTR Attr = { pName, Val }; m_vAttr.push_back( Attr ); }

		pstr			GetName( void )					const	{ return m_Name.c_str(); }
		bool			IsChildEmpty( void )			const	{ return m_vChild.empty(); } 
		int				GetChildNum( void )				const	{ return (int)m_vChild.size(); }
		Node* 			GetChild( int Index )					{ return m_vChild[Index]; } 
		const Node* 	GetChild( int Index )			const	{ return m_vChild[Index]; }

		const Node* 	FindChild( pstr pName, const Node* pStartNode = NULL )	const;
		
		int				GetAttrNum( void )				const	{ return (int)m_vAttr.size(); }
		pstr			GetAttrName( int Index )		const	{ return m_vAttr[Index].Name.c_str(); }
		const Value&	GetAttrVal( int Index )			const	{ return m_vAttr[Index].Val; }

		const Value*	FindAttr( pstr pName )						const;
		Value			GetAttr( pstr pName, pstr pDefault = "" )	const;

		std::string		GetNodeStr( int Indent )					const;

	private:
		typedef std::vector< Node* > VecNode;

		struct ATTR
		{
			std::string		Name;
			Value			Val;
		};
		typedef std::vector< ATTR > VecAttr;

	private:
		std::string	m_Name;
		VecAttr		m_vAttr;
		VecNode		m_vChild;
	};

public:
	XML( void )													{}
	~XML()														{}

	bool				Load( pstr pPath );
	bool				Save( pstr pPath )				const;

	Node&				GetRoot( void )							{ return m_RootNode; }
	const Node&			GetRoot( void )					const	{ return m_RootNode; }

private:
	void				_LoadNode( Node* pNode, pstr pStr );
	void				_SaveNode( FileHandle& Handle, const Node* pNode, int Indent )	const;

private:
	Node				m_RootNode;
};

}