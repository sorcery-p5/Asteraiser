#pragma once

#include "CollideTypes.h"


////////////////////////////////////////////////////////////////////////////////
//
//	当たりオブジェクトのリスナー
//
////////////////////////////////////////////////////////////////////////////////
class CollideObjectListener
{
public:
	virtual bool		IsHit( const COLLIDE_OWNER& Owner )		const	{ return true; }
};
