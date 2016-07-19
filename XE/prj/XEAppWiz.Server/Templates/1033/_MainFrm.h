
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once

#include "XDlgConsole.h"
#include "MFC/CoolDialogBar.h"
#include "MFC/XEServerMainFrm.h"

class CMainFrame : public CFrameWnd, public XEServerMainFrame
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:

// 작업입니다.
public:

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
	CStatusBar        m_wndStatusBar;
	XDlgConsole			m_dlgConsole;
	CCoolDialogBar		m_wndConsole;

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

};


