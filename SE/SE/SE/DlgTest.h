#pragma once
#include "Resource.h"
#include "d3dView.h"


// CDlgTest 폼 뷰입니다.

class CDlgTest : public CFormView, public XD3DView
{
	DECLARE_DYNCREATE(CDlgTest)

protected:
	CDlgTest();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CDlgTest();
public:
	enum { IDD = IDD_FORMVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	static CDlgTest *CreateOne( CWnd *pParent );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
protected:
	virtual void OnDraw(CDC* /*pDC*/);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};


