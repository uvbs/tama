s_pInstance
#include "stdafx.h"
#include "XServerMain.h"
#include "etc/Timer.h"
#include "XEUserMng.h"
#include "XFramework/XHttpMng.h"
#include "XFramework/MFC/XEServerMainFrm.h"
#include "XFramework/XDetectDeadLock.h"
#include "XResObj.h"
#include "XFramework/XEProfile.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#if _DEV_LEVEL == DLV_LOCAL
#pragma message("_DEV_LEVEL == DLV_LOCAL ============================================")
#elif _DEV_LEVEL == DLV_DEV_CREW
#pragma message("_DEV_LEVEL == DLV_DEV_CREW ============================================")
#else
#pragma message("_DEV_LEVEL == unknown")
#endif

//XEServerMain *XEServerMain::sGet() = NULL;
//XAhgoWorldSvr *WORLDSVR = NULL;
//XUserMng *USERMNG = NULL;
XLibrary* XEServerMain::CreateXLibrary() {
	return new XELibrary( this );
}

//////////////////////////////////////////////////////////////////////////
void XELibrary::ConsoleMessage( LPCTSTR szMsg )
{
	// 이제 이 내부에서 XLOG류 매크로 쓰지말것. 모두 XLIB->Console()로 바뀜.
	// 디폴트 동작은 통합환경 콘솔장에 메시지 출력하고 릴리즈면 파일에 로그 기록.
//	__xLog2( XLOGTYPE_LOG, szMsg );
//	XLibrary::ConsoleMessage( szMsg );
	// 메인스레드일때만 콘솔창 사용
	if( GET_CURR_THREAD_ID() == GetidMainThread() ) {
		if( m_pMain ) {
			// 추가 행동으로 어플내 콘솔창에 메시지 출력
			TCHAR szBuff[ 8192 ];
			int len = _tcslen( szMsg );
			if( len > 7000 ) {
				XCLEAR_ARRAY( szBuff );
				_tcsncpy_s( szBuff, szMsg, 7000 );
			} else {
				XE::GetTimeString( szBuff );
				_tcscat_s( szBuff, _T(": ") );
				_tcscat_s( szBuff, szMsg );
			}
			// 구체적으로 CEditBox콘솔에 찍는함수 콜
			m_pMain->ConsoleMessage( szBuff );
		}
//		if( GetDlgConsole() )
//			GetDlgConsole()->MessageString( szBuff );
	}
}
//////////////////////////////////////////////////////////////////////////
XEServerMain* XEServerMain::s_pInstance = nullptr;
////////////////////////////////////////////////////////////////
XEServerMain* XEServerMain::sGet() {	return s_pInstance; }
void XEServerMain::sDestroyInstance() {
	s_pInstance = nullptr;
}

// 디버그모드에선 브레이크가 잡히고 트레이스도 남긴다. 서버버전은 콘솔창에도 메시지를 남긴다.
// #define XBREAK2(EXP)	\
// 			(((EXP)) ? (XBREAK_POINT(), __xLogf( XLOGTYPE_ERROR, _T("%s(%d) %s():*********************\r\n%s\n"), __FILENAME, __LINE__, __TFUNC__, _T(#EXP) ), 1) : 0)		// 브레이크부터 걸리고 로그기록함.
// #define XBREAKF2(EXP, F, ...) \
// 			(((EXP)) ? (XBREAK_POINT(), __xLogf( XLOGTYPE_ERROR, XTSTR3("%s\n%s(%d) %s():*********************\r\n", F, "\n"), _T(#EXP), __FILENAME, __LINE__, __TFUNC__,##__VA_ARGS__ ), 1) : 0)		// 브레이크부터 걸리고 로그기록함.
// //		#define XBREAKF(EXP, F, ...)		(((EXP)) ? __xLogf( XLOGTYPE_ERROR XTSTR3("%s,%s(%d) %s():*********************\r\n",F,"\n"), #EXP, __FILENAME,__LINE__,__TFUNC__,##__VA_ARGS__), (XBREAK_POINT(), 1)  : 0)
// // 		#define XBREAKF(EXP, F, ...)		(((EXP)) ? __xLogf( XLOGTYPE_ERROR XTSTR3("%s,%s(%d) %s():*********************\r\n",F,"\n"), #EXP, __FILENAME,__LINE__,__TFUNC__,##__VA_ARGS__), (XBREAK_POINT(), 1)  : 0)
// #define XASSERT2(EXP) \
// 			((!(EXP)) ? (XBREAK_POINT(), __xLogf( XLOGTYPE_ERROR, _T("%s(%d) %s():*********************\r\n%s\n"), __FILENAME, __LINE__, __TFUNC__, _T(#EXP) ), 0) : 1)		// 브레이크부터 걸리고 로그기록함.
// #define XASSERTF2(EXP, F, ...) \
// 			((!(EXP)) ? (XBREAK_POINT(), __xLogf( XLOGTYPE_ERROR, XTSTR3("%s\n%s(%d) %s():*********************\r\n", F, "\n"), _T(#EXP), __FILENAME, __LINE__, __TFUNC__,##__VA_ARGS__ ), 0) : 1)		// 브레이크부터 걸리고 로그기록함.

// static int aaa = 0;
// bool TestFunc() {
// 	++aaa;
// 	return true;
// }
//////////////////////////////////////////////////////////////////////////
//
XEServerMain::XEServerMain()
{
// 	if( XBREAKF2( TestFunc(), "hello there:%d", aaa ) ) {
// 		XTRACE("break ok");
// 	} else {
// 		XTRACE("break error");
// 	}
	XBREAK( s_pInstance != nullptr );
	s_pInstance = this;
	Init();
	m_spLock = std::make_shared<XLock>();
	ID idThread = ::GetCurrentThreadId();
// 	m_idThreadMain = idThread;
	XDetectDeadLock::sGet()->OnCreate();
	XDetectDeadLock::sGet()->AddThread( idThread, nullptr, _T("main") );
//	XE::g_Lang = XE::xLANG_KOREAN;
	XLIB = CreateXLibrary();
	XE::InitResourceManager( this, XE::xLT_PACKAGE_ONLY );
	const auto seedNew = xGenerateSeed();
	xSRand( seedNew );
	srand( seedNew );
// 	xSRand( timeGetTime() );
// 	srand( timeGetTime() );
	CTimer::Initialize();		// pause가능한 가상타이머를 초기화한다
	XE::LANG.LoadINI( XE::MakePath( _T(""), _T("lang.txt") ) );	// 장차 서버에선 빠질 예정
	m_arySocketSvr.Create( 5 );
}

void XEServerMain::Destroy()
{
	//	SAFE_DELETE( m_pUserMng );
	XHttpMng::sDestroySingleton();
	// 게임 리소스 삭제
	XTRACE( "destroy Game" );
	SAFE_DELETE( m_pGame );
// 	// 서버객체 삭제
// 	XARRAYLINEAR_LOOP( m_arySocketSvr, XEWinSocketSvr*, pSocketSvr )
// 	{
// 		pSocketSvr->SetbLoopAccept( FALSE );	// 스레드들 빠져나오도록 함.
// 		//		pSocketSvr->EnterCS();			// accept처리중엔 삭제못하도록 막음.
// 		// 소켓이나 스레드핸들 닫는순간 스레드는 그자리에서 실행이 끝나는지
// 		// accept()는 리턴값을 주는지 확인해볼것.
// #pragma message("check this")
// 		XTRACE( "destroy socketsvr %s", pSocketSvr->GetszName() );
// 		pSocketSvr->OnDestroy();
// 		SAFE_DELETE( pSocketSvr );
// 	} END_LOOP;
//	XEServerMain::sGet() = NULL;
//	s_spInstance = nullptr
	XTRACE( "destroy XServerMain finish" );
	s_pInstance = nullptr;
}

void XEServerMain::OnDestroy()
{
	// 서버객체 삭제
	XARRAYLINEAR_LOOP( m_arySocketSvr, XEWinSocketSvr*, pSocketSvr ) {
		pSocketSvr->SetbLoopAccept( FALSE );	// 스레드들 빠져나오도록 함.
		// 소켓이나 스레드핸들 닫는순간 스레드는 그자리에서 실행이 끝나는지
		// accept()는 리턴값을 주는지 확인해볼것.
#pragma message("check this")
		XTRACE( "destroy socketsvr %s", pSocketSvr->GetszName() );
		pSocketSvr->OnDestroy();
		SAFE_DELETE( pSocketSvr );
	} END_LOOP;
	XEProfile::sDestroy();
}

//#include "XMain.h"	
// XServerMain이 생성되고 가장먼저 호출된다.
void XEServerMain::Create( LPCTSTR )
{
#ifdef _XPROFILE
	XEProfile::sCreateSingleton();
	XEProfile::sGet()->SetpDelegate( this );
#endif // _XPROFILE
	// 게임리소스를 먼저 읽고 서버객체를 여는게 나을거 같다.
	m_pGame = CreateContents();
	DidFinishCreate();		// this의 Create가 끝난후 다른 Create가 있다면 하위클래스에 맡긴다.
	// 모든 로딩이 다 끝나면 이제부턴 Alert창을 허용하지 않는다.
//#ifdef _XDEVELOPER
// #if _DEV_LEVEL == DLV_DEV_PERSONAL
// 	XE::bAllowAlert = TRUE;				// 프로그래머외 개발자가 쓰는 서버의 경우만 메시지박스 뜨도록 허용함.
// #else
// 	XE::bAllowAlert = FALSE;			// 그 외는 모두 불가.
// #endif
}
/**
 초기 ini읽는것은 엔진에서 뺄것. 별로 범용적이지도 않고 함수콜 순서도 헷갈리고
 ini형태를 꼭 써야하는 문제도 있고 해서.
 필요하면 Create()함수 초반에서 알아서 구현해서 쓸것.
*/
BOOL XEServerMain::LoadINI( LPCTSTR szFile )
{
	CToken token;
	if( token.LoadAbsolutePath( szFile, XE::TXT_EUCKR ) == xFAIL ) {
		XALERT( "\"%s\" not found", szFile );
		return FALSE;
	}
	CONSOLE( "%s loaded.", szFile );
	// virtual
	return LoadINI( token );
}

float XEServerMain::CalcDT()
{
	static DWORD dwPrevTime = GetTickCount();
	DWORD dwTime = GetTickCount();
	DWORD delta = dwTime - dwPrevTime;
	dwPrevTime = dwTime;
	float dt = (float)delta / ((float)TICKS_PER_SEC/60.0f);
	// pause
    {
        dt *= m_fAccel;				// 프레임스키핑 적용
    }
	if( m_bPause )
		dt = 0;
	return dt;
}

//extern int __g_State;
void XEServerMain::Process()
{
#ifdef _XPROFILE
	XEProfile::sGet()->Process();
#endif // _XPROFILE
	XPROF_OBJ_AUTO();
	float dt = CalcDT();
	m_FPS = CalcFPS();
	//
//	__g_State = 1;
	{
		XPROF_OBJ("pSocketSvr->Process");
		XARRAYLINEAR_LOOP( m_arySocketSvr, XEWinSocketSvr*, pSocketSvr )
		{
			auto llMilli1 = GET_FREQ_TIME();
			pSocketSvr->Process( dt );
#ifdef _XBOT
			auto llPass = GET_FREQ_TIME() - llMilli1;
			if( llPass > 3000000 ) {		// 1초이상 걸린경우 break
				// 보고서 작성 1.txt 2.txt식으로
				static int si = 1;
				XResFile file;
				_tstring strFile = XFORMAT("pf%d.txt", si );
				if( file.OpenForWrite( strFile.c_str() ) ) {
					for( auto& prof : pSocketSvr->m_aryTime ) {
						file.Printf( _T("%d\t%s\r\n"), prof.m_dwPass, prof.m_str.c_str() );
					}
				}
				++si;
			}
#endif // _XBOT
		} END_LOOP;
	}
	//
	{
		XPROF_OBJ( "XHttpMng::Process" );
		if( XHttpMng::_sGetSingletonPtr() )
			XHttpMng::_sGetSingletonPtr()->Process();
	}
	if( m_pGame ) {
		xENTER(0);
//		__g_State = 2;
		XPROF_OBJ( "m_pGame->FrameMove" );
		m_pGame->FrameMove( dt );
		xLEAVE(0);
	}
//	__g_State = 3;
	// USERMNG->FrameMove( dt );
	CTimer::UpdateTimer( dt );
}

/**
 윈도우 타이틀을 갱신한다. 보통 서버 소켓객체를 Add시킨후 호출한다.
*/
void XEServerMain::UpdateTitle()
{
	XEServerMainFrame::sGet()->SetTitle();
}

XEWinSocketSvr* XEServerMain::AddSocketServer( XEWinSocketSvr* pSocketSvr ) 
{
	m_arySocketSvr.Add( pSocketSvr );
	return pSocketSvr;
}

/**
 @brief 프로파일링 종료
*/
// void XEServerMain::OnFinishProfile( XEProfile* pProfile )
// {
// #ifdef _XPROFILE
// 	TCHAR szSpace[ 256 ] = {_T(' '),};
// 	char cFile[ 256 ];
// 	XE::MakeFilenameFromTime( cFile, 256, "pf" );
// 	_tstring strFile = C2SZ( cFile );
// 	XResFile file;
// 	if( file.OpenForWrite( strFile.c_str() ) ) {
// 		XARRAYLINEAR_LOOP_AUTO( XEProfile::sGetResult(), &result ) {
// 			memset( szSpace, ' ', sizeof(TCHAR) * 256 );
// 			szSpace[ result.depth * 2 ] = 0;
// 			file.Printf( _T("%s%s  %.3f  %d  %d  %d\r\n"), szSpace, result.strFunc.c_str()
// 																								, result.ratioShare
// 																								, result.mcsExecAvg
// 																								, result.mcsExecMin
// 																								, result.mcsExecMax );
// 		} END_LOOP;
// 		CONSOLE("프로파일 결과 저장완료:%s", strFile.c_str() );
// 	}
// #endif // _XPROFILE
// }
// 


void XEServerMain::ShowLogFilter( const std::string& strTag )
{
	if( strTag == "tags" ) {
		for( auto& tag : CONSOLE_MAIN->GetlistTags() ) {
			const _tstring strt = C2SZ(tag);
			ConsoleMessage( strt.c_str() );
		}
	} else
	if( strTag == "idacc" ) {
		for( auto& idacc : CONSOLE_MAIN->GetlistLogidAcc() ) {
			const _tstring strt = XFORMAT("%d", idacc);
			ConsoleMessage( strt.c_str() );
		}
	}
}
