
// MainFrm.h : CMainFrame Ŭ������ �������̽�
//

#pragma once

#include "XDlgConsole.h"
#include "MFC/CoolDialogBar.h"
#include "MFC/XEServerMainFrm.h"

class CMainFrame : public CFrameWnd, public XEServerMainFrame
{
	
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
	CStatusBar        m_wndStatusBar;
	XDlgConsole			m_dlgConsole;
	CCoolDialogBar		m_wndConsole;

// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

};


