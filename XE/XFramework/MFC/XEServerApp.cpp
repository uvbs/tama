////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <afxwin.h>
//#include "resource.h"       
#include "XEServerApp.h"
#include "XEServerView.h"
//#include "WorldServerView.h"
#include "bugslayer/Include/BugslayerUtil.h"
//#include "XMain.h"
#include "XFramework/server/XServerMain.h"
#include "XFramework/server/XEUserMng.h"
#include "XFramework/XEOption.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XEServerApp* APP = nullptr;
namespace XE {
	_tstring x_strCmdLine;
};
#ifndef _DEBUG
char __xbuffer[ 1024 ] = {0,};;
LONG __stdcall TheCrashHandlerFunction ( EXCEPTION_POINTERS * pExPtrs )
{
	FILE *fp;
/*	TCHAR szLog[ 1024 ];
	MakeLogFilenameFromDay( szLog, 1024 );
	_tfopen_s( &fp, _T("error_s.txt"), T("at") );
	if( fp )
	{
		TCHAR szTemp[ 1024 ];
		_tcscpy_s( szTemp, szLog );
		_tcscat_s( szTemp, _T("\n") );
		_fputts( szTemp );
		fclose(fp);
	}
	_tfopen_s( &fp, szLog, _T("at") ); */
	TCHAR szFilename[ 64 ] = _T("error_s.txt");
#ifdef _LOGIN_SERVER
	_tcscpy_s( szFilename, _T("error_login.txt") );
#endif
#ifdef _GAME_SERVER
	_tcscpy_s( szFilename, _T("error_game.txt") );
#endif
	_tfopen_s( &fp, szFilename, _T("at") );
	if( fp )
	{
		xPutsTimeStringW( fp );
//		fputs( __xbuffer, fp );
//		fputs( "\n", fp );
		pExPtrs = pExPtrs ;
    
		LPCTSTR szStr = GetFaultReason ( pExPtrs ) ;
		_fputts( _T("FaultReason\n\n"), fp );
		if( szStr )
			_fputts( szStr, fp );
		_fputts( _T("\n"), fp );

		szStr = GetRegisterString ( pExPtrs ) ;
		_fputts( _T("Register\n"), fp );
		if( szStr )
			_fputts( szStr, fp );
		_fputts( _T("\n"), fp );
        
		szStr = GetFirstStackTraceString ( GSTSO_MODULE     |
											GSTSO_SYMBOL    |
											GSTSO_SRCLINE    ,
										   pExPtrs        ) ;
		if( szStr )
			_fputts( szStr, fp );
		_fputts( _T("\n"), fp );
		while ( NULL != szStr )
		{
			szStr = GetNextStackTraceString ( GSTSO_MODULE     |
												GSTSO_SYMBOL   |
												GSTSO_SRCLINE   ,
											  pExPtrs        ) ;
			if( szStr )
				_fputts( szStr, fp );
			_fputts( _T("\n"), fp );
		}
	
		fclose(fp);
	}
    LONG lReturnVal = EXCEPTION_EXECUTE_HANDLER ;
	lReturnVal = EXCEPTION_EXECUTE_HANDLER ;

	// 죽기전에 접속되어 있던 XUser들 Save()호출하고 죽자.
	XARRAYLINEAR_LOOP( XEServerMain::sGet()->GetarySocketSvr(), XEWinSocketSvr*, pSocketSvr )
	{
		pSocketSvr->GetpUserMng()->Save();
	} END_LOOP;
	// 세이브했다는 기록 남김
#ifdef _LOGIN_SERVER
	_tcscpy_s( szFilename, _T("error_login.txt") );
#endif
#ifdef _GAME_SERVER
	_tcscpy_s( szFilename, _T("error_game.txt") );
#endif
	_tfopen_s( &fp, szFilename, _T("at") );
	if( fp )
	{
		xPutsTimeStringW( fp );
		_fputts( _T("user saved\n"), fp );
		fclose( fp );
	}

    return ( lReturnVal ) ;
}
#endif // not debug

#include <crtdbg.h>  // For _CrtSetReportMode

void myInvalidParameterHandler(const wchar_t* expression,
	const wchar_t* function, 
	const wchar_t* file, 
	unsigned int line, 
	uintptr_t pReserved)
{
//	XLOG( "Invalid parameter detected in function %s. File: %s Line: %d\n", function, file, line);
//	XLOG( "Expression: %s\n", expression);
	// 내 익셉션 핸들러를 부르기 위해 한번더 튕겨버린다.
	volatile int *p = NULL;
	*p = 1;
	// 이렇게 해서 나오는 콜스택이 제대로 나올때가 있고 아닐때가 있네
	exit(1);
}


void XEServerApp::sInitInstance( CWinApp *pWinApp )
{
	XE::x_strCmdLine = pWinApp->m_lpCmdLine;
#ifndef _DEBUG
	// strcpy_s같은 함수들이 익셉션 낼때 assert창이 뜨지 않도록 한다.
	_CrtSetReportMode(_CRT_ASSERT, 0);
	//
   BOOL bRet = SetCrashHandlerFilter ( &TheCrashHandlerFunction ) ;
	if( bRet != TRUE )
		XLOGXN( "경고:에러검출 시스템이 활성화되지 않았습니다. return code:%d", (int)bRet );
	else
		XLOGXN( "에러검출 시스템이 활성화되었습니다" );
	// strcpy_s 같은 함수들이 익셉션낼때 핸들러를 가로채도록 한다.
	_invalid_parameter_handler oldHandler, newHandler;
	newHandler = myInvalidParameterHandler;
	oldHandler = _set_invalid_parameter_handler(newHandler);

#endif // not debug
}

XEServerApp::XEServerApp() 
{
	APP = this;
	Init();
}

void XEServerApp::OnIdle( void )
{
	if( XEServerMain::sGet() ) {
		XEServerMain::sGet()->Process();
		if( XEServerMain::sGet()->GetbDestroy() ) {
			//		SAFE_DELETE( XEServerMain::sGet() );
			//		XEServerMain::sDestroyInstance();
			AfxGetMainWnd()->SendMessage( WM_CLOSE );
		} else
		if( XEServerView::sGetView() ) {
			if( m_timerSec.IsOff() ) {
				m_timerSec.Set( 1.0f, TRUE );
			} else
			if( m_timerSec.IsOver() ) {
				XEServerView::sGetView()->Invalidate(0);
				m_timerSec.Reset();
			}
		}
	}
#ifdef _DEBUG
	XBREAK( m_msecSleepMain < 0 || m_msecSleepMain > 1000 || m_msecSleepMain == XE::INVALID_INT );
#endif // _DEBUG
	Sleep( m_msecSleepMain );
// #ifdef _XLOW_SERVER
// 	Sleep(1);
// #else
// 	Sleep(0);
// #endif

}

