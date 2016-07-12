#pragma once
#include "d3dView.h"

class XMyD3DView : public CView, public XD3DView
{
public:
	XMyD3DView() {}
	virtual ~XMyD3DView() {}

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	static XMyD3DView *CreateOne( CWnd *pParent, XMyD3DView *pView );
	virtual void Update( void ) {	Invalidate( FALSE );	}
protected:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual void OnInitialUpdate();     // 생성된 후 처음입니다.
	virtual void Draw( void ) {}
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
