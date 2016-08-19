#pragma once

#include "d3dView.h"

// CAdjustView 폼 뷰입니다.

class CAdjustView : public CFormView, public XD3DView
{
	DECLARE_DYNCREATE(CAdjustView)

	int m_x, m_y;	
	CPoint m_pointMouse, m_pointRMouse;
	int m_DragMove;
	BOOL m_bViewBaseSpr;		// 기준스프라이트 온/오프

protected:
	CAdjustView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CAdjustView();

//	CToolBar m_ToolBar;
public:
	enum { IDD = IDD_ADJUSTVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	static CAdjustView *CreateOne( CWnd *pParent );

	void Update() { Invalidate( FALSE ); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
protected:
	virtual void OnDraw(CDC* /*pDC*/);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};

CAdjustView *GetAdjustView();


