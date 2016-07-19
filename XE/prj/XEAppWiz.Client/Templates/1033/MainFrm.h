
// MainFrm.h : CMainFrame Ŭ������ �������̽�
//

#pragma once
#include "XDlgConsole.h"
#include "_DirectX/XE_MFCFramework.h"

#ifdef _DEBUG
#ifdef WIN32
// ����������
//	#define PHY_WIDTH	960		// windows������ âũ��
//	#define PHY_HEIGHT	640	  
// 4:3-�����е� ����
//	#define PHY_WIDTH	768		// windows������ âũ��
//	#define PHY_HEIGHT	1024	  
// 16:9
//#define PHY_WIDTH	576		// 
//#define PHY_HEIGHT	1024	  
// 5:3- ������s2(480x800)
//	#define PHY_WIDTH		640		// windows������ âũ��
//	#define PHY_HEIGHT		1066
// 8:5-(800x1280)
//	#define PHY_WIDTH		640		// windows������ âũ��
//	#define PHY_HEIGHT		1024

//#define PHY_WIDTH	720		// windows������ âũ��
//#define PHY_HEIGHT	1280
	#define PHY_WIDTH		1280		// windows������ âũ��
	#define PHY_HEIGHT	720
#endif // win32
#else // debug
	#define PHY_WIDTH	1280		// windows������ âũ��
	#define PHY_HEIGHT	720
#endif // not debug
class CMainFrame : public CFrameWnd, public XE_MFCFramework
{
	
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Ư���Դϴ�.
public:
	XDlgConsole m_dlgConsole;

// �۾��Դϴ�.
public:
	void SetTitle( void );

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
	CCoolDialogBar m_wndConsole;

// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnCheatMode();
	afx_msg void OnUpdateCheatMode(CCmdUI *pCmdUI);

};

extern CMainFrame *g_pMainFrm;
#define MAINFRAME	g_pMainFrm
#define D3DDEVICE		g_pMainFrm->GetDevice()



