#pragma once
/*
 *  xLib.h
 *  blTest
 *
 *  Created by xuzhu on 10. 10. 13..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */

#ifdef WIN32
  #ifdef _UNICODE
    #define __TFUNC__ __FUNCTIONW__	// vc2013에서 L__FUNCTION이 빨간줄로 표시되어 보기싫어서 이렇게 한건데 함수명이 안나옴
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

#ifdef min
#undef  min
#endif
#ifdef max
#undef  max
#endif

#include "Types.h"
#ifdef _VER_IPHONE
#include "xString.h"
#endif
#include "Global.h"
#ifdef __cplusplus
#include "XE.h"
#endif
#include "Debug.h"
#include "path.h"
#include "xUtil.h"
#include "Alert.h"
#include "ResMng.h"
#include "xMath.h"
#include "xColor.h"
#include "ConvertString.h"

#include "Timer.h"
#include "Token.h"
#include "XList4.h"
#include "XSurface.h"
#include "XGraphics.h"

