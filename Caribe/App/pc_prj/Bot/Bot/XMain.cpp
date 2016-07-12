#include "stdafx.h"
#include "XMain.h"
#include "XBotObj.h"
#include "XBotMng.h"
#include "XDlgConsole.h"
#include "BotView.h"
#include "Network/client/XNetworkConnection.h"
#include "XFSMConnector.h"
#include "XWinSocketBot.h"

using namespace XGAME;


bool XOption::Parse( const std::string& strcKey, CToken& token ) 
{
	if( strcKey == "num" ) {
		m_numBots = token.GetNumber();
	} else
	if( strcKey == "id_start" ) {
		m_idStartBot = token.GetNumber();
	} else
	if( strcKey == "ip_login" ) {
		if( token.GetToken() )
			m_strIP = SZ2C( token.m_Token );
	} else 
	if( strcKey == "sec_online" ) {
		m_secOnline = token.GetNumber();
	} else {
		XBREAKF( 1, "알수없는 옵션:%s", C2SZ(strcKey) );
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////

std::shared_ptr<XMain> XMain::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XMain>& XMain::sGet() {	if( s_spInstance == nullptr )		s_spInstance = std::shared_ptr<XMain>( new XMain );	return s_spInstance;}
void XMain::sDestroyInstance() {
	s_spInstance.reset();
}
//////////////////////////////////////////////////////////////////////////
void XLibMFC::ConsoleMessage( LPCTSTR szMsg ) 
{
	if( m_pMain )
		m_pMain->ConsoleMessage( szMsg );
}

////////////////////////////////////////////////////////////////
XMain::XMain()
{
	Init();
}

void XMain::Destroy()
{
}

void XMain::DidFinishCreate()
{
	TCHAR szFull[ MAX_PATH ];
	GetModuleFileName( NULL, szFull, MAX_PATH );		// 실행파일의 폴더를 얻어옴
 	auto ss = XE::GetFileTitleX( szFull );
	_tstring str = ss;
	str += _T(".ini");
	m_Option.LoadAbs( str.c_str() );
	// 봇 객체 생성
	const int numBot = m_Option.GetnumBots();
	const int idStart = m_Option.GetidStartBot();
	for( int i = 0; i < numBot; ++i ) {
		auto pBotObj = new XBotObj( idStart + i );
		pBotObj->OnCreate();
		XBotMng::sGet()->AddBotObj( pBotObj );
	}
	XBotMng::sGet()->DoStart();

}

float XMain::CalcDT()
{
	static DWORD dwPrevTime = GetTickCount();
	DWORD dwTime = GetTickCount();
	DWORD delta = dwTime - dwPrevTime;
	dwPrevTime = dwTime;
	float dt = (float)delta / ( (float)TICKS_PER_SEC / 60.0f );
// 	if( m_bPause )
// 		dt = 0;
	return dt;
}

// 소켓 객체에 로그인부분만 복사해서 사용하도록 우선 구현한다.
void XMain::Process() 
{
	float dt = CalcDT();
	m_FPS = CalcFPS();
	XBotMng::sGet()->Process( dt );
	//
	if( m_timerSec.IsOff() )
		m_timerSec.Set( 1.f );
	if( m_timerSec.IsOver() ) {
		m_timerSec.Reset();
		CBotView::sGet()->Update();
	}
	CTimer::UpdateTimer( dt );
}

_tstring XMain::GetstrInfo( int idxCtrl )
{
	if( idxCtrl == 0) {
		_tstring str = XE::Format( _T( "fps:%d\n" ), m_FPS );
		str += XFORMAT( "전체 봇 개수:%d \n", XBotMng::sGet()->GetMaxObj() );
		for( int i = 1; i < xConnector::xFID_MAX; ++i ) {
			auto state = (xConnector::xtFSMId)i;
//			str += XFORMAT( "[%s]: %d\n", XGAME::GetstrEnumByxtState( state )
			str += XFORMAT( "[%s]: %d\n", xConnector::GetenumFSMId( state )
																	, XBotMng::sGet()->GetNumBotByState( state ) );
		}
		str += XFORMAT("sizeMaxQ:%d\n", XENetworkConnection::sGetmaxQueue());
		// 평균 로그인시간 측정
		double secAvg = XFSMLoginingL::s_secSum / XFSMLoginingL::s_cntSample;
		str += XFORMAT("avg login: %.1f\n", (float)secAvg );
		str += XFORMAT("fpsConn:%d/%d\n", XMain::sGet()->m_fpsConnL.GetFps()
																		, XMain::sGet()->m_fpsConnG.GetFps() );
		str += XFORMAT("fpsLogin:%d/%d\n", XMain::sGet()->m_fpsLoginL.GetFps()
																		, XMain::sGet()->m_fpsLoginG.GetFps() );
		str += XFORMAT( "fpsDupl:%d\n", XMain::sGet()->m_fpsDupl.GetFps() );
		XENetworkConnection::sClearmaxQueue();
		return str;
	} else
	if( idxCtrl == 1 ) {
		_tstring str;
		return str;
	}
	return _tstring();
}

void XMain::ConsoleMessage( LPCTSTR szMsg )
{
#ifdef WIN32
	if( GetDlgConsole() )
		GetDlgConsole()->MessageString( szMsg );
#endif
}

