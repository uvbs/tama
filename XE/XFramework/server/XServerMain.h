#pragma once
#include "XFramework/server/XWinSocketSvr.h"
#include "XFramework/XConsoleMain.h"
#include "XLibrary.h"
#include "Network/xenDef.h"

class XEServerMain;
class XELibrary : public XLibrary
{
	XEServerMain *m_pMain;
	void Init() {
		m_pMain = NULL;
	}
	void Destroy() {}
	//
	void ConsoleMessage( LPCTSTR szMsg ) override;
public:
	XELibrary( XEServerMain *pMain ) { 
		Init(); 
		m_pMain = pMain;
	}
	virtual ~XELibrary() { Destroy(); }
};

class XEUserMng;

/**
 컨텐츠 메인객체
*/
class XEContents
{
public:
	XEContents() {}
	virtual ~XEContents() {}
	//
	virtual void Create( void ) = 0;
	virtual void FrameMove( float dt ) = 0;
};

////////////////////////////////////////////////////////////////
/**
 앱 메인 객체
*/
#ifdef _XPROFILE
#include "XFramework/XEProfile.h"
#endif // _XPROFILE
class XEServerMain : public XConsoleMain
#ifdef _XPROFILE
										, public XEProfileDelegate
#endif // _XPROFILE
{
public:
	static XEServerMain* sGet();
	static void sDestroyInstance();
private:
	void Init() {
		m_pGame = NULL;
		m_fAccel = 1.0f;
		m_bPause = FALSE;
		m_FPS = 0;
	}
	void Destroy();
protected:
	XArrayLinear<XEWinSocketSvr*> m_arySocketSvr;
	XEContents *m_pGame;	
public:
	// new직후 Create()를 반드시 호출해줄것.
	XEServerMain();
	virtual ~XEServerMain() { Destroy(); }
	//
	GET_ACCESSOR( XArrayLinear<XEWinSocketSvr*>&, arySocketSvr );
	GET_ACCESSOR( XSPLock, spLock );
	//
	BOOL LoadINI( LPCTSTR szFile );
	virtual BOOL LoadINI( CToken &token )=0;
	virtual void Create( LPCTSTR/* szINI*/ );
	float CalcDT( void );
	XEWinSocketSvr* AddSocketServer( XEWinSocketSvr* pSocketSvr );
	// virtual
	// 소켓객체의 생성을 하위클래스에 맡긴다.
	// this의 Create가 모두 끝나면 하위클래스에 호출된다.
	virtual void DidFinishCreate( void ) = 0;
	// 매 루프 돌아가야 하는 게임객체가 있다면 생성을 하위에 맡긴다.
	virtual XEContents* CreateContents( void ) { return NULL; }
	//
	// 저수준 레이어 XLibrary객체를 생성 
	virtual XLibrary* CreateXLibrary( void );
	// 매 프레임 호출되어 프로세스 된다.
	virtual void Process( void );
	// this인스턴스가 파괴되기전에 호출된다.
	virtual void OnDestroy();
	/**
	 메모리 풀 생성을 SAFE_DELETE( MAIN )외부로 뺀이유는?
	 객체를 삭제할때 취하위 자식부터 삭제되서 올라온다. 
	 상위딴에서 다시 하위딴으로 내려가는 버추얼콜을 쓰게 되니 에러가 나서 먼저 삭제하기 위해 뺐었다.
	 근데 그러면 메모리풀에 사용된 커넥션등의 수많은 객체들이 주소를 잃고 다시 삭제하면서 죽는다.
	*/
//	virtual void CreateMemoryPool( void )=0;
//	virtual void DestroyMemoryPool( void )=0;
	/**
	typeLog: 로그의 종류(xenDef.h 참조)
	szDefault: 엔진이 제공하는 디폴트 메시지
	param1: 로그종류마다 용도가 다른 다용도 파라메터
	*/
	inline virtual void DoLog( XE::xtServerLog typeLog, LPCTSTR szDefault, DWORD param1=0 ) {
		CONSOLE( "%s", szDefault );
	}
	virtual void ConsoleMessage( LPCTSTR szMsg ) {}
	void UpdateTitle( void );
	void ShowLogFilter( const std::string& strTag );
	//
	float m_fAccel;
	BOOL m_bPause;
	int m_FPS;
private:
	XSPLock m_spLock;
	static XEServerMain* s_pInstance;
};


