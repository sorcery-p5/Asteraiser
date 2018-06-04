#pragma once

namespace P5
{

//-----------------------------------------------------------------------------
//	�V�F�C�v�p���_
//-----------------------------------------------------------------------------
struct SHAPE_VERTEX
{
	float	x, y, z;
	float	rhw;
	dword	color;
	float	dmy[2];
};


///////////////////////////////////////////////////////////////////////////////
//
//	�V�F�C�v���
//
///////////////////////////////////////////////////////////////////////////////
class Shape : public DrawItem
{
public:
	Shape( int Num );
	virtual ~Shape();

protected:
	virtual void		_OnRender( void )			const;

	void				_SetColor( Color Col );

protected:
	SHAPE_VERTEX*		m_aVertex;
	int					m_Num;
};


}