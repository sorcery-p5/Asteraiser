#pragma once


///////////////////////////////////////////////////////////////////////////////
//
//	P5
//
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//	システム
//-----------------------------------------------------------------------------
#ifndef WINVER
#define WINVER _WIN32_WINNT_WINXP
#endif

#ifndef _WIN32_WINNT                   
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#endif						

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS _WIN32_WINNT_WINXP
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

#ifndef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_WINXP
#endif

#define WIN32_LEAN_AND_MEAN

#pragma warning (disable : 4100)
#pragma warning (disable : 4127)
#pragma warning (disable : 4201)
#pragma warning (disable : 4238)
#pragma warning (disable : 4244)
#pragma warning (disable : 4245)
#pragma warning (disable : 4786)
#pragma warning (disable : 4996)

#include <windows.h>
#include <shellapi.h>
#include <dbghelp.h>
#include <mmsystem.h>

#include <vector>
#include <list>
#include <set>
#include <map>
#include <queue>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


//-----------------------------------------------------------------------------
//	型定義
//-----------------------------------------------------------------------------
typedef unsigned char		byte;
typedef unsigned short		word;
typedef unsigned long		dword;
typedef unsigned __int64	qword;
typedef unsigned int		uint;
typedef	const char*			pstr;

namespace P5
{
class Matrix3;
class Angle;
class Point;
class Label;
}

typedef std::vector<int>	IntVec;
typedef std::vector<float>	FloatVec;

//-----------------------------------------------------------------------------
//	定数
//-----------------------------------------------------------------------------
#define	PI				3.1415926535897932384626433832795f
#define	PI2				6.283185307179586476925286766559f
#define	PI05			1.5707963267948966192313216916398f
#define	PI025			0.78539816339744830961566084581988f
#define	DIV_PI_180		0.017453292519943295769236907684886f
#define	DIV_180_PI		57.295779513082320876798154814105f
#define	DIV_PI2_WORD	9.5873799242852576857380474343247e-5f
#define	DIV_WORD_PI2	10430.378350470452724949566316381f
#define	SQ2				0.70710678118654752440084436210485f
#define EPS				0.0001f

//-----------------------------------------------------------------------------
//	インクルード
//-----------------------------------------------------------------------------
#include "Common/Util/Template.h"
#include "Common/Util/HandlePtr.h"
#include "Common/Util/RefPtr.h"
#include "Common/Util/Alert.h"
#include "Common/Arith/Arith.h"
#include "Common/Arith/Rand.h"
#include "Common/Arith/Point.h"
#include "Common/Arith/Angle.h"
#include "Common/Arith/Rect.h"
#include "Common/Arith/Matrix3.h"
#include "Common/Util/Color.h"
#include "Common/Util/Label.h"
#include "Common/Util/String.h"
#include "Common/Util/XML.h"
#include "Common/Util/Memory.h"
#include "Common/Util/Directory.h"
#include "Common/Util/EnumUtil.h"

#include "System/Window.h"
#include "Graphix/Graphix.h"
#include "Sign/Sign.h"
#include "Tone/Tone.h"


//-----------------------------------------------------------------------------
//	公開
//-----------------------------------------------------------------------------
namespace P5
{
	struct INIT_PARAM
	{
		pstr	pName;
		Point	Size;
		bool	bFull;
		uint	IconID;
		uint	IconSmallID;
	};

	bool			Init( HINSTANCE Hinst, const INIT_PARAM& InitParam );
	void			Exit( void );
	bool			Update( void );

	Window*			GetWindow( void );
	Graphix*		GetGraphix( void );
	Sign*			GetSign( void );
	Tone*			GetTone( void );

};