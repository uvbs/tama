#pragma once
////////////////////////////////////////////////////////////////////////////
#include "etc/Timer.h"

class CView;
class CWinApp;
class XEOption;
// 참고: XServerApp은 엔진단이고 CWinApp의 상속을 받는 CMyTestApp는
// VC가 만들어 준거긴 하지만 유저프로젝트 커스텀이므로 엔진 템플릿 구역으로 봐야한다.
// 그러므로 템플릿이 자동 생성 해주는 코드에 한해서.CMyTestApp에 엔진 초기화 코드를 넣어도 된다.
class XEServerApp 
{
public:
// 	static XEServerApp& sGet() {
// 		return 
// 	}
private:
	CTimer m_timerSec;
	int m_msecSleepMain = 1;
	void Init() {}
	void Destroy() {}
public:
	XEServerApp();
	virtual ~XEServerApp() { Destroy(); }
	//
	GET_ACCESSOR_CONST( int, msecSleepMain );
	inline void SetmsecSleepMain( int msec ) {
		XBREAK( msec < 0 || msec > 1000 || msec == XE::INVALID_INT );
		m_msecSleepMain = msec;
	}
	//
	static void sInitInstance( CWinApp *pWinApp );
	void OnIdle( void );
	void UpdateSleepMain( const XEOption* pEnv );
	inline void UpdateSleepMain( std::shared_ptr<const XEOption> spEnv ) {
		UpdateSleepMain( spEnv.get() );
	}
};

namespace XE {
	extern _tstring x_strCmdLine;
};

extern XEServerApp* APP;