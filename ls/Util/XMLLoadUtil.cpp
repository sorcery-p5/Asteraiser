#include "stdafx.h"
#include "XMLLoadUtil.h"

#include "Resource/ResourceManager.h"
#include "Resource/TextureResource.h"
#include "Resource/FontResource.h"

namespace
{
	pstr	NODE_RECT			= "Rect";
	pstr	NODE_PAGE			= "Page";
	pstr	NODE_ARRAY			= "Array";
	
	pstr	ATTR_TRANS			= "Trans";
	pstr	ATTR_PATH			= "Path";
	pstr	ATTR_NAME			= "Name";
	pstr	ATTR_TEX			= "Tex";
	pstr	ATTR_RECT			= "Rect";
	pstr	ATTR_STRIDE			= "Stride";
	pstr	ATTR_LINE			= "Line";
	pstr	ATTR_MAX			= "Max";
	pstr	ATTR_PIECE			= "Piece";
}

////////////////////////////////////////////////////////////////////////////////
//
//	XML�ǂݍ��݃��[�e�B���e�B
//
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//	�e�N�X�`���m�[�h�̓ǂݍ���
//******************************************************************************
void LoadTexNode( TexResMap& mapTexRes, ResourceManager* pResMng, const XML::Node* pNode )
{
	_ASSERT( pNode );

	Color DefaultTrans = Color::Zero();

	const XML::Value* pTransVal = pNode->FindAttr( ATTR_TRANS );
	if( pTransVal && !pTransVal->IsEmpty() ) DefaultTrans = pTransVal->GetColor();
	
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pFileNode = pNode->GetChild( i );

		//	�����F�̐ݒ�
		Color Trans( DefaultTrans );
		const XML::Value* pTransVal = pFileNode->FindAttr( ATTR_TRANS );
		if( pTransVal && !pTransVal->IsEmpty() ) Trans = pTransVal->GetColor();

		P5::GetGraphix()->GetDevice()->SetTransparentColor( Trans );

		//	�e�N�X�`���ǂݍ���
		const XML::Value* pValue = pFileNode->FindAttr( ATTR_PATH );
		if( pValue && !pValue->IsEmpty() )
		{
			TextureResource* pTexRes = pResMng->LoadResource<TextureResource>( pValue->GetStr() );

			if( pTexRes )
			{
				mapTexRes.insert( TexResMap::value_type( pTexRes->GetName(), pTexRes ) );
			}
		}
	}
}

//******************************************************************************
//	�t�H���g�m�[�h�̓ǂݍ���
//******************************************************************************
void LoadFontNode( FontResMap& mapFontRes, ResourceManager* pResMng, const XML::Node* pNode )
{
	_ASSERT( pNode );
	
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pFileNode = pNode->GetChild( i );

		//	�t�H���g�ǂݍ���
		const XML::Value* pValue = pFileNode->FindAttr( ATTR_PATH );
		if( pValue && !pValue->IsEmpty() )
		{
			FontResource* pFontRes = pResMng->LoadResource<FontResource>( pValue->GetStr() );

			if( pFontRes )
			{
				mapFontRes.insert( FontResMap::value_type( pFontRes->GetName(), pFontRes ) );
			}
		}
	}
}

//******************************************************************************
//	�e�N�X�`���s�[�X�m�[�h�̓ǂݍ���
//******************************************************************************
void LoadPieceNode( PieceMap& mapPiece, const XML::Node* pNode )
{
	_ASSERT( pNode );
	
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pPieceNode = pNode->GetChild( i );

		Piece*	pPiece	= 0;
		Label	Name	= pPieceNode->GetAttr( ATTR_NAME ).GetStr();
		Label	TexName	= pPieceNode->GetAttr( ATTR_TEX ).GetStr();
		if( Name.IsEmpty() ) continue;

		//	��`�s�[�X
		if( stricmp( pPieceNode->GetName(), NODE_RECT ) == 0 )
		{
			Rect Rc = pPieceNode->GetAttr( ATTR_RECT ).GetRect();

			pPiece = new RectPiece( TexName, Rc );
		}
		//	�y�[�W�s�[�X
		else if( stricmp( pPieceNode->GetName(), NODE_PAGE ) == 0 )
		{
			Rect Rc			= pPieceNode->GetAttr( ATTR_RECT ).GetRect();
			Point Stride	= pPieceNode->GetAttr( ATTR_STRIDE ).GetPoint();
			int	Line		= pPieceNode->GetAttr( ATTR_LINE ).GetInt();
			int	Max			= pPieceNode->GetAttr( ATTR_MAX ).GetInt();
			
			pPiece = new PagePiece( TexName, Rc, Stride, Line, Max );
		}
		//	�z��s�[�X
		else if( stricmp( pPieceNode->GetName(), NODE_ARRAY ) == 0 )
		{
			std::vector<Rect> vRect;
			for( int RectID = 0; RectID < pPieceNode->GetChildNum(); RectID++ )
			{
				//	��`�m�[�h�����o���B
				const XML::Node* pRectNode = pPieceNode->GetChild( RectID );

				const XML::Value* pValue = pRectNode->FindAttr( ATTR_RECT );
				if( pValue && !pValue->IsEmpty() )
				{
					vRect.push_back( pValue->GetRect() );
				}
			}

			if( !vRect.empty() )
			{
				pPiece = new ArrayPiece( TexName, &vRect[0], (int)vRect.size() );
			}
		}

		//	�o�^
		if( pPiece )
		{
			mapPiece.insert( PieceMap::value_type( Name, pPiece ) );
		}
	}
}