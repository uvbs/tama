/*
 *  String.h
 *  Skinning
 *
 *  Created by xuzhu on 10. 7. 20..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#pragma once
#ifndef WIN32
#include "etc/Types.h"
#include "etc/debug.h"
#include <sys/types.h>
#include <sys/stat.h>

char* strlwr( CHAR* pstring );
char* strupr( CHAR* pstring );
int stricmp( LPCSTR str1, LPCSTR str2);
void strrev( char *str );

inline int fopen_s( FILE **fpp, const char *filename, const char *mode ) {
	*fpp = fopen( filename, mode );
    return (*fpp)? 1 : 0;
}

inline char* strcpy_s( char *dst, int size, const char *src ) {
  #ifdef _XDEBUG
	if( XBREAKF( strlen(src)+1 > size, "src:%s(%d)", src, strlen(src)+1 ) )
		return NULL;
  #endif
	return strcpy( dst, src );
}

inline char* strcat_s( char *dst, int size, const char *src ) {
  #ifdef _XDEBUG
	if( XBREAKF( strlen(src)+strlen(dst)+1 > size, "src:%s(%d)", src, strlen(src)+strlen(dst)+1 ) )
		return NULL;
  #endif
	return strcat( dst, src );
}

inline void memcpy_s( void *dst, int dstSize, const void *src, int srcSize ) {
	memcpy( dst, src, srcSize );
}

inline char* strlwr_s( char *str, size_t sizeInWord ) {
    return strlwr( str );
}

int wcslen( const unichar* szStr );

inline int _tmkdir( const char *path ) {
    return mkdir( path, 0777 );
}

//inline char* strcpy_s( char *dst, const char *src ) {
//	return strcpy( dst, src );
//} 
//#define strcpy_s	strcpy
void sprintf_s( char *cOut, int len, const char *cFormat, ... );
void sprintf_s( char *cOut, const char *cFormat, ... );
//#define sprintf_s	sprintf
//#define strcat_s	strcat 
//#ifdef __cplusplus
template <size_t size>
char* strcpy_s(
				 char (&strDestination)[size],
				 const char *strSource 
				 ) // C++ only
{
	return strcpy_s( strDestination, size, strSource );
}

template <size_t size>
char* strcat_s(
			   char (&strDestination)[size],
			   const char *strSource 
			   ) // C++ only
{
	return strcat_s( strDestination, size, strSource );
}

template <size_t size>
char* strlwr_s( char (&str)[size] ) // C++ only
{
	return strlwr_s( str, size );
}

//#endif
#define strcmpi stricmp
#ifdef _UNICODE
//	#include "xwString.h"
//	#define _tcsicmp wcscmp		// 유니코드버전에선 파일명을 모두 대소문자를 구분해서 사용하게 하고 tcsicmp를 사용하지 않는다
	#define _tcscmp_s wcscmp
	#define _tcscpy_s wcscpy
	#define _tcscat_s wcscat 
	#define _tcslen_s wcslen  
//	#define _tstoi atoi 
//	#define _tstof atof 
	#define _tfopen_s fopen
	#define _tcschr_s wcschr
	#define _tcsrchr_s wcsrchr
	#define _tcstol	strtol
	#define _tcstoul	strtoul
//	#define _tcsupr_s strupr 
//	#define _totupper_s toupper
	#define _fputts_s fputws
	#define _fgetts_s fgetws
	#define _tsprintf_s(B, F, ...) swprintf( B, sizeof(B), F, ##__VA_ARGS )
	#define _stprintf_s _tsprintf
#else
	#define _tcsicmp stricmp
	#define _tcscmp strcmp
	#define _tcscpy_s strcpy_s
	#define _tcsncpy_s strncpy
	#define _tcscat_s strcat_s 
	#define _tcslen strlen 
	#define _tcslwr strlwr_s
    #define _tcslwr_s strlwr_s
	#define _strlwr_s strlwr_s
	#define _tstoi atoi
    #define _ttoi atoi 
	#define _tstof atof 
	#define _tfopen_s fopen_s
	#define _tcschr strchr 
	#define _tcsrchr strrchr
	#define _tcsupr strupr 
	#define _tcstol	strtol
	#define _tcstoul	strtoul
	#define _tstoi64 atoll
	#define _ttoi64 atoll
    #define _tcsrev strrev
	#define _totupper toupper
	#define _fputts fputs
	#define _fgetts fgets
	#define fscanf_s	fscanf 
	#define _tsprintf sprintf
	#define _stprintf_s sprintf
	#define _vstprintf_s	vsprintf
	#define vsprintf_s	vsprintf
    #define fprintf_s  fprintf
#endif

// CToken등에서 wchar로 저장되어있는 파일에서 파일명같은걸 읽었다면 NSString으로 UTF-8로 변환하여 Load( const char *)함수에 사용
#endif  // not win32

