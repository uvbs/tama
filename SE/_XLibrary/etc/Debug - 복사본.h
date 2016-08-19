#ifndef		__DEBUG_H__
#define		__DEBUG_H__

#ifdef _WIN32
#include <windows.h>
#endif 
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#if defined(_LE) || defined(_XCONSOLE)
	#ifdef _SE
	#include "ConsoleView.h"
	#endif
	#if !defined(_SE) && defined(_XCONSOLE)
	#include "XDlgConsole.h"
	#endif
#endif


#define		XLOGTYPE_LOG		0
#define		XLOGTYPE_ERROR		1
#define		XLOGTYPE_ALERT		2		// 로그와 에러의 중간. 메시지창을 띄우지만 프로그램을 종료하진 않는다
//extern char	g_strError[];

//void xError(char *str, ...);
int __xLog( int type, LPCTSTR str, ...);
void XTrace( LPCTSTR szFormat, ... );

#ifdef _VER_MAC
#define __FILENAME ((strrchr(__FILE__, '/') ? : __FILE__ - 1) + 1) 
#else
#define __FILENAME ((_tcsrchr(_T(__FILE__), _T('\\')) ? _tcsrchr(_T(__FILE__), _T('\\')) : _T(__FILE__) - 1) + 1) 
#endif

#ifdef _VER_IPHONE
void _NSLog( const char *str, ... );
#define CONSOLE _NSLog
#endif

//if( EXP )
//	XLOG("");
// 이런경우 마스터버전이 될때 에러나지 않도록 할것
// 피씨에서 XLOG에 상수에 줄바꿈문자가 있다느니 이런 에러는 서명있는UTF8로 저장하면 해결된다
#define	XLOG(F,...) __xLog( XLOGTYPE_LOG, _T("%s(%d) %s():\n")_T(F), __FILENAME,__LINE__,__TFUNC__, ##__VA_ARGS__ )	
#ifdef WIN32
	#define	XNLOG(F,...) XTrace( _T(F)_T("\n"), ##__VA_ARGS__ )	
	#define XTRACE( STR )		XTrace( _T(STR) )
#else
	#define	XNLOG(F,...) __xLog( XLOGTYPE_LOG, _T(F)_T("\n"), ##__VA_ARGS__ )	
	#define XTRACE( STR )		_NSLog( STR )
#endif
#define	XERROR(F,...) __xLog( XLOGTYPE_ERROR, _T("ERROR:%s(%d) %s:\n")_T(F), __FILENAME,__LINE__,__TFUNC__, ##__VA_ARGS__ )
#define	XLOG_ALERT(F,...) __xLog( XLOGTYPE_ALERT, _T("%s(%d) %s():\n")_T(F), __FILENAME,__LINE__,__TFUNC__, ##__VA_ARGS__ )	
// XBREAK의 기능도 갖고 있으면서 마스터본에서도 로그를 반드시 남겨야 하는거면 이걸 사용.
#define	XLOGE(EXP)	((EXP)? XLOG(#EXP), (AfxDebugBreak(), 1) : 0)		// EXP가 참이면 로그를 남기고 1을 리턴



void xPutsTimeString( FILE *fp );

/*
if( XBREAK( i == 1 ) )
	error message;
XBREAK( i == 1 );
if( XASSERT( i == 1 ) )
	성공처리
p = XASSERT( Find(1) );
*/

// 조건 EXP를 만족하면 에러창을 띄움
#ifdef _XDEBUG
	#ifdef WIN32
		#ifndef _AFX
		#define AfxDebugBreak	DebugBreak
		#endif
		#if defined(_DEBUG)	
			// 프로그래머 디버거 모드. XBREAK/ASSERT류는 릴리즈에선 모두 사라진다는걸 명심할것.
			#define XBREAK(EXP)		((EXP) ? (AfxDebugBreak(), 1)  : 0)		// 디버그모드에선 브레이크잡으면서 트레이스 메시지도 띄움
			#define XBREAKF(EXP, F, ...)		((EXP) ? (AfxDebugBreak(), 1)  : 0)		// 프로그래머 디버거 모드에선 브레이크 잡히니까 경고창띄우는게 필요 없을거 같아서 경고창 뺌
			#define XBREAK_ERROR(EXP, F, ...)		((EXP) ? XERROR( F, ##__VA_ARGS__ ), (AfxDebugBreak(), 1)  : 0)
#pragma message( "if( XASSERT( Func() == 1 ) )   이런식으로 쓸때 함수가 두번호출되는 문제 해결할것" )
			#define XASSERT(EXP)		(( !(EXP) ) ? (AfxDebugBreak(), 0)  : 1 )
			#define XASSERTF(EXP, F, ...)		(( !(EXP) ) ? XLOG( F, ##__VA_ARGS__ ), (AfxDebugBreak(), 0)  : 1)
			#define XASSERT_ERROR(EXP, F, ...)		(( !(EXP) ) ? XERROR( F, ##__VA_ARGS__ ), (AfxDebugBreak(), 0)  : 1)
		#else
			// 테스터 모드
			#define XBREAK(EXP)		((EXP) ? XLOG_ALERT( #EXP ), 1  : 0)
			// 브레이크가 걸려야 하는상황인데 alert창이 안뜨는경우는 없을거 같아서 아예 뺌						#define XBREAKF(EXP, F, ...)		((EXP) ? XLOG( F, ##__VA_ARGS__ ), 1  : 0)
			#define XBREAKF(EXP, F, ...)		((EXP) ? XLOG_ALERT( F, ##__VA_ARGS__ ), 1   : 0)	// 테스터모드에선 브레이크는 안되니까 경고창이 떠야함
			#define XBREAK_ERROR(EXP, F, ...)		((EXP) ? XERROR( F, ##__VA_ARGS__ ), 1  : 0)
			#define XASSERT(EXP)		(( !(EXP) ) ? XLOG_ALERT( #EXP ), 0  : 1)
			#define XASSERTF(EXP, F, ...)		(( !(EXP) ) ? XLOG_ALERT( F, ##__VA_ARGS__ ), 0  : 1)
			#define XASSERT_ERROR(EXP, F, ...)		(( !(EXP) ) ? XERROR( F, ##__VA_ARGS__ ), 0  : 1)
		#endif
	#else // WIN32
		#include "assert.h"
		#if defined(DEBUG)
            int AfxDebugBreak();
			// 아이폰 배포시에는 디버그 버전으로 돌리는게 낫고 그러자면 알림창도 떠야하니까 LOG_ALERT로 바꿈
			#define XBREAK(EXP)					((EXP) ? XLOG_ALERT( #EXP ), AfxDebugBreak(), 1 : 0)			// win32가 아니라면 게임에서 쓰는거기때문에 XLOG()로 바꿈
			#define XBREAKF(EXP, F, ...)		((EXP) ? XLOG_ALERT( F, ##__VA_ARGS__ ), AfxDebugBreak(), 1  : 0)
			#define XBREAK_ERROR(EXP, F, ...)	((EXP) ? XERROR( F, ##__VA_ARGS__ ), AfxDebugBreak(), 1  : 0)
			#define XASSERT(EXP)				(( !(EXP) ) ? XLOG_ALERT( #EXP ), AfxDebugBreak(), 0 : 1)
			#define XASSERTF(EXP, F, ...)		(( !(EXP) ) ? XLOG_ALERT( F, ##__VA_ARGS__ ), AfxDebugBreak(), 0  : 1)
			#define XASSERT_ERROR(EXP, F, ...)	(( !(EXP) ) ? XERROR( F, ##__VA_ARGS__ ), AfxDebugBreak(), 0  : 1)
		#else
			#define XBREAK(EXP)					((EXP) ? XLOG_ALERT( #EXP ), 1 : 0)			// win32가 아니라면 게임에서 쓰는거기때문에 XLOG()로 바꿈
			#define XBREAKF(EXP, F, ...)		((EXP) ? XLOG_ALERT( F, ##__VA_ARGS__ ), 1  : 0)
			#define XBREAK_ERROR(EXP, F, ...)	((EXP) ? XERROR( F, ##__VA_ARGS__ ), 1  : 0)
			#define XASSERT(EXP)				(( !(EXP) ) ? XLOG_ALERT( #EXP ), 0 : 1)
			#define XASSERTF(EXP, F, ...)		(( !(EXP) ) ? XLOG_ALERT( F, ##__VA_ARGS__ ), 0  : 1)
			#define XASSERT_ERROR(EXP, F, ...)	(( !(EXP) ) ? XERROR( F, ##__VA_ARGS__ ), 0  : 1)
		#endif // not DEBUG
	#endif// not WIN32
#else // XDEBUG
	// 최종마스터 버전
	#define XBREAK(EXP)		(EXP)						// _LOGA버전을 따로만들지 말고 _DEBUG(디버거띄운상태)에서는 Alert창이 안나오고 테스터모드(_XDEBUG만있는)에서는 Alert창이 뜨도록 하자
//	#define XBREAKF(EXP, F, ...)		(0)
	#define XBREAKF(EXP, F, ...)		(EXP)				// 컴파일러가 최적화에서 (i == 1); 같은 조건만 있는 문장은 알아서 삭제해주길 기대하는수밖에
	#define XBREAK_ERROR(EXP, F, ...)	(0)
	#define XASSERT(EXP)		(EXP)
//	#define XASSERT_LOG(EXP, F, ...)		(EXP)
	#define XASSERTF(EXP, F, ...)		(EXP)
	#define XASSERT_ERROR(EXP, F, ...)		(EXP)
#endif // not XDEBUG
								
// XBREAK( pDelObjAct->GetpAction() != pAction );
#define xError	XERROR
#ifdef _VER_MAC
#define		DebugOutput		XLOG
#else
#define		DebugOutput		TRACE
#endif

#endif

