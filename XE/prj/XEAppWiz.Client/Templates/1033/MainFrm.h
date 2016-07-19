
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once
#include "XDlgConsole.h"
#include "_DirectX/XE_MFCFramework.h"

#ifdef _DEBUG
#ifdef WIN32
// 아이폰비율
//	#define PHY_WIDTH	960		// windows에서의 창크기
//	#define PHY_HEIGHT	640	  
// 4:3-아이패드 비율
//	#define PHY_WIDTH	768		// windows에서의 창크기
//	#define PHY_HEIGHT	1024	  
// 16:9
//#define PHY_WIDTH	576		// 
//#define PHY_HEIGHT	1024	  
// 5:3- 갤럭시s2(480x800)
//	#define PHY_WIDTH		640		// windows에서의 창크기
//	#define PHY_HEIGHT		1066
// 8:5-(800x1280)
//	#define PHY_WIDTH		640		// windows에서의 창크기
//	#define PHY_HEIGHT		1024

//#define PHY_WIDTH	720		// windows에서의 창크기
//#define PHY_HEIGHT	1280
	#define PHY_WIDTH		1280		// windows에서의 창크기
	#define PHY_HEIGHT	720
#endif // win32
#else // debug
	#define PHY_WIDTH	1280		// windows에서의 창크기
	#define PHY_HEIGHT	720
#endif // not debug
class CMainFrame : public CFrameWnd, public XE_MFCFramework
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:
	XDlgConsole m_dlgConsole;

// 작업입니다.
public:
	void SetTitle( void );

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;
	CCoolDialogBar m_wndConsole;

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnCheatMode();
	afx_msg void OnUpdateCheatMode(CCmdUI *pCmdUI);

};

extern CMainFrame *g_pMainFrm;
#define MAINFRAME	g_pMainFrm
#define D3DDEVICE		g_pMainFrm->GetDevice()



