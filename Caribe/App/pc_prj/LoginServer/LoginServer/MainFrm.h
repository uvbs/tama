
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once

#include "XFramework/MFC/XDlgConsole.h"
#include "XFramework/MFC/CoolDialogBar.h"
#include "XFramework/MFC/XEServerMainFrm.h"

class XDatabase;
class XDBUAccount;
class XPacket;
class CMainFrame : public CFrameWnd, public XEServerMainFrame
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 	void cbCreateNewAccount( XDatabase *pDB,
// 							XDBUAccount *pAccOut,
// 							_tstring& strUUID,
// 							_tstring strName );
// 	void OnResultCreateNewAccount( BOOL bFound, XDBUAccount* pAccount );
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
	afx_msg void OnReloadIni();
	afx_msg void OnControlPanel();
	afx_msg void OnUpdateControlPanel( CCmdUI* pCmdUI );
//	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnCreateDummy();
	CString GetstrTitle() const;
};


