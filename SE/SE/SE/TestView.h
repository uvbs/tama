#pragma once
#include "d3dView.h"

// CTestView 뷰입니다.

class CTestView : public CView, public XD3DView
{
	DECLARE_DYNCREATE(CTestView)

protected:
	CTestView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CTestView();

public:
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
};


