
// [!output PROJECT_NAME]View.h : [!output PROJECT_NAME]View 클래스의 인터페이스
//

#pragma once
#include "_DirectX/d3dView.h"
#include "client/XAppDelegate.h"


class [!output PROJECT_NAME]View : public CScrollView, public XD3DView
{
	XAppDelegate appDelegate;
protected: // serialization에서만 만들어집니다.
	[!output PROJECT_NAME]View();
	DECLARE_DYNCREATE([!output PROJECT_NAME]View)

// 특성입니다.
public:
	[!output PROJECT_NAME]Doc* GetDocument() const;

// 작업입니다.
public:
	void Draw( void );
	void Update( void ) { Invalidate( FALSE ); }

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.

// 구현입니다.
public:
	virtual ~[!output PROJECT_NAME]View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

#ifndef _DEBUG  // [!output PROJECT_NAME]View.cpp의 디버그 버전
inline [!output PROJECT_NAME]Doc* [!output PROJECT_NAME]View::GetDocument() const
   { return reinterpret_cast<[!output PROJECT_NAME]Doc*>(m_pDocument); }
#endif

[!output PROJECT_NAME]View* GetView( void );