#pragma once
#include "d3dView.h"


// XViewGraph 뷰입니다.
class XDlgEditInterpolation;
class XViewGraph : public CView, public XD3DView
{
	XE::VEC2 m_vMouse;
	XDlgEditInterpolation *m_pDlg;
	DECLARE_DYNCREATE(XViewGraph)

	void DrawInterpolationLine();;

protected:
	XViewGraph();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~XViewGraph();

public:
	GET_SET_ACCESSOR( XDlgEditInterpolation*, pDlg );
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};


