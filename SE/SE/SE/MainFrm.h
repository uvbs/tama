
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once
#include "WndLeftTool.h"
#include "WndThumbView.h"
#include "WndAdjustView.h"
#include "WndSpline.h"
#include "WndFrameBar.h"
#include "WndTimeline.h"
#include "WndConsole.h"
#include "WndActionList.h"
class CMainFrame : public CFrameWndEx, public CD3DApplication
{
    HWND       m_hwndRenderWindow;
    HWND       m_hwndRenderFullScreen;
	BOOL m_bSplitInit;

protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:
//	CSplitterWnd m_wndSplitter;
//	CSplitterWnd m_wndSplitter3;
	BOOL m_bEditing;		// Edit Box에 포커스 가있는중
	bool m_bCtrl = false;
	bool m_bAlt = false;
	bool m_bShift = false;
// 작업입니다.
public:
	void DoPlayStopToggle( bool bToStart );

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void SetTitle();
	HRESULT Create( HWND hwndRenderWindow );
	HRESULT AdjustWindowForChange();

    virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT OneTimeSceneInit();
	virtual HRESULT InitDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;

public:
//	CWndLeftTool		m_wndLeftTool;
	CWndThumbView	m_wndThumbView;
	CWndAdjustView	m_wndAdjustView;
	CWndSpline			m_wndSpline;
	CWndTimeline		m_wndTimeline;
	CWndConsole		m_wndConsole;
	CWndActionList		m_wndActionList;
	// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
//	afx_msg void OnPlay();
//	afx_msg void OnUpdatePlay(CCmdUI *pCmdUI);
	afx_msg void OnPlay();
	afx_msg void OnUpdatePlay(CCmdUI *pCmdUI);
	afx_msg void OnPlayStop();
	afx_msg void OnUpdatePlayStop( CCmdUI *pCmdUI );
	afx_msg void OnFileOpen();
	afx_msg void OnToolStop();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSprOpen();
	afx_msg void OnSprSave();
	afx_msg void OnSprSaveas();
	afx_msg void OnUpdateSprSave(CCmdUI *pCmdUI);
	afx_msg void OnSprNew();
	afx_msg void OnMoveMode();
	afx_msg void OnUpdateMoveMode(CCmdUI *pCmdUI);
	afx_msg void OnRotateMode();
	afx_msg void OnUpdateRotateMode(CCmdUI *pCmdUI);
	afx_msg void OnScaleMode();
	afx_msg void OnUpdateScaleMode(CCmdUI *pCmdUI);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPrevFrame();
	afx_msg void OnNextFrame();
	afx_msg void OnUpdatePrevFrame(CCmdUI *pCmdUI);
	afx_msg void OnUpdateNextFrame(CCmdUI *pCmdUI);
	afx_msg void OnLockAxisx();
	afx_msg void OnLockAxisy();
	afx_msg void OnLockAxisxy();
	afx_msg void OnUpdateLockAxisx(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLockAxisy(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLockAxisxy(CCmdUI *pCmdUI);
	afx_msg void OnOpenContImage();
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);
	afx_msg void OnOpenAlignedImage();
	afx_msg void OnUpdateOpenAlignedImage(CCmdUI *pCmdUI);
	afx_msg void OnScaleView1();
	afx_msg void OnScaleView2();
	afx_msg void OnScaleView3();
	afx_msg void OnScaleView4();
	afx_msg void OnFrameskip();
	afx_msg void OnUpdateFrameskip(CCmdUI *pCmdUI);
	afx_msg void OnConvert();
	afx_msg void OnBnClickedButtAddLayer();
	afx_msg void OnBnClickedButtCreateobj();
	afx_msg void OnAddLayerImage();
	afx_msg void OnAddLayerObject();
	afx_msg void OnCompConverter();
	afx_msg void OnLoadBg();
	afx_msg void OnModeMoveByBg();
	afx_msg void OnUpdateModeMoveByBg( CCmdUI *pCmdUI );
};

extern CMainFrame *g_pMainFrm;
#define D3DDEVICE		g_pMainFrm->GetDevice()
