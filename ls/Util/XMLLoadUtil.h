#pragma once

class ResourceManager;
class TextureResource;
class FontResource;
typedef std::map< Label, TextureResource* > TexResMap;
typedef std::map< Label, FontResource* >	FontResMap;


////////////////////////////////////////////////////////////////////////////////
//
//	XML読み込みユーティリティ
//
////////////////////////////////////////////////////////////////////////////////

void LoadTexNode( TexResMap& mapTexRes, ResourceManager* pResMng, const XML::Node* pNode );
void LoadFontNode( FontResMap& mapFontRes, ResourceManager* pResMng, const XML::Node* pNode );
void LoadPieceNode( PieceMap& mapPiece, const XML::Node* pNode );