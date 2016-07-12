#pragma once
/**
 게임개발용 프레임워크
*/
// 클라이언트 엔진에만 필요한 define을 넣으시오
#define _CLIENT
#define _XDYNA_RESO
//#define _SPR_USE_LUA
#define _XUSE_LUA
#define _XHSL2					// HSL알고리즘에 색범위가 추가된 버전.

#include "xLib.h"

#ifdef _VER_DX
  #include <d3d9.h>
#endif
#ifdef WIN32
#include <mmsystem.h>
  #ifdef NDEBUG
    #include "BugslayerUtil.h"
  #endif
#endif

#ifdef __cplusplus
#include <map>
#include <string>
#include <list>
#include <memory>

#include "XE3d.h"
#include "etc/XGraphics.h"
#include "XImage.h"
#include "XImageMng.h"
//#include "../XDrawGraph.h"
//#include "XFramework/client/XEContent.h"
//#include "XFramework/client/XClientMain.h"
#include "XFramework/XTextTable.h"
#include "XFramework/client/XClientMain.h"
#endif // c++

#ifdef WIN32
	#ifdef _XLIBPNG
	  #pragma comment( lib, "../../../../../XE/Third_Party_lib/libpng_win/lib/libpng.lib" )
	#endif
	#ifdef _XOOLONG
		#ifdef _DEBUG
			#pragma comment( lib, "../../../../../XE/bin/Debug/WinOolong.lib" )
		#else
			#pragma comment( lib, "../../../../../XE/bin/Release/WinOolong.lib" )
		#endif
	#endif // _XOOLONG
	#ifdef _DEBUG
	  #pragma comment( lib, "../../../../../XE/bin/MTDLL_Debug/xzlibd.lib" )
	#else
	  #pragma comment( lib, "../../../../../XE/bin/MTDLL_Release/xzlib.lib" )
	#endif
	#ifdef _XFTGL
		#ifdef _DEBUG
			#pragma comment( lib, "../../../../../XE/bin/MTDLL_Debug/Freetype2_d.lib" )
			#pragma comment( lib, "../../../../../XE/bin/MTDLL_Debug/ftgl_d.lib" )
		#else
			#pragma comment( lib, "../../../../../XE/bin/MTDLL_Release/Freetype2.lib" )
			#pragma comment( lib, "../../../../../XE/bin/MTDLL_Release/ftgl.lib" )
		#endif
	#endif
#endif
#ifdef _XEIGEN
	#ifdef _DEBUG
	  #pragma comment( lib, "../../../../../XE/bin/MTDLL_Debug/eigen_d.lib" )
	#else
	  #pragma comment( lib, "../../../../../XE/bin/MTDLL_Release/eigen.lib" )
	#endif
#endif
