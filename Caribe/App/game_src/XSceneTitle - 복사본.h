#pragma once
#include "XSceneBase.h"
#include "client/XLayout.h"
#include "_Wnd2/XWndEdit.h"
#include "XFacebook.h"

class XGame;
class XSceneTitle : public XSceneBase
					        , public XDelegateWndEdit
									, public XDelegateFacebook
{
public:
	static xSec s_secLoginLockStart;   // 로그인락 시작시간
	static xSec s_secLoginLockTotal;   // 로그인락 총 대기시간
private:
	XLayoutObj m_Layout;
	XTimerTiny m_timerLoginLock;
	_tstring m_strLogin/* = _T( "login.txt" )*/;
	XWndScrollView *m_pScrollView = nullptr;
	XE::VEC2 m_vFocus;
	XE::VEC2 m_deltaScroll;
	XE::VEC2 m_sizeMap;
	//
	void Init()  {
	}
	void Destroy();
protected:
public:
	XSceneTitle( XGame *pGame );
	virtual ~XSceneTitle(void) { Destroy(); }
	//
	// virtual
	virtual void Create( void );
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	//
	int OnClickTitle( XWnd* pWnd, DWORD p1, DWORD p2 );
	void OnConnectedGameSvr(void);
	void OnConnectedLoginSvr();
	void RecvDuplicateNickName(void);
	void RecvNoAccount( XGAME::xtConnectParam param );
//	void RecvReconnectTry(void);
	//void RecvNickNameChage(_tstring strName, int result);
	int OnClosePopup(XWnd* pWnd, DWORD p1, DWORD p2);		// 팝업 닫기
	int OnClickNicknameBox(XWnd* pWnd, DWORD p1, DWORD p2);
	int OnCreateTryConnect(XWnd* pWnd, DWORD p1, DWORD p2);
	virtual void OnDelegateEnterEditBox(XWndEdit *pWndEdit, LPCTSTR szString);
	int OnGotoAppStore( XWnd* pWnd, DWORD p1, DWORD p2 );
	void Update() override;
	int OnClickIpChange( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnEnterIP( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnYesHaveAccount( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnNoHaveNoAccount( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickEmailBox( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickPasswordBox( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickLogin( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickLoginCancel( XWnd* pWnd, DWORD p1, DWORD p2 );
	void DoPopupSelectLogin();
	int RecvWrongPassword( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnOkWrongPassword( XWnd* pWnd, DWORD p1, DWORD p2 );
	void RecvClosedSvr();
  void OnInitLogin();
  void OnAutoUpdate();
  void OnRecvLoginLockForBattle( xSec secStart, xSec secTotal );
  void OnRecvLoginLockFree();
  void ClearLoginLock();
	void OnSendCrashDump( int sizeDump );
	int OnClickLoginFacebook( XWnd* pWnd, DWORD p1, DWORD p2 );

	int OnClickUpdatePopupOk(XWnd* pWnd, DWORD p1, DWORD p2);
	void DelegateFacebookCertResult( const char *cUserId, const char *cUsername, DWORD param ) override;
};

extern XSceneTitle *SCENE_TITLE;


