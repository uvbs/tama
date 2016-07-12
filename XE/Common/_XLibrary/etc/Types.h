﻿/*
 *  Types.h
 *  Skinning
 *
 *  Created by xuzhu on 10. 7. 20..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */

#pragma once
//#ifndef __TYPES_H__
//#define __TYPES_H__

//#define ID		DWORD
typedef unsigned long ID;
#define XUINT64		unsigned long long
#define XINT64		long long
typedef unsigned long		BIT;		///< 비트단위로 사용하는 데이타형
typedef unsigned long xSec;		// 윈도우에선 time_t가 64비트지만 실체로는 32비트만 쓰므로 기기와의 호환을 위해 32비트로 고정함.

//#ifdef _VER_IOS
#ifndef WIN32
typedef struct tagRECT {
	int left, top, right, bottom;
} RECT;
typedef RECT * LPRECT;

//typedef signed char BOOL;		// object-c에선 BOOL이 기본적으로 signed char로 되어있어서 이걸로 같이씀
#define BOOL int
//typedef int BOOL;		// WIN32에선 int라서 통일시키기 위해 바꿔봄
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int UINT;
typedef unsigned long DWORD;
typedef long LONG;
typedef long long LONGLONG;
typedef float FLOAT;
//typedef struct tagLARGE_INTEGER {
//	LONGLONG QuadPart;
//} LARGE_INTEGER;
typedef WORD * LPWORD;
	#ifdef _UNICODE
	//	#define TCHAR unichar
	//	#define LPCTSTR const unichar *		// define으로 한이유는 xcode디버거에서 LPCSTR이라는 형으로 인식해서 디버거창에 문자열이 안보이는 문제때문에...
	//	#define LPTSTR TCHAR *
	#else
		#define TCHAR char
		#define LPCTSTR const char *		// define으로 한이유는 xcode디버거에서 LPCSTR이라는 형으로 인식해서 디버거창에 문자열이 안보이는 문제때문에...
		#define _T(A) A
		#define LPTSTR char *
        #define WCHAR unsigned short
        #define LPCWSTR const unsigned short *		// define으로 한이유는 xcode디버거에서 LPCSTR이라는 형으로 인식해서 디버거창에 문자열이 안보
	#endif
#define CHAR char					// 이것은 EUCKR(멀티바이트)에도 쓸수있고 UTF8에도 쓸수있다
#define LPCSTR const char *
#define LPSTR char *
#define _tstring std::string
#endif // not win32

// UTF16
/*#ifdef _VER_IOS
    #ifndef unichar
    typedef unsigned short unichar;
    #endif
    #define UNICHAR unichar
    #define LPCUSTR const unichar *
    #define LPUSTR unichar *
    #define _U(A)	L##A
#endif */
#ifdef _WIN32
	#define UNICHAR TCHAR
	#define LPCUSTR LPCTSTR
	#define LPUSTR LPTSTR
	#define _U(A)	_T(A)
	#define _tstring std::wstring
#else
#ifdef unichar
#if sizeof(unichar) != 2 
#error "sizeof(unichar) != 2"
#endif
#else
#endif
    #ifndef unichar
    typedef unsigned short unichar;
    #endif
    #define UNICHAR unichar
    #define LPCUSTR const unichar *
    #define LPUSTR unichar *
    #define _UL(A)	L##A
#endif

#define XEPSILON		0.001f
inline int xFloatToInt( float f ) {			// 0.01f가 0.00999999로 되는 문제때문에.
	if( f > 0 )
		return (int)(f + XEPSILON);
	else if( f < 0 )
		return (int)(f - XEPSILON);
	return (int)f;
}

//#endif // __TYPES_H__

