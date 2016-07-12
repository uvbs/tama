
// MainFrm.h : CMainFrame Ŭ������ �������̽�
//

#pragma once

#include "XDlgConsole.h"
#include "XFramework/MFC/CoolDialogBar.h"

class CMainFrame : public CFrameWnd
{
	XDlgConsole m_dlgConsole;
	CCoolDialogBar m_wndConsole;
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Ư���Դϴ�.
public:

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// �����Դϴ�.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;

// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnReadyOffline();
	afx_msg void OnUpdateReadyOffline( CCmdUI *pCmdUI );
	DECLARE_MESSAGE_MAP()

};


