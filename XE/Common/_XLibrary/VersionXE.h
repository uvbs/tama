//
//  VersionXE.h
//  xe
//
//  Created by JungWoo Sung on 13. 6. 2..
//  Copyright (c) 2013년 JungWoo Sung. All rights reserved.
//
#pragma once

#pragma warning( disable : 4189 )	//  지역 변수가 초기화되었으나 참조되지 않았습니다.
#pragma warning( disable : 4100 )	//  참조되지 않은 정식 매개 변수입니다.
#pragma warning( disable : 4100 )	//  참조되지 않은 정식 매개 변수입니다.

//#include "MyVersionXE.h"

#ifdef __APPLE__
#include <TargetConditionals.h>
#pragma message("build apple")
#endif

//#define _XMASTER		// 정섭용 마스터본 빌드

// 클라 서버엔진 공통 define
#ifdef _SERVER
  #define _XDEBUG
#else
  #ifndef _XMASTER
	#define _XDEBUG
  #endif
#endif
#ifndef _XMASTER
  #define _DEV
#endif
#define _XGAME		// 툴과 구분짓는 디파인

#ifdef TARGET_OS_IPHONE
	#pragma message("xe build iOS")
	#define _VER_IOS
#endif // IOS

#ifdef ANDROID
//	#pragma message("xe build android")
	#define _VER_ANDROID
#endif // android
#if defined(WIN32)
	#if defined(XE_GLES2)	// Debug_GLES2 빌드에 정의되어있음.
		#define _VER_OPENGL
	#else
		#define _VER_DX
	#endif
#elif defined(_VER_IOS) || defined(_VER_ANDROID)
//	#pragma message("xe - opengl")
	#define	_VER_OPENGL
	//#define _XVAO   // vertex array object(OES)사용여부
	#define _XUTF8
#endif

// 치트 기능 ON/OFF
#ifdef WIN32
  #define _CHEAT				// 치트 기능들
#elif defined(_VER_ANDROID) || defined(_VER_IOS)
  #ifndef _XMASTER
	#define _CHEAT				// 치트 기능들
  #endif
#endif

// 클라이언트 서버 모두에게 필요한 define을 넣으시오
#ifdef _VER_IOS
	#define _BUNDLE
	#ifdef _XDEBUG
		#define _XDEBUG_ALLOC
	#endif
	#define _LOG_ALERT			// 로그가 발생할때 UIAlertView로 보여줄지 말지
	#define _XFTGL
	#define _XBSD_SOCKET
//	#define _XPATCH				// 패치서버버전으로 빌드하려면 주석을 해제하시오
#endif // iOS

#ifdef _VER_ANDROID
	#define _LOG_ALERT			// 로그가 발생할때 UIAlertView로 보여줄지 말지
	#define _XFTGL
	#define _XOOLONG
//	#define _XPATCH				// 패치서버버전으로 빌드하려면 주석을 해제하시오
#endif // android

#ifdef WIN32
  #ifdef _VER_OPENGL
  	#define _XFTGL
    #define _XOOLONG
  #endif
//	#define _XPATCH				// 패치서버버전으로 빌드하려면 주석을 해제하시오
#endif // win32

//#define _XPROFILE			// 프로파일링 테스트
#if defined(WIN32) && defined(_XFTGL)
#define FTGL_LIBRARY_STATIC
#endif

#define TIXML_USE_STL

#define _XPACKAGE
#define _XHEART_BEAT		//
#define _XRECONNECT
//#define _XMODE_TOP_CUT		// 게임뷰 상단을 잘라버리는 모드(거울에만 적용)Debug_AccRef빌드에 정의되어 있다.

#define _XSOUND2			// sound.txt를 사용하는 버전
#define _XNEW_WND_TEXT		// WndTextString 동적할당 버전
#define _XCRYPT_PACKET	// 패킷암호화
#define _XBUG_140123_SPR	// 스프라이트 링크오브젝트 알파 오류 버그수정
#define _XINLINE_ARRAY
#define _XWIN_ZERO_SIZE		// 윈도우사이즈가 0일때 터치이벤트를 true로 통과한다.
#define _XACCOUNT_REF		// XDBAccount객체 RefCnt방식으로...
#define _XSPRITE2			// 스프라이트 객체를 서피스 상속방식에서 서피스 포함방식으로 변경
#define _XPACKET2			// XPacket클래스 포맷 바뀜
#define _XSKILL_SYS			// 
#define _XLIBPNG			// png로더를 libpng로 바꾼다. 안드로이드나 ios는 이것이 기본이다.
#define _XASYNC_SPR		// 스레드 비동기 로딩
//#define _XSPR_LAZY_LOAD		// 리소스 없으면 일단찍고 나중에 로드하는 버전
#define _XSPR_LOAD2			// 로딩루팅 개선한 버전

#define DLV_LOCAL					1		// 개발자 개인자리의 개발하는 서버
#define DLV_DEV_PERSONAL	2		// 개발자 개인용
#define DLV_DEV_CREW			3		// 192번 개발섭(개발자만 접속)
#define DLV_DEV_EXTERNAL	4		// 외부공개용 개발 테섭(외부관계자 접속용)
#define DLV_OPEN_BETA			5		// 오픈베타(일반유저용)
#define DLV_MASTER				6		// 정식버전

// 아 슈발 버전이 너무 많아진다. 좀더 간단한 방법을...
#if defined(_XGMO)
#define _DEV_LEVEL	DLV_OPEN_BETA
#elif defined(_X192)	// 192번 개발서버 & 접속용
#define _DEV_LEVEL	DLV_DEV_CREW
#elif defined(_XCREW)	// 개발자 개인용 빌드
#define _DEV_LEVEL	DLV_DEV_PERSONAL
#else
#define _DEV_LEVEL	DLV_LOCAL
#endif


#ifdef _XUZHU
  #if !defined(_VER_DX) && defined(WIN32)
// 	#define _XGLM				// 수학라이브러리 GLM으로 교체
  #endif
//  #define _XOBJMNG_OPTIMIZE	// 오브젝트 매니저 XList3로 쓰는 버전
  #define _XPROFILE
#endif

#ifdef _JAEBUM
#endif


#undef min
#undef max












