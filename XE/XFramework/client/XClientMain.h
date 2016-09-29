#pragma once
#include "XApp.h"
//#include "XBaseFontDat.h"
//#include "XFontMng.h"
//#include "XWindow.h"
#include "etc/InputMng.h"
#include "XLibrary.h"
#include "XFramework/XConsoleMain.h"

//class XBaseScene;
class XLua;
class XFontMng;
class XEContent;
class XBaseFontDat;
class XBaseFontObj;

// 게임윈도우의 좌표계 영역
//#define XGAME_SIZE		XE::GetGameSize()
namespace XE {
// s_dwDraw용 비트
enum {
	xeBitNone = 0,
	xeBitNoDraw = 0x01,
	xeBitNoDP = 0x02, // no draw primitive
	xeBitNoTexture = 0x04, // SetTexture생략
	xeBitSmallTex = 0x08,		// 작은 텍스쳐 사용
	xeBitNoDrawBar = 0x10,	// DrawProgressBar생략
	xeBitNoFont = 0x20,			// 폰트그리기 생략
	xeBitNoProcess = 0x40,	// ObjMng의 객체들 frameMove생략
};
}

class XELibrary : public XLibrary
{
	XClientMain *m_pMain;
	void Init() {
		m_pMain = NULL;
	}
	void Destroy() {}
protected:
	void ConsoleMessage( LPCTSTR szMsg ) override;
public:
	XELibrary( XClientMain *pMain ) { 
		Init(); 
		m_pMain = pMain;
	}
	virtual ~XELibrary() { Destroy(); }
	//
};

// 엔진의 메인
class XClientMain : public XConsoleMain
{
public:
	static XE::VEC2 sCalcLogicalResolution( const XE::VEC2& sizePhy, const XE::VEC2& sizeGame );
	enum xtRestore {
		xRST_NONE,	// 초기화상태
		xRST_TRUE,	// 리스토어 해야하는 상태
		xRST_FALSE,	// 디바이스자원 날아간 상태
	};
	// 터치 이벤트
	enum xtTouchEvent {
		xTE_DOWN,
		xTE_MOVE,
		xTE_UP,
		xTE_ZOOM,	// 핀치투줌(확대/축소)
#ifdef WIN32
		xTE_RDOWN,		// win32전용
		xTE_RUP,
#endif
	};
#ifdef _CHEAT
	bool m_bCtrl = false;
	bool m_bAlt = false;
	XE::VEC2 m_vTouchStart;		// 우클릭 영역표시를 위해 최초 찍은 좌표
	XE::VEC2 m_vTouchCurr;
	BOOL m_bDebugDrawArea;
#ifdef WIN32
	bool m_bViewFrameRate = false;
#else
	bool m_bViewFrameRate = false;
#endif
#endif
private:
	XEContent *m_pGame;
	ID m_idThreadMain = 0;
	BOOL m_bExit;
	BOOL m_bFrameSkip;
	BOOL m_bFrameMove;	// FrameMove()를 진행하느냐
	BOOL m_bDraw;		// Draw()를 진행하느냐
	BOOL m_bResume;		//
	BOOL m_bDrawSkipThisFrame;		// 이번 한프레임만 드리는걸 생략함.
	XE::xRESULT_AUTHEN *m_pResultAuthen;
// 	XE::xResult_FacebookProfileImage m_resultFacebookProfileImage;
	bool m_bFrameSkipReset = false;
	void Init() {
		m_pGame = NULL;
		m_fAccel = 1.0f;
		m_bPause = FALSE;
		m_bNextFrame = FALSE;
		m_bExit = FALSE;
		m_bFrameSkip = TRUE;
		m_bFrameMove = TRUE;
		m_bDraw = TRUE;
		m_Restore = xRST_NONE;
		m_bResume = FALSE;
//		m_LoginAuthen = xAUT_NONE;
		m_pResultAuthen = NULL;
		m_bDrawSkipThisFrame = FALSE;
#ifdef _CHEAT
		m_bDebugDrawArea = FALSE;
#ifdef _VER_ANDROID
        m_PushState = 0;
#endif
#endif
	}
	void Destroy();
public:
	float m_fAccel;
	BOOL m_bPause;		// 이건 UI와 시간까지 모두 멈춰버리니 잠시 게임멈춰두고 UI처리하는거엔 쓰지 말것.
	BOOL m_bNextFrame;
	xtRestore m_Restore;
	static XFps s_fps;
public:
	XClientMain();
	virtual ~XClientMain() { Destroy(); }
	//
	GET_SET_ACCESSOR( bool, bViewFrameRate );
	GET_SET_ACCESSOR( BOOL, bFrameSkip );
	GET_ACCESSOR( XEContent*, pGame );
	GET_SET_ACCESSOR( BOOL, bPause );
	GET_SET_ACCESSOR( BOOL, bNextFrame );
	GET_ACCESSOR( bool, bFrameSkipReset );
#ifdef _CHEAT
	GET_SET_ACCESSOR_CONST( BOOL, bDebugDrawArea );
#endif // _CHEAT
	void ResetFrameSkip() {
		m_bFrameSkipReset = true;
	}
//	GET_SET_ACCESSOR(BOOL, bFrameMove);
//	GET_SET_ACCESSOR(BOOL, bDraw);
//	GET_SET_ACCESSOR(xtAuthen, LoginAuthen);
	GET_SET_ACCESSOR(XE::xRESULT_AUTHEN*, pResultAuthen);
//	GET_SET_ACCESSOR_CONST( const XE::xResult_FacebookProfileImage&, resultFacebookProfileImage );
	XBaseFontDat* GetSystemFontDat();
	XBaseFontObj* GetSystemFontObj();
	//
	void Create( XE::xtDevice device, int wPhy, int hPhy, float wLog, float hLog );
	float CalcDT();
	void ClearDT();
// 	XE::VEC2 CalcLogicalResolution( const XE::VEC2& sizePhy, const XE::VEC2& sizeGame );
	void DoExit();
	void StopAnimation();
	void StartAnimatiom();
	void OnAlert();
	virtual void OnCheatMode();
	void OnTouchEvent( xtTouchEvent event, int cntTouches, float x, float y, float scale=0 );
//	virtual void OnTouchesDown( int cntTouches, float x, float y ) {}
//	virtual void OnTouchesMove( int cntTouches, float x, float y ) {}
//	virtual void OnTouchesUp( int cntTouches, float x, float y ) {}
	void OnPauseHandler();
	void OnResumeHandler();
	void OnKeyDown( int keyCode );
	void OnKeyUp( int keyCode );
#ifdef _CHEAT
#ifdef _VER_ANDROID
    int m_PushState;
#endif
	void SaveCheat();
	void LoadCheat();
	virtual void SaveCheat( FILE *fp ) {}
	virtual void LoadCheat( CToken& token ) {}
#endif
	// virtual 
	// this의 Create가 모두 끝나면 하위클래스에 호출된다.
	virtual void DidFinishCreate() {};
	// 엔진 초기화를 끝내고 리소스를 읽기 직전시점에 호출된다. 패치 클라이언트를 삽입하려면 이곳에 넣어야 한다.
	virtual void DidFinishInitEngine() {}
	// 매 루프 돌아가야 하는 게임객체가 있다면 생성을 하위에 맡긴다.
	virtual XEContent* CreateGame() { return NULL; }
	// 사용자정의 폰트매니저가 있다면 virtual로 생성해준다.
	virtual XFontMng* CreateFontMng();
	// 저수준 레이어 XLibrary객체를 생성 
	virtual XLibrary* CreateXLibrary();

	// 매 프레임 호출되어 프로세스 된다.
//	virtual void Process();
	virtual void FrameMove();
	virtual void OnDelegateFrameMove( float dt );
	virtual void Draw();
	virtual void DrawDebugInfo( float x, float y );
	virtual void RestoreDevice();
	/**
	 리소스 로딩방식을 지정하려면 이 함수를 상속받아 구현해야 한다.
	 typeLoad가 원하는 방식의 값이 왔을때 TRUE를 리턴하면 된다.
	*/
	virtual BOOL OnSelectLoadType( XE::xtLoadType typeLoad ) { 
//		return (typeLoad == XE::xLT_WORK_TO_PACKAGE_COPY)? TRUE : FALSE;	// default
		return FALSE;	// XClientMain::Create에서 디폴트 값을 지정하는방식으로 바뀜
	}
	virtual void ConsoleMessage( LPCTSTR szMsg ) {};
	template<int _Size>
	BOOL DoEditBox( TCHAR (&pOutText)[_Size] ) { 
		return DoEditBox( pOutText, _Size ); 
	}
	virtual BOOL DoEditBox( TCHAR *pOutText, int lenOut ) { 
		XBREAKF(1, "%s", _T("구현되지않음"));
		return FALSE; 
	}
	void OnDestroy();
	virtual bool RequestCheatAuth() { return true; }
#ifdef WIN32
	bool IsThreadMain() const;
#endif // WIN32
};

//extern volatile XClientMain *_MAIN;


