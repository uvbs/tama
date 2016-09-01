#pragma once
#ifdef _WIN32
#include <windows.h>
#endif 
#include <VersionXE.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "xeDef.h"
//#include "XLibrary.h"  // XLIB때문에 여기다 넣지말것. 헤더 꼬임.
//#include "XSystem.h"

#define		XLOGTYPE_LOG		0
#define		XLOGTYPE_ERROR		1
#define		XLOGTYPE_ALERT		2		// 로그와 에러의 중간. 메시지창을 띄우지만 프로그램을 종료하진 않는다
//extern char	g_strError[];

inline volatile void voidFunc() {}
//#define XBREAK_POINT()		XSYSTEM::xDebugBreak()
// 함수로하지 않고 매크로로 만든이유는 브레이크가 걸렸을때 해당 코드화면에서 바로 정지하도록 하기 위함.
#if defined(WIN32)
	#if defined(_XNOT_BREAK)
		#define XBREAK_POINT()		voidFunc()	// assert매크로에서 브레이크의 리턴값으로 분기가 잘못되는걸 막기위해 void로 함.
	#else
	#ifdef _DEBUG
		#if defined(_AFX) || defined(_AFXDLL)
			#define XBREAK_POINT() 		AfxDebugBreak()
		#else
			#define XBREAK_POINT() 		DebugBreak()
		#endif
	#else
		#define XBREAK_POINT() 		voidFunc()			// 이걸따로둔 이유. release에선 AfxDebugBreak/DebugBreak함수가 아예 없는함수인것처럼 동작한다. 만약 (BREAK_POINT(), XXX)와 같은식으로 썼을때 ( , xxx)로 변환되어 컴팔에러남. 그래서 빈 함수를 만듬.
	#endif // _DEBUG
	#endif
#elif defined(_VER_IOS)
	#define XBREAK_POINT()		voidFunc()
#else
	#define XBREAK_POINT()		voidFunc()	
#endif 

// 이제 이 버전을 쓸것.
int __xLog( int type, LPCTSTR _szBuff );		// format 없이 다이렉트로 찍는버전
int __xLogf( int type, LPCTSTR format, ... );
int __xLogfA( int type, const char* format, ... );
int __xLogTag( const char* cTag, int type, LPCTSTR _szBuff );
int __xLogfTag( const char* cTag, int type, LPCTSTR format, ... );
int __xLogfTagA( const char* cTag, int type, const char* format, ... );
void XTrace( LPCTSTR szFormat, ... );
// int __xLibConsole( LPCTSTR szFormat, ... );
// int __xLibConsoleA( const char* cFormat, ... );

LPCTSTR MakeLogFilenameFromDay( TCHAR *pOut, int len );

namespace XE {
	extern BOOL bAllowAlert;		// 메시지창 허용 여부
	const char* GetLogFilename( char *pOut, int len );
	template<int size>
	const char* GetLogFilename( char (&pBuff)[size] ) {
		return GetLogFilename( pBuff, size );
	}
	const char* MakeFilenameFromDay( char *pOut, int len, const char *prefix );
	const char* MakeFilenameFromTime( char *pOut, int len, const char *prefix );
	inline void MakeFilenameFromTime( std::string *pOut, const char *prefix ) {
		char cBuff[1024];
		*pOut = MakeFilenameFromTime( cBuff, 1024, prefix );
	}
	void WriteLogToFile( const char *cFile, LPCTSTR szBuff );
	void WriteLogToFile( LPCTSTR szBuff );
	void SetbAllowLog( int typeLog, bool bFlag );
	bool GetbAllowLog( int typeLog );
	bool GetbAllowLog();
	void SetbAllowLog( bool bFlag );
};

#ifndef WIN32
#define __FILENAME ((strrchr(__FILE__, '/') ? : __FILE__ - 1) + 1)
#else
#define __FILENAME ((_tcsrchr(_T(__FILE__), _T('\\')) ? _tcsrchr(_T(__FILE__), _T('\\')) : _T(__FILE__) - 1) + 1) 
#endif

#ifdef _VER_IOS
void _NSLog( const char *str, ... );
#endif

#if (_MSC_VER >= 1900)
#define XTSTR( A, B )  _T(A##B)
#define XTSTR3( A, B, C )  _T(A##B##C)
#define XSTR( A, B )  A##B
#define XSTR3( A, B, C )  (A##B##C)
#else
#define XTSTR( A, B )  _T(A)_T(B)
#define XTSTR3( A, B, C )  _T(A)_T(B)_T(C)
#define XSTR( A, B )  A##B
#define XSTR3( A, B, C )  (A##B##C)
#endif

// 파일과 VC/이클립스 ide의 콘솔창에 쓴다. call한 파일명과 함수명도 함께 기록한다. 
// 파일명/함수명을 쓰고 \n을 하고 본문을 쓰고 다시 \n을 한다. 주로 치명적인 에러를 표시할때 사용한다.
// 릴리즈모드일때는 alert창으로도 표시한다.
// 이것은 아래의 다른 #define들이 사용하는 매크로다. 외부cpp에서 이것을 직접 콜하진 말것.

#ifdef _XDEBUG
	//if( EXP )
	//	XLOG("");
	// 이런경우 마스터버전이 될때 에러나지 않도록 할것
	// 피씨에서 XLOG에 상수에 줄바꿈문자가 있다느니 이런 에러는 서명있는UTF8로 저장하면 해결된다
	// XBREAK_POINT내부에서 release일때는 무시하게 하고 매크로는 하나로 통합함.
	#define	XERROR(F,...) ( XBREAK_POINT(), __xLogf( XLOGTYPE_ERROR, XTSTR("ERROR:%s(%d) %s:**************************************\n",F), __FILENAME,__LINE__,__TFUNC__, ##__VA_ARGS__ ))
	//_ERROR였다가 LOG로 바꿨다. XBREAK는 엄밀히 말해 브레이크포인트인데 ERROR로 처리해 exit()해버리면 안되기 때문이다.
	#define	XLOG_ALERT(F,...) ( XBREAK_POINT(), __xLogf( XLOGTYPE_ALERT, XTSTR("%s(%d) %s():****************************************\n",F), __FILENAME,__LINE__,__TFUNC__, ##__VA_ARGS__ ))	
	#define	XLOG(F,...)			__xLogf( XLOGTYPE_LOG, XTSTR("%s(%d) %s():\n    ",F), __FILENAME,__LINE__,__TFUNC__, ##__VA_ARGS__ )
#ifdef WIN32
		#define	XLOGX(F, ...)		__xLogf( XLOGTYPE_LOG, _T(F),##__VA_ARGS__)
		#define	XLOGXN(F, ...)	__xLogf( XLOGTYPE_LOG, XTSTR(F,"\r\n"),##__VA_ARGS__)			// 유니코드버전
		#define	XLOGXNA(F, ...)  __xLogfA( XLOGTYPE_LOG, F"\r\n",##__VA_ARGS__)			// euckr버전
	#ifdef _DEBUG
			#define XTRACE( F, ... )		XTrace( XTSTR(F,"\n"), ##__VA_ARGS__ )		// 파일에 쓰지 않는 버전
	#else
			#define XTRACE( F, ... )		(0)
	#endif
		#define xENTER(A)	(0)
		#define xLEAVE(A)	(0)
// win32
#else 
		#define xENTER(A)	(0)
		#define xLEAVE(A)	(0)
#endif // not win32
	//Android
#ifdef _VER_ANDROID
		#define	XLOGX(F,...) __xLogf( XLOGTYPE_LOG, _T(F), ##__VA_ARGS__ )
		#define	XLOGXN(F,...) __xLogf( XLOGTYPE_LOG, _T(F), ##__VA_ARGS__ )
		#define XLOGXNA		XLOGXN
		#define XTRACE(F, ...)		XTrace( _T(F), ##__VA_ARGS__ )
		#define	AXLOGX		XTRACE		// 안드로이드에만 나타나면 되는 로그 
		#define	AXLOGXN		XTRACE		// 안드로이드에만 나타나면 되는 로그 
#else
		// 안드로이드가 아닌플랫폼에선 모두 사용하지 않게 함.
		#define	AXLOGX(F,...)		(0)
		#define	AXLOGXN(F,...)	(0)
#endif
	// iOS
	#ifdef _VER_IOS 
		//  #define	XLOGX(F,...)  _NSLog
		//  #define	XLOGXN(F,...) (0)
		#define	XLOGX  _NSLog
		#define	XLOGXN _NSLog
		#define	XLOGXNA	XLOGXN
		#define XTRACE( F, ... )		_NSLog( F, ##__VA_ARGS__ )
	#endif // ios
	// 이제 XLIB->Console이 생겼으므로 콘솔창 출력은 플랫폼 공통으로 이걸로 한다.
	// 게임내 콘솔창에 메시지를 출력한다. 디버깅용이 아니다.
	#ifdef _XBOT
		#define CONSOLE( F, ... )		(0)
	#else
		#define CONSOLE( F, ... )		__xLogf( XLOGTYPE_LOG, _T(F),##__VA_ARGS__)
		#define CONSOLE_TAG( TAG, F, ... )		__xLogfTag( TAG, XLOGTYPE_LOG, _T(F),##__VA_ARGS__)
	#endif
// XDEBUG
#else 
// not XDEBUG
	#define	XLOG(F,...)		(0)
	#define	XLOGX(F, ...)	(0)
	#define	XLOGXN(F, ...)	(0)
	#define	XLOGXNA(F, ...)  (0)
	#define XTRACE( F, ... )	(0)
	#define xENTER(A)	(0)
	#define xLEAVE(A)	(0)
	#define	AXLOGX(F,...)		(0)
	#define	AXLOGXN(F,...)	(0)
	#define	XERROR(F,...) (0)
	#define	XLOG_ALERT(F,...) (0)
	#define CONSOLE( F, ... )		(0)

#endif // not XDEBUG
//


void xPutsTimeString( FILE *fp );
#ifdef WIN32
void xPutsTimeStringW( FILE *fp );
#endif 

// 조건 EXP를 만족하면 에러창을 띄움
#ifdef _XDEBUG
	// ide에선 브레이크가 잡힌다. 릴리즈일경우 파일에도 쓴다.
	// 끝에 \n만했었는데 안드로이드에서 줄바꿈이 안되서 \r까지 붙임.
	#define XBREAK(EXP)	\
			(((EXP)) ? (XBREAK_POINT(), __xLogf( XLOGTYPE_ERROR, _T("%s(%d) %s():*********************\r\n%s\r\n"), __FILENAME, __LINE__, __TFUNC__, _T(#EXP) ), 1) : 0)		// 브레이크부터 걸리고 로그기록함.
	#define XBREAKF(EXP, F, ...) \
			(((EXP)) ? (XBREAK_POINT(), __xLogf( XLOGTYPE_ERROR, XTSTR3("%s\n%s(%d) %s():*********************\r\n", F, "\r\n"), _T(#EXP), __FILENAME, __LINE__, __TFUNC__,##__VA_ARGS__ ), 1) : 0)		// 브레이크부터 걸리고 로그기록함.
	#define XASSERT(EXP) \
			((!(EXP)) ? (XBREAK_POINT(), __xLogf( XLOGTYPE_ERROR, _T("%s(%d) %s():*********************\r\n%s\r\n"), __FILENAME, __LINE__, __TFUNC__, _T(#EXP) ), 0) : 1)		// 브레이크부터 걸리고 로그기록함.
	#define XASSERTF(EXP, F, ...) \
			((!(EXP)) ? (XBREAK_POINT(), __xLogf( XLOGTYPE_ERROR, XTSTR3("%s\n%s(%d) %s():*********************\r\n", F, "\r\n"), _T(#EXP), __FILENAME, __LINE__, __TFUNC__,##__VA_ARGS__ ), 0) : 1)		// 브레이크부터 걸리고 로그기록함.
	// 평가식에 따라 콘솔에 로그를 남긴다. 브레이크를 걸지는 않는다.
	#define CONSOLE_EXP(EXP, TAG, MSG) \
			(((EXP)) ? (__xLogTag( TAG, XLOGTYPE_LOG, MSG), 1) : 0)
	#define CONSOLEF_EXP(EXP, TAG, F, ...) \
			(((EXP)) ? (__xLogfTag( TAG, XLOGTYPE_LOG, _T(F),##__VA_ARGS__), 1) : 0)
// _XDEBUG
#else
// not _XDEBUG
	#define XBREAK(EXP)					(( (EXP) ) ? 1 : 0)			// win32가 아니라면 게임에서 쓰는거기때문에 XLOG()로 바꿈
	#define XBREAKF(EXP, F, ...)		(( (EXP) ) ? 1  : 0)
	#define XASSERT(EXP)				(( !(EXP) ) ? 0 : 1)
	#define XASSERTF(EXP, F, ...)		(( !(EXP) ) ? 0  : 1)
#endif 


#define XBREAK_IF(cond)            if(XBREAK(cond)) break

#define xError	XERROR
#ifdef WIN32
#define		DebugOutput		TRACE
#else
#define		DebugOutput		XLOG
#endif

// 파일 로딩시간 측정용(프로파일링) 로그
#if defined(_CLIENT) && defined(_CHEAT)
	#define XLP1	const auto __llTime = XE::GetFreqTime()
	#define XLP2	const auto __llPass = XE::GetFreqTime() - __llTime
	#define XLOGP(F,...)		\
		if( XE::GetMain()->m_bDebugMode && XE::GetMain()->m_bDebugProfilingLoad ) {\
			CONSOLE( F, ##__VA_ARGS__ ); \
		}
#else
	#define XLP1	(0)
	#define XLP2	(0)
	#define XLOGP(F,...) (0)
#endif

#define xOUTPUT_DEBUG_STRING(F, ...)		{ \
	TCHAR szMsg[ 1024 ]; \
	_stprintf_s( szMsg, XTSTR(F,"\n"),##__VA_ARGS__ ); \
	::OutputDebugString( szMsg ); \
}

