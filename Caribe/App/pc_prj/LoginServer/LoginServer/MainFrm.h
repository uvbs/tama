
// MainFrm.h : CMainFrame Ŭ������ �������̽�
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
	
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 	void cbCreateNewAccount( XDatabase *pDB,
// 							XDBUAccount *pAccOut,
// 							_tstring& strUUID,
// 							_tstring strName );
// 	void OnResultCreateNewAccount( BOOL bFound, XDBUAccount* pAccount );
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
	afx_msg void OnReloadIni();
	afx_msg void OnControlPanel();
	afx_msg void OnUpdateControlPanel( CCmdUI* pCmdUI );
//	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnCreateDummy();
	CString GetstrTitle() const;
};


