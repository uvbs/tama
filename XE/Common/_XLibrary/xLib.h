#pragma once

/**
 X Engine의 최상위 헤더
 게임프레임워크, 콘솔프레임워크, WIN32프레임워크, MFC프레임워크등 모든 프레임워크의 공통헤더만 들어간다.
*/

#define _XE
#include "VersionXE.h"
#include "xLibResPath.h"

#ifdef WIN32
	#define WIDEN2(x) L ## x	// 안되면 원래대로 L ## #x로 돌려놓을것.
	#define WIDEN(x) WIDEN2(x)
	#ifdef _UNICODE
		#define __TFUNC__ WIDEN(__FUNCTION__)	// vc2013에서 L__FUNCTION이 빨간줄로 표시되어 보기싫어서 이렇게 한건데 함수명이 안나옴
	#else
		#define __TFUNC__ __FUNCTION__
	#endif
// 	#ifdef _UNICODE
// 		#define __TFUNC__ _T(__FUNCTION__)
// 	#else
// 		#define __TFUNC__ __FUNCTION__
// 	#endif
#else
	#define __TFUNC__ __FUNCTION__
#endif

#ifdef _VER_IOS
	#define _XUTF8
#endif
#ifndef WIN32
#undef min
#undef max
#endif // not win32

#if defined(_VER_DX) && defined(_VER_OPENGL)
#error "_VER_DX와 _VER_OPENGL은 함께 쓸 수 없음."
#endif // defined(_VER_DX) && defined(_VER_OPENGL)

#ifdef __cplusplus
#include <string>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>
#include <random>
#include <tuple>
#include <atomic>
#include <functional>
#include <memory>
#include <random>
#ifdef _VER_OPENGL
//	#include "OpenGL2/XOpenGL.h"	// 엔진단에서만 호출되게 해야한다 게임쪽 프로젝트에서 이게 직접적으로 불려져선 안된다.
#endif
#include "etc/types.h"
#include "etc/Global.h"
#include "xeDef.h"
#include "XE.h"
//#ifdef _CLIENT
//#include "XE3d.h"
//#endif
#include "etc/Debug.h"
#ifndef WIN32
#include "etc/iOS/xStringiOS.h"
#endif
#include "etc/path.h"
#include "etc/xUtil.h"
#include "etc/Alert.h"
#include "etc/xLang.h"
//#include "XResObj.h"
//#include "XResMng.h"
#include "XLibrary.h"
#include "etc/xMath.h"
#include "etc/xColor.h"
#include "etc/ConvertString.h"
#include "etc/ContainerSTL.h"
//#include "XSystem.h"

#include "etc/Timer.h"
#include "etc/XTimer2.h"
#include "etc/Token.h"
#include "XList.h"
#include "XFramework/XTextTable.h"
#include "XFramework/XConstant.h"
//#include "XPool.h"
#include "XFramework/XEProfile.h"
#include "../XGameUtil.h"
#include "XArchive.h"
/*
#ifdef WIN32
#include "tinyxml/tinyxml.h"
#else
#include "tinyxml/tinyxml.h"
#endif
*/
#endif // c++
//#pragma comment( lib, "tinyxml.lib" )

