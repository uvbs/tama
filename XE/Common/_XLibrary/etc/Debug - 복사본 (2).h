#pragma once
#ifdef _WIN32
#include <windows.h>
#endif 
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "xeDef.h"
//#include "XLibrary.h"  // XLIB때문에 여기다 넣지말것. 헤더 꼬임.
#include "XSystem.h"

#define		XLOGTYPE_LOG		0
#define		XLOGTYPE_ERROR		1
#define		XLOGTYPE_ALERT		2		// 로그와 에러의 중간. 메시지창을 띄우지만 프로그램을 종료하진 않는다
//extern char	g_strError[];

//void xError(char *str, ...);
 int __xLog( int type, LPCTSTR str, ...);
int __xLogx( LPCTSTR str, ...);
void XTrace( LPCTSTR szFormat, ... );
int __xLibConsole( LPCTSTR szFormat, ... );
int __xLibConsoleA( const char* cFormat, ... );

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
#else
#define XTSTR( A, B )  _T(A)_T(B)
#define XTSTR3( A, B, C )  _T(A)_T(B)_T(C)
#endif


#ifdef _XDEBUG
	//if( EXP )
	//	XLOG("");
	// 이런경우 마스터버전이 될때 에러나지 않도록 할것
	// 피씨에서 XLOG에 상수에 줄바꿈문자가 있다느니 이런 에러는 서명있는UTF8로 저장하면 해결된다
	//#define	XLOG(F,...) __xLog( XLOGTYPE_LOG, _T("%s(%d) %s():\n    ")_T(F), __FILENAME,__LINE__,__TFUNC__, ##__VA_ARGS__ )
#ifdef _DEBUG
//	#define	XERROR(F,...) ( XBREAK_POINT(), __xLog( XLOGTYPE_ERROR, _T("ERROR:%s(%d) %s:**************************************\n")_T(F), __FILENAME,__LINE__,__TFUNC__, ##__VA_ARGS__ ))
	#define	XERROR(F,...) ( XBREAK_POINT(), __xLog( XLOGTYPE_ERROR, XTSTR("ERROR:%s(%d) %s:**************************************\n",F), __FILENAME,__LINE__,__TFUNC__, ##__VA_ARGS__ ))
#else
	#define	XERROR(F,...) ( (0), __xLog( XLOGTYPE_ERROR, XTSTR("ERROR:%s(%d) %s:**************************************\n",F), __FILENAME,__LINE__,__TFUNC__, ##__VA_ARGS__ ))
	#define	XERROR(F,...) ( (0), __xLog( XLOGTYPE_ERROR, XTSTR("ERROR:%s(%d) %s:**************************************\n",F), __FILENAME,__LINE__,__TFUNC__, ##__VA_ARGS__ ))
#endif
	//_ERROR였다가 LOG로 바꿨다. XBREAK는 엄밀히 말해 브레이크포인트인데 ERROR로 처리해 exit()해버리면 안되기 때문이다.
	#define	XLOG_ALERT(F,...) __xLog( XLOGTYPE_ALERT, XTSTR("%s(%d) %s():****************************************\n",F), __FILENAME,__LINE__,__TFUNC__, ##__VA_ARGS__ )	
//	#define	XLOG(F,...)			__xLibConsole( _T("%s(%d) %s():\n    ")_T(F), __FILENAME,__LINE__,__TFUNC__, ##__VA_ARGS__ )
	#define	XLOG(F,...)			__xLibConsole( XTSTR("%s(%d) %s():\n    ",F), __FILENAME,__LINE__,__TFUNC__, ##__VA_ARGS__ )
#ifdef WIN32
		#define	XLOGX(F, ...)		__xLibConsole(_T(F),##__VA_ARGS__)
		#define	XLOGXN(F, ...)	__xLibConsole(XTSTR(F,"\r\n"),##__VA_ARGS__)			// 유니코드버전
		#define	XLOGXNA(F, ...)  __xLibConsoleA(F"\r\n",##__VA_ARGS__)			// euckr버전
// 		{ \
// 			char __xcBuff[ 1024 ]; \
// 			sprintf_s( __xcBuff, F,##__VA_ARGS__); \
// 			if( XLIB ) \
// 				XLIB->Console( _T("%s"), C2SZ(__xcBuff) ); \
// 		}
	#ifdef _DEBUG
			#define XTRACE( F, ... )		XTrace( _T(F), ##__VA_ARGS__ )
	#else
			#define XTRACE( F, ... )		(0)
	#endif
		//	#define xENTER(A)	TRACE( "%s-enter%d ", __FUNCTION__, A );
		//	#define xLEAVE(A)	TRACE( "%s-leave%d ", __FUNCTION__, A );
		#define xENTER(A)	(0)
		#define xLEAVE(A)	(0)
#else // win32
		#define xENTER(A)	(0)
		#define xLEAVE(A)	(0)
#endif // not win32
	//Android
#ifdef _VER_ANDROID
		#define	XLOGX(F,...) __xLogx( _T(F), ##__VA_ARGS__ )
		#define	XLOGXN(F,...) __xLogx( _T(F)_T("\r\n"), ##__VA_ARGS__ )
		#define XLOGXNA		XLOGXN
// 		#define	XLOGX(F, ...)		__xLibConsole(_T(F),##__VA_ARGS__)
// 		#define	XLOGXN(F, ...)	__xLibConsole(_T(F)_T("\r\n"),##__VA_ARGS__)
// 		#define	XLOGXNA(F, ...)  { \
// 			char __xcBuff[ 1024 ]; \
// 			sprintf_s( __xcBuff, F,##__VA_ARGS__); \
// 			if( XLIB ) \
// 				XLIB->Console( _T("%s"), C2SZ(__xcBuff) ); \
// 		}
		#define XTRACE(F, ...)		__xLogx( _T(F), ##__VA_ARGS__ )
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
		#define CONSOLE_D( F, ... )	(0)
	#else
#if defined(_VER_ANDROID)
		#define CONSOLE( F, ... )		__xLogx( _T(F), ##__VA_ARGS__ )
		// 콜함수까지 출력하는 버전. 디버깅용으로도 쓸수 있다.
		#define CONSOLE_D( F, ... )	__xLogx( _T(F), ##__VA_ARGS__ )
#elif defined(_VER_IOS)
		#define CONSOLE( F, ... )		__xLogx( _T(F), ##__VA_ARGS__ )
		// 콜함수까지 출력하는 버전. 디버깅용으로도 쓸수 있다.
		#define CONSOLE_D( F, ... )	__xLogx( _T(F), ##__VA_ARGS__ )
#elif defined(WIN32)
		#define CONSOLE( F, ... )		__xLibConsole(XTSTR(F,"\n"),##__VA_ARGS__)
		// 콜함수까지 출력하는 버전. 디버깅용으로도 쓸수 있다.
		#define CONSOLE_D( F, ... )	__xLibConsole(XTSTR3("%s(%d) %s():*********************\r\n",F,"\n"), __FILENAME,__LINE__,__TFUNC__,##__VA_ARGS__)
#else
#error "unknown platform"
#endif
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
#ifdef _SERVER
	#ifndef _AFXDLL
	//#error "support only afx"
	#endif
#define CONSOLE_D( F, ... )	__xLibConsole(XTSTR3("%s(%d) %s():*********************\r\n",F,"\n"), __FILENAME,__LINE__,__TFUNC__,##__VA_ARGS__)
	#if defined(_DEBUG)	
		// 디버그모드에선 브레이크가 잡히고 트레이스도 남긴다. 서버버전은 콘솔창에도 메시지를 남긴다.
		#define XBREAK(EXP)				(( (EXP) ) ? CONSOLE_D( #EXP ), (XBREAK_POINT(), 1)  : 0)		
//		#define XBREAKF(EXP, F, ...)		(( (EXP) ) ? CONSOLE_D( XTSTR("%s",F), __TFUNC__, ##__VA_ARGS__ ), (XBREAK_POINT(), 1)  : 0)
		#define XBREAKF(EXP, F, ...)		(( (EXP) ) ? __xLibConsole(XTSTR3("%s,%s(%d) %s():*********************\r\n",F,"\n"), #EXP, __FILENAME,__LINE__,__TFUNC__,##__VA_ARGS__), (XBREAK_POINT(), 1)  : 0)
		#define XASSERT(EXP)				(( !(EXP) ) ? CONSOLE_D( #EXP ), (XBREAK_POINT(), 0)  : 1 )
		#define XASSERTF(EXP, F, ...)		(( !(EXP) ) ? CONSOLE_D( F, ##__VA_ARGS__ ), (XBREAK_POINT(), 0)  : 1)
	#else
		// 릴리즈 버전
		#define XBREAK(EXP)				(( (EXP) ) ? XLOG( #EXP ), CONSOLE_D( #EXP ), 1  : 0)
		#define XBREAKF(EXP, F, ...)		(( (EXP) ) ? XLOG( F, ##__VA_ARGS__ ), CONSOLE_D( F, ##__VA_ARGS__ ), 1   : 0)	
		#define XASSERT(EXP)				(( !(EXP) ) ? XLOG( #EXP ), CONSOLE_D( #EXP ), 0  : 1)
		#define XASSERTF(EXP, F, ...)		(( !(EXP) ) ? XLOG( F, ##__VA_ARGS__ ), CONSOLE_D( F, ##__VA_ARGS__ ), 0  : 1)
	#endif // release
#else
// not _SERVER
	#ifdef _XDEBUG
		#ifdef WIN32
			#ifndef _AFX
			#define AfxDebugBreak	DebugBreak
			#endif // not afx
			#if defined(_DEBUG)	
				// 프로그래머 디버거 모드. XBREAK/ASSERT류는 릴리즈에선 모두 사라진다는걸 명심할것.
//				#define XBREAK(EXP)		(( (EXP) ) ? XLOG( #EXP ), (AfxDebugBreak(), 1)  : 0)		// 디버그모드에선 브레이크잡으면서 트레이스 메시지도 띄움
				#define XBREAK(EXP)		(( (EXP) ) ? /*CONSOLE( #EXP ),*/ (AfxDebugBreak(), 1)  : 0)		// 디버그모드에선 브레이크잡으면서 트레이스 메시지도 띄움
				#define XBREAKF(EXP, F, ...)		(( (EXP) ) ? CONSOLE_D( F, ##__VA_ARGS__ ), (AfxDebugBreak(), 1)  : 0)		// 프로그래머 디버거 모드에선 브레이크 잡히니까 경고창띄우는게 필요 없을거 같아서 경고창 뺌
				#define XASSERT(EXP)		(( !(EXP) ) ? XLOG( #EXP ), (AfxDebugBreak(), 0)  : 1 )
				#define XASSERTF(EXP, F, ...)		(( !(EXP) ) ? XLOG( F, ##__VA_ARGS__ ), (AfxDebugBreak(), 0)  : 1)
	//			#define XASSERT_ERROR(EXP, F, ...)		(( !(EXP) ) ? XERROR( F, ##__VA_ARGS__ ), (AfxDebugBreak(), 0)  : 1)
			#else
				// 테스터 모드
				#define XBREAK(EXP)		(( (EXP) ) ? XLOG_ALERT( #EXP ), 1  : 0)
				// 브레이크가 걸려야 하는상황인데 alert창이 안뜨는경우는 없을거 같아서 아예 뺌						#define XBREAKF(EXP, F, ...)		(( (EXP) ) ? XLOG( F, ##__VA_ARGS__ ), 1  : 0)
				#define XBREAKF(EXP, F, ...)		(( (EXP) ) ? XLOG_ALERT( F, ##__VA_ARGS__ ), 1   : 0)	// 테스터모드에선 브레이크는 안되니까 경고창이 떠야함
				#define XASSERT(EXP)		(( !(EXP) ) ? XLOG_ALERT( #EXP ), 0  : 1)
				#define XASSERTF(EXP, F, ...)		(( !(EXP) ) ? XLOG_ALERT( F, ##__VA_ARGS__ ), 0  : 1)
// #define XBREAK(EXP)		(( (EXP) ) ? CONSOLE_D( #EXP ), 1  : 0)
// // 브레이크가 걸려야 하는상황인데 alert창이 안뜨는경우는 없을거 같아서 아예 뺌						#define XBREAKF(EXP, F, ...)		(( (EXP) ) ? XLOG( F, ##__VA_ARGS__ ), 1  : 0)
// #define XBREAKF(EXP, F, ...)		(( (EXP) ) ? CONSOLE_D( F, ##__VA_ARGS__ ), 1   : 0)	// 테스터모드에선 브레이크는 안되니까 경고창이 떠야함
// #define XASSERT(EXP)		(( !(EXP) ) ? CONSOLE_D( #EXP ), 0  : 1)
// #define XASSERTF(EXP, F, ...)		(( !(EXP) ) ? CONSOLE_D( F, ##__VA_ARGS__ ), 0  : 1)
			#endif
		// win32
		#else 
		// NOT WIN32
			#include "assert.h"
			#ifdef _VER_ANDROID
			inline int AfxDebugBreak() {return 1;}
			#else
			int AfxDebugBreak();
			#endif
			#if defined(DEBUG)
				// 아이폰 배포시에는 디버그 버전으로 돌리는게 낫고 그러자면 알림창도 떠야하니까 LOG_ALERT로 바꿈
				#define XBREAK(EXP)					(( (EXP) ) ? XLOG_ALERT( #EXP ), AfxDebugBreak(), 1 : 0)			// win32가 아니라면 게임에서 쓰는거기때문에 XLOG()로 바꿈
				#define XBREAKF(EXP, F, ...)		(( (EXP) ) ? XLOG_ALERT( F, ##__VA_ARGS__ ), AfxDebugBreak(), 1  : 0)
	//			#define XBREAK_ERROR(EXP, F, ...)	(( (EXP) ) ? XERROR( F, ##__VA_ARGS__ ), AfxDebugBreak(), 1  : 0)
				#define XASSERT(EXP)				(( !(EXP) ) ? XLOG_ALERT( #EXP ), AfxDebugBreak(), 0 : 1)
				#define XASSERTF(EXP, F, ...)		(( !(EXP) ) ? XLOG_ALERT( F, ##__VA_ARGS__ ), AfxDebugBreak(), 0  : 1)
	//			#define XASSERT_ERROR(EXP, F, ...)	(( !(EXP) ) ? XERROR( F, ##__VA_ARGS__ ), AfxDebugBreak(), 0  : 1)
			#else
				#define XBREAK(EXP)					(( (EXP) ) ? XLOG_ALERT( #EXP ), 1 : 0)			// win32가 아니라면 게임에서 쓰는거기때문에 XLOG()로 바꿈
				#define XBREAKF(EXP, F, ...)		(( (EXP) ) ? XLOG_ALERT( F, ##__VA_ARGS__ ), 1  : 0)
	//			#define XBREAK_ERROR(EXP, F, ...)	(( (EXP) ) ? XERROR( F, ##__VA_ARGS__ ), 1  : 0)
				#define XASSERT(EXP)				(( !(EXP) ) ? XLOG_ALERT( #EXP ), 0 : 1)
				#define XASSERTF(EXP, F, ...)		(( !(EXP) ) ? XLOG_ALERT( F, ##__VA_ARGS__ ), 0  : 1)
	//			#define XASSERT_ERROR(EXP, F, ...)	(( !(EXP) ) ? XERROR( F, ##__VA_ARGS__ ), 0  : 1)
			#endif // not DEBUG
		#endif// not WIN32
		// XDEBUG
	#else 
		// not XDEBUG
		#include "assert.h"
		#ifdef _VER_ANDROID
		inline int AfxDebugBreak() {return 1;}
		#else
			#ifndef _AFX
			int AfxDebugBreak();
			#endif // not afx
		#endif
		#define XBREAK(EXP)					(( (EXP) ) ? 1 : 0)			// win32가 아니라면 게임에서 쓰는거기때문에 XLOG()로 바꿈
		#define XBREAKF(EXP, F, ...)		(( (EXP) ) ? 1  : 0)
		#define XASSERT(EXP)				(( !(EXP) ) ? 0 : 1)
		#define XASSERTF(EXP, F, ...)		(( !(EXP) ) ? 0  : 1)
	#endif // not XDEBUG
#endif // not _XSERVER
// XBREAK( pDelObjAct->GetpAction() != pAction );
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

