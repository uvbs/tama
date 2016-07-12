#pragma once
#include <stdio.h>
#include "etc/Types.h"
#include "XLibrary.h"
//#include "etc/InputMng.h"
#include "XLock.h"

class XLua;

class XLibConsole : public XLibrary
{
	void Init() {}
	void Destroy() {}
protected:
	virtual void ConsoleMessage( LPCTSTR szMsg ) {
#ifdef WIN32
		_tprintf( _T("%s"), szMsg );
#endif
		XTRACE( "%s", szMsg );
	}
public:
	XLibConsole() { Init(); }
	virtual ~XLibConsole() { Destroy(); }
};

// 콘솔툴 같은거 만들때 쓰는 메인 프레임워크
class XConsoleMain
{
	BOOL m_bDestroy;
	XLock m_lockString;		// ConvertString용
#if defined(_SERVER) || defined(_XBOT)
	ID m_idThreadMain = 0;		// 메인스레드 아이디
#endif // _SERVER
#ifdef _SERVER
	XList4<ID> m_listLogidAcc;
#endif // _SERVER
	XList4<std::string> m_listTags;
	bool m_bLoadingLog = false;				// 태그나 idacc파일을 로딩하는동안은 검색같은거 못하게.
	void Init() {
		m_bDebugMode = FALSE;
		m_bDestroy = FALSE;
	}
	void Destroy();
public:
	BOOL m_bDebugMode;			// 디버그 모드
	bool m_bDebugProfilingLoad = false;
public:
	XConsoleMain();
	virtual ~XConsoleMain() { Destroy(); }
	//
	GET_SET_ACCESSOR_CONST( BOOL, bDestroy );
	GET_SET_ACCESSOR_CONST( BOOL, bDebugMode );
	GET_ACCESSOR( XLock&, lockString );
#if defined(_SERVER) || defined(_XBOT)
	GET_ACCESSOR_CONST( ID, idThreadMain );
#endif // _SERVER
	GET_ACCESSOR_CONST( const XList4<std::string>&, listTags );
	//
	void Create( void );
	// virtual 
	// this의 Create가 모두 끝나면 하위클래스에 호출된다.
	virtual void DidFinishCreate( void ) {}
	// 엔진 초기화를 끝내고 리소스를 읽기 직전시점에 호출된다. 패치 클라이언트를 삽입하려면 이곳에 넣어야 한다.
	virtual void DidFinishInitEngine( void ) {}
	// 저수준 레이어 XLibrary객체를 생성 
	virtual XLibrary* CreateXLibrary( void );
	virtual void ConsoleMessage( LPCTSTR szMsg ) {};
	/**
	 리소스 로딩방식을 지정하려면 이 함수를 상속받아 구현해야 한다.
	 typeLoad가 원하는 방식의 값이 왔을때 TRUE를 리턴하면 된다.
	 디폴트로는 xLT_PACKAGE_ONLY 로 되어있다.
	*/
	virtual BOOL OnSelectLoadType( XE::xtLoadType typeLoad ) { return FALSE; }
	//
	virtual LPCTSTR GetVirtualPackageDir( void ) {
#ifdef _XPATCH
	#ifdef WIN32
		// 패치 테스트용
		return _T( "win32/patch/package/package_android/" );
	#else
		return _T("Resource/");
	#endif
#else
		return _T("Resource/");
#endif
	}
	virtual LPCTSTR GetVirtualWorkDir( void ) {
		return _T("Work/");
	}
	virtual void OnError( XE::xtError codeError, DWORD p1=0, DWORD p2=0 ) {};
	void CheckLockFree();
	virtual void Process() {}
#ifdef _SERVER
	void LoadTags();
	bool IsLogidAcc( ID idAcc );
	void AddLogidAcc( ID idAcc );
	void DelLogidAcc( ID idAcc );
	void LoadidAcc();
	GET_ACCESSOR_CONST( const XList4<ID>&, listLogidAcc );
#endif // _SERVER
	bool IsHaveTag( const char* tag );
};

extern XConsoleMain *CONSOLE_MAIN;
